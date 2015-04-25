#pragma once

#include "Common.h"

enum class GeometryOperation
{
	Line,
	Arc
};

class TutorialGeometryData
{
public:
	vector<Vec2> vertices;
	vector<GeometryOperation> operations;

	Size size;
	Vec2 position; // relative to parent
};