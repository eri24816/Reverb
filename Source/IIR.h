#pragma once
#include <queue>
#include "Matrix.h"
namespace IIR {

	class Module {
	public:
		int inputDim = 2;
		virtual float* update(float* input) = 0;
	};

	// Delay line
	class Delay : public Module {
	public:
		std::queue<float> queues[8]; // Max. 8 delay lines
		Delay(float inputDim_,const int numDelaySamples[]){
			inputDim = inputDim_;
			for (int i = 0; i < inputDim; i++) {
				for(int j=0;j< numDelaySamples[i];j++)
					queues[i].push(0);
			}
			
		}
		float* update(float* input) {
			for (int i = 0; i < inputDim; i++) {
				queues[i].push(input[i]);
				input[i] = queues[i].front();
				queues[i].pop();
			}
			return input;
		}
	};

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

#define NCH 4
	// IIR system that simulates reverb
	class Reverb : Module {
		
		Delay inDelay,fbDelayLine;
		float feedBack[NCH]{0};

		Matrix  distrib,outDistrib;
		Matrix feedbackmatrix;
	public:
		Reverb() :
			inDelay(2, new int[] {100, 120}),
			fbDelayLine(NCH, new int[] {4050, 8402 ,5860,3075,4046,6013,7086,5000}),
			distrib(NCH, 2, new float[] { 0.094, 0.142, -0.189, 0.124, 0.02, 0.161, 0.026, 0.023, 0.079, -0.068, -0.134, 0.286, 0.161, -0.216, 0.086, -0.043 }),
			outDistrib(2, NCH, new float[] {-0.05, -0.207, 0.18, -0.172, -0.221, -0.287, 0.077, -0.095, -0.252, 0.29, -0.163, 0.064, -0.272, -0.135, 0.211, 0.085})
		{
			Matrix eigenvectors = Matrix(NCH, NCH, new float[]{ -0.342, 1.19, -0.103, 0.196, 0.196, 1.07, 0.887, 1.075, -0.028, 0.419, 1.299, 0.278, 0.491, 0.581, -0.374, 0.853, -0.063, 0.12, -0.059, 0.684, -0.031, -0.324, 1.366, -0.432, 0.033, -0.3, 1.142, 0.744, -0.226, -0.009, 1.475, 0.95, 0.227, 1.462, 0.207, 0.989, 0.501, 1.215, -0.215, 0.865, 1.013, 0.531, 1.078, 0.448, 1.018, 1.347, 0.887, 0.302, 1.458, 1.034, 0.902, 0.084, 0.795, 0.07, -0.245, -0.182, 1.368, -0.102, 0.352, 0.417, 0.284, 1.093, 0.267, 0.046 });
			Matrix eigenvalues = diag(new float[]{0.883, 0.834, 0.702, 0.83, 0.804, 0.842, 0.913, 0.898 }, NCH);
			feedbackmatrix = eigenvectors * eigenvalues * ~eigenvectors;
		}

		// PluginProcessor calls this
		float* update(float* input) override{


			input = distrib * inDelay.update(input);

			add(NCH,input, fbDelayLine.update(feedBack));

			float* output = feedbackmatrix * input;

			copy(NCH,feedBack, output);

			return outDistrib*output;
		}
	};
}
