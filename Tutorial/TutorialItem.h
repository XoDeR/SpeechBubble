#pragma once

#include "Common.h"

class TutorialTextData;
class TutorialGeometryData;

class FigureData
{
public:
	vector<Point> list;
};

class TutorialItem : public Node
{
public:
	static TutorialItem* create(string filename);
	static TutorialItem* create(unique_ptr<TutorialGeometryData> tutorialGeometryData, unique_ptr<TutorialTextData> tutorialTextData);
private:
	TutorialItem(unique_ptr<TutorialGeometryData> tutorialGeometryData, unique_ptr<TutorialTextData> tutorialTextData);
	virtual void onExit();
	virtual bool init();

	void processLine(Vec2 p1, Vec2 p2);
	void processArc(Vec2 p1, Vec2 p2, Vec2 p3);

	void drawLine(FigureData& figureData);
	void drawArc(FigureData& figureData);

	void appendCubicBezier(vector<Vec2>& vertices, FigureData& figureData, int segments);

	void drawSolidConcavePoly(vector<Vec2>& vertices, const Color4F& color);

	void drawGeometry();
	void copyGeometryToTexture();
	void createSpriteFromGeometryTexture();
	void applyShaders();

	void applyActionForDebugging();

	DrawNode* geometry;
	RenderTexture* renderTexture;
	Sprite* sprite; // to apply shaders to

	unique_ptr<TutorialGeometryData> tutorialGeometryData;
	unique_ptr<TutorialTextData> tutorialTextData;

	Color4F borderColor;
	Color4F fillColor;
	float borderWidth;

	int cornerSegments;

	vector<FigureData> figureList; 

	vector<Vec2> polyVertices;
};