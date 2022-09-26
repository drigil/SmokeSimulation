#include "BoundingBox.h"

BoundingBox::BoundingBox() {

}

BoundingBox::BoundingBox(std::vector<Face> faces){
	this->faces = faces;
}

void BoundingBox::setFaces(std::vector<Face> &faces) {
	this->faces = faces;
}