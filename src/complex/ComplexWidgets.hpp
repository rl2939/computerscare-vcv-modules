#pragma once

using namespace rack;


namespace cpx {

	struct ComplexOutport : ComputerscareSvgPort {
		ComplexOutport() {
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewR.svg")));
			/*if(TYPE==1) {
				setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantL.svg")));
			} else if(TYPE==2) {
				setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantR.svg")));
			}
			else if(TYPE==3) {
				setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewL.svg")));
			}
			else if(TYPE==4) {
				setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewR.svg")));
			}*/
			//background->wrap();
		}
	};

	struct CompolyTypeLabelSwitch : app::SvgSwitch {
		widget::TransformWidget* tw;
		CompolyTypeLabelSwitch() {
			shadow->opacity = 0.f;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-labels/xy.svg")));
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-labels/rtheta.svg")));

		}
	};

	struct HalfSize : TransformWidget {
		HalfSize(Vec pos,float scaleProportion) {
			box.pos = pos;
			scale(scaleProportion);
			TransformWidget();
		}
	};




	struct CompolyOutWidget : Widget {
		ComplexOutport* port;
		int paramID;
		ComputerscareComplexBase* module;

		CompolyOutWidget(math::Vec pos,ComputerscareComplexBase *cModule, int portID,int compolyTypeParamID) {

			module=cModule;
			paramID = compolyTypeParamID;
			CompolyTypeLabelSwitch* compolyLabel = createParam<CompolyTypeLabelSwitch>(Vec(0,0),cModule,compolyTypeParamID);

			TransformWidget* tw = new TransformWidget();
			tw->box.pos = pos.minus(Vec(20,0));
			tw->scale(0.5);

			tw->addChild(compolyLabel);

			//ComplexOutport* square = new ComplexOutport(1);
			port = createOutput<ComplexOutport>(pos,cModule,portID);

			addChild(tw);
			addChild(port);
			// addParam(createParam<SmallKnob>(Vec(rightInputX, rectInSectionY).plus(trimRelPos), module, ComputerscareNomplexPumbers::IMAGINARY_INPUT_TRIM));

        //addOutput(createOutput<cpx::ComplexOutport>(Vec(output1X, rectInSectionY+30), module, ComputerscareNomplexPumbers::RECT_IN_RECT_OUT ));
        
		}
		void step() {
			if(module) {
				int outMode = module->params[paramID].getValue();
				if(outMode==0) {
					port->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewL.svg")));
				} else {
					port->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantL.svg")));

				}
			}
			Widget::step();
		}
	};
}
