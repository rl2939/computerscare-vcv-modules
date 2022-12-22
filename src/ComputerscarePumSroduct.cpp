#include "Computerscare.hpp"
#include "ComputerscarePolyModule.hpp"

struct ComputerscarePumSroduct;

const int numKnobs = 16;
const int numToggles = 16;

const int numOutputs = 8;

const int numSteps = 8;

const int numDividers = 16;


struct ComputerscarePumSroduct : ComputerscarePolyModule {
	ComputerscareSVGPanel* panelRef;
	enum ParamIds {
		KNOB,
		TOGGLES = KNOB + numKnobs,
		POLY_CHANNELS = TOGGLES + numToggles,
		GLOBAL_SCALE,
		GLOBAL_OFFSET,
		GLOBAL_PATTERN_KNOB,
		GLOBAL_WEIRDNESS_KNOB,
		PATTERN_KNOB,
		WEIRDNESS_KNOB = PATTERN_KNOB + numOutputs,
		NUM_PARAMS = WEIRDNESS_KNOB + numOutputs

	};
	enum InputIds {
		CLOCK_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUTPUT,
		NUM_OUTPUTS = CV_OUTPUT + numOutputs
	};
	enum LightIds {
		NUM_LIGHTS
	};

	dsp::PulseGenerator clockPulse;
	dsp::PulseGenerator resetPulse;

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;

	dsp::ClockDivider divider[numDividers];

	int index = 0;

	int flips[numDividers] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	float coefs[numDividers * numOutputs];


	ComputerscarePumSroduct()  {

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		for (int i = 0; i < numOutputs; i++) {
			configOutput(CV_OUTPUT + i, "CV " + std::to_string(i + 1));
			configParam(PATTERN_KNOB + i, 0.f, 10.f, 0.f, "Channel " + std::to_string(i + 1));
			for (int j = 0; j < numDividers; j++) {
				coefs[i * numDividers + j] = i * numDividers + j;
			}
		}


		configParam(POLY_CHANNELS, 1.f, 16.f, 16.f, "Poly Channels");
		configParam(GLOBAL_SCALE, -2.f, 2.f, 1.f, "Scale");
		configParam(GLOBAL_OFFSET, -10.f, 10.f, 0.f, "Offset", " volts");

		getParamQuantity(POLY_CHANNELS)->randomizeEnabled = false;
		getParamQuantity(POLY_CHANNELS)->resetEnabled = false;
		getParamQuantity(GLOBAL_SCALE)->randomizeEnabled = false;
		getParamQuantity(GLOBAL_OFFSET)->randomizeEnabled = false;



		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");

		setDividers();
		identityCoefs();

	}

	void randomizeCoefs() {
		for (int i = 0; i < numOutputs; i++) {
			for (int j = 0; j < numDividers; j++) {
				if ( random::uniform() > 0.6) {
					coefs[i * numDividers + j] = random::uniform();
				}

			}
		}
	}
	void wiggleCoefs() {
		for (int i = 0; i < numOutputs; i++) {
			for (int j = 0; j < numDividers; j++) {
				if ( random::uniform() > 0.6) {
					coefs[i * numDividers + j] += 0.25 - random::uniform() / 2;
				}

			}
		}
	}
	void identityCoefs() {
		for (int i = 0; i < numOutputs; i++) {
			for (int j = 0; j < numDividers; j++) {
				coefs[i * numDividers + j] = i == j ? 1.f : 0.f;
			}
		}
	}

	void setDividers() {
		int p2 = 1;
		for (int i = 0; i < numDividers; i++) {
			divider[i].setDivision(p2);
			//p2 += 1;
			p2 *= 2;
		}
	}
	float sumProduct(int ch) {
		float out = 0;
		int offset = ch * numDividers;
		float divisor = 0.f;
		for (int i = 0; i < numDividers; i++) {
			int dex = offset + i;
			if (coefs[dex] > 0.1) {
				divisor += 1.f;
				out += coefs[dex] * flips[i];
			}
		}
		return out / (divisor > 0.f ? divisor : 1);
	}
	void process(const ProcessArgs &args) override {
		ComputerscarePolyModule::checkCounter();

		// Reset
		if (/*resetButtonTrigger.process(params[RESET_PARAM].getValue()) |*/ resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f)) {
			resetPulse.trigger(1e-3f);
			// Reset step index
			index = 0;
		}

