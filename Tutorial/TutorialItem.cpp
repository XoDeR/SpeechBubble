#include "TutorialItem.h"
#include "TutorialTextData.h"
#include "TutorialGeometryData.h"
#include "Triangulation.h"

//// Loading from file
#include "Util.h"
#include "pugixml.hpp"
using namespace pugi;

TutorialItem::TutorialItem(unique_ptr<TutorialGeometryData> tutorialGeometryData, unique_ptr<TutorialTextData> tutorialTextData)
	: geometry(nullptr)
	, renderTexture(nullptr)
	, sprite(nullptr)
	, tutorialGeometryData(std::move(tutorialGeometryData))
	, tutorialTextData(std::move(tutorialTextData))
{
}

bool TutorialItem::init()
{
	Node::init();
	setContentSize(tutorialGeometryData->size);
	setPosition(tutorialGeometryData->position); // top left
	setAnchorPoint(Vec2(0.0f, 1.0f)); // top left

	////
	borderWidth = 1;
	cornerSegments = 6;
	borderColor = Color4F(1, 1, 1, 1); // white
	fillColor = Color4F(0, 0, 0, 1); // black
	////

	geometry = DrawNode::create();
	geometry->setContentSize(tutorialGeometryData->size);
	geometry->setPosition(Vec2(tutorialGeometryData->size.width / 2.0f, tutorialGeometryData->size.height / 2.0f));
	geometry->setAnchorPoint(Vec2(0.5f, 0.5f));
	addChild(geometry, 200);

	// need to add 1.0f to height here unless pixels will be lost
	// some bug in cocos2d-x
	// maybe connected to size not % 2 == 0
	renderTexture = RenderTexture::create(tutorialGeometryData->size.width, tutorialGeometryData->size.height + 1.0f, Texture2D::PixelFormat::RGBA8888);
	//renderTexture->setContentSize(tutorialGeometryData->size);
	renderTexture->setPosition(Vec2(tutorialGeometryData->size.width / 2.0f, tutorialGeometryData->size.height / 2.0f));
	renderTexture->setAnchorPoint(Vec2(0.5f, 0.5f));
	addChild(renderTexture, 300);

	drawGeometry();
	copyGeometryToTexture();
	createSpriteFromGeometryTexture();
	applyShaders();

	applyActionForDebugging();

	return true;
}

TutorialItem* TutorialItem::create(unique_ptr<TutorialGeometryData> tutorialGeometryData, unique_ptr<TutorialTextData> tutorialTextData)
{
	TutorialItem* tutorialItem = new TutorialItem(std::move(tutorialGeometryData), std::move(tutorialTextData));
	if (tutorialItem && tutorialItem->init())
	{
		tutorialItem->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(tutorialItem);
	}

	return tutorialItem;
}

