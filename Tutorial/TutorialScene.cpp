#include "TutorialScene.h"
#include "TutorialLayer.h"

TutorialScene::TutorialScene()
	: tutorialLayer(nullptr)
{

}

void TutorialScene::onExit()
{
	Scene::onExit();
	stopAllActions();
	unscheduleAllCallbacks();
	removeAllChildren();
}

TutorialScene::~TutorialScene()
{
}

bool TutorialScene::init()
{
	Scene::init();

	tutorialLayer = unique_ptr<TutorialLayer>(new TutorialLayer());
	tutorialLayer->init();
	addChild(tutorialLayer.get(), TUTORIAL_LAYER_Z_ORDER);

	return true;
}