#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetFrameRate(60);

	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);
	ofSetLogLevel(OF_LOG_NOTICE);

	// Kinect initialization code
	kinect.initSensor();

	// We first set up data streams for the Kinect (color, depth and skeleton tracking)
	float kinect_width = 640;
	float kinect_height = 480;
	//kinect.initColorStream(kinect_width, kinect_height, true);
	kinect.initDepthStream(kinect_width, kinect_height, true);
	kinect.initSkeletonStream(true);

	// Launch Kinect
	kinect.start();

	max_grayImage.allocate(kinect_width, kinect_height);
	min_grayImage.allocate(kinect_width, kinect_height);
	
	box2d.init();
	box2d.enableEvents();
	box2d.setGravity(0, 1);
	box2d.setFPS(30.0);

	plate_width = 130;
	plate_height = 12;

	//RIGHT PLATE
	rightPlate.setPhysics(0.0, 0, 0.5);
	rightPlate.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, plate_width, plate_height+20);
	//rightSupportR.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, 10, 15);
	//rightSupportL.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, 10, 15);

	//LEFT PLATE
	leftPlate.setPhysics(0.0, 0, 0.5);
    leftPlate.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, plate_width, plate_height+20);
	//leftSupportR.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, 10, 15);
	//leftSupportL.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, 10, 15);

	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);

	// load 1 sfx soundfile
	box_contact_sound.loadSound("fustapla08.wav");
	box_contact_sound.setMultiPlay(true);
	box_contact_sound.setLoop(false);

	circle_contact_sound.loadSound("taulacantometall08.wav");
	circle_contact_sound.setMultiPlay(true);
	circle_contact_sound.setLoop(false);

	life_sound.loadSound("life.wav");
	life_sound.setMultiPlay(true);
	life_sound.setLoop(false);

	ambience_music.loadSound("lemoncreme.mp3");
	ambience_music.setMultiPlay(false);
	ambience_music.setLoop(true);
	ambience_music.play();
	
	bHardMode = false;
	bCreateBox = true; // Variable to control the interval at which you create boxes
	bStartGame = false; // put to FALSE
	bStartCountdown = false; 
	bGameOver = false;
	bCreateLifes = false;
	bLifeCreated = false;
	bContactLeftPlate = false;
	bContactRightPlate = false;
	bRightTurn = false;
	bLoosePoints = false;
	bWinPoints = false;
	bCounterInited = false;
	
	countCycles = 0;
	total_lifes = 3;
	total_time = 0;
	total_points = 0;
	max_boxes = 3;
	bonus_left = 1;
	bonus_right = 1;
	countdown = 15;
	
	life.loadImage("vida.png");
	points_font.loadFont("coolvetica rg.ttf", 70, true);
	countdown_font.loadFont("coolvetica rg.ttf", 60, true);
	countdown_font.setLetterSpacing(1.2);
	gameOver_font.loadFont("coolvetica rg.ttf", 100, true);
	finalpoints_font.loadFont("coolvetica rg.ttf", 40, true);
	mode_font.loadFont("coolvetica rg.ttf", 20, true);

	x_kinect_scale = ofGetWidth()/640.0;
	y_kinect_scale = ofGetHeight()/480.0;

	x_scale = 1;
	y_scale = 1;
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e) {
	
	if(e.a != NULL && e.b != NULL && !bGameOver) 
	{ 
		//-------------LEFT----------------
		// if the contact is with the leftPlate
		if ( !bContactLeftPlate && (e.a->GetBody() == leftPlate.body || e.b->GetBody() == leftPlate.body) )
		{
			// and one of the colored boxes
			for(int i=0; i<color_boxes.size(); i++)
			{
				if ( e.a->GetBody() == color_boxes[i].get()->body || e.b->GetBody() == color_boxes[i].get()->body) 
				{
					box_contact_sound.play();
					if ( canBeAttached(&leftPlate, color_boxes[i].get()) )
					{
						bContactLeftPlate = true;
						ofPtr<ColorRect> b = color_boxes[i];
						color_boxes_left.push_back(b);
						color_boxes.erase(color_boxes.begin()+i);
						bonus_left = 1;
						total_points += 50*bonus_left;
						bLoosePoints = false;
						bWinPoints = true;
						break;
					}
				}	
			}

			// and one of the black circles
			for(int i=0; i<circles.size(); i++) 
			{
				if (e.a->GetBody() == circles[i].get()->body || e.b->GetBody() == circles[i].get()->body) 
				{
					total_points -= 50;
					circle_contact_sound.play();
					bLoosePoints = true;
					bWinPoints = false;
					break;
				}
			}
		}
		
		// LEFT STACK
		if (!color_boxes_left.empty())
		{
			// if the contact is with the last of the stacked boxes 
			if ( e.a->GetBody() == color_boxes_left.back().get()->body || e.b->GetBody() == color_boxes_left.back().get()->body )
			{
				// and the falling color boxes 
				for(int i=0; i<color_boxes.size(); i++) 
				{
					if (e.a->GetBody() == color_boxes[i].get()->body || e.b->GetBody() == color_boxes[i].get()->body) 
					{
						box_contact_sound.play();
						if ( canBeAttached(color_boxes_left.back().get(), color_boxes[i].get()) )
						{
							ofPtr<ColorRect> b = color_boxes[i];
							color_boxes_left.push_back(b);
							color_boxes.erase(color_boxes.begin()+i);
							if (color_boxes_left.size()%4 == 0) bonus_left *= 2;
							total_points += 50*bonus_left;
							bLoosePoints = false;
							bWinPoints = true;
							break;
						}
					}
				}

				// and one of the black circles
				for(int i=0; i<circles.size(); i++) 
				{
					if (e.a->GetBody() == circles[i].get()->body || e.b->GetBody() == circles[i].get()->body) 
					{
						total_points -= 50;
						circle_contact_sound.play();
						bLoosePoints = true;
						bWinPoints = false;
						break;
					}
				}
			}
		}

		//-------------RIGHT----------------
		// if the contact is with the rightPlate
		if ( !bContactRightPlate && (e.a->GetBody() == rightPlate.body || e.b->GetBody() == rightPlate.body) )
		{
			// and one of the colored boxes
			for(int i=0; i<color_boxes.size(); i++)
			{
				if ( e.a->GetBody() == color_boxes[i].get()->body || e.b->GetBody() == color_boxes[i].get()->body) 
				{
					box_contact_sound.play();
					if ( canBeAttached(&rightPlate, color_boxes[i].get()) )
					{
						bContactRightPlate = true;
						ofPtr<ColorRect> b = color_boxes[i];
						color_boxes_right.push_back(b);
						color_boxes.erase(color_boxes.begin()+i);
						bonus_right = 1;
						total_points += 50*bonus_right;
						bLoosePoints = false;
						bWinPoints = true;
						break;
					}
				}
			}

			// and one of the black circles
			for(int i=0; i<circles.size(); i++) 
			{
				if (e.a->GetBody() == circles[i].get()->body || e.b->GetBody() == circles[i].get()->body) 
				{
					total_points -= 50;
					circle_contact_sound.play();
					bLoosePoints = true;
					bWinPoints = false;
					break;
				}
			}
		}
		
		// RIGHT STACK
		if (!color_boxes_right.empty())
		{
			// if the contact is with the last of the stacked boxes 
			if ( e.a->GetBody() == color_boxes_right.back().get()->body || e.b->GetBody() == color_boxes_right.back().get()->body )
			{
				// and the falling color boxes 
				for(int i=0; i<color_boxes.size(); i++) 
				{
					if (e.a->GetBody() == color_boxes[i].get()->body || e.b->GetBody() == color_boxes[i].get()->body) 
					{
						box_contact_sound.play();
						if ( canBeAttached(color_boxes_right.back().get(), color_boxes[i].get()) )
						{
							ofPtr<ColorRect> b = color_boxes[i];
							color_boxes_right.push_back(b);
							color_boxes.erase(color_boxes.begin()+i);
							if (color_boxes_right.size()%4 == 0) bonus_right *= 2;
							total_points += 50*bonus_right;
							bLoosePoints = false;
							bWinPoints = true;
							break;
						}
					}
				}

				// and one of the black circles
				for(int i=0; i<circles.size(); i++) 
				{
					if (e.a->GetBody() == circles[i].get()->body || e.b->GetBody() == circles[i].get()->body) 
					{
						total_points -= 50;
						circle_contact_sound.play();
						bLoosePoints = true;
						bWinPoints = false;
						break;
					}
				}
			}
		}
		
	}
}

