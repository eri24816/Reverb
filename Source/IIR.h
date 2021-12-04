#pragma once
#include <queue>
#include "Matrix.h"
namespace IIR {

	/* Vector operations */
	// Copy b to a
	inline float* copy(int n, float* a, float* b) {
		while (n--)a[n] = b[n];
		return a;
	}

	// Add b to a
	inline float* add(int n, float* a, float* b) {
		while (n--)a[n] += b[n];
		return a;
	}

	// Multiply a(vector) with b(scalar)
	inline float* mult(int n, float* a, float b) {
		while (n--)a[n] *= b;
		return a;
	}

	template<int inputDim>
	class Module {
	public:
		virtual float* update(float* input) = 0;
	};

	
	// Delay lines
	template<int inputDim>
	class Delay : public Module<inputDim>{
	private:
		std::queue<float> queues[8]; // Max. 8 delay lines
		float outputBuffer[10]{0};
	public:
		
		Delay(const int numDelaySamples[]){
			for (int i = 0; i < inputDim; i++) {
				for(int j=0;j< numDelaySamples[i];j++)
					queues[i].push(0);
			}
		}
		Delay( int numDelaySamples){
			for (int i = 0; i < inputDim; i++) {
				for (int j = 0; j < numDelaySamples; j++)
					queues[i].push(0);
			}
		}
		float* update(float* input) override{
			for (int i = 0; i < inputDim; i++) {
				queues[i].push(input[i]);
				outputBuffer[i] = queues[i].front();
				queues[i].pop();
			}
			return outputBuffer;
		}
		void SetNumDelaySamples(const int numDelaySamples[]) {
			for (int i = 0; i < inputDim; i++) {
				while (queues[i].size() > numDelaySamples[i]) {
					queues[i].pop();
				}
				while (queues[i].size() < numDelaySamples[i]) {
					queues[i].push(0);
				}
			}
		}
	};

	class DelayLine {
	private:
		float* buffer;
		int length;
		int pointer=0;
	public:
		DelayLine(int length) :buffer(new float[length] {0}),length(length) {}
		float update(float input) {
			float output = buffer[pointer];
			buffer[pointer] = input;
			pointer++;
			if (pointer == length)pointer = 0;
			return output;
		}
	};

	template<int inputDim>
	class Delay2 : public Module<inputDim> {
	private:
		float* arr[8];
		int pos[8]{ 0 };
		int len[8];
		float outputBuffer[10];
	public:

		Delay2( const int numDelaySamples[]) {
			for (int i = 0; i < inputDim; i++) {
				arr[i] = new float[numDelaySamples[i]]{ 0 };
				len[i] = numDelaySamples[i];
			}
		}
		Delay2(float inputDim, int numDelaySamples){
			for (int i = 0; i < inputDim; i++) {
				arr[i] = new float[numDelaySamples]{ 0 };
				len[i] = numDelaySamples;
			}
		}
		float* update(float* input) override {
			for (int i = 0; i < inputDim; i++) {
				outputBuffer[i] = arr[i][pos[i]];
				arr[i][pos[i]] = input[i];
				pos[i]++;
				if (pos[i] == len[i])pos[i] = 0;
			}
			return outputBuffer;
		}
	};

	template<int inputDim>
	class Lowpass :Module<inputDim> {
		float a=0.5;
		float feedback[10]{ 0 };
	public:
		float* update(float* input) override {

			mult(inputDim, input, 1 - a);
			mult(inputDim, feedback, a);

			add(inputDim, input, feedback);

			copy(inputDim, feedback, input);

			return mult(inputDim, input,1);
		}
		void setA(float a) {
			this->a = exp(-(1.f/44100)*6.283*a);
		}
	};

	template<int inputDim>
	class Allpass : Module<inputDim> {
		Delay2 ff1, ff2, fb1, fb2;
		float feedback[10]{ 0 }, temp[10]{ 0 }, fbtemp[10]{ 0 };
	public:
		float R2, twoRCosTheta;
		Allpass(float inputDim, float R, float theta) :
			Module(inputDim),
			twoRCosTheta(2 * R * cos(theta)),
			R2(R* R),
			ff1(inputDim, 1),
			ff2(inputDim, 2),
			fb1(inputDim, 1),
			fb2(inputDim, 2)
		{
		}
		int sampleCount = 0;
		float* update(float* input)override {
			// y[n] = r*r*x[n]-2*r*cos(theta)*x[n-1]+x[n-2]+2*r*cos(theta)*y[n-1]-r*r*y[n-2]
			copy(inputDim, temp, input);

			mult(inputDim, input, R2);
			add(inputDim, input, mult(inputDim, ff1.update(temp), -twoRCosTheta));
			add(inputDim, input, ff2.update(temp));
			//copy(inputDim, fbtemp, feedback);
			add(inputDim, input, mult(inputDim, feedback, twoRCosTheta));
			add(inputDim, input, mult(inputDim, fb1.update(fbtemp), -R2));

			copy(inputDim, feedback, input);


			sampleCount++;

			return input;
		}
	};