TutorialItem* TutorialItem::create(string filename)
{
	xml_document doc;
	xml_parse_result result = doc.load_file(FileUtils::getInstance()->fullPathForFilename(filename).c_str());

	// GeometryData
	unique_ptr<TutorialGeometryData> tutorialGeometryData(new TutorialGeometryData());

	xpath_node nodeGeometryData = doc.select_single_node("/TutorialItemData/GeometryData");
	
	tutorialGeometryData->size = getVec2FromString(nodeGeometryData.node().attribute("size").value());
	tutorialGeometryData->position = getVec2FromString(nodeGeometryData.node().attribute("position").value());
	
	xpath_node_set nodeSetVertices = nodeGeometryData.node().select_nodes("Vertices/Point");
	for (xpath_node_set::const_iterator it = nodeSetVertices.begin(); it != nodeSetVertices.end(); ++it)
	{
		xpath_node nodeVertex = *it;
		// need to adjust vertices
		// consider invert y axis
		// input -- y axis goes down
		// needed  -- y axis goes up (OpenGL)
		//  input coords are absolute
		// needed -- relative
		Vec2 input = getVec2FromString(nodeVertex.node().attribute("position").value());
		// relative
		Vec2 relative = input - tutorialGeometryData->position;
		// invert y
		Vec2 value = relative;
		value.y = tutorialGeometryData->size.height - value.y;
		tutorialGeometryData->vertices.push_back(value);
	}

	// invert  position's y
	tutorialGeometryData->position.y = 768 - tutorialGeometryData->position.y;

	xpath_node_set nodeSetOperations = nodeGeometryData.node().select_nodes("Operations/Figure");
	for (xpath_node_set::const_iterator it = nodeSetOperations.begin(); it != nodeSetOperations.end(); ++it)
	{
		xpath_node nodeFigure = *it;
		string figureStr = nodeFigure.node().attribute("name").value();
		if (figureStr == "Arc")
		{
			tutorialGeometryData->operations.push_back(GeometryOperation::Arc);
		} 
		else if (figureStr == "Line")
		{
			tutorialGeometryData->operations.push_back(GeometryOperation::Line);
		}
	}

	// TextData
	unique_ptr<TutorialTextData> tutorialTextData(new TutorialTextData());

	xpath_node nodeTitle = doc.select_single_node("/TutorialItemData/TextData/Title");
	if (nodeTitle.node().empty() != true)
	{
		tutorialTextData->title = nodeTitle.node().child_value();
		tutorialTextData->titlePosition = getVec2FromString(nodeTitle.node().attribute("position").value());
	}

	xpath_node nodeDesc = doc.select_single_node("/TutorialItemData/TextData/Description");
	if (nodeDesc.node().empty() != true)
	{
		tutorialTextData->description = nodeDesc.node().child_value();
		tutorialTextData->descriptionPosition = getVec2FromString(nodeDesc.node().attribute("position").value());
	}

	return TutorialItem::create(std::move(tutorialGeometryData), std::move(tutorialTextData));
}

void TutorialItem::onExit()
{
	Node::onExit();
	stopAllActions();
	unscheduleAllCallbacks();
	removeAllChildren();
}

void TutorialItem::drawGeometry()
{
	int currentVertex = 0;
	for (GeometryOperation geometryOperation : tutorialGeometryData->operations)
	{
		if (geometryOperation == GeometryOperation::Arc)
		{
			processArc(tutorialGeometryData->vertices[currentVertex], tutorialGeometryData->vertices[currentVertex + 1], tutorialGeometryData->vertices[currentVertex + 2]);
			currentVertex++;
			currentVertex++;
		}
		else if (geometryOperation == GeometryOperation::Line)
		{
			// last figure will be always line
			// if the figure is last then first vertex should be taken as it finish point
			bool lastFigure = false;
			if (currentVertex + 1 == tutorialGeometryData->vertices.size())
			{
				lastFigure = true;
			}

			if (lastFigure == true)
			{
				processLine(tutorialGeometryData->vertices[currentVertex], tutorialGeometryData->vertices[0]);
				break;
			} 
			else
			{
				processLine(tutorialGeometryData->vertices[currentVertex], tutorialGeometryData->vertices[currentVertex + 1]);
				currentVertex++;
			}

		}
	}

	drawSolidConcavePoly(polyVertices, fillColor);
	//geometry->drawSolidPoly(&polyVertices[0], polyVertices.size(), fillColor);

	for (int i = 0; i < tutorialGeometryData->operations.size(); ++i)
	{
		GeometryOperation geometryOperation = tutorialGeometryData->operations[i];
		if (geometryOperation == GeometryOperation::Arc)
		{
			drawArc(figureList[i]);
		}
		else if (geometryOperation == GeometryOperation::Line)
		{
			drawLine(figureList[i]);
		}
	}
}

void TutorialItem::copyGeometryToTexture()
{
	renderTexture->begin();

	geometry->visit();

	renderTexture->end();

	geometry->setVisible(false);
}