//--------------------------------------------------------------
void testApp::update(){

	kinect.update();
	
	// SILHOUETTE OF THE USER
	max_grayImage.setFromPixels(kinect.getDepthPixelsRef());

	max_grayImage.threshold(225, true);
	max_grayImage.blurGaussian(15);
	min_grayImage = max_grayImage;
	min_grayImage.erode();
	min_grayImage.erode();
	min_grayImage.erode();
	
	max_grayImage.dilate();
	max_grayImage.dilate();
	max_grayImage.dilate();

	max_grayImage.absDiff(min_grayImage);
	max_grayImage.contrastStretch();
	max_grayImage.erode();
	max_grayImage.erode();
	max_grayImage.erode();
	max_grayImage.invert();
	
	// start countdown when we reach max_boxes in both stacks
	if (bStartCountdown && !bGameOver) 
	{
		countdown -= 1.0/30.0; //decrease time by one
		if (countdown <= 0.0) bGameOver = true;
	}

	// track kinect skeleton
	for( int i = 0; i < kinect.getSkeletons().size(); i++) 
	{
		// TRACK OF THE LEFT HAND
		if( kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT) != kinect.getSkeletons().at(i).end() ) 
		{ 
			bStartGame = true;
			
			SkeletonBone wristJoint = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second; 
			SkeletonBone handJoint = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
			
			trackJoint(wristJoint, handJoint, hand_l_x, hand_l_y, hand_l_z);
			hand_l_x *= x_kinect_scale;
			hand_l_x -= 50;
			hand_l_y *= y_kinect_scale;	
		}

		// TRACK OF THE RIGHT HAND
		if( kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT) != kinect.getSkeletons().at(i).end() ) 
		{ 
			SkeletonBone wristJoint = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second; 
			SkeletonBone handJoint = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
		
			trackJoint(wristJoint, handJoint, hand_r_x, hand_r_y, hand_r_z);
			hand_r_x *= x_kinect_scale;
			hand_r_x += 50;
			hand_r_y *= y_kinect_scale;
		}
	}

	countCycles++;
    
	int boxes_temp = 150;
	if (bHardMode)
	{
		boxes_temp = 10;
		countCycles = 0;
	}
	
	// wait some cycles before asking again to create more boxes
	if(countCycles == boxes_temp) {
		bCreateBox = true;
		countCycles = 0;
	}

	// create some color boxes every so often
	if((int)ofRandom(0, 60) == 0 && bCreateBox && bStartGame && !bStartCountdown) {
		
		bCreateBox = false;

		float w = ofRandom(75, 95);
		float h = ofRandom(15, 30);
		
		if(bHardMode)
		{
			w = ofRandom(55, 110);
			h = ofRandom(15, 45);
		}
		
		ofPtr<ColorRect> b = ofPtr<ColorRect>(new ColorRect);
		b.get()->setPhysics(5, 0.1, 1.5);
		b.get()->setColor(); 
		
		int x_range = ofRandom(ofGetWidth()/6 - 10, ofGetWidth()/6 + 60); // range of values for the left stack
		if (bRightTurn && color_boxes_right.size() < max_boxes) // if right turn and right stack not full
		{
			x_range = ofRandom(5*ofGetWidth()/6-60, 5*ofGetWidth()/6+10);
			bRightTurn = false;
		}
		else // if not right turn, next iteration right turn
		{
			bRightTurn = true;
		}

		if (color_boxes_left.size() >= max_boxes) // if left stack is full, always right turn
		{
			x_range = ofRandom(5*ofGetWidth()/6-60, 5*ofGetWidth()/6+10);
		}

		b.get()->setup(box2d.getWorld(), x_range, -h, w, h);
		color_boxes.push_back(b);
	}

	int circles_temp = 150;
	if (bStartCountdown) circles_temp = 90;
	if (bHardMode)
	{
		if (bStartCountdown) circles_temp = 20;
		circles_temp = 60;
	}
	// create some black circles every so often
	if((int)ofRandom(0, circles_temp) == 0 && bStartGame) {	
		float r = ofRandom(12, 30);
		circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
		circles.back().get()->setPhysics(8.0, 0.43, 0.3);
		circles.back().get()->setup(box2d.getWorld(), ofRandom(0, ofGetWidth()), -r, r);
	}

	// add a life when we have less than 3
	if((int)ofRandom(0, 100) == 0 && bCreateLifes && !bLifeCreated) {
		bCreateLifes = false;
		lifePosition.x = ofRandom(life.getWidth()*2, ofGetWidth()-life.getWidth()*2);
		lifePosition.y = - life.getHeight();
		bLifeCreated = true;
	}

	if(bLifeCreated)
	{
		lifePosition.y += 1.5;
	}

	if (lifePosition.y > ofGetHeight()) bLifeCreated = false; // when life is not caught by the user
	
	// Set the position of the LEFT plate and supports
	float temp_left_x = leftPlate.getPosition().x;
	float temp_left_y = leftPlate.getPosition().y;

	leftPlate.setPosition(hand_l_x - 30, hand_l_y + leftPlate.getHeight()/2 - 15);
	//leftSupportR.setPosition(hand_l_x-leftPlate.getWidth()/2 - 30, hand_l_y - leftPlate.getHeight()/2);
	//leftSupportL.setPosition(hand_l_x+leftPlate.getWidth()/2 - 30, hand_l_y - leftPlate.getHeight()/2);

	x_left_movement = leftPlate.getPosition().x - temp_left_x;
	y_left_movement = leftPlate.getPosition().y - temp_left_y;

	// Set the position of the RIGHT plate and supports
	float temp_right_x = rightPlate.getPosition().x;
	float temp_right_y = rightPlate.getPosition().y;
	
	rightPlate.setPosition(hand_r_x + 30, hand_r_y + rightPlate.getHeight()/2 - 15);
	//rightSupportR.setPosition(hand_r_x-rightPlate.getWidth()/2 + 30, hand_r_y - rightPlate.getHeight()/2);
	//rightSupportL.setPosition(hand_r_x+rightPlate.getWidth()/2 + 30, hand_r_y - rightPlate.getHeight()/2);

	x_right_movement = rightPlate.getPosition().x - temp_right_x;
	y_right_movement = rightPlate.getPosition().y - temp_right_y;


	box2d.update();	
	box2d.wakeupShapes();

	for(int i=0; i<color_boxes.size(); i++) 
	{	
		// remove color boxes offscreen
		if(ofxBox2dBaseShape::shouldRemoveOffScreen(color_boxes[i]))
		{
			if (color_boxes[i].get()->getPosition().y > ofGetHeight())
			{
				if (!bGameOver)
				{
					total_lifes--;
					total_points -= 100;
				}
				bLoosePoints = true;
				bWinPoints = false;
				if (total_lifes < 0) bGameOver = true;
				bCreateLifes = true;
				color_boxes.erase(color_boxes.begin()+i);
			}
		}
	}

	//--------LEFT---------
	// move all the LEFT stack together
	int leftStackHeight = leftPlate.getHeight()/2;
	for (int i=0; i<color_boxes_left.size(); i++)
	{
		// check if life object collides with any color_box
		if ( bLifeCreated && !bGameOver && lifeHasCollided(color_boxes_left[i].get()))
		{
			total_lifes++; // if collides we get one more life
			life_sound.play();
			total_points += 100;
			bLoosePoints = false;
			bWinPoints = true;
			bLifeCreated = false;
		}
		
		// check alignment of the boxes in the stack so they can fall
		float pos_x = color_boxes_left[i].get()->getPosition().x;
		float pos_y = color_boxes_left[i].get()->getPosition().y;
		if (i > 0) // if is not the first box
		{
			leftStackHeight += color_boxes_left[i-1].get()->getHeight()/2 + color_boxes_left[i].get()->getHeight()/2; // y position for the next box
			int disaligned_box = allBoxesAligned(color_boxes_left);
			if ( disaligned_box == -1 ) //means all boxes are well aligned
			{
				//color_boxes_left[i].get()->setPosition(pos_x + x_left_movement, pos_y + y_left_movement); // gets disaligned vertically
				color_boxes_left[i].get()->setPosition(pos_x + x_left_movement, leftPlate.getPosition().y - leftStackHeight);
			}
			else // if certain boxes not aligned, we remove all of them from the first not aligned from the stack vector
			{
				bonus_left /= 2; // divide the bonus points by 2
				ofPtr<ColorRect> b = color_boxes_left[disaligned_box]; // move the left_stack boxes vector to the normal_color boxes vector
				color_boxes.push_back(b);
				for (int j=disaligned_box+1; j < color_boxes_left.size(); j++) // remove all upper elements from that one
				{
					ofPtr<ColorRect> b = color_boxes_left[j];
					color_boxes.push_back(b);
				}
				color_boxes_left.erase(color_boxes_left.begin()+disaligned_box, color_boxes_left.end());
				break;
			}
		}
		else // if is the first box check with the plate
		{
			leftStackHeight += color_boxes_left[i].get()->getHeight()/2;
			if ( boxesAligned(&leftPlate, color_boxes_left[i].get(), leftPlate.getWidth()/1.8) ) // if they are aligned we move the box with the stack
			{
				color_boxes_left[i].get()->setPosition(pos_x + x_left_movement, leftPlate.getPosition().y - leftStackHeight);
			}
			else
			{
				bContactLeftPlate = false;
				ofPtr<ColorRect> b = color_boxes_left[i];
				color_boxes.push_back(b);
				for (int j=i+1; j < color_boxes_left.size(); j++)
				{
					ofPtr<ColorRect> b = color_boxes_left[j];
					color_boxes.push_back(b);
				}
				color_boxes_left.erase(color_boxes_left.begin()+i, color_boxes_left.end());
				break;
			}
		}
	}

	//--------RIGHT---------
	// move all the RIGHT stack together
	int rightStackHeight = rightPlate.getHeight()/2;
	for (int i=0; i<color_boxes_right.size(); i++)
	{
		// check if life object collides with any color_box
		if ( bLifeCreated && !bGameOver &&  lifeHasCollided(color_boxes_right[i].get()) )
		{
			total_lifes++; // if collides we get one more life
			life_sound.play();
			total_points += 100;
			bLoosePoints = false;
			bWinPoints = true;
			bLifeCreated = false;
		}
		
		// check alignment of the boxes in the stack so they can fall
		float pos_x = color_boxes_right[i].get()->getPosition().x;
		float pos_y = color_boxes_right[i].get()->getPosition().y;
		if (i > 0) // if is not the first box
		{
			rightStackHeight += color_boxes_right[i-1].get()->getHeight()/2 + color_boxes_right[i].get()->getHeight()/2; // y position for the next box
			int disaligned_box = allBoxesAligned(color_boxes_right);
			if ( disaligned_box == -1 ) //means all boxes are well aligned
			{
				color_boxes_right[i].get()->setPosition(pos_x + x_right_movement, rightPlate.getPosition().y - rightStackHeight);
			}
			else // if certain boxes not aligned, we remove all of them from the first not aligned from the stack vector
			{
				bonus_left /= 2; // divide the bonus points by 2
				ofPtr<ColorRect> b = color_boxes_right[disaligned_box]; // move the right_stack boxes vector to the normal_color boxes vector
				color_boxes.push_back(b);
				for (int j=disaligned_box+1; j < color_boxes_right.size(); j++) // remove all upper elements from that one
				{
					ofPtr<ColorRect> b = color_boxes_right[j];
					color_boxes.push_back(b);
				}
				color_boxes_right.erase(color_boxes_right.begin()+disaligned_box, color_boxes_right.end());
				break;
			}
		}
		else // if is the first box check with the plate
		{
			rightStackHeight += color_boxes_right[i].get()->getHeight()/2;
			if ( boxesAligned(&rightPlate, color_boxes_right[i].get(), rightPlate.getWidth()/1.8) ) // if they are aligned we move the box with the stack
			{
				color_boxes_right[i].get()->setPosition(pos_x + x_right_movement, rightPlate.getPosition().y - rightStackHeight);
			}
			else
			{
				bContactRightPlate = false;
				ofPtr<ColorRect> b = color_boxes_right[i];
				color_boxes.push_back(b);
				for (int j=i+1; j < color_boxes_right.size(); j++)
				{
					ofPtr<ColorRect> b = color_boxes_right[j];
					color_boxes.push_back(b);
				}
				color_boxes_right.erase(color_boxes_right.begin()+i, color_boxes_right.end());
				break;
			}
		}
	}
	
	
	// check if life object collides with rightPlate
	if(bLifeCreated && !bGameOver && lifeHasCollided(&rightPlate))
	{
		total_lifes++;
		life_sound.play();
		total_points += 100;
		bLoosePoints = false;
		bWinPoints = true;
		bLifeCreated = false;
	}

	// check if life object collides with leftPlate
	if(bLifeCreated && !bGameOver && lifeHasCollided(&leftPlate))
	{
		total_lifes++;
		life_sound.play();
		total_points += 100;
		bLoosePoints = false;
		bWinPoints = true;
		bLifeCreated = false;
	}
	
	//remove black circles out of screen
	for(int i=0; i<circles.size(); i++) 
	{
		if(ofxBox2dBaseShape::shouldRemoveOffScreen(circles[i]))
		{
			if (circles[i].get()->getPosition().y > ofGetHeight()) circles.erase(circles.begin()+i);
		}
	}

	if (color_boxes_left.size() >= max_boxes && color_boxes_right.size() >= max_boxes) 
		bStartCountdown = true;

	//std::cout << pointsCycles << std::endl;
	if (!bCounterInited && (bLoosePoints || bWinPoints) )
	{
		pointsCycles = countCycles;
		bCounterInited = true;
	}
	if (countCycles == pointsCycles + 10)
	{
		bLoosePoints = false;
		bWinPoints = false;
	}


}

