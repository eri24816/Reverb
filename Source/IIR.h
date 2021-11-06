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

	class Module {
	public:
		int inputDim = 2;
		virtual float* update(float* input) = 0;
		Module(int inputDim):inputDim(inputDim){}
	};

	// Delay line
	class Delay : public Module {
	private:
		std::queue<float> queues[8]; // Max. 8 delay lines
		float outputBuffer[10]{0};
	public:
		
		Delay(float inputDim,const int numDelaySamples[]):Module(inputDim){
			for (int i = 0; i < inputDim; i++) {
				for(int j=0;j< numDelaySamples[i];j++)
					queues[i].push(0);
			}
		}
		Delay(float inputDim, int numDelaySamples) :Module(inputDim) {
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
	};

	class DelayLine {
	private:
		float* buffer;
		int length;
		int pointer=0;
	public:
		DelayLine(int inputDim, int length) :buffer(new float[length] {0}),length(length) {}
		float update(float input) {
			float output = buffer[pointer];
			buffer[pointer] = input;
			pointer++;
			if (pointer == length)pointer = 0;
			return output;
		}
	};

	class Delay2 : public Module {
	private:
		float* arr[8];
		int pos[8]{ 0 };
		int len[8];
		float outputBuffer[10];
	public:

		Delay2(float inputDim, const int numDelaySamples[]) :Module(inputDim) {
			for (int i = 0; i < inputDim; i++) {
				arr[i] = new float[numDelaySamples[i]]{ 0 };
				len[i] = numDelaySamples[i];
			}
		}
		Delay2(float inputDim, int numDelaySamples) :Module(inputDim) {
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

	class Lowpass :Module {
		Delay delay1, delay2, delay3, delay4;
		float c1, c2, c3, c4;
		float temp[10];
	public:
		Lowpass(int inputDim) :Module(inputDim), 
			delay1(inputDim, 1),
			delay2(inputDim, 2),
			delay3(inputDim, 3),
			delay4(inputDim, 4)
		{}
		float* update(float* input) override {
			copy(inputDim, temp, input);

			add(inputDim, input, delay1.update(temp));
			add(inputDim, input, delay2.update(temp));
			add(inputDim, input, delay3.update(temp));
			add(inputDim, input, delay4.update(temp));

			return mult(inputDim, input, 0.2);
		}
		void setAmount(float amount) {
			
		}
	};
	class Allpass : Module {
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
	class Allpass2: Module {
	private:
		float R2[10], twoRCosTheta[10];
		float x1[10] = { 0 }, x2[10] = {0}, y1[10] = {0}, y2[10] = { 0 };
		float output[10];
	public:
		Allpass2(float inputDim,float R[],float theta[]):
			Module(inputDim)
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
	};

	class Comb : Module {
		Delay2 delay;
	public:
		Comb(int inputDim) :Module(inputDim),delay(inputDim,10) {}
		float* update(float* input)override {
			add(inputDim, input, delay.update(input));
			return input;
		}
	};


#define NCH 8
	// IIR system that simulates reverb
	 class Reverb : Module {
		
		float feedBack[NCH]{0};

		Matrix  distrib,outDistrib;
		Matrix feedbackmatrix;
	public:
		Delay inDelay, fbDelayLine;
		Lowpass delayFilters;
		Allpass2 allpass;

		Reverb() :
			Module(2),
			inDelay(inputDim, new int[] {100, 120}),
			delayFilters(NCH),
			allpass(NCH, new float[]{ 0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.95 }, new float[]{1,2,3,1.5,2.5,0.5,1.9,2.8}),
			fbDelayLine(NCH, new int[] {1232, 1402 ,3860,3575,3046,2137,5864,100}),
			distrib(NCH, inputDim, new float[] { 0.094, 0.142, -0.189, 0.124, 0.02, 0.161, 0.026, 0.023, 0.079, -0.068, -0.134, 0.286, 0.161, -0.216, 0.086, -0.043 }),
			outDistrib(inputDim, NCH, new float[] {-0.05, -0.207, 0.18, -0.172, -0.221, -0.287, 0.077, -0.095, -0.252, 0.29, -0.163, 0.064, -0.272, -0.135, 0.211, 0.085})
		{
			Matrix eigenvectors = Matrix(NCH, NCH, new float[]{ -2.542, 1.19, -0.103, 0.196, 0.196, 1.07, 0.887, 1.075, -0.028, 0.419, 1.299, 0.278, 0.491, 0.581, -0.374, 0.853, -0.063, 0.12, -0.059, 0.684, -0.031, -0.324, 1.366, -0.432, 0.033, -0.3, 1.142, 0.744, -0.226, -0.009, 1.475, 0.95, 0.227, 1.462, 0.207, 0.989, 0.501, 1.215, -0.215, 0.865, 1.013, 0.531, 1.078, 0.448, 1.018, 1.347, 0.887, 0.302, 1.458, 1.034, 0.902, 0.084, 0.795, 0.07, -0.245, -0.182, 1.368, -0.102, 0.352, 0.417, 0.284, 1.093, 0.267, 0.046 });
			Matrix eigenvalues = diag(new float[]{0.7,0.71, 0.72,0.715,0.714,0.7,0.74,0.73}, NCH);
			feedbackmatrix = eigenvectors * eigenvalues * ~eigenvectors;
		}

		// PluginProcessor calls this
		float* update(float* input) override{



			input = distrib * inDelay.update(input);

			//delayFilters.update(feedBack);

			add(NCH,input, fbDelayLine.update(feedBack));

			input = allpass.update(input);

			float* output = feedbackmatrix * input;

			copy(NCH,feedBack, output);

			

			return outDistrib*output;
		}


	};
}
