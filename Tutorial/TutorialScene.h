#pragma once

#include "Common.h"

class TutorialLayer;

class TutorialScene : public Scene
{
public:
	TutorialScene();

	virtual ~TutorialScene();
	
	// Scene
	virtual bool init();
	virtual void onExit();
	
	CREATE_FUNC(TutorialScene);
private:
	static const int TUTORIAL_LAYER_Z_ORDER = 10;
	unique_ptr<TutorialLayer> tutorialLayer;
};
