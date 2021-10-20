#pragma once

namespace IIR {

	class Operation {
		
	public:
		int inputSize = 2;
		int outputSize = 2;
		virtual float* update(float* input) = 0;
	};

	class Mult : public Operation {
	public:
		float multiplier = 0.2;

		float* update(float* input) override {
			for (int i = 0; i < inputSize; i++) {
				input[i] *= multiplier;
			}
			return input;
		}
	};

	class System {
		Mult mult;
	public:
		float* update(float* input){
			return mult.update(input);
		}
	};

}
