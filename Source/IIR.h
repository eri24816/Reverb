#pragma once
#include <queue>
#include "Matrix.h"
namespace IIR {

	class Operation {
		
	public:
		int inputDim = 2;
		virtual float* update(float* input) = 0;
	};

	// Multiply vector by scalar
	class Mult : public Operation {
		
	public:
		float multiplier = 0.2;

		float* update(float* input) override {
			for (int i = 0; i < inputDim; i++) {
				input[i] *= multiplier;
			}
			return input;
		}
	};


	// Delay line
	class Delay : public Operation {
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
	class System {
		
		Delay inDelay,fbDelayLine;
		float feedBack[2]{0,0};
	public:
		System():inDelay(2, new int[]{100,100}), fbDelayLine(2, new int[] {4000, 4000 }) {
			Matrix eigenvectors = Matrix(3,3, new float[]{ 1, 5, 2, -2, 1, -4, 9, -10, 2 });
			//eigenvectors.elem = { 1, 5, 2, -2, 1, -4, 9, -10, 2 };
			Matrix eigenvalues = diag(new float[]{ 0.99,0.98,0.97 },3);
			Matrix m = eigenvectors * eigenvalues * ~eigenvectors;
		}

		// PluginProcessor calls this
		float* update(float* input){

			input = inDelay.update(input);

			add(2,input, feedBack);

			float* output = fbDelayLine.update(mult(2,input,0.8f));

			copy(2,feedBack, output);

			return output;
		}
	};
}
