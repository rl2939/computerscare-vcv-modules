#include "Computerscare.hpp"


Plugin *plugin;


void init(Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);

	p->addModel(modelComputerscareDebug);
	p->addModel(modelComputerscarePatchSequencer);
}