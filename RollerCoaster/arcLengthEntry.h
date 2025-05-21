#ifndef ARCLENGTHENTRY_H
#define ARCLENGTHENTRY_H

#include "vertex.h"

#pragma once

struct ArcLengthEntry {
	float arcLength;
	float t;
	Vertex arcVertex;
};

#endif