	template<int inputDim>
	class Allpass2: Module<inputDim> {
	private:
		float R2[10], twoRCosTheta[10];
		float x1[10] = { 0 }, x2[10] = {0}, y1[10] = {0}, y2[10] = { 0 };
		float output[10];
	public:
		Allpass2(float R[],float theta[])
		{
			for (int i = 0; i < inputDim; i++) {
				twoRCosTheta[i]=(2 * R[i] * cos(theta[i]));
				R2[i] = R[i]* R[i];
			}
		}
		int sampleCount = 0;
		float* update(float* input)override {
			// y[n] = r*r*x[n]-2*r*cos(theta)*x[n-1]+x[n-2]+2*r*cos(theta)*y[n-1]-r*r*y[n-2]
			
			for (int i =0;i<inputDim;i++) {
				output[i] = R2[i] * input[i] - twoRCosTheta[i] * x1[i] + x2[i] + twoRCosTheta[i] * y1[i] - R2[i]*y2[i];
				x2[i] = x1[i];
				x1[i] = input[i];

				y2[i] = y1[i];
				y1[i] = output[i];
			}

			return output;
		}

		void SetR(float* r) {
			for (int i = 0; i < inputDim; i++) {
				R[i] = r[i];
			}
			for (int i = 0; i < inputDim; i++) {
				twoRCosTheta[i] = (2 * R[i] * cos(theta[i]));
				R2[i] = R[i] * R[i];
			}
		}
	};

	template<int inputDim>
	class Comb : Module<inputDim> {
		Delay2<inputDim> delay;
	public:
		float* update(float* input)override {
			add(inputDim, input, delay.update(input));
			return input;
		}
	};

	template<int inputDim>
	class DCBlocker :Module<inputDim> {
		float inTemp[10], delay[10]{ 0 }, feedback[10]{ 0 };
	public:
		float* update(float* input)override {
			copy(inputDim, inTemp, input);

			add(inputDim, input,delay);
			add(inputDim, input, feedback);

			copy(inputDim, delay, inTemp);
			mult(inputDim, delay, -1);
			copy(inputDim, feedback,input);
			mult(inputDim, feedback, 0.995);
			return input;
		}
	};


#define NCH 8
	// IIR system that simulates reverb
	template<int inputDim>
	class Reverb : Module<inputDim> {
		
		float feedBack[NCH]{0};

		Matrix  distrib,outDistrib;
		Matrix feedbackmatrix;

		float dryAmount = 1;
		float wetAmount = 1;
		
		float _roomSize = 1;
		float _roomShape = 1;
		float _decay = 1;
		float _damping = 1;
		float _modulationDepth = 1;

	public:
		Delay<NCH> inDelay, fbDelayLine;
		Lowpass<NCH> delayFilters;
		Allpass2<NCH> allpass;
		DCBlocker<NCH> dcBlocker;

		Reverb() :
			inDelay(new int[] {100, 120}),
			allpass(new float[] { 0.9, 0.99, 0.9, 0.95, 0.8, 0.96, 0.75, 0.97 }, new float[] {0.7, 0.8, 1, 1.5, 0.73, 1, 0.93, 1.8}),
			fbDelayLine( new int[] {3505,4725,7528,5270,8578,4324,5462,4256}),// The actual delay time is one sample more than the numbers here.
			distrib(NCH, inputDim, new float[] { 0.094, 0.142, -0.189, 0.124, 0.02, 0.161, 0.026, 0.023, 0.079, -0.068, -0.134, 0.286, 0.161, -0.216, 0.086, -0.043 }),
			outDistrib(inputDim, NCH, new float[] {-0.05, -0.207, 0.18, -0.172, -0.221, -0.287, 0.077, -0.095, -0.252, 0.29, -0.163, 0.064, -0.272, -0.135, 0.211, 0.085})
			