//--------------------------------------------------------------
void testApp::draw(){

	//ofScale(x_scale, y_scale);
	ofPushMatrix();
	ofSetColor(255);
	ofDisableAlphaBlending();
		ofScale(x_kinect_scale, y_kinect_scale);
		ofSetHexColor(0xFDEFC2);
		max_grayImage.draw(0, 0);
		//ofTranslate(x_translate, y_translate);
		//kinect.draw(0, 0);
		//kinect.drawDepth(0, 0);
	ofEnableAlphaBlending();
	ofPopMatrix();

	// draw skeleton for debug
	/*ofPushStyle();
	ofSetColor(255, 0, 0);
	ofSetLineWidth(3.0f);
	auto skeletons = kinect.getSkeletons();
	for(auto & skeleton : skeletons) {
		for(auto & bone : skeleton) {
			switch(bone.second.getTrackingState()) {
			case SkeletonBone::Inferred:
				ofSetColor(0, 0, 255);
				break;
			case SkeletonBone::Tracked:
				ofSetColor(0, 255, 0);
				break;
			case SkeletonBone::NotTracked:
				ofSetColor(255, 0, 0);
				break;
			}

			auto index = bone.second.getStartJoint();
			auto connectedTo = skeleton.find((_NUI_SKELETON_POSITION_INDEX) index);
			if (connectedTo != skeleton.end()) {
				ofLine(connectedTo->second.getScreenPosition()*x_kinect_scale, bone.second.getScreenPosition()*y_kinect_scale);
			}

			ofCircle(bone.second.getScreenPosition()*x_kinect_scale, 10.0f);
		}
	}
	ofPopStyle();*/

	for(int i=0; i<circles.size(); i++) {
		ofFill();
		ofSetColor(0);
		circles[i].get()->draw();
	}

	for(int i=0; i<color_boxes_left.size(); i++) {
		color_boxes_left[i].get()->draw();
	}

	for(int i=0; i<color_boxes_right.size(); i++) {
		color_boxes_right[i].get()->draw();
	}

	for(int i=0; i<color_boxes.size(); i++) {
		color_boxes[i].get()->draw();
	}

	// brown color for the plates
	ofSetHexColor(0x472A0B);
	
	ofRect(leftPlate.getPosition().x - leftPlate.getWidth()/2, leftPlate.getPosition().y - leftPlate.getHeight()/2, plate_width, plate_height); 
	//leftSupportR.draw();
	//leftSupportL.draw();

	ofRect(rightPlate.getPosition().x - rightPlate.getWidth()/2, rightPlate.getPosition().y - rightPlate.getHeight()/2, plate_width, plate_height);
	//rightSupportR.draw();
	//rightSupportL.draw();
	
	// draw lifes
	for (int i = 0; i <= total_lifes; i++) 
		life.draw(ofGetWidth() - i*life.width - i*10, ofGetHeight() - life.height - 10);

	if(bLifeCreated)
		life.draw(lifePosition.x, lifePosition.y);
	
	//string info = "";
	//info += "FPS: "+ofToString(ofGetFrameRate(), 1)+"\n";
	//info += "TOTAL LIFES: "+ofToString(total_lifes)+"\n";
	//info += "TOTAL POINTS: "+ofToString(total_points)+"\n";
	
	string info = "Mode: ";
	if (bHardMode) info += "Hard Mode";
	else info += "Easy Mode\n";
	ofSetColor(0);
	mode_font.drawString(info, ofGetWidth()-250, 30);
	
	if (bLoosePoints)
	{
		ofSetHexColor(0xC41616);
	}
	else if (bWinPoints)
	{
		ofSetHexColor(0x73B539);
	}
	else
	{
		ofSetColor(0);
	}
	
	points_font.drawString(ofToString(total_points), 50, ofGetHeight() - 15);

	if (bStartCountdown)
	{
		string format = "0:";
		if (countdown <= 10.0)
		{
			format += "0";
			ofSetHexColor(0xC41616);
		}
		else ofSetColor(0, 0, 0);
		countdown_font.drawString(format+ofToString(int(countdown)), ofGetWidth()/2 - countdown_font.stringWidth("0:00")/2, 100);
	}

	if (bGameOver)
	{
		string message;
		if (total_lifes < 0 && total_points < 0)
		{
			message = "GAME OVER";
			ofSetHexColor(0xC41616);
		}
		else
		{
			message = "YOU WIN!";	
			ofSetHexColor(0x73B539);
		}
		gameOver_font.drawString( message, ofGetWidth()/2 - gameOver_font.stringWidth(message)/2, ofGetHeight()/2 - 60 );
		string total = ofToString(total_points) + " points";
		finalpoints_font.drawString( total,  ofGetWidth()/2 - finalpoints_font.stringWidth(total)/2, ofGetHeight()/2);
		finalpoints_font.drawString( "PULSE ENTER TO RESTART", ofGetWidth()/2 - finalpoints_font.stringWidth("PULSE ENTER TO RESTART")/2, ofGetHeight()/2 + 70);
	}
}

