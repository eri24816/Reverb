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
	public:
		std::queue<float> queues[8]; // Max. 8 delay lines
		Delay(float inputDim,const int numDelaySamples[]):Module(inputDim){
			for (int i = 0; i < inputDim; i++) {
				for(int j=0;j< numDelaySamples[i];j++)
					queues[i].push(0);
			}
			
		}
		float* update(float* input) override{
			for (int i = 0; i < inputDim; i++) {
				queues[i].push(input[i]);
				input[i] = queues[i].front();
				queues[i].pop();
			}
			return input;
		}
	};

	class Lowpass :Module {
		Delay delay1, delay2, delay3, delay4;
		float temp[10], temp2[10];
	public:
		Lowpass(int inputDim) :Module(inputDim), 
			delay1(inputDim, new int[inputDim] { 1,1, 1, 1, 1, 1, 1, 1 }),
			delay2(inputDim, new int[inputDim] { 2, 2, 2, 2, 2, 2, 2, 2}),
			delay3(inputDim, new int[inputDim] { 3, 3, 3, 3, 3, 3, 3, 3 }),
			delay4(inputDim, new int[inputDim] { 4, 4, 4, 4, 4, 4, 4, 4})
		{}
		float* update(float* input) override {
			// Because Delay.update uses its input array to store it output, so copy the input in advance for adding later.
			copy(inputDim, temp2, input);
			copy(inputDim, temp, temp2);

			add(inputDim, input, delay1.update(temp));

			copy(inputDim, temp, temp2);
			add(inputDim, input, delay2.update(temp));

			copy(inputDim, temp, temp2);
			add(inputDim, input, delay3.update(temp));

			copy(inputDim, temp, temp2);
			add(inputDim, input, delay4.update(temp));

			return mult(inputDim, input, 0.2);
		}
	};



#define NCH 8
	// IIR system that simulates reverb
	class Reverb : Module {
		
		Delay inDelay,fbDelayLine;
		Lowpass delayFilters;
		float feedBack[NCH]{0};

		Matrix  distrib,outDistrib;
		Matrix feedbackmatrix;
	public:
		Reverb() :
			Module(2),
			inDelay(inputDim, new int[] {100, 120}),
			delayFilters(NCH),
			fbDelayLine(NCH, new int[] {123, 1402 ,3860,375,3046,213,586,100}),
			distrib(NCH, inputDim, new float[] { 0.094, 0.142, -0.189, 0.124, 0.02, 0.161, 0.026, 0.023, 0.079, -0.068, -0.134, 0.286, 0.161, -0.216, 0.086, -0.043 }),
			outDistrib(inputDim, NCH, new float[] {-0.05, -0.207, 0.18, -0.172, -0.221, -0.287, 0.077, -0.095, -0.252, 0.29, -0.163, 0.064, -0.272, -0.135, 0.211, 0.085})
		{
			Matrix eigenvectors = Matrix(NCH, NCH, new float[]{ -2.542, 1.19, -0.103, 0.196, 0.196, 1.07, 0.887, 1.075, -0.028, 0.419, 1.299, 0.278, 0.491, 0.581, -0.374, 0.853, -0.063, 0.12, -0.059, 0.684, -0.031, -0.324, 1.366, -0.432, 0.033, -0.3, 1.142, 0.744, -0.226, -0.009, 1.475, 0.95, 0.227, 1.462, 0.207, 0.989, 0.501, 1.215, -0.215, 0.865, 1.013, 0.531, 1.078, 0.448, 1.018, 1.347, 0.887, 0.302, 1.458, 1.034, 0.902, 0.084, 0.795, 0.07, -0.245, -0.182, 1.368, -0.102, 0.352, 0.417, 0.284, 1.093, 0.267, 0.046 });
			Matrix eigenvalues = diag(new float[]{0.912, 0.913, 0.915, 0.916, 0.921, 0.91, 0.918, 0.919 }, NCH);
			feedbackmatrix = eigenvectors * eigenvalues * ~eigenvectors;
		}

		// PluginProcessor calls this
		float* update(float* input) override{


			input = distrib * inDelay.update(input);

			delayFilters.update(feedBack);

			add(NCH,input, fbDelayLine.update(feedBack));

			float* output = feedbackmatrix * input;

			copy(NCH,feedBack, output);

			return outDistrib*output;
		}
	};
}
