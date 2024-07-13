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

   void setOutputVoltages(int outIndex,int outMode,int compChannelIndex,float x, float y, float r, float theta) {
        int outputBlock = compChannelIndex > 7 ? 1 : 0;
        if(outMode==0) {
            //interleaved rectangular
            outputs[outIndex+outputBlock].setVoltage(x,(compChannelIndex*2) % 16);
            outputs[outIndex+outputBlock].setVoltage(y,(compChannelIndex*2+1) % 16);
        } else if(outMode==1) {
            //interleaved polar
            outputs[outIndex+outputBlock].setVoltage(r,(compChannelIndex*2) % 16);
            outputs[outIndex+outputBlock].setVoltage(theta,(compChannelIndex*2+1) % 16);
        } else if(outMode==2) {
            //separated rectangular
            outputs[outIndex].setVoltage(x,compChannelIndex);
            outputs[outIndex+1].setVoltage(y,compChannelIndex);
        } else if(outMode==3) {
            //separated polar
            outputs[outIndex].setVoltage(r,compChannelIndex);
            outputs[outIndex+1].setVoltage(theta,compChannelIndex);
        }
    }

    void setOutputChannels(int outIndex,int outMode,int compolyChannels) {
        if(outMode==0 || outMode ==1) {
            //interleaved
            int numTotalPolyChannels = compolyChannels*2;
            int numChannels1 = numTotalPolyChannels >= 16 ? 16 : numTotalPolyChannels;
            int numChannels2 = numTotalPolyChannels >= 16 ? numTotalPolyChannels-16 : 0;

            outputs[outIndex+0].setChannels(numChannels1);
            outputs[outIndex+1].setChannels(numChannels2);
        } else {
            outputs[outIndex+0].setChannels(compolyChannels);
            outputs[outIndex+1].setChannels(compolyChannels);
        }
    }

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
	std::vector<int> getInputChannelIndices(int outputIndex,int wrapMode, std::vector<int> channelCounts) {
        std::vector<int> output;
        for(int i = 0; i < channelCounts.size(); i++) {
        	int myInputChannelIndex=0;;
        	int myNumChannels = channelCounts[i];
      	    if(wrapMode == WRAP_NORMAL) {
      	    	 /*
	                If monophonic, copy ch1 to all
	                Otherwise use the poly channels
	            */
      	    	if(myNumChannels == 1) {
      	    		myInputChannelIndex = 0;
      	    	} else {
      	    		myInputChannelIndex = outputIndex;
      	    	}
		        } else if(wrapMode == WRAP_CYCLE) {
		        	// Cycle through the poly channels
		        	myInputChannelIndex = outputIndex % myNumChannels;
		        } else if(wrapMode == WRAP_MINIMAL) {
		          // Do not copy channel 1 if monophonic
		        	myInputChannelIndex = outputIndex;
		        } else if(wrapMode == WRAP_STALL) {
		          // Go up to the maximum channel, and then use the final value for the rest
		        	myInputChannelIndex = outputIndex>myNumChannels-1 ? myNumChannels-1 : outputIndex;
		        }
		        //reverse
		        //pingpong
		        //random shuffled

		        output.push_back(myInputChannelIndex);
        }
        return output;
    }

};
