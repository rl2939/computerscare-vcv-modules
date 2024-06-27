#pragma once

using namespace rack;





namespace cpx {

	void drawArrowTo(NVGcontext* vg,math::Vec tipPosition) {
		float angle = tipPosition.arg();
		float len = tipPosition.norm();

		nvgSave(vg);
    nvgBeginPath(vg);
    nvgStrokeWidth(vg, 3.f);
    nvgStrokeColor(vg,  COLOR_COMPUTERSCARE_DARK_GREEN);
    nvgFillColor(vg,  COLOR_COMPUTERSCARE_LIGHT_GREEN);
   
		nvgRotate(vg,angle);
		nvgMoveTo(vg,0,-5);
		nvgLineTo(vg,0,5);
		nvgLineTo(vg,len,1);
		nvgLineTo(vg,len,-1);

 		nvgClosePath(vg);
    nvgFill(vg);
    nvgStroke(vg);

    nvgRestore(vg);
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
			Vec newZ = pixelsDiff.div(originalMagnituteRadiusPixels).mult(origComplexLength);

			module->params[paramA].setValue(newZ.x);
			module->params[paramA+1].setValue(newZ.y);

		}
	}



	void draw(const DrawArgs &args) override {
		float pxRatio = APP->window->pixelRatio;
		//background
		/*nvgFillColor(args.vg, nvgRGB(127, 0, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, 10, 15);
		nvgFill(args.vg);*/
		//nvgResetTransform(args.vg);
			//	nvgScale(args.vg, 1/pxRatio, 1/pxRatio);
		//circle at complex radius 1
			nvgTranslate(args.vg,box.size.x/2,box.size.y/2);
	      nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 2.f);
	      nvgFillColor(args.vg,  nvgRGB(0, 10, 30));
	      //nvgMoveTo(args.vg,box.size.x/2,box.size.y/2);
	      nvgEllipse(args.vg, 0,0,box.size.x/2,box.size.y/2);
	      nvgClosePath(args.vg);
	      nvgFill(args.vg);

   			nvgBeginPath(args.vg);
	      nvgStrokeWidth(args.vg, 2.f);
	      nvgStrokeColor(args.vg,  nvgRGB(40, 110, 80));
	      nvgMoveTo(args.vg, 0,0);

	      Vec tip = pixelsDiff.normalize().mult(10.f);

	      drawArrowTo(args.vg,tip);

	    //  nvgLineTo(args.vg,tip.x,tip.y);
	      //nvgLineTo(args.vg,pixelsDiff.x,pixelsDiff.y);
	     //	nvgClosePath(args.vg);
	    //  nvgStroke(args.vg);


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
	      nvgFillColor(args.vg,  nvgRGB(0, 10, 30));
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


	struct CompolyOutWidget : Widget {
		ComplexOutport* port;
		std::vector<ComplexOutport*> ports;
		int paramID;
		int numPorts = 2;
		int lastOutMode = -1;
		ComputerscareComplexBase* module;

		CompolyOutWidget(math::Vec pos,ComputerscareComplexBase *cModule, int portID,int compolyTypeParamID) {

			module=cModule;
			paramID = compolyTypeParamID;
			CompolyTypeLabelSwitch* compolyLabel = createParam<CompolyTypeLabelSwitch>(Vec(0,0),cModule,compolyTypeParamID);

			TransformWidget* tw = new TransformWidget();
			tw->box.pos = pos.minus(Vec(40,0));
			tw->scale(1.0);

			tw->addChild(compolyLabel);
			addChild(tw);

			ports.resize(numPorts);

			for(int i = 0; i < numPorts; i++) {

				port = createOutput<ComplexOutport>(pos,cModule,portID+i);
				ports[i] = port;
				pos=pos.plus(Vec(40,0));
				addChild(port);
			}
			
      
		}
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
}
