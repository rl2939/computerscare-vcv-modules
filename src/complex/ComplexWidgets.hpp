#pragma once

using namespace rack;





namespace cpx {

	inline void drawArrowTo(NVGcontext* vg,math::Vec tipPosition,float baseWidth=5.f) {
		float angle = tipPosition.arg();
		float len = tipPosition.norm();

		//nvgSave(vg);
    nvgBeginPath(vg);
    nvgStrokeWidth(vg, 1.f);
    nvgStrokeColor(vg,  COLOR_COMPUTERSCARE_DARK_GREEN);
    nvgFillColor(vg,  COLOR_COMPUTERSCARE_LIGHT_GREEN);
   
		nvgRotate(vg,angle);
		nvgMoveTo(vg,0,-baseWidth);
		//nvgArcTo(vg,0,0,0,baseWidth/2,13);
		//nvgLineTo(vg,-2,-baseWidth/2);
		//nvgLineTo(vg,-2,baseWidth/2);
		nvgQuadTo(vg,baseWidth/2,0,0,baseWidth);
		nvgLineTo(vg,0,baseWidth);
		nvgLineTo(vg,len,0);
		//nvgLineTo(vg,len,-1);

 		nvgClosePath(vg);
    nvgFill(vg);
    nvgStroke(vg);

   // nvgRestore(vg);
	}


	struct ComplexXY : TransparentWidget {
	ComputerscareComplexBase* module;

	Vec clickedMousePosition;
	Vec thisPos;
	math::Vec deltaPos;
	
	Vec pixelsOrigin;
	Vec pixelsDiff;

	Vec origComplexValue;
	float origComplexLength;

	Vec newZ;

	int paramA;

	bool editing=false;

	float originalMagnituteRadiusPixels = 120.f;

	ComplexXY(ComputerscareComplexBase* mod,int indexParamA) {
		module=mod;
		paramA = indexParamA;
		//box.size = Vec(30,30);
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
	void onHoverKey(const HoverKeyEvent& e) override {
		if(e.key == GLFW_KEY_ESCAPE) {
			DEBUG("escape");
		}
	}

	void onDragEnd(const event::DragEnd &e) override {
		editing=false;
	}

	void step() {
		if(editing && module) {
			thisPos = APP->scene->getMousePos();

			//in scaled pixels

			pixelsDiff = thisPos.minus(pixelsOrigin);
			newZ = pixelsDiff.div(originalMagnituteRadiusPixels).mult(origComplexLength);

			module->params[paramA].setValue(newZ.x);
			module->params[paramA+1].setValue(newZ.y);
		} else {
			if(module) {
				newZ = Vec(module->params[paramA].getValue(),module->params[paramA+1].getValue());
			}
		}
	}



	void draw(const DrawArgs &args) override {
		float pxRatio = APP->window->pixelRatio;
		//background

		float fullR = box.size.x/2;

		//circle at complex radius 1
		nvgSave(args.vg);
		//nvgResetTransform(args.vg);
			nvgTranslate(args.vg,fullR,fullR);
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 2.f);
	      nvgFillColor(args.vg,  nvgRGBA(0, 10, 30,50));
	      //nvgMoveTo(args.vg,box.size.x/2,box.size.y/2);
	      nvgEllipse(args.vg, 0,0,fullR,fullR);
	      nvgClosePath(args.vg);
	      nvgFill(args.vg);

   			nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 2.f);
	      nvgStrokeColor(args.vg,  nvgRGB(40, 110, 80));
	      nvgMoveTo(args.vg, 0,0);


	      float length=newZ.norm();
	      Vec tip = newZ.normalize().mult(2*fullR/M_PI*std::atanf(length));

