#pragma once

using namespace rack;


namespace cpx {

	struct ComplexXY : TransparentWidget {
	ComputerscareComplexBase* module;

	Vec clickedMousePosition;
	Vec thisPos;
	math::Vec deltaPos;
	
	Vec pixelsOrigin;

	Vec origComplexValue;
	float origComplexLength;

	int paramA;

	bool editing=false;

	float originalMagnituteRadiusPixels = 120.f;

	ComplexXY(ComputerscareComplexBase* mod,int indexParamA) {
		module=mod;
		paramA = indexParamA;
		TransparentWidget();
	}

	void onButton(const event::Button &e) override {

		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if(e.action == GLFW_PRESS){
				e.consume(this);
				editing=true;
				clickedMousePosition = APP->scene->getMousePos();
				if(module) {
					float complexA = module->params[paramA].getValue();
					float complexB = module->params[paramA+1].getValue();
					DEBUG("orig: %f,%f",complexA,complexB);
					origComplexValue = Vec(complexA,complexB);
					origComplexLength=origComplexValue.norm();
					
					if(origComplexLength < 0.1) {
						origComplexLength = 1;
						pixelsOrigin = clickedMousePosition;
					} else {
						pixelsOrigin = clickedMousePosition.minus(origComplexValue.mult(originalMagnituteRadiusPixels/origComplexLength));
					}


					

				}
			} 
		}
	}

	void onDragEnd(const event::DragEnd &e) override {
		editing=false;
	}

	void step() {
		if(editing && module) {
			thisPos = APP->scene->getMousePos();

			//in scaled pixels

			Vec pixelsDiff = thisPos.minus(pixelsOrigin);
			Vec newZ = pixelsDiff.div(originalMagnituteRadiusPixels).mult(origComplexLength);

			module->params[paramA].setValue(newZ.x);
			module->params[paramA+1].setValue(newZ.y);

		}
	}

	void drawLayer(const DrawArgs &args,int layer) override {

		//background
		nvgFillColor(args.vg, nvgRGB(127, 0, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, 10, 15);
		nvgFill(args.vg);

		if(layer==1) {
			if(editing) {


				nvgSave(args.vg);
				//reset to "undo" the zoom
				nvgReset(args.vg);

				nvgTranslate(args.vg,pixelsOrigin.x,pixelsOrigin.y);

				//circle at the zero point
				nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgFillColor(args.vg,  nvgRGB(0, 22, 12));
	      nvgEllipse(args.vg, 0, 0,10.f, 10.f);
	      nvgClosePath(args.vg);
	      nvgFill(args.vg);


				//circle at the current complex value
				nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgStrokeColor(args.vg,  nvgRGB(0, 100, 200));
	      nvgEllipse(args.vg, 0, 0,originalMagnituteRadiusPixels, originalMagnituteRadiusPixels);
	      nvgClosePath(args.vg);
	      nvgStroke(args.vg);

	      //circle at complex radius 1
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgStrokeColor(args.vg,  nvgRGB(0, 220, 100));
	      nvgEllipse(args.vg, 0, 0,originalMagnituteRadiusPixels/origComplexLength, originalMagnituteRadiusPixels/origComplexLength);
	      nvgClosePath(args.vg);
	      nvgStroke(args.vg);

	      //circle at complex radius 10
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgStrokeColor(args.vg,  nvgRGB(240, 30, 51));
	      nvgEllipse(args.vg, 0, 0,10.f*originalMagnituteRadiusPixels/origComplexLength, 10.f*originalMagnituteRadiusPixels/origComplexLength);
	      nvgClosePath(args.vg);
	      nvgStroke(args.vg);


	     

	      //line from the zero point to the original complex number
	      Vec originalComplexPixels =origComplexValue.mult(originalMagnituteRadiusPixels/origComplexLength).plus(pixelsOrigin);
	    	nvgReset(args.vg);
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 5.f);
	      nvgStrokeColor(args.vg,  nvgRGB(140, 120, 80));
	      nvgMoveTo(args.vg, pixelsOrigin.x,pixelsOrigin.y);
	      nvgLineTo(args.vg,originalComplexPixels.x,originalComplexPixels.y);
	     	nvgClosePath(args.vg);
	      nvgStroke(args.vg);

	      //line from the zero point to the users mouse
	    	nvgReset(args.vg);
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 5.f);
	      nvgStrokeColor(args.vg,  nvgRGB(40, 220, 80));
	      nvgMoveTo(args.vg, pixelsOrigin.x,pixelsOrigin.y);
	      nvgLineTo(args.vg,thisPos.x,thisPos.y);
	     	nvgClosePath(args.vg);
	      nvgStroke(args.vg);

	      nvgRestore(args.vg);

			} 

		}
			

		Widget::drawLayer(args,layer);
	}
};

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
