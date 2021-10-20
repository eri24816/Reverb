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

	// IIR system that simulates reverb
	class System {
		Mult mult;
		Delay delay;
		float feedBack[2]{0,0};
	public:
		System():delay(2, new int[]{ 4000,4000 }) {
			mult.multiplier = 0.9f;
		}

		// PluginProcessor calls this
		float* update(float* input){

			input[0] += feedBack[0];
			input[1] += feedBack[1];

			float* output = delay.update(mult.update(input));

			feedBack[0] = output[0];
			feedBack[1] = output[1];

			return output;
		}
	};

}
