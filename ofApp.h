/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		WIDTH = 1280,
		HEIGHT = 720,
	};
	
	/****************************************
	****************************************/
	/********************
	********************/
	int id_Animation;
	
	ofxAssimpModelLoader model;
	ofxAssimpAnimation *modelAnimation;
	
	ofEasyCam		camera;
	bool b_cam;
	
	/********************
	********************/
	ofLight PointLight;
	
	ofImage img_back;
	
	/********************
	GUI
	********************/
	/* */
	ofxPanel gui;
	
	ofxToggle b_DispLight;
	
	/*  */
	ofxGuiGroup guiGp_point;
	ofxColorSlider pointColor_diffuse;
	ofxColorSlider pointColor_specular;
	ofxVec3Slider point_position;
	ofxFloatSlider point_attenuation_constant;
	ofxFloatSlider point_attenuation_linear;
	ofxFloatSlider point_attenuation_quadratic;
	
	/* */
	ofxVec3Slider cam_Target;
	
	/****************************************
	****************************************/
	void setup_gui();
	void apply_gui_parameter();
	
	
public:
	/****************************************
	****************************************/
	ofApp(int _id_Animation = 0);
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