	      drawArrowTo(args.vg,tip,box.size.x/7);
	      nvgRestore(args.vg);

	}
	void drawLayer(const DrawArgs &args,int layer) override {

		
		if(layer==1) {
			if(editing) {

				float pxRatio = APP->window->pixelRatio;

				nvgSave(args.vg);
				//reset to "undo" the zoom
				nvgResetTransform(args.vg);
				nvgScale(args.vg, pxRatio, pxRatio);

				nvgTranslate(args.vg,pixelsOrigin.x,pixelsOrigin.y);

				//circle at complex radius 1
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgFillColor(args.vg,  nvgRGBA(0, 10, 30,60));
	      nvgEllipse(args.vg, 0, 0,originalMagnituteRadiusPixels/origComplexLength, originalMagnituteRadiusPixels/origComplexLength);
	      nvgClosePath(args.vg);
	      nvgFill(args.vg);

				//circle at the zero point
				nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgFillColor(args.vg,  nvgRGB(249, 220, 214));
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

	      

	      //circle at complex radius 10
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 3.f);
	      nvgStrokeColor(args.vg,  nvgRGB(240, 30, 51));
	      nvgEllipse(args.vg, 0, 0,10.f*originalMagnituteRadiusPixels/origComplexLength, 10.f*originalMagnituteRadiusPixels/origComplexLength);
	      nvgClosePath(args.vg);
	      nvgStroke(args.vg);


	     

	      //line from the zero point to the original complex number
	      Vec originalComplexPixels =origComplexValue.mult(originalMagnituteRadiusPixels/origComplexLength);
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 5.f);
	      nvgStrokeColor(args.vg,  nvgRGB(140, 120, 80));
	      nvgMoveTo(args.vg, 0,0);
	      nvgLineTo(args.vg,originalComplexPixels.x,originalComplexPixels.y);
	     	nvgClosePath(args.vg);
	      nvgStroke(args.vg);

	      //line from the zero point to the users mouse
	      
	      drawArrowTo(args.vg,pixelsDiff);
	     	
	    
	      nvgRestore(args.vg);
			} 
		}
		Widget::drawLayer(args,layer);
	}
};


	struct ComplexOutport : ComputerscareSvgPort {
		ComplexOutport() {
			//setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewR.svg")));
		}
	};

	struct CompolyTypeLabelSwitch : app::SvgSwitch {
		widget::TransformWidget* tw;
		CompolyTypeLabelSwitch() {
			shadow->opacity = 0.f;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-labels/xy.svg")));
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-labels/rtheta.svg")));
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-labels/x.svg")));
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-labels/r.svg")));
		}
	};

template <typename BASE>
struct CompolyInOrOutWidget : Widget {
	ComputerscareComplexBase* module;
	int paramID;
	int numPorts = 2;
	int lastOutMode = -1;
	std::vector<BASE*> ports;

	void step() {
		if(module) {
			int outMode = module->params[paramID].getValue();

			if(lastOutMode != outMode) {
				lastOutMode = outMode;
						if(ports[0] && ports[1]) {
							if(outMode==0) {
								ports[0]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewL.svg")));
								ports[1]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewL.svg")));
							} else if(outMode==1) {
								ports[0]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantL.svg")));
								ports[1]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantL.svg")));
							} else if(outMode==2) {
								ports[0]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewL.svg")));
								ports[1]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-skewR.svg")));
							} else if(outMode==3) {
								ports[0]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantR.svg")));
								ports[1]->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/complex-outjack-slantL.svg")));
							}
						}
					}
			}
			
		Widget::step();
	}
};


	struct CompolyOutWidget : CompolyInOrOutWidget<ComplexOutport> {
		ComplexOutport* port;
		
		CompolyOutWidget(math::Vec pos,ComputerscareComplexBase *cModule, int firstPortID,int compolyTypeParamID,float scale=1.0,bool isOutput=true) {

			module=cModule;
			paramID = compolyTypeParamID;
			CompolyTypeLabelSwitch* compolyLabel = createParam<CompolyTypeLabelSwitch>(Vec(0,0),cModule,compolyTypeParamID);

			TransformWidget* tw = new TransformWidget();
			tw->box.pos = pos.minus(Vec(20,0));
			tw->scale(scale);

			tw->addChild(compolyLabel);
			addChild(tw);

			ports.resize(numPorts);

			for(int i = 0; i < numPorts; i++) {
				if(isOutput) {
					port = createOutput<ComplexOutport>(pos,cModule,firstPortID+i);
				}
				else {
					port = createInput<ComplexOutport>(pos,cModule,firstPortID+i);
				}
				ports[i] = port;
				pos=pos.plus(Vec(30,0));
				addChild(port);
			}
			
      
		}
		
	};
}
