#pragma once

#include "Common.h"

class TutorialScene;

class TutorialLayer : public Layer
{
public:
	TutorialLayer();
	virtual ~TutorialLayer();
	virtual bool init();
	void onEnterTransitionDidFinish();
private:
	virtual void onExit();
	TutorialScene* tutorialScene;
};
