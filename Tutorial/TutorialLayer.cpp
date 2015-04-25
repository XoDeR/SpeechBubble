#include "TutorialLayer.h"
#include "TutorialScene.h"
#include "TutorialItem.h"

TutorialLayer::TutorialLayer()
	: tutorialScene(nullptr)
{

}

void TutorialLayer::onExit()
{
	Layer::onExit();
	stopAllActions();
	unscheduleAllCallbacks();
	removeAllChildren();
}

TutorialLayer::~TutorialLayer()
{
}

bool TutorialLayer::init()
{
	Layer::init();

	auto bg = Sprite::create("MapLevelSelect-1024.png");
	bg->setPosition(getContentSize().width / 2.0f, getContentSize().height / 2.0f);
	addChild(bg, -10);

	return true;        
}

void TutorialLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	tutorialScene = static_cast<TutorialScene*>(getParent());

	TutorialItem* tutorialItem = TutorialItem::create("TutorialItemData.xml");
	addChild(tutorialItem, 100);
}