void testApp::trackJoint(SkeletonBone bone1, SkeletonBone bone2, float &x, float &y, float &z)
{
	x = bone1.getScreenPosition().x;
	y = bone1.getScreenPosition().y;
	z = bone1.getStartPosition().z;
}

//--------------------------------------------------------------
void testApp::exit()
{
	box2d.disableGrabbing();
	box2d.disableEvents();

	circles.clear();
	color_boxes.clear();
	color_boxes_left.clear();
	color_boxes_right.clear();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	
	if(key == 't')
	{
		float x_scale_temp = ofGetWidth();
		float y_scale_temp = ofGetHeight();
		ofToggleFullscreen();
		x_kinect_scale = ofGetWidth()/640.0;
		y_kinect_scale = ofGetHeight()/480.0;
		x_scale = ofGetWidth()/x_scale_temp;
		y_scale = ofGetHeight()/y_scale_temp;
		//std::cout << x_scale << std::endl;
	}

	if (key == OF_KEY_RETURN)
	{
		if(bGameOver) 
		{
			exit();
			setup();
		}
	}

	if (key == OF_KEY_ESC)
	{
		exit();
	}

	if (key == 'h')
	{
		if (bHardMode) bHardMode = false;
		else bHardMode = true;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

bool testApp::lifeHasCollided(ofxBox2dRect* box)
{
	
	float box_width = box->getWidth();
	float box_height = box->getHeight();
	float box_x = box->getPosition().x;
	float box_y = box->getPosition().y;

	// if the box is one of the plates put the "fake" width and height of the plate and adjust the y position
	if ( box->body == rightPlate.body || box->body == leftPlate.body )
	{
		box_width = plate_width;
		box_height = plate_height;
		box_y = box_y - leftPlate.getHeight()/2;
	}
	
	// check if life object collides with the_box
	float max_x = box_x + box_width/2 + life.getWidth()/2;
	float min_x = box_x - box_width/2 - life.getWidth()/2;
	float max_y = box_y + box_height/2 + life.getHeight()/2;
	float min_y = box_y - box_height/2 - life.getHeight()/2;

	float life_x = lifePosition.x + life.getWidth()/2;
	float life_y = lifePosition.y + life.getHeight()/2;

	if(life_x <= max_x && life_x >= min_x && life_y <= max_y && life_y >= min_y && bLifeCreated)
	{
		return true;
	}
	else return false;
}

bool testApp::boxesAligned(ofxBox2dRect* baseBox, ofxBox2dRect* box, float dist)
{
	float pos_x = baseBox->getPosition().x;
	float pos_y = baseBox->getPosition().y;
	return ( abs(pos_x - box->getPosition().x) < dist );
}

int testApp::allBoxesAligned(vector<ofPtr<ColorRect> >	boxes)
{
	for (int i=2; i<boxes.size(); i++)
	{
		for (int j=i-1; j >= 1; j--)
		{
			if ( !boxesAligned(boxes[j].get(), boxes[i].get(), boxes[j].get()->getWidth()/2.2) )
			{
				return j+1;
			}
		}
	}

	return -1;
}

bool testApp::canBeAttached(ofxBox2dRect* baseBox, ofxBox2dRect* box)
{
	float dist_x = abs(box->getPosition().x - baseBox->getPosition().x);
	float dist_y = abs(box->getPosition().y - baseBox->getPosition().y);
	float min_y = box->getHeight()/2 + baseBox->getHeight()/2;
	
	return ( dist_x < baseBox->getWidth()/4 && dist_y < min_y + 1 );
}