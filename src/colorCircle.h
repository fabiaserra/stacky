#pragma once
#include "ofxBox2d.h"

//static int hexColors[4] = {0x31988A, 0xFDB978, 0xFF8340, 0xE8491B};

class ColorCircle : public ofxBox2dCircle {
	
public:
	ColorCircle() {}
	ofColor color;

	void setColor()
	{
		color.setHex(hexColors[(int)ofRandom(7)]);
	}

	void draw() {
		ofFill();
		ofSetColor(color.r, color.g, color.b);
		ofxBox2dCircle::draw();
	}
};