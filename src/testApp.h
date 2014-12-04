#pragma once

#include "ofMain.h"

#include "ofxKinectCommonBridge.h"
#include "ofxBox2d.h"
#include "colorRect.h"
#include "colorCircle.h"
#include "ofxOpenCv.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void trackJoint(SkeletonBone bone1, SkeletonBone bone2, float &x, float &y, float &z);

		// this is the function for contacts
		void contactStart(ofxBox2dContactArgs &e);

		bool lifeHasCollided(ofxBox2dRect* box); // check if the life object has collided a box
		bool boxesAligned(ofxBox2dRect* baseBox, ofxBox2dRect* box, float dist); // check if boxes are horizontally aligned
		int allBoxesAligned(vector<ofPtr<ColorRect> >	 boxes);
		bool canBeAttached(ofxBox2dRect* baseBox, ofxBox2dRect* box); // check if boxes are close enough to be attached into the same stack

		ofxKinectCommonBridge kinect;

		// when the box hits we play this sound
		ofSoundPlayer  box_contact_sound;
		ofSoundPlayer  circle_contact_sound;
		ofSoundPlayer  ambience_music;
		ofSoundPlayer  life_sound;
		
		ofxBox2d								box2d;				//	the box2d world
		
		vector		<ofPtr<ofxBox2dCircle> >	circles;			//	default box2d circles
		
		vector		<ofPtr<ColorRect> >			color_boxes;		//	colored box2d rects

		vector		<ofPtr<ColorRect> >			color_boxes_left;	//	colored box2d rects in the left stack
		vector		<ofPtr<ColorRect> >			color_boxes_right;	//	colored box2d rects in the right stack


		ofxBox2dRect                        rightPlate;				//	right plate to stack the box objects
		ofxBox2dRect						rightSupportR;
		ofxBox2dRect						rightSupportL;

		ofxBox2dRect						leftPlate;				//	left plate to stack the box objects
		ofxBox2dRect						leftSupportR;
		ofxBox2dRect						leftSupportL;

		ofxCvGrayscaleImage					max_grayImage;			// images where we store the user silhouette
		ofxCvGrayscaleImage					min_grayImage;
		
		ofImage								life;					// image for the life objects
		ofVec2f								lifePosition;			// position of the life object
		
		ofTrueTypeFont						points_font;
		ofTrueTypeFont						countdown_font;
		ofTrueTypeFont						gameOver_font;
		ofTrueTypeFont						finalpoints_font;
		ofTrueTypeFont						mode_font;	

		
		float								plate_width;			//	visible width of the plates
		float								plate_height;			//	visible height of the plates
		
		
		int			total_lifes;	// number of lifes remaining
		int			total_points;   // counter of total punctuation
		int			countCycles;	// variable to count frames, to control application flow
		float		time_period;
		float		total_time;
		int			max_boxes;
		int			bonus_left;
		int			bonus_right;
		float		countdown;
		int			pointsCycles;
			
		bool		bHardMode;			// a flag if we want to make the game harder
		bool		bCreateBox;			// a flag for creating a new box
		bool		bStartGame;			// a flag when skeleton detected
		bool		bCreateLifes;		// a flag for creating lifes
		bool		bLifeCreated;		// a flag when life is created
		bool		bContactRightPlate; // a flag when there is a contact between right plate and first box
		bool		bContactLeftPlate;	// a flag when there is a contact between left plate and first box
		bool		bRightTurn;			// a flag to altern the left side and right side of falling boxes
		bool		bStartCountdown;	// a flag when the stacks are full and the game is about to finish
		bool		bGameOver;			// a flag when game finishes (WIN or GAME OVER)
		bool		bLoosePoints;		// a flag to know when we have lost points
		bool		bWinPoints;			// a flag to know when we have won points
		bool		bCounterInited;		// a flag to know when the counter for the points has been initialized


		// tracking values of the right hand
		float hand_r_x;
		float hand_r_y;
		float hand_r_z;

		// tracking values of the left hand
		float hand_l_x;
		float hand_l_y;
		float hand_l_z;

		float x_kinect_scale;	// scaling factor for the kinect to be full screen
		float y_kinect_scale;

		float x_scale;			// scaling factor of all the screen
		float y_scale;

		float x_translate;		// amount of units we have to translate the kinect to center its image
		float y_translate;

		float x_left_movement;	// move difference of the left plate between two frames
		float y_left_movement;

		float x_right_movement; // move difference of the left plate between two frames
		float y_right_movement;
		

};
