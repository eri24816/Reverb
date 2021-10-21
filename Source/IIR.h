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

	// IIR system that simulates reverb
	class Reverb : Module {
		Delay inDelay,fbDelayLine;
		float feedBack[3]{0,0,0};

		Matrix  distrib,outDistrib;
		Matrix feedbackmatrix;
	public:
		Reverb() :
			inDelay(2, new int[] {100, 120}),
			fbDelayLine(3, new int[] {2000, 4000 ,3000}),
			distrib(3, 2, new float[] { 0.8, -0.3, 0.4, 0.3, -0.8, 0.3 }),
			outDistrib(2,3, new float[] { -0.8, 0.3, 0.4, 0.4, -0.8, 0.3 })
		{
			Matrix eigenvectors = Matrix(3,3, new float[]{ 1, 5, 2, -2, 1, -4, 9, -10, 2 });
			Matrix eigenvalues = diag(new float[]{ 0.9,0.93,0.8 },3);
			feedbackmatrix = eigenvectors * eigenvalues * ~eigenvectors;
		}

		// PluginProcessor calls this
		float* update(float* input) override{
			//DBG(distrib[0][0]);

			input = distrib * inDelay.update(input);

			add(3,input, fbDelayLine.update(feedBack));

			float* output = fbDelayLine.update(feedbackmatrix*input);

			copy(3,feedBack, output);

			return outDistrib*output;
		}
	};
}
