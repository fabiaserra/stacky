#pragma once
#include "ofxBox2d.h"

static int hexColors[5] = {0x45B5C9, 0xC19846, 0xD00909, 0x5A9024, 0xE9871D};
//#define HALF_LIFE_WIDTH 34

class ColorRect : public ofxBox2dRect {
	
public:
	ColorRect() {
		//ofxBox2dRect::setData(this);
	}
	ofColor color;
	bool bHit;
	float max_x, min_x;
	float max_y, min_y;

	void setColor()
	{
		color.setHex(hexColors[(int)ofRandom(5)]);
	}

	//void update()
	//{
	//	max_x = getPosition().x + getWidth() + HALF_LIFE_WIDTH;
	//	min_x = getPosition().x - getWidth() + HALF_LIFE_WIDTH;
	//	max_y = getPosition().y + getHeight() + HALF_LIFE_WIDTH;
	//	min_y = getPosition().y - getHeight() + HALF_LIFE_WIDTH;
	//}

	void draw() {
		ofFill();
		if (bHit) ofSetColor(color.r, color.g, color.b);
		else ofSetColor(0);
		ofxBox2dRect::draw();
	}
};