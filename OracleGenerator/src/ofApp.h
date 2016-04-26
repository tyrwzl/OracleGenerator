#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "binary_mat.hpp"

#include <vector>


class ofApp : public ofBaseApp {

	std::vector< std::vector<ofVec4f> > lines;
	std::vector< std::vector<ofVec3f> > auto_lines;

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void exit();
	void guiEvent(ofxUIEventArgs &e);

	//void drawFbo();

	void setUI();
	void ComplementColorFlying();
	void ComplementNotColoring();
	void colorFill();
	void UpdateDataFile(std::string& datafile_name);
	void ChangeNumberToColor(int joint_number);
	bool CheckPostureDuplication();
	void LoadNextImage();
	 
	// ofxUI
	ofxUISuperCanvas *ui;
	ofxUITextArea *ta1;
	ofxUITextArea *ta2;
	ofxUITextArea *ta3;
	ofxUITextArea *ta4;
	//ofxUITextInput *ti_rad;
	//ofxUITextInput *ti_th;
	ofxUISlider *sl_rad;
	ofxUISlider *sl_th;
	ofxUIRadio *rd_col;
	ofxUIToggle *col_rec;
	ofxUIToggle *tgl_auto;
	int draw_radius;
	int color_hex;
	// In drawing line, need Hex color code
	ofColor color;
	ofVec2f mouse_position;
	ofVec2f cord;

	//auto 
	ofVec4f auto_position;
	//ofVec2f rep_position;
	int flag_auto;
	int rep_depth;
	int th;
	
	// GUI parameter
	//ofxPanel gui;
	//ofxIntSlider radius;
	//ofxLabel mouse_position;
	

	// Binary depth data loader
	BinaryMat bm;

	cv::Mat mat_color;
	cv::Mat buffer;
	//cv::Mat mat_color_rgb;

	// Depth value
	int depth;
	int h, s, v;
	int body_value;
	int auto_value;
	int head_depth;

	// Image
	ofImage backImage1;
	ofImage backImage2;
	ofImage backImage3;
	ofImage zoomImage;
	ofImage depthImage;
	ofImage captureImage;


	// fbo
	//ofFbo lineFbo;

	// flag
	bool flag_save_draw;
	bool flag_show_image;
	bool flag_depth_read;
	bool flag_color;
	bool auto_head;
	bool flag_joint;
	int  joint_number;
	bool flag_alpha;
	//bool flag_auto;
	//bool flag_auto_second;
	//bool flag_auto_drag;

	// pre-joint coordinate
	std::vector<ofVec3f> point_joint;
	std::vector<ofVec3f> joints;
	std::vector<std::vector<ofVec3f>> postures;

	// file name
	std::string file_name;
	std::string number_file;
	int path_number;
};
