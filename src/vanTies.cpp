#include "vanTies.h"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelAd);
	p->addModel(modelAdje);
	#ifndef METAMODULE
	p->addModel(modelBufke);
	#endif
	p->addModel(modelFuns);
	p->addModel(modelSjoegele);
}
