#pragma once

using namespace rack;


namespace cpx {
	struct ComplexOutport : ComputerscareSvgPort {
		ComplexOutport() {
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantL.svg")));
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
		CompolyOutWidget(math::Vec pos,ComputerscareComplexBase *module, int portID,int compolyTypeParamID) {

			CompolyTypeLabelSwitch* compolyLabel = createParam<CompolyTypeLabelSwitch>(Vec(0,0),module,compolyTypeParamID);

			HalfSize* half = new HalfSize(pos.minus(Vec(20,0)),0.5);
			half->addChild(compolyLabel);

			ComplexOutport* port = createOutput<ComplexOutport>(pos,module,portID);

			addChild(half);
			addChild(port);
			// addParam(createParam<SmallKnob>(Vec(rightInputX, rectInSectionY).plus(trimRelPos), module, ComputerscareNomplexPumbers::IMAGINARY_INPUT_TRIM));

        //addOutput(createOutput<cpx::ComplexOutport>(Vec(output1X, rectInSectionY+30), module, ComputerscareNomplexPumbers::RECT_IN_RECT_OUT ));
        
		}
	};
}