		bool resetGate = resetPulse.process(args.sampleTime);

		bool clock = false;
		bool clockGate = false;

		if (inputs[CLOCK_INPUT].isConnected()) {
			// External clock
			// Ignore clock while reset pulse is high
			if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f) && !resetGate) {
				clock = true;
			}
			clockGate = clockTrigger.isHigh();
		}

		if (clock) {
			clockPulse.trigger(1e-3f);
			index++;
			if (index >= numSteps) {
				index = 0;
			}

			for (int i = 0; i < numDividers; i++) {

				if (divider[i].process()) {
					flips[i] += 1;
					flips[i] %= 2;
				}
				else {

				}
			}
		}

		for (int i = 0; i < numOutputs; i++) {
			outputs[CV_OUTPUT + i].setVoltage(math::clamp(sumProduct(i) * 10.f , -10.f, 10.f));
		}
	}
	void checkPoly() override {
		polyChannels = params[POLY_CHANNELS].getValue();
		if (polyChannels == 0) {
			polyChannels = 16;
			params[POLY_CHANNELS].setValue(16);
		}
		outputs[CV_OUTPUT].setChannels(1);
	}
};

struct NoRandomSmallKnob : SmallKnob {
	NoRandomSmallKnob() {
		SmallKnob();
	};
};
struct NoRandomMediumSmallKnob : RoundKnob {
	std::shared_ptr<Svg> enabledSvg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/computerscare-medium-small-knob.svg"));

	NoRandomMediumSmallKnob() {
		setSvg(enabledSvg);
		RoundKnob();
	};
};


struct ComputerscarePumSroductWidget : ModuleWidget {
	ComputerscarePumSroductWidget(ComputerscarePumSroduct *module) {

		setModule(module);
		//setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComputerscarePumSroductPanel.svg")));
		box.size = Vec(4 * 15, 380);
		{
			ComputerscareSVGPanel *panel = new ComputerscareSVGPanel();
			panel->box.size = box.size;
			//panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComputerscareKnolyPobsPanel.svg")));
			//addChild(panel);
		}
		channelWidget = new PolyOutputChannelsWidget(Vec(1, 24), module, ComputerscarePumSroduct::POLY_CHANNELS);
		//addOutput(createOutput<PointingUpPentagonPort>(Vec(30, 22), module, ComputerscarePumSroduct::POLY_OUTPUT));

		//addInput()

		addInput(createInput<InPort>(Vec(9, 78), module, ComputerscarePumSroduct::CLOCK_INPUT));
		addInput(createInput<InPort>(Vec(29, 78), module, ComputerscarePumSroduct::RESET_INPUT));




		addChild(channelWidget);



		addParam(createParam<NoRandomSmallKnob>(Vec(11, 54), module, ComputerscarePumSroduct::GLOBAL_SCALE));
		addParam(createParam<NoRandomMediumSmallKnob>(Vec(32, 57), module, ComputerscarePumSroduct::GLOBAL_OFFSET));


		float xx;
		float yy;
		float xInitial = 11.f;
		float yInitial = 186;
		float ySpacing =  24;
		float yRightColumnOffset = 14.3 / 8;
		for (int i = 0; i < numOutputs; i++) {
			xx = xInitial + 24.3 * (i - i % 8) / 8;
			yy = yInitial + ySpacing * (i % 8) +  yRightColumnOffset * (i - i % 8) ;
			addParam(createParam<NoRandomSmallKnob>(Vec(xx, yy), module, ComputerscarePumSroduct::PATTERN_KNOB + i));

			addOutput(createOutput<PointingUpPentagonPort>(Vec(xx + 20, yy), module, ComputerscarePumSroduct::CV_OUTPUT + i));




			//addLabeledKnob(std::to_string(i + 1), xx, yy, module, i, (i - i % 8) * 1.2 - 3 + (i == 8 ? 5 : 0), 2);
		}

	}

	PolyOutputChannelsWidget* channelWidget;
	PolyChannelsDisplay* channelDisplay;
	//DisableableSmoothKnob* fader;
	SmallLetterDisplay* smallLetterDisplay;
};

Model *modelComputerscarePumSroduct = createModel<ComputerscarePumSroduct, ComputerscarePumSroductWidget>("computerscare-pum-sroduct");