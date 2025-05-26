#ifndef ARCLENGTHENTRY_H
#define ARCLENGTHENTRY_H

#include "vertex.h"

#pragma once
// Will be used to save the calculated results of the bezier curves
struct ArcLengthEntry {
	float arcLength;
	float t;
	Vertex arcVertex;
};

#endif