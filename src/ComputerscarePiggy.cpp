#include "Computerscare.hpp"

#include "dtpulse.hpp"
#include "golyFunctions.hpp"

struct ComputerscarePiggy;


/*
	knob1: number of channels output 1-16
	knob2: algorithm
	knob3: param 1
*/
const std::string PiggyAvailableAlgorithmsArr[5] = {"Linear", "Sigmoid", "Hump", "Sinusoid", "Pseudo-Random"};


//template <const std::string& options>
struct GolyAlgoParamQuantity : ParamQuantity {
	std::string getDisplayValueString() override {
		int val = getValue();
		return PiggyAvailableAlgorithmsArr[val];
	}
};

struct ComputerscarePiggy : ComputerscarePolyModule {
	int counter = 0;
	int numChannels = 16;
	ComputerscareSVGPanel* panelRef;
	Goly goly;
	float currentValues[16] = {0.f};
	std::vector<std::string> availableAlgorithms;

	enum ParamIds {
		ALGORITHM,
		IN_OFFSET,
		IN_SCALE,
		OUT_SCALE,
		OUT_OFFSET,
		POLY_CHANNELS,
		NUM_PARAMS
	};
	enum InputIds {
		CHANNEL_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		POLY_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};


	ComputerscarePiggy()  {

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam<GolyAlgoParamQuantity>(ALGORITHM , 0.f, 4.f, 0.f, "Algorithm");
		configParam(IN_OFFSET, -1.f, 1.f, 0.f, "Channel Center");

		configParam(IN_SCALE, -2.f, 2.f, 1.f, "Channel Spread");

		configParam(OUT_SCALE, -20.f, 20.f, 10.f, "Output Scale");
		configParam(OUT_OFFSET, -10.f, 10.f, 0.f, "Output Offset");
		configParam<AutoParamQuantity>(POLY_CHANNELS, 1.f, 16.f, 16.f, "Poly Channels");

		availableAlgorithms.push_back("Linear");
		availableAlgorithms.push_back("Sigmoid");
		availableAlgorithms.push_back("Hump");
		availableAlgorithms.push_back("Sinusoid");
		availableAlgorithms.push_back("Pseudo-Random");


		goly = Goly();

	}
	void updateCurrents() {
		std::vector<float> golyParams = {params[IN_OFFSET].getValue(), params[IN_SCALE].getValue(), params[OUT_SCALE].getValue(), params[OUT_OFFSET].getValue()};
		goly.invoke((int) std::floor(params[ALGORITHM].getValue()), golyParams, params[POLY_CHANNELS].getValue());
	}
	void setAlgorithm(int algoVal) {
		params[ALGORITHM].setValue(algoVal);
	}
	void checkPoly() override {
		int knobSetting = params[POLY_CHANNELS].getValue();
		polyChannels = knobSetting == 0 ? 16 : knobSetting;
		outputs[POLY_OUTPUT].setChannels(polyChannels);
	}
	void process(const ProcessArgs &args) override {
		ComputerscarePolyModule::checkCounter();
		counter++;
		if (counter > 13) {
			counter = 0;
			updateCurrents();
			//numChannels=(int) (std::floor(params[POLY_CHANNELS].getValue()));
		}


		//outputs[POLY_OUTPUT].setChannels(numChannels);
		for (int i = 0; i < polyChannels; i++) {
			outputs[POLY_OUTPUT].setVoltage(goly.currentValues[i], i);
		}
	}

};
struct setAlgoItem : MenuItem
{
	ComputerscarePiggy *penerator;
	int mySetVal;
	setAlgoItem(int setVal)
	{
		mySetVal = setVal;
	}

	void onAction(const event::Action &e) override
	{
		penerator->setAlgorithm(mySetVal);
	}
	void step() override {
		rightText = CHECKMARK(penerator->params[ComputerscarePiggy::ALGORITHM].getValue() == mySetVal);
		MenuItem::step();
	}
};

/*struct SetAllItem : MenuItem {
	ComputerscareRolyPouter *pouter;

	Menu *createChildMenu() override {
		Menu *menu = new Menu;
		for (int i = 1; i < 17; i++) {
			ssmi *menuItem = new ssmi(i);
			menuItem->text = "Set all to ch. " + std::to_string(i);
			menuItem->pouter = pouter;
			menu->addChild(menuItem);
		}
		return menu;
	}

};*/
struct AlgorithmChildMenu : MenuItem {
	ComputerscarePiggy *penerator;

	Menu *createChildMenu() override {
		Menu *menu = new Menu;
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Select an Algorithm... NOW"));

		for (unsigned int i = 0; i < penerator->availableAlgorithms.size(); i++) {
			setAlgoItem *menuItem = new setAlgoItem(i);
			//ParamSettingItem *menuItem = new ParamSettingItem(i,ComputerscarePiggy::ALGORITHM);

			menuItem->text = penerator->availableAlgorithms[i];
			menuItem->penerator = penerator;
			menu->addChild(menuItem);
		}

		return menu;
	}

};
struct PeneratorDisplay : TransparentWidget {
	ComputerscarePiggy *module;

	PeneratorDisplay() {

	}
	void draw(const DrawArgs &args) override {
		float valsToDraw[16] = {1.f};
		int ch = 16;
		if (module) {
			ch = module->polyChannels;
			for (int i = 0; i < ch; i++) {
				valsToDraw[i] = module->goly.currentValues[i];
			}
		}
		else {
			for (int i = 0; i < ch; i++) {
				valsToDraw[i] = random::uniform() * 10;
			}
		}
		DrawHelper draw = DrawHelper(args.vg);
		Points pts = Points();

		nvgTranslate(args.vg, box.size.x / 2, box.size.y / 2 + 5);
		pts.linear(ch, Vec(0, -box.size.y / 2), Vec(0, 150));
		std::vector<Vec> polyVals;
		std::vector<NVGcolor> colors;
		std::vector<Vec> thicknesses;

		for (int i = 0; i < 16; i++) {
			polyVals.push_back(Vec(valsToDraw[i] * 2, 0.f));
			colors.push_back(draw.sincolor(0, {1, 1, 0}));

			thicknesses.push_back(Vec(160 / (1 + ch), 0));
		}
		draw.drawLines(pts.get(), polyVals, colors, thicknesses);
	}
};
struct ComputerscarePiggyWidget : ModuleWidget {
	ComputerscarePiggyWidget(ComputerscarePiggy *module) {

		setModule(module);
		//setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComputerscarePiggyPanel.svg")));
		box.size = Vec(150, 380);
		{
			ComputerscareSVGPanel *panel = new ComputerscareSVGPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/computerscare-piggy-tilted.svg")));
			addChild(panel);
		}

		/*PeneratorDisplay *display = new PeneratorDisplay();
		display->module = module;
		display->box.pos = Vec(0, 120);
		display->box.size = Vec(box.size.x, 400);
		addChild(display);

		float xx;
		float yy;
		addLabeledKnob<ScrambleSnapKnob>("Algo", 4, 324, module, ComputerscarePiggy::ALGORITHM, 0, 0, true);
		addLabeledKnob<SmoothKnob>("center", 28, 80, module, ComputerscarePiggy::IN_OFFSET, 0, 0);
		addLabeledKnob<SmallKnob>("spread", 5, 86, module, ComputerscarePiggy::IN_SCALE, 0, 0);
		addLabeledKnob<SmallKnob>("scale", 33, 290, module, ComputerscarePiggy::OUT_SCALE, 0, 0);
		addLabeledKnob<SmoothKnob>("offset", 2, 284, module, ComputerscarePiggy::OUT_OFFSET, 0, 0);

		//addLabeledKnob("ch out",5,90,module,ComputerscarePiggy::POLY_CHANNELS,-2,0);

		channelWidget = new PolyOutputChannelsWidget(Vec(28, 309), module, ComputerscarePiggy::POLY_CHANNELS);
		addChild(channelWidget);

		addOutput(createOutput<InPort>(Vec(28, 329), module, ComputerscarePiggy::POLY_OUTPUT));*/

	}
	void appendContextMenu(Menu* menu) override {
		ComputerscarePiggy* penerator = dynamic_cast<ComputerscarePiggy*>(this->module);

		menu->addChild(new MenuEntry);


		AlgorithmChildMenu *algoMenu = new AlgorithmChildMenu();
		algoMenu->text = "Algorithm";
		algoMenu->rightText = RIGHT_ARROW;
		algoMenu->penerator = penerator;
		menu->addChild(algoMenu);
	}


	template <typename BASE>
	void addLabeledKnob(std::string label, int x, int y, ComputerscarePiggy *module, int paramIndex, float labelDx, float labelDy, bool snap = false) {

		smallLetterDisplay = new SmallLetterDisplay();
		smallLetterDisplay->box.size = Vec(5, 10);
		smallLetterDisplay->fontSize = 14;
		smallLetterDisplay->value = label;
		smallLetterDisplay->textAlign = 1;

		if (snap) {
			addParam(createParam<BASE>(Vec(x, y), module, paramIndex));
		}
		else {
			addParam(createParam<BASE>(Vec(x, y), module, paramIndex));

		}
		smallLetterDisplay->box.pos = Vec(x + labelDx, y - 12 + labelDy);


		//addChild(smallLetterDisplay);

	}
	PolyOutputChannelsWidget* channelWidget;

	SmallLetterDisplay* smallLetterDisplay;
};

Model *modelComputerscarePiggy = createModel<ComputerscarePiggy, ComputerscarePiggyWidget>("computerscare-piggy");
