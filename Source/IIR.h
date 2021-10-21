#pragma once
#include <queue>
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


	// Multiply vector by matrix
	class Matrix : public Operation {

	};

	// Delay line
	class Delay : public Operation {
	public:
		std::queue<float> queues[2];
		Delay(float inputDim_,const int numDelaySamples[]){
			DBG(numDelaySamples[0]);
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
		
		Delay delay;
		float feedBack[2]{0,0};
	public:
		System():delay(2, new int[]{4000,3700 }) {
		}

		// PluginProcessor calls this
		float* update(float* input){

			add(2,input, feedBack);

			float* output = delay.update(mult(2,input,0.8f));

			copy(2,feedBack, output);

			return output;
		}
	};

}