		{
			/*
			Matrix eigenvectors = Matrix(NCH, NCH, new float[]{ -2.542, 1.19, -0.103, 0.196, 0.196, 1.07, 0.887, 1.075, -0.028, 0.419, 1.299, 0.278, 0.491, 0.581, -0.374, 0.853, -0.063, 0.12, -0.059, 0.684, -0.031, -0.324, 1.366, -0.432, 0.033, -0.3, 1.142, 0.744, -0.226, -0.009, 1.475, 0.95, 0.227, 1.462, 0.207, 0.989, 0.501, 1.215, -0.215, 0.865, 1.013, 0.531, 1.078, 0.448, 1.018, 1.347, 0.887, 0.302, 1.458, 1.034, 0.902, 0.084, 0.795, 0.07, -0.245, -0.182, 1.368, -0.102, 0.352, 0.417, 0.284, 1.093, 0.267, 0.046 });
			Matrix eigenvalues = diag(new float[]{0.8,0.81, 0.82,0.815,0.814,0.8,0.84,0.83}, NCH);
			feedbackmatrix = eigenvectors * eigenvalues * ~eigenvectors;
			*/
			
			// Matrix that have long reverb time
			feedbackmatrix = Matrix(NCH, NCH, new float[] {
				0.1, 0.9, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.1, 0.9, 0, 0.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.1, 0.9, 0, 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.1, 0.9, 0, 0.0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.1, 0.9, 0, 0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.9, 0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.9,
				0.9, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1,
			});
			/*
			// Matrix that make low freq gain more
			feedbackmatrix = Matrix(NCH, NCH, new float[] {
				0.7, 0.3, 0, 0.0, 0.0, 0.0, 0.0, 0.0,
				0.3, 0.7, 0, 0, 0.0, 0.0, 0.0, 0.0,
				0.2, 0.2, 0.1, 0.5, 0, 0.0, 0.0, 0.0,
				0.2, 0.2, 0.0, 0.1, 0.5, 0, 0.0, 0.0,
				0.2, 0.2, 0.0, 0.0, 0.1, 0.5, 0, 0.0,
				0.2, 0.2, 0.0, 0.0, 0.0, 0.1, 0.5, 0,
				0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.1, 0.5,
				 0.2, 0.2, 0.5, 0.0, 0.0, 0.0, 0.0, 0.1,
			});
			*/
		}

		// PluginProcessor calls this
		float* update(float* input) override{

			if (channel >0) {
				float inputToAllpass[NCH]{ 0 };
				for (int i = 0; i < NCH; i++) {
					inputToAllpass[i] = input[0];
				}
				float*  temp= allpass.update(inputToAllpass);
				if(JUCE_LIVE_CONSTANT(false))
					temp = feedbackmatrix * temp;
				delayFilters.update(temp);
				input[0] = temp[channel];
				input[1] = temp[channel];
				return input;
			}
			
			float dry[2];
			copy(inputDim, dry, input);

			input = distrib * inDelay.update(input);

			delayFilters.update(feedBack);

			//dcBlocker.update(feedBack);

			add(NCH,input, fbDelayLine.update(feedBack));

			input = allpass.update(input);

			input = mult(inputDim, input, _decay);

			float* output = feedbackmatrix * input;


			copy(NCH,feedBack, output);

			return dcBlocker.update(add(inputDim,mult(inputDim,outDistrib*output,wetAmount), mult(inputDim, dry,dryAmount)));
		}

		int channel = -1;

		void ChangeChannel(int channel) {

			this->channel = channel;
		}

		void SetDryWetRatio(float value) {
			value *= 3.14159265 / 2;
			dryAmount = cos(value);
			wetAmount = sin(value)*3;
		}


		float GetRoomSize() { return _roomSize; }
		float GetRoomShape() { return _roomShape; }
		float GetDecay() { return _decay; }
		float GetDamping() { return _damping; }
		float GetModulationDepth() { return _modulationDepth; }

		float oneStd[8] = { -1.4268266754370598, -1.2363520626685995, -0.8852182973907836, 0.33680311221022213, 0.3939763764975236, 0.4007880613193235, 0.8197178114919129, 1.5971116739774605 };

		void SetRoomSize(float v) { 
			_roomSize=v;
			int numDelaySamples[NCH];
			for (int i = 0; i < NCH; i++) {
				numDelaySamples[i] = oneStd * _roomShape * roomSize + roomSize;
				if (numDelaySamples[i] < 1)numDelaySamples[i] = 1;
			}
			fbDelayLine.SetNumDelaySamples(numDelaySamples);
		}

		void SetRoomShape(float v) {
			_roomShape = v;
			int numDelaySamples[NCH];
			for (int i = 0; i < NCH; i++) {
				numDelaySamples[i] = oneStd * _roomShape + roomSize;
				if (numDelaySamples[i] < 1)numDelaySamples[i] = 1;
			}
			fbDelayLine.SetNumDelaySamples(numDelaySamples);
		}

		void SetDecay(float v) {
			_decay = v;
		}

		void SetDamping(float v) {
			_damping = v; 
			delayFilters.setA(_damping)
		}

		float baseR[8] = { 0.9, 0.99, 0.9, 0.95, 0.8, 0.96, 0.75, 0.97 };
		void SetModulationDepth(float v) {
			_modulationDepth = v; 
			allpass.SetR(mult(inputDim, baseR, v));
		}

	};
}
