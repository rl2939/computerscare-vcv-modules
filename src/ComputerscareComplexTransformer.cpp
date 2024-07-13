#include "Computerscare.hpp"
#include "complex/ComplexWidgets.hpp"

#include <array>

struct ComputerscareComplexTransformer;




struct ComputerscareComplexTransformer : ComputerscareComplexBase {
	ComputerscareSVGPanel* panelRef;
	enum ParamIds {

		COMPOLY_CHANNELS,
		
		SCALE_VAL_AB,
		SCALE_TRIM_AB = SCALE_VAL_AB+2,

		OFFSET_VAL_AB = SCALE_TRIM_AB+2,
		OFFSET_TRIM_AB = OFFSET_VAL_AB+2,
		
		MAIN_INPUT_MODE,
		MAIN_SCALE_INPUT_MODE,
		MAIN_OFFSET_INPUT_MODE,
		MAIN_OUTPUT_MODE,
		NUM_PARAMS
	};
	enum InputIds {
		MAIN_INPUT,
		SCALE_INPUT=MAIN_INPUT+2,
		OFFSET_INPUT=SCALE_INPUT+2,
		NUM_INPUTS=OFFSET_INPUT+2,
	};
	enum OutputIds {
		COMPOLY_MAIN_OUT_A,
		COMPOLY_MAIN_OUT_B,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};




	ComputerscareComplexTransformer()  {

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);


		configParam(SCALE_VAL_AB    , -10.f, 10.f, 0.f, "Channel " );
		configParam(SCALE_VAL_AB + 1, -10.f, 10.f, 0.f, "Channel ");
		getParamQuantity(SCALE_VAL_AB)->randomizeEnabled = false;
		getParamQuantity(SCALE_VAL_AB+1)->randomizeEnabled = false;

		configParam(OFFSET_VAL_AB    , -10.f, 10.f, 0.f, "Channel " );
		configParam(OFFSET_VAL_AB + 1, -10.f, 10.f, 0.f, "Channel ");
		getParamQuantity(OFFSET_VAL_AB)->randomizeEnabled = false;
		getParamQuantity(OFFSET_VAL_AB+1)->randomizeEnabled = false;


		configParam<AutoParamQuantity>(COMPOLY_CHANNELS, 0.f, 16.f, 0.f, "Compoly Output Channels");
		getParamQuantity(COMPOLY_CHANNELS)->randomizeEnabled = false;
		getParamQuantity(COMPOLY_CHANNELS)->resetEnabled = false;

		configParam<cpx::CompolyModeParam>(MAIN_OUTPUT_MODE,0.f,3.f,0.f,"Main Output Mode");
		configParam(MAIN_INPUT_MODE,0.f,3.f,0.f);
		configParam(MAIN_SCALE_INPUT_MODE,0.f,3.f,0.f);
		configParam(MAIN_OFFSET_INPUT_MODE,0.f,3.f,0.f);

