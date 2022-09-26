#pragma once
#include <vector>
#include "Face.h"
 
class BoundingBox {
public:
	std::vector<Face> faces; // Contains vertices of faces in Clockwise Order
	
	BoundingBox();
	BoundingBox(std::vector<Face> faces);
	void setFaces(std::vector<Face> &faces);

};