/*
	ComputerscareComplexBase

	Compluterxare

	Extends Module with a polyChannels member variable,
	and a polyphony channels widget that is "bound" to the
	the module instance's value

*/


#pragma once

using namespace rack;

struct ComputerscareComplexBase;


struct ComputerscareComplexBase : ComputerscareMenuParamModule {
	enum wrapModes {
        WRAP_NORMAL,
        WRAP_CYCLE,
        WRAP_MINIMAL,
        WRAP_STALL
  };

	int calcOutputCompolyphony(int knobSetting,std::vector<int> inputCompolyphonyChannels) {
		int numInputsToConsider = inputCompolyphonyChannels.size();

		int outputCompolyphony;

		int maxOfInputsCompolyphony = 0;

		for(int i = 0; i < numInputsToConsider; i++) {
			maxOfInputsCompolyphony= std::max(maxOfInputsCompolyphony,inputCompolyphonyChannels[i]);
		}

		//automatic, use max of input channels compolyphony
		if(knobSetting == 0) {
			if(maxOfInputsCompolyphony == 0) {
				outputCompolyphony = 1;
			} else {
				outputCompolyphony = maxOfInputsCompolyphony;
			}
		} else {
			outputCompolyphony = knobSetting;
		}

		return outputCompolyphony;
	}

};