		configOutput<cpx::CompolyPortInfo<MAIN_OUTPUT_MODE,0>>(COMPOLY_MAIN_OUT_A, "f(z)");
    configOutput<cpx::CompolyPortInfo<MAIN_OUTPUT_MODE,1>>(COMPOLY_MAIN_OUT_B, "f(z)");

	}
	void process(const ProcessArgs &args) override {
		ComputerscarePolyModule::checkCounter();
		
		float offsetX = params[OFFSET_VAL_AB].getValue();
		float offsetY = params[OFFSET_VAL_AB+1].getValue();

		float scaleX = params[SCALE_VAL_AB].getValue();
		float scaleY = params[SCALE_VAL_AB+1].getValue();

		math::Vec scaleRect = Vec(scaleX,scaleY);




		for (int i = 0; i < polyChannels; i++) {
			if(i < 8) {

	

				outputs[COMPOLY_MAIN_OUT_A].setVoltage( offsetX, 2*i);
				outputs[COMPOLY_MAIN_OUT_A].setVoltage( offsetY, 2*i+1);
			} 
			
			//outputs[POLY_OUTPUT].setVoltage(params[KNOB + i].getValue()*trim + offset, i);
		}
	}
	void checkPoly() override {
		polyChannels = params[COMPOLY_CHANNELS].getValue();
		if (polyChannels == 0) {
			//polyChannels = 16;
		//	params[POLY_CHANNELS].setValue(16);
		}

		//outputs[COMPOLY_MAIN_OUT_A].setChannels(polyChannels);
	}
	json_t *dataToJson() override {
    json_t *rootJ = json_object();
    return rootJ;
  }

    void dataFromJson(json_t *rootJ) override {
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

struct DisableableSmoothKnob : RoundKnob {
	std::shared_ptr<Svg> enabledSvg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/computerscare-medium-small-knob.svg"));
	std::shared_ptr<Svg> disabledSvg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/computerscare-medium-small-knob-disabled.svg"));

	int channel = 0;
	bool disabled = false;
	ComputerscarePolyModule *module;

	DisableableSmoothKnob() {
		setSvg(enabledSvg);
		shadow->box.size = math::Vec(0, 0);
		shadow->opacity = 0.f;
	}
	void step() override {
		if (module) {
			bool candidate = channel > module->polyChannels - 1;
			if (disabled != candidate) {
				setSvg(candidate ? disabledSvg : enabledSvg);
				onChange(*(new event::Change()));
				fb->dirty = true;
				disabled = candidate;
			}
		}
		else {
		}
		RoundKnob::step();
	}
};

struct ComputerscareComplexTransformerWidget : ModuleWidget {
	ComputerscareComplexTransformerWidget(ComputerscareComplexTransformer *module) {

		setModule(module);
		//setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComputerscareComplexTransformerPanel.svg")));
		box.size = Vec(8 * 15, 380);
		{
			ComputerscareSVGPanel *panel = new ComputerscareSVGPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComputerscareComplexTransformerPanel.svg")));
			addChild(panel);
		}
		channelWidget = new PolyOutputChannelsWidget(Vec(92, 4), module, ComputerscareComplexTransformer::COMPOLY_CHANNELS);
	

		//addOutput(createOutput<PointingUpPentagonPort>(Vec(30, 22), module, ComputerscareComplexTransformer::POLY_OUTPUT));

		addChild(channelWidget);





    cpx::ComplexXY* offsetValAB = new cpx::ComplexXY(module,ComputerscareComplexTransformer::OFFSET_VAL_AB);
    offsetValAB->box.size=Vec(25,25);
    offsetValAB->box.pos=Vec(32, 27);
    addChild(offsetValAB);

    cpx::ComplexXY* scaleValAB = new cpx::ComplexXY(module,ComputerscareComplexTransformer::SCALE_VAL_AB);
    scaleValAB->box.size=Vec(25,25);
    scaleValAB->box.pos=Vec(5, 27);
    addChild(scaleValAB);


    cpx::CompolyPortsWidget* mainInput = new cpx::CompolyPortsWidget(Vec(50, 120),module,ComputerscareComplexTransformer::MAIN_INPUT,ComputerscareComplexTransformer::MAIN_INPUT_MODE,1.f,false);
    addChild(mainInput);

		//addParam(createParam<NoRandomSmallKnob>(Vec(11, 54), module, ComputerscareComplexTransformer::GLOBAL_SCALE));
		//addParam(createParam<NoRandomMediumSmallKnob>(Vec(32, 57), module, ComputerscareComplexTransformer::GLOBAL_OFFSET));

    cpx::CompolyPortsWidget* mainOutput = new cpx::CompolyPortsWidget(Vec(50, 320),module,ComputerscareComplexTransformer::COMPOLY_MAIN_OUT_A,ComputerscareComplexTransformer::MAIN_OUTPUT_MODE);
    addChild(mainOutput);


	}

	PolyOutputChannelsWidget* channelWidget;
	PolyChannelsDisplay* channelDisplay;
	DisableableSmoothKnob* fader;
	SmallLetterDisplay* smallLetterDisplay;
};

Model *modelComputerscareComplexTransformer = createModel<ComputerscareComplexTransformer, ComputerscareComplexTransformerWidget>("computerscare-complex-transformer");