void TutorialItem::createSpriteFromGeometryTexture()
{
	auto finalTexture = renderTexture->getSprite()->getTexture();
	finalTexture->setAntiAliasTexParameters();

	sprite = Sprite::createWithTexture(finalTexture);
	sprite->setPosition(Vec2(tutorialGeometryData->size.width / 2.0f, tutorialGeometryData->size.height / 2.0f));
	sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
	addChild(sprite, 400);
	
	renderTexture->setVisible(false);
}

void TutorialItem::processLine(Vec2 p1, Vec2 p2)
{
	polyVertices.push_back(p1);

	FigureData figureData;
	figureData.list.push_back(p1);
	figureData.list.push_back(p2);
	figureList.push_back(figureData);
}

void TutorialItem::processArc(Vec2 p1, Vec2 p2, Vec2 p3)
{
	float kappa = 0.552228474;

	FigureData figureData;

	figureData.list.push_back(p1);
	figureData.list.push_back(p1 + (p3 - p2) * kappa);
	figureData.list.push_back(p3 + (p1 - p2) * kappa);
	figureData.list.push_back(p3);

	figureList.push_back(figureData);

	appendCubicBezier(polyVertices, figureData, cornerSegments);
}

void TutorialItem::appendCubicBezier(vector<Vec2>& vertices, FigureData& figureData, int segments)
{
	float t = 0;
	for (int i = 0; i < segments - 1; i++) // last segment point is not included
	{
		float x = powf(1 - t, 3) * figureData.list[0].x + 3.0f * powf(1 - t, 2) * t * figureData.list[1].x + 3.0f * (1 - t) * t * t * figureData.list[2].x + t * t * t * figureData.list[3].x;
		float y = powf(1 - t, 3) * figureData.list[0].y + 3.0f * powf(1 - t, 2) * t * figureData.list[1].y + 3.0f * (1 - t) * t * t * figureData.list[2].y + t * t * t * figureData.list[3].y;
		vertices.push_back(Vec2(x, y));
		t += 1.0f / segments;
	}
}

void TutorialItem::drawLine(FigureData& figureData)
{
	geometry->drawLine(figureData.list[0], figureData.list[1], borderColor);
}

void TutorialItem::drawArc(FigureData& figureData)
{
	geometry->drawCubicBezier(figureData.list[0], figureData.list[1], figureData.list[2], figureData.list[3], cornerSegments, borderColor);
}

void TutorialItem::drawSolidConcavePoly(vector<Vec2>& vertices, const Color4F& color)
{
	vector<Vec2> result;
	Triangulate::Process(vertices, result);
	
	int tcount = result.size() / 3;
	for (int i = 0; i < tcount; i++)
	{
		const Vec2 &p1 = result[i * 3 + 0];
		const Vec2 &p2 = result[i * 3 + 1];
		const Vec2 &p3 = result[i * 3 + 2];
		geometry->drawTriangle(p1, p2, p3, color);
	}
}

void TutorialItem::applyShaders()
{
	string shaderFolder = "Data\\Shader\\";

	GLchar* fragmentSource = (GLchar*)String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename(shaderFolder + "TutorialItem.fsh").c_str())->getCString();

	auto p = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragmentSource);
	auto glProgramState = GLProgramState::getOrCreateWithGLProgram(p);
	auto s = sprite->getTexture()->getContentSizeInPixels();
	glProgramState->setUniformVec2("resolution", Vec2(s.width, s.height));
	sprite->setGLProgramState(glProgramState);
}

void TutorialItem::applyActionForDebugging()
{
	auto rotate1 = RotateBy::create(2.4f, -360);
	auto scale1 = ScaleBy::create(2.4f, 2.0f);
	auto rotate2 = RotateBy::create(2.4f, 360);
	auto scale2 = ScaleBy::create(2.4f, 0.5f);
	auto delay = DelayTime::create(5.0f);

	auto sequence = Sequence::create(rotate1, scale1, rotate2, scale2, delay, nullptr);
	auto rfe = RepeatForever::create(sequence);
	
	sprite->runAction(rfe);
}


