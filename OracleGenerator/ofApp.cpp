#include "ofApp.h"

#define _USE_MATH_DEFINES
#include <math.h>

// x, y is display coordinate
inline unsigned short getDepth(int x, int y, cv::Mat& d) {
	if (212 <= x && x <= 724 && 0 <= y && y <= 424)
		return *((unsigned short *)(d.data + d.step[0] * y + d.step[1] * (x - 212)));
	else
		return 0;
}

inline int getColorIndex(int x, int y, cv::Mat& c) {
	if (212 <= x && x <= 724 && 0 <= y && y <= 424)
		return c.step[0] * y + ((x - 212) * 3);
	else
		return 0;
}

//--------------------------------------------------------------
void ofApp::setup() {
	// setup variables
	draw_radius = 10;
	color.setHex(0x000000);
	color_hex = 0xED1A3D;
	zoomImage.allocate(100, 100, OF_IMAGE_COLOR);
	//num_press = 0;

	// init flag
	flag_save_draw = false;
	flag_show_image = false;
	flag_depth_read = false;
	flag_color = false;
	flag_auto = 0;
	body_value = 0;
	flag_joint = false;
	joint_number = 0;
	flag_alpha = false;
	flag_not_color = true;
	flag_fly_color = false;

	path_number = 0;

	point_joint.resize(10);
	joints.resize(10);
	//flag_auto_second = false;
	//flag_auto_drag = false;



	// setup UI
	setUI();
	//ui->loadSettings("settings.xml");

	// setup background
	ofBackground(200, 200, 200);
	backImage1.load("back1.png");
	backImage2.load("back2.png");
	backImage3.load("back3.png");

	////setup fbo
	//lineFbo.allocate(512, 424, GL_RGB);
	//lineFbo.begin();
	//ofClear(255, 255, 255);
	//lineFbo.end();


	// setup GUI
	//gui.setup();
	//gui.add(radius.setup("radius", 10, 0, 50));
	//gui.add(mouse_position.setup("Mouse Position", ofToString(mouseX)+", "+ofToString(mouseY)));


	// set log level
	ofSetLogLevel(OF_LOG_FATAL_ERROR);

	// disable antialiasing
	ofDisableAntiAliasing();

	// loading binary
	//ofSetWindowShape(lb.GetDepthWidth(), lb.GetDepthHeight());
	/*
	if (!lb.LoadMatBinary("/Users/takahiro/of_v0.9.0_osx_release/apps/myApps/mySketch/bin/data/output")){
	ofLog(OF_LOG_NOTICE, "can't load binary");
	}
	lb.ImageGenerator("/Users/takahiro/of_v0.9.0_osx_release/apps/myApps/mySketch/bin/data/output.jpg");
	//depthImage.setImageType(OF_IMAGE_GRAYSCALE);
	if (!depthImage.load("output.jpg")) {
	ofLog(OF_LOG_NOTICE, "can't load image");
	}*/
}

//--------------------------------------------------------------
void ofApp::update() {
	ui->setPosition(0, 0);
	ta1->setTextString("x:" + ofToString(mouse_position.x-212) + " y:" + ofToString(mouse_position.y) + " depth:" + ofToString(depth));
	ta2->setTextString("H:" + ofToString(h) + " S:" + ofToString(s) + " V:" + ofToString(v));
	sl_rad->setValue(draw_radius);
	sl_th->setValue(th);
	col_rec->setValue(flag_auto);
	tgl_auto->setValue(flag_auto);

	cord.x = depth * tan( (mouse_position.x - 256 - 212) * M_PI * 70 / (360 * 256));
	cord.y = depth * tan( (- mouse_position.y + 212)     * M_PI * 60 / (360 * 212));
	ta3->setTextString("X:" + ofToString(cord.x) + " Y:" + ofToString(cord.y) + " Z:" + ofToString(depth));
	ta4->setTextString("FILENAME:"+number_file);
	//ofLog(OF_LOG_FATAL_ERROR, "x:"+ofToString(auto_position.x)+", y:" + ofToString(auto_position.y) + ", z:" + ofToString(auto_position.z) + ", w:" + ofToString(auto_position.w));
	//lineFbo.begin();
	//drawFbo();
	//lineFbo.end();
}

//--------------------------------------------------------------
//void ofApp::drawFbo() {
//
//}

//--------------------------------------------------------------
void ofApp::draw() {
	// draw depth image
	if (flag_show_image) {
		ofSetColor(255);
		depthImage.draw(212, 0);
	}
	depthImage.draw(212, 550);

	int alpha = 100;
	if (flag_alpha)
		alpha = 50;
	else
		alpha = 100;
	// draw lines
 	for (const std::vector<ofVec4f>& line : lines)
		for (unsigned int i = 1; i < line.size(); i++) {
			if (line[i - 1].z == -1) {
				color.setHex(line[i - 1].w);// , alpha);
				ofSetColor(color);
				ofDrawRectangle(line[i - 1].x, line[i - 1].y, 1, 1);
			} if (line[i].z == -1) {
				color.setHex(line[i].w);// , alpha);
				ofSetColor(color);
				ofDrawRectangle(line[i].x, line[i].y, 1, 1);
			}
			else {
				color.setHex(line[i - 1].w);// , alpha);	
				ofSetColor(color);
				ofDrawCircle(line[i - 1].x, line[i - 1].y, line[i - 1].z);
				ofDrawCircle(line[i].x, line[i].y, line[i].z);
				double the = atan((line[i].y - line[i - 1].y) / (line[i].x - line[i - 1].x));

				ofBeginShape();
				ofFill();
				ofVertex(line[i - 1].x + line[i - 1].z * cos(M_PI_2 + the), line[i - 1].y + line[i - 1].z * sin(the + M_PI_2));
				ofVertex(line[i].x + line[i].z * cos(M_PI_2 + the), line[i].y + line[i].z * sin(the + M_PI_2));
				ofVertex(line[i].x + line[i].z * cos(M_PI_2 + M_PI + the), line[i].y + line[i].z * sin(the + M_PI_2 + M_PI));
				ofVertex(line[i - 1].x + line[i - 1].z * cos(M_PI_2 + M_PI + the), line[i - 1].y + line[i - 1].z * sin(the + M_PI_2 + M_PI));
				ofEndShape();
			}
		}

	//for (const std::vector<ofVec3f>& line : auto_lines)
	//	for (unsigned int i = 0; i < line.size(); i++) {
	//		color.setHex(line[i].z);
	//		ofSetColor(color);
	//		ofDrawRectangle(line[i].x, line[i].y, 1, 1);
	//	}


	// for auto
	if (flag_auto == 2 || flag_auto == 3) {
		ofSetColor(255, 0, 0);
		ofFill();
		ofDrawRectangle(auto_position.x - 5, auto_position.y - 5, 10, 10);
		ofDrawRectangle(auto_position.z - 5, auto_position.w - 5, 10, 10);


		ofBeginShape();
		ofNoFill();
		ofVertex(auto_position.x, auto_position.y);
		ofVertex(auto_position.x, auto_position.w);
		ofVertex(auto_position.z, auto_position.w);
		ofVertex(auto_position.z, auto_position.y);
		ofVertex(auto_position.x, auto_position.y);
		ofEndShape();
	}

	if (flag_joint && 0 < joint_number) {
		for (int i = 0; i < joint_number; ++i) {
			ChangeNumberToColor(i);
			ofSetColor(color);
			ofBeginShape();
			ofDrawCircle(point_joint[i].x, point_joint[i].y, 10);
			ofEndShape();
		}
	}

	// draw back image
	ofSetColor(255, 255, 255);
	backImage1.draw(0, 0);
	backImage2.draw(212, 424);
	backImage3.draw(724, 0);




	// save image
	if (flag_save_draw) {
		//ofFileDialogResult saveFileResult = ofSystemSaveDialog(file_name, "Save your file");
		//if (saveFileResult.bSuccess) {
		ofLog(OF_LOG_FATAL_ERROR, "ERROR1");
		captureImage.grabScreen(212, 0, bm.GetDepthWidth(), bm.GetDepthHeight());
		captureImage.save(file_name + "_oracle.png");
		cv::Mat cvColor = cv::Mat(bm.GetDepthHeight(), bm.GetDepthWidth(), CV_8UC3, captureImage.getPixels());
		cv::cvtColor(cvColor, cvColor, cv::COLOR_RGB2BGR);
		bm.SaveMatBinary(file_name + "_oracle", cvColor);
		flag_save_draw = false;
		lines.clear();
		UpdateDataFile(file_name);
		int si = file_name.size();
		// 58:57 → 1文字 59:57 KITA→47
		number_file = file_name.substr(path_number);
		ofLog(OF_LOG_FATAL_ERROR, "ERROR2");

		if (!bm.LoadMatBinary(file_name)) {

		}
		//bm.ImageGenerator(openFileResult.getPath() + ".jpg");
		if (!depthImage.load(file_name + "color.bmp")) {
			ofLog(OF_LOG_FATAL_ERROR, "can't load image");
		}
		else {
			flag_depth_read = true;
			cv::Mat color = cv::imread(file_name + "color.bmp");
			cv::cvtColor(color, color, CV_BGR2RGB);
			depthImage.setFromPixels(color.ptr(), color.cols, color.rows, OF_IMAGE_COLOR, false);
		}
		//depthImage.mirror(false, true);
		flag_show_image = true;
		ofLog(OF_LOG_FATAL_ERROR, "ERROR3");
		mat_color.release();
		buffer.release();
		buffer = cv::Mat::zeros(424, 512, CV_8SC1);
		mat_color = cv::imread(file_name + "color.bmp");
		//cv::medianBlur(mat_color, mat_color_rgb, 5);
		ofLog(OF_LOG_FATAL_ERROR, "ERROR4");
		cv::medianBlur(mat_color, mat_color, 5);
		cv::cvtColor(mat_color, mat_color, CV_BGR2HSV);
		//cv:imshow("color", mat_color);

		flag_joint = true;

	}

	// zoom procedure
	if ((212 < mouse_position.x) && (mouse_position.x < 724) && (25 < mouse_position.y) && (mouse_position.y < 424))
	{
		zoomImage.grabScreen(mouse_position.x - 50, mouse_position.y - 50, 100, 100);
		zoomImage.draw(800, 0, 300, 300);
		ofSetColor(200, 200, 200);
		ofBeginShape();
		ofDrawCircle(950, 150, draw_radius * 3);
		ofEndShape();
	}


}

//--------------------------------------------------------------
void ofApp::exit()
{
	ui->saveSettings("settings.xml");
	delete ui;

}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
	if (e.getName() == "RADIUS")
	{
		ofxUISlider *slider = e.getSlider();
		draw_radius = slider->getScaledValue();
	}
	else if (e.getName() == "COLOR")
	{
		ofxUIRadio *radio = (ofxUIRadio *)e.widget;
		body_value = radio->getValue();
		switch (body_value) {
		case 0:
			color_hex = 0xED1A3D;
			th = 100;
			break;
		case 1:
			color_hex = 0xFFD400;
			break;
		case 2:
			color_hex = 0xB9C42F;
			break;
		case 3:
			color_hex = 0x008000;
			break;
		case 4:
			color_hex = 0x0090A8;
			break;
		case 5:
			color_hex = 0x0067C0;
			break;
		case 6:
			color_hex = 0x233B6C;
			break;
		case 7:
			color_hex = 0xA757A8;
			break;
		case 8:
			color_hex = 0xA52A2A;
			break;
		case 9:
			color_hex = 0xF58F98;
			break;
		case 10:
			color_hex = 0xC8C8C8;
			break;
		default:
			color_hex = 0x000000;
			break;
		}

	}
	else if (e.getName() == "AUTO") {
		ofxUIToggle *toogle = (ofxUIToggle *)e.getToggle();
		if (toogle->getValue()) {
			flag_auto = 1;
			ofLog(OF_LOG_FATAL_ERROR, "START AUTO 1");
		}
		else {
			flag_auto = 0;
			ofLog(OF_LOG_FATAL_ERROR, "End AUTO 0");
		}
	}
	else if (e.getName() == "thre") {
		ofxUISlider *slider = e.getSlider();
		th = slider->getScaledValue();
	}
	else if (e.getName() == "MINPLU") {
		ofxUIRadio *radio = (ofxUIRadio *)e.widget;
		auto_value = radio->getValue();
		ofLog(OF_LOG_FATAL_ERROR, ofToString(auto_value));
	}
	else if (e.getName() == "COLOR RECOG") {
		ofxUIToggle *toogle = (ofxUIToggle *)e.getToggle();
		if (toogle->getValue()) {
			flag_color = 1;
		}
		else {
			flag_color = 0;
		}
	}
	else if (e.getName() == "NOT COLORING") {
		ofxUIToggle *toogle = (ofxUIToggle *)e.getToggle();
		if (toogle->getValue()) {
			flag_not_color = 1;
		}
		else {
			flag_not_color = 0;
		}
	}
	else if (e.getName() == "FLY COLORING") {
		ofxUIToggle *toogle = (ofxUIToggle *)e.getToggle();
		if (toogle->getValue()) {
			flag_fly_color = 1;
		}
		else {
			flag_fly_color = 0;
		}
	}
	//else if (e.getName() == "ti_rad") {
	//	ofxUITextInput *ti = (ofxUITextInput *)e.widget;
	//	if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
	//	draw_radius = ofToInt(ti->getTextString());
	//}
	//else if (e.getName() == "ti_th") {
	//	ofxUITextInput *ti = (ofxUITextInput *)e.widget;
	//	if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
	//		th = ofToInt(ti->getTextString());
	//}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == ('z' - 'a' + 1))
	{
		if (flag_joint) {
			--joint_number;
		}
		else if (lines.size())
			lines.pop_back();
	}
	if (key == 'c') {
		lines.clear();
		buffer.release();
		buffer = cv::Mat::zeros(424, 512, CV_8SC1);
	}
	if (key == 's') {
		flag_save_draw = true;
		flag_show_image = false;
	}
	if (key == 'h')
		flag_show_image = !flag_show_image;
	if (key == ' ') {
		//Open the Open File Dialog
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a binary file");

		//Check if the user opened a file
		if (openFileResult.bSuccess) {
			//ofLogVerbose("User selected a file" + openFileResult.getPath());
			file_name = openFileResult.getPath();
			number_file = openFileResult.getName();
			path_number = file_name.size() - number_file.size();

			if (!bm.LoadMatBinary(openFileResult.getPath())) {
				//ofLog(OF_LOG_NOTICE, "can't load binary");
			}
			//bm.ImageGenerator(openFileResult.getPath() + ".jpg");
			string s = openFileResult.getPath() + "color.bmp";
			if (!depthImage.loadImage(s)) {
				//ofLog(OF_LOG_NOTICE, "can't load image");
			}
			else {
				flag_depth_read = true;
				cv::Mat color = cv::imread(openFileResult.getPath() + "color.bmp");
				cv::cvtColor(color, color, CV_BGR2RGB);
				depthImage.setFromPixels(color.ptr(), color.cols, color.rows, OF_IMAGE_COLOR, false);
			}
			//depthImage.mirror(false, true);
			flag_show_image = true;
			mat_color = cv::imread(openFileResult.getPath() + "color.bmp");
			buffer = cv::Mat::zeros(424, 512, CV_8SC1);
			//cv::imshow("color", mat_color);
			//cv::medianBlur(mat_color, mat_color_rgb, 5);
			cv::medianBlur(mat_color, mat_color, 5);
			cv::cvtColor(mat_color, mat_color, CV_BGR2HSV);
			//cv::imshow("color", mat_color);
		}
		else {
			ofLogVerbose("User hit cancel");
		}
		flag_joint = true;

	}
	if (key == 'b') {
		flag_auto = 1;
		flag_color = true;
	}
	if (key == 'a') {
		if (flag_auto) {
			flag_auto = 0;
		}
		else {
			flag_auto = 1;
		}
	}
	if (key == 'd') {
		if (auto_head) {
			auto_head = 0;
		}
		else {
			auto_head = 1;
		}
	}
	if (key == 'r') {
		color_hex = 0xFFD400;
		flag_auto = 0;
	}
	if (key == 'l') {
		color_hex = 0xB9C42F;
		flag_auto = 0;
	}
	if (key == 'v') {
		if (flag_alpha)
			flag_alpha = true;
		else
			flag_alpha = false;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	mouse_position.x = x;
	mouse_position.y = y;
	if (flag_depth_read) {
		if (212 <= x || x <= 724 || 0 <= y || y <= 424)
		depth = getDepth(x, y, bm.GetDepthData());//*((unsigned short *)(bm.GetDepthData().data + bm.GetDepthData().step[0] * (int)mouse_position.y + bm.GetDepthData().step[1] * (int)(mouse_position.x-212)));
		int a = getColorIndex(x, y, mat_color);
		//int a = mat_color.step[0] * y + ((x - 212) * 3);
		h = mat_color.data[a];
		s = mat_color.data[a + 1];
		v = mat_color.data[a + 2];
	}
	//ofLog(OF_LOG_FATAL_ERROR, ofToString(depth));

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	if (flag_auto == 0 && flag_joint == 0) {
		//lines.push_back(std::vector<ofVec4f>());
		if (button == 0)
			(lines.end() - 1)->push_back(ofVec4f(x, y, draw_radius, color_hex));
		else if (button == 2)
			(lines.end() - 1)->push_back(ofVec4f(x, y, draw_radius, 0xC8C8C8));
	}
	mouse_position.x = x;
	mouse_position.y = y;
	if (flag_depth_read) {
		depth = getDepth(x, y, bm.GetDepthData());//(short)*((unsigned short *)(bm.GetDepthData().data + bm.GetDepthData().step[0] * (int)mouse_position.y + bm.GetDepthData().step[1] * (int)(mouse_position.x-212)));
		int a = getColorIndex(x, y, mat_color);//mat_color.step[0] * y + ((x - 212) * 3);
		h = mat_color.data[a];
		s = mat_color.data[a + 1];
		v = mat_color.data[a + 2];
	}
	//ofLog(OF_LOG_FATAL_ERROR, ofToString(depth));
	if (flag_auto == 2) {
		if ((212 < mouse_position.x) && (mouse_position.x < 724) && /*(25 < mouse_position.y) && */(mouse_position.y < 424)) {
			auto_position.z = x;
			auto_position.w = y;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (flag_joint) {
		if (212 <= x && x <= 724 && 0 <= y && y <= 424) {
			int depth = (int)getDepth(x, y, bm.GetDepthData());
			point_joint[joint_number].x = x;
			point_joint[joint_number].y = y;
			point_joint[joint_number].z = depth;
			joints[joint_number].x = depth * tan((x - 256 - 212) * M_PI * 70 / (360 * 256));
			joints[joint_number].y = depth * tan((-y + 212)      * M_PI * 60 / (360 * 212));
			joints[joint_number].z = depth;
			//joints[joint_number].w = 
			if (joint_number == 9) {
				joint_number = 0;
				flag_joint = false;
				if (CheckPostureDuplication())
					//TODO:color Filling
					colorFill();
				else
					LoadNextImage();
			}
			else joint_number++;
		}
	}
	else if (flag_auto == 0) 
		lines.push_back(std::vector<ofVec4f>());
	else if (flag_auto == 1) {
		if ((212 < mouse_position.x) && (mouse_position.x < 724) && /*(25 < mouse_position.y) && */ (mouse_position.y < 424)) {
			auto_position.x = x;
			auto_position.y = y;
			flag_auto = 2;
			ofLog(OF_LOG_FATAL_ERROR, "START AUTO 2");
		}
	}
	else if (flag_auto == 3) {
		if ((212 < mouse_position.x) && (mouse_position.x < 724) && /*(25 < mouse_position.y) && */(mouse_position.y < 424)) {
			rep_depth = getDepth(x, y, bm.GetDepthData());
			for (int i = auto_position.y; i < auto_position.w; ++i) {
				for (int j = auto_position.x; j < auto_position.z; ++j) {
					int d = getDepth(j, i, bm.GetDepthData());
					switch (auto_value)
					{
					case 0:
						if (((rep_depth - th) < d) && (d < (rep_depth))) {
							(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
							ofLog(OF_LOG_FATAL_ERROR, "0");
						}
						break;
					case 1:
						if (((rep_depth - th) < d) && ((d < (rep_depth + th)))) {
							(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
							ofLog(OF_LOG_FATAL_ERROR, "1");
						}
						break;
					case 2:
						if (((rep_depth) < d) && (d < (rep_depth + th))) {
							(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
							ofLog(OF_LOG_FATAL_ERROR, "2");
						}
						break;
					default:
						if (((rep_depth - th) < d) && ((d < (rep_depth + th)))) {
							(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
							ofLog(OF_LOG_FATAL_ERROR, "HIT");
						}
						break;
					}
				}
			}
			flag_auto = 1;
			ofLog(OF_LOG_FATAL_ERROR, "START AUTO 1");
		}
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (flag_color) {
		flag_color = false;
		//TODO:color fill
		colorFill();
		//cout << "Color Recognition START!!" << endl;
		//
		//int min[2] = { 700, 400 };
		//int max[2] = { 300, 20  };

		//lines.push_back(std::vector<ofVec4f>());
		//// y-axis
		//for (int i = 20; i < 400; ++i) {
		//	// x-axis
		//	for (int j = 310; j < 700; ++j) {
		//		int a = mat_color.step[0] * i + ((j - 212 ) * 3);
		//		int d = getDepth(j, i, bm.GetDepthData());

		//		if (500 < d && d < 1100) {
		//			if (i < 200) {
		//				color_hex = 0xFFB74C;
		//			}
		//			else if (j < 500) {
		//				color_hex = 0xA52A2A;
		//			}
		//			else {
		//				color_hex = 0xF58F98;
		//			}
		//			if (mat_color.data[a] >= 10 && mat_color.data[a] <= 25 && mat_color.data[a + 1] < 100 && 100 < mat_color.data[a + 2])
		//			{
		//				if ( j < 500)
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
		//				else 
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
		//			}
		//			else if (mat_color.data[a] >= 10 && mat_color.data[a] <= 25 && mat_color.data[a + 1] < 190 && 100 < mat_color.data[a + 2] && mat_color.data[a + 2] < 200) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
		//			}
		//		}

		//		if (500 < d && d < 1100) {
		//			if (50 < mat_color.data[a]) {
		//				if ((mat_color.data[a] > 174) && (mat_color.data[a + 2] > 230) && (mat_color.data[a + 1] > 130)) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x233B6C));
		//				}
		//				else if ((mat_color.data[a] > 150) && (mat_color.data[a] < 174) && (mat_color.data[a + 2] > 250)) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xED1A3D));
		//					// head@ˆ—
		//					 min[0] = std::min(j, min[0]);
		//					 min[1] = std::min(i, min[1]);
		//					 max[0] = std::max(j, max[0]);
		//					 max[1] = std::max(i, max[1]);
		//				}
		//				else if ((mat_color.data[a] > 125) && (mat_color.data[a] < 131) && (mat_color.data[a + 1] > 100)) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xB9C42F));
		//				}
		//				else if ((mat_color.data[a] > 104) && (mat_color.data[a] < 117) && (mat_color.data[a + 1] > 237)) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xFFD400));
		//				}
		//				else if ((mat_color.data[a] > 93) && (mat_color.data[a] < 105) && (mat_color.data[a + 1] > 40) && (mat_color.data[a + 2] > 130)) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0067C0));
		//				}
		//				else if ((65 < mat_color.data[a]) && (mat_color.data[a] < 90) && (mat_color.data[a + 1] > 150)) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0090A8));
		//				}
		//			}
		//			else {
		//				if ((d < 810) && (10 < mat_color.data[a]) && (mat_color.data[a] < 30) && (140 < mat_color.data[a + 1]) && (250 < mat_color.data[a + 2])) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xA757A8));
		//				}
		//				else if ((d < 810) && (28 < mat_color.data[a]) && (mat_color.data[a] < 35) && (30 < mat_color.data[a + 1]) && (mat_color.data[a + 1] < 150) && (250 < mat_color.data[a + 2])) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xA757A8));
		//				}
		//				else if ((1000 < d) && (20 < mat_color.data[a]) && (mat_color.data[a] < 33) && (100 < mat_color.data[a + 2])) {
		//					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x008000));
		//				}
		//			}
		//		}
		//	}
		//}
		//cout << min[0] << ","<< min[1] << "," << max[0] <<","<< max[1] << endl;

		//int depth_x = (min[0] + max[0]) / 2;
		//int depth_y = (min[1] + max[1]) / 2;
		//head_depth = getDepth(depth_x, depth_y, bm.GetDepthData());
		//for (int l = min[1]; l < max[1]; ++l) {
		//	for (int m = min[0]; m < max[0]; ++m) {
		//		int d = getDepth(m, l, bm.GetDepthData());
		//		if (((head_depth - 100) < d) && ((d < (head_depth + 100)))) {
		//			(lines.end() - 1)->push_back(ofVec4f(m, l, -1, 0xED1A3D));
		//		}
		//	}
		//}
		//color_hex = 0xED1A3D;

	}
	else if (flag_auto == 2) {
		if (body_value == 1 || body_value == 9 || body_value == 10) {
			flag_auto = 1;
			lines.push_back(std::vector<ofVec4f>());
			for (int i = auto_position.y; i < auto_position.w; ++i) {
				for (int j = auto_position.x; j < auto_position.z; ++j) {
					int a = getColorIndex(x, y, mat_color);//mat_color.step[0] * i + ((j - 212) * 3);
					int d = getDepth(j, i, bm.GetDepthData());

					if (500 < d && d < 1100) {
						if (mat_color.data[a] >= 10 && mat_color.data[a] <= 25 && mat_color.data[a + 1] < 100 && 100 < mat_color.data[a + 2])
						{
							(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
						}
						else if (mat_color.data[a] >= 10 && mat_color.data[a] <= 25 && mat_color.data[a + 1] < 190 && 100 < mat_color.data[a + 2] && mat_color.data[a + 2] < 200) {
							(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
						}
					}
				}
			}
		}
		else {
			flag_auto = 3;
			ofLog(OF_LOG_FATAL_ERROR, "START AUTO 3");
			//auto_lines.push_back(std::vector<ofVec3f>());
			lines.push_back(std::vector<ofVec4f>());
		}
	}
	//if (flag_auto_second) {
	//	auto_lines.push_back(std::vector<ofVec3f>());
	//	std::vector<int> accum;
	//	int mean;
	//	for (int i = auto_position.y; i < auto_position.w; ++i) {
	//		for (int j = auto_position.x; j < auto_position.z; ++j) {
	//			accum.push_back(getDepth(j, i, bm.GetDepthData()));
	//		}
	//	}
	//	mean = std::accumulate(accum.begin(), accum.end(), 0) / accum.size();
	//	ofLog(OF_LOG_FATAL_ERROR, ofToString(mean));
	//	for (int i = auto_position.y; i < auto_position.w; ++i) {
	//		for (int j = auto_position.x; j < auto_position.z; ++j) {
	//			int d = getDepth(j, i, bm.GetDepthData());
	//			if ( ( (mean-10) < d) && ( (d < (mean+10)))) {
	//				(auto_lines.end() - 1)->push_back(ofVec3f(j, i, color_hex));
	//				ofLog(OF_LOG_FATAL_ERROR, "HIT");
	//			}
	//		}
	//	}
	//}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::setUI() {

	vector<string> names;
	names.push_back("head : RED");
	//names.push_back("neck : ORANGE");
	names.push_back("R-shoulder : YELLOW");
	names.push_back("L-shoulder : YELLOWGREEN");
	names.push_back("body : GREEN");
	names.push_back("R-upper arm : BLUEGREEN");
	names.push_back("L-upper arm : BLUE");
	names.push_back("R-lower arm : INDIGO");
	names.push_back("L-lower arm : PURPLE");
	names.push_back("R-hand : BROWN");
	names.push_back("L-hand : PINK");
	names.push_back("ERASE");

	vector<string> vec_auto;
	vec_auto.push_back("-");
	vec_auto.push_back("-/+");
	vec_auto.push_back("+");

	ui = new ofxUISuperCanvas("SETTINGS");

	ui->addSpacer();
	ui->addLabel("DRAW RADIUS");
	sl_rad = ui->addSlider("RADIUS", 1.0, 50, 10.0);
	//ti_rad = ui->addTextInput("ti_rad", "");
	//ti_rad->setAutoClear(false);



	ui->addSpacer();
	rd_col = ui->addRadio("COLOR", names, OFX_UI_ORIENTATION_VERTICAL);

	ui->addSpacer();
	tgl_auto = ui->addToggle("AUTO", false);
	ui->addRadio("MINPLU", vec_auto, OFX_UI_ORIENTATION_HORIZONTAL);

	ui->addSpacer();
	ui->addLabel("THRESHOLD");
	sl_th = ui->addSlider("thre", 1, 100, 10);
	//ti_th = ui->addTextInput("ti_th", "");
	//ti_th->setAutoUnfocus(false);

	ui->addSpacer();
	ui->addLabel("INFO");
	ta1 = ui->addTextArea("INFO", "x:0 y:0 depth:0", OFX_UI_FONT_SMALL);
	ui->addSpacer();
	ta2 = ui->addTextArea("info", "H:0 S:0 V:0", OFX_UI_FONT_SMALL);

	ui->addSpacer();
	ta3 = ui->addTextArea("info", "X:0 Y:0 Z:0", OFX_UI_FONT_SMALL);

	ui->addSpacer();
	col_rec = ui->addToggle("COLOR RECOG", false);

	ui->addSpacer();
	ta4 = ui->addTextArea("info", "FILENAME:", OFX_UI_FONT_SMALL);

	ui->addSpacer();
	not_color = ui->addToggle("NOT COLORING", true);
	fly_color = ui->addToggle("FLY COLORING", false);

	ui->autoSizeToFitWidgets();
	ui->setPosition(0, 0);

	ofAddListener(ui->newGUIEvent, this, &ofApp::guiEvent);

}

void ofApp::ComplementColorFlying()
{

	for (int i = 0; i < 350; ++i) {
		for (int j = 360; j < 612; ++j) {
			int a = getColorIndex(j, i, mat_color);//mat_color.step[0] * i + ((j - 212) * 3);
			int c = buffer.step[0] * i + (j - 212);
			int d = getDepth(j, i, bm.GetDepthData());

			int color_array[12] = { 0 };

			if (mat_color.data[a] < 2 && mat_color.data[a + 1] < 2) {

				for (int yi = -9; yi < 10; ++yi) {
					for (int xi = -9; xi < 10; ++xi) {
						if (i + yi < 0 || 423 < i + yi || j - 212 + xi < 0 || 511 < j - 212 + xi) continue;
						int b = buffer.step[0] * (i + yi) + (j + xi - 212);
						int d_temp = getDepth(j + xi, i + yi, bm.GetDepthData());
						if (2 < buffer.data[b] && buffer.data[b] < 8 && abs(d-d_temp) < 10)
						color_array[buffer.data[b]]++;
					}
				}
				int max = 0;
				int max_i = 0;
				for (int ci = 0; ci < 12; ++ci) {
					if (max < color_array[ci]) {
						max = color_array[ci];
						max_i = ci;
					}
				}

				switch (max_i)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x008000));
					buffer.data[c] = 3;
					break;
				case 4:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0090A8));
					buffer.data[c] = 4;
					break;
				case 5:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0067C0));
					buffer.data[c] = 5;
					break;
				case 6:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x233B6C));
					buffer.data[c] = 6;
					break;
				case 7:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xA757A8));
					buffer.data[c] = 7;
					break;
				case 8:
					break;
				default:
					break;
				}
			}
		}
	}
}

void ofApp::ComplementNotColoring() {
	for (int i = 0; i < 350; ++i) {
		for (int j = 360; j < 612; ++j) {
			int c = buffer.step[0] * i + (j - 212);
			int d = getDepth(j, i, bm.GetDepthData());

			if (buffer.data[c] == 0 && d < 1000) {
				int a = getColorIndex(j, i, mat_color);//mat_color.step[0] * i + ((j - 212) * 3);
				int h = mat_color.data[a];
				int s = mat_color.data[a];
				int v = mat_color.data[a];

				int color_array[12] = { 0 };

				for (int yi = -9; yi < 10; ++yi) {
					for (int xi = -9; xi < 10; ++xi) {
						if (i + yi < 0 || 423 < i + yi || j - 212 + xi < 0 || 511 < j - 212 + xi) continue;
						int a_temp = getColorIndex(j+xi, i+yi, mat_color);//mat_color.step[0] * (i+yi) + ((j+xi - 212) * 3);
						int dist_hsv = abs(h - mat_color.data[a_temp]) + abs(s - mat_color.data[a_temp + 1]) + abs(v - mat_color.data[a_temp + 2]);
						int b = buffer.step[0] * (i + yi) + (j + xi - 212);
						int d_temp = getDepth(j + xi, i + yi, bm.GetDepthData());
						if (2 < buffer.data[b] && buffer.data[b] < 8 && abs(d - d_temp) < 20 && dist_hsv < 200)
							color_array[buffer.data[b]]++;
					}
				}
				int max = 0;
				int max_i = 0;
				for (int ci = 0; ci < 12; ++ci) {
					if (max < color_array[ci]) {
						max = color_array[ci];
						max_i = ci;
					}
				}

 				switch (max_i)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x008000));
					buffer.data[c] = 3;
					break;
				case 4:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0090A8));
					buffer.data[c] = 4;
					break;
				case 5:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0067C0));
					buffer.data[c] = 5;
					break;
				case 6:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x233B6C));
					buffer.data[c] = 6;
					break;
				case 7:
					(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xA757A8));
					buffer.data[c] = 7;
					break;
				case 8:
					break;
				default:
					break;
				}
			}
		}
	}
}

void ofApp::colorFill()
{
	/*
	lines.push_back(std::vector<ofVec4f>());
	for (int i = 0; i < 10; ++i) {
		/*
		int mean_h = 0;
		int mean_s = 0;
		int mean_v = 0;
		int add_number = 0;
		for (int yi = 0; yi < 5; ++yi) {
			for (int xi = 0; xi < 5; ++xi) {
				if (abs(point_joint[joint_number].z - getDepth(point_joint[joint_number].x + xi - 2, point_joint[joint_number].y + yi - 2, bm.GetDepthData())) < 100) {
					int a = getColorIndex(point_joint[joint_number].x + xi - 2, point_joint[joint_number].y + yi - 2, mat_color);
					mean_h += mat_color.data[a];
					mean_s += mat_color.data[a + 1];
					mean_v += mat_color.data[a + 2];
					++add_number;
				}
			}
		}
		mean_h = mean_h / add_number;
		mean_s = mean_s / add_number;
		mean_v = mean_v / add_number;
		
		
		for (int yi = 0; yi < 100; ++yi) {
			for (int xi = 0; xi < 100; ++xi) {
				if (abs(point_joint[joint_number].z - getDepth(point_joint[joint_number].x + xi - 49, point_joint[joint_number].y + yi - 49, bm.GetDepthData())) < 100) {
					int x = point_joint[joint_number].x + xi - 49;
					int y = point_joint[joint_number].y + yi - 49;
					int a = getColorIndex(point_joint[joint_number].x + xi - 49, point_joint[joint_number].y + yi - 49, mat_color);
					switch (i) {
					case 2:
						if ((mat_color.data[a] > 155) && (mat_color.data[a] < 174) && (mat_color.data[a + 2] > 180)) 
							(lines.end() - 1)->push_back(ofVec4f(x, y, -1, 0x008000));
						break;
					}
					/*
					int dist = 0;
					for (int ai = 0; ai < 3; ai++)
						dist += abs(mean_h - mat_color.data[a + ai]);
					if (dist < 100) {
						ChangeNumberToColor(joint_number);
						(lines.end() - 1)->push_back(ofVec4f(point_joint[joint_number].x + xi - 49, point_joint[joint_number].y + yi - 49, -1, color.getHex()));
					}
					else {
						ChangeNumberToColor(joint_number);
					}
					
				}
			}
		}

	}*/
	
	cout << "Color Recognition START!!" << endl;
	color_hex = 0xED1A3D;

	int min[2] = { 700, 400 };
	int max[2] = { 300, 20 };

	lines.push_back(std::vector<ofVec4f>());
	// y-axis
	for (int i = 0; i < 350; ++i) {
		// x-axis
		for (int j = 360; j < 612; ++j) {
			int a = getColorIndex(j, i, mat_color);//mat_color.step[0] * i + ((j - 212) * 3);
			int b = buffer.step[0] * i + (j - 212);
			int d = getDepth(j, i, bm.GetDepthData());

			//// ”§F
			//if (500 < d && d < 1100) {
			//	if (i > 212) {
			//		if (j < 500) {
			//			// ‰EŽè
			//			color_hex = 0xA52A2A;
			//		}
			//		else {
			//			// ¶Žè
			//			color_hex = 0xF58F98;
			//		}
			//		if (mat_color.data[a] >= 10 && mat_color.data[a] <= 25 && mat_color.data[a + 1] < 100 && 100 < mat_color.data[a + 2])
			//		{
			//			if (j < 500)
			//				(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
			//			else
			//				(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
			//		}
			//	}
			//	if (mat_color.data[a] >= 10 && mat_color.data[a] <= 25 && 50 < mat_color.data[a+1] && mat_color.data[a + 1] < 190 && 100 < mat_color.data[a + 2] && mat_color.data[a + 2] < 200) {
			//		(lines.end() - 1)->push_back(ofVec4f(j, i, -1, color_hex));
			//	}
			//}

			if (500 < d ) {
				
					// Ô‚¢•ž 0x233B6C
					if ((mat_color.data[a] > 174) && (mat_color.data[a + 2] > 230) && (mat_color.data[a + 1] > 130)) {
						//(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xA757A8));
					}
					// ƒsƒ“ƒN•ž
					else if ((mat_color.data[a] > 155) && (mat_color.data[a] < 174) && (mat_color.data[a + 2] > 180)) {
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x008000));
						buffer.data[b] = 3;
						//(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xED1A3D));
						// head@ˆ—
						//if (i < 200) {
						//	min[0] = std::min(j, min[0]);
						//	min[1] = std::min(i, min[1]);
						//	max[0] = std::max(j, max[0]);
						//	max[1] = std::max(i, max[1]);
						//}
					}

					// ‚Þ‚ç‚³‚« 0xB9C42F
					else if ((125 < mat_color.data[a]) && (mat_color.data[a] < 149) && (60 < mat_color.data[a + 1]) && (90 < mat_color.data[a+2])) {
						//(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xFFD400));
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0090A8)); 
						buffer.data[b] = 4;
					}
					// ‚ ‚¨ 0xFFD400
					else if ((90 < mat_color.data[a]) && (mat_color.data[a] < 117) && (90 < mat_color.data[a+1]) && (100 < mat_color.data[a + 2])) {
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0067C0));
						buffer.data[b] = 5;
					}
					else if ((90 < mat_color.data[a]) && (mat_color.data[a] < 117) && (mat_color.data[a + 1] < 100) && (250 < mat_color.data[a + 2])) {
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0067C0));
						buffer.data[b] = 5;
					}
					// …F 0x0067C0
					else if ((mat_color.data[a] > 93) && (mat_color.data[a] < 105) && (mat_color.data[a + 1] > 40) && (mat_color.data[a + 2] > 130)) {
						//(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x0090A8));
					}
					// ‚Ý‚Ç‚è 0x0090A8
					else if ((60 < mat_color.data[a]) && (mat_color.data[a] < 90) && (70 < mat_color.data[a+1]) && (170 < mat_color.data[a + 2])) {
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0xA757A8));
						buffer.data[b] = 7;
					}
				
				
					// ƒIƒŒƒ“ƒW 0xA757A8
					if ((10 < mat_color.data[a]) && (mat_color.data[a] < 33) && (100 < mat_color.data[a + 1]) && (250 < mat_color.data[a + 2])) {
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x233B6C));
						buffer.data[b] = 6;
					}
					else if ((d < 810) && (28 < mat_color.data[a]) && (mat_color.data[a] < 35) && (30 < mat_color.data[a + 1]) && (mat_color.data[a + 1] < 150) && (250 < mat_color.data[a + 2])) {
						(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x233B6C));
						buffer.data[b] = 6;
					}
					// ‰©F
					else if ((1000 < d) && (20 < mat_color.data[a]) && (mat_color.data[a] < 33) && (100 < mat_color.data[a + 2])) {
						//(lines.end() - 1)->push_back(ofVec4f(j, i, -1, 0x008000));
					}
				
			}
		}
	}
	//cout << min[0] << "," << min[1] << "," << max[0] << "," << max[1] << endl;
	//int depth_x = (min[0] + max[0]) / 2;
	//int depth_y = (min[1] + max[1]) / 2;
	//head_depth = getDepth(depth_x, max[1], bm.GetDepthData());
	//if (!auto_head) {
	//	for (int l = min[1]; l < max[1]; ++l) {
	//		for (int m = min[0]; m < max[0]; ++m) {
	//			int d = getDepth(m, l, bm.GetDepthData());
	//			if (((head_depth - 100) < d) && ((d < (head_depth + 100)))) {
	//				(lines.end() - 1)->push_back(ofVec4f(m, l, -1, 0xED1A3D));
	//			}
	//		}
	//	}
	//	color_hex = 0xED1A3D;
	//}
	color_hex = 0xED1A3D;
	if (flag_not_color)
		ComplementColorFlying();
	if (flag_fly_color)
		ComplementNotColoring();
}

void ofApp::UpdateDataFile(std::string& datafile_name)
{
	int len = datafile_name.size();
	char last_char[2];
	last_char[0] = datafile_name.back();
	if (last_char[0] == '9') {
		char c;
		c = *(datafile_name.c_str() + (len-2));
		if (c == '9') {
			c = *(datafile_name.c_str() + (len - 3));
			++c;
			char ch[4];
			ch[0] = c;
			ch[1] = '0';
			ch[2] = '0';
			ch[3] = '\0';
			datafile_name.replace(len - 3, 3, ch);
		}
		else if (c == '\\') {
			char ch[3];
			ch[0] = '1';
			ch[1] = '0';
			ch[2] = '\0';
			datafile_name.replace(len - 1, 2, ch);
		}
		else {
			++c;
			char ch[3];
			ch[0] = c;
			ch[1] = '0';
			ch[2] = '\0';
			datafile_name.replace(len - 2, 2, ch);
		}
		//datafile_name.replace()
	}
	else {
		++last_char[0];
		last_char[1] = '\0';
		datafile_name.replace(len - 1, 1, last_char);
	}
}

void ofApp::ChangeNumberToColor(int joint_number)
{
	switch (joint_number)
	{
	case 0:
		color.setHex(0xed1a3d);
		break;
	case 1:
		color.setHex(0xffd400);
		break;
	case 2:
		color.setHex(0xb9c42f);
		break;
	case 3:
		color.setHex(0x008000);
		break;
	case 4:
		color.setHex(0x0090a8);
		break;
	case 5:
		color.setHex(0x0067c0);
		break;
	case 6:
		color.setHex(0x233b6c);
		break;
	case 7:
		color.setHex(0xa757a8);
		break;
	case 8:
		color.setHex(0xa52a2a);
		break;
	case 9:
		color.setHex(0xf58f98);
		break;
	default:
		break;
	}
}

bool ofApp::CheckPostureDuplication()
{
	if (postures.size() == 0) {
		postures.push_back(joints);
		return true;
	}



	for (const std::vector<ofVec3f>& pi: postures) {
		bool isSimilar = true;

		for (int i = 0; i < 10; ++i) {
			double dist = (joints[i].x - pi[i].x) * (joints[i].x - pi[i].x) + (joints[i].y - pi[i].y) * (joints[i].y - pi[i].y) + (joints[i].z - pi[i].z) * (joints[i].z - pi[i].z);
			if (dist > 2500) {
				isSimilar = false;
				break;
			}
		}

		if (isSimilar)
			return false;
	}

	postures.push_back(joints);
	return true;
}

void ofApp::LoadNextImage()
{
		//ofFileDialogResult saveFileResult = ofSystemSaveDialog(file_name, "Save your file");
		//if (saveFileResult.bSuccess) {
		//captureImage.grabScreen(212, 0, bm.GetDepthWidth(), bm.GetDepthHeight());
		//captureImage.save(file_name + "_oracle.png");
		//cv::Mat cvColor = cv::Mat(bm.GetDepthHeight(), bm.GetDepthWidth(), CV_8UC3, captureImage.getPixels());
		//cv::cvtColor(cvColor, cvColor, cv::COLOR_RGB2BGR);
		//bm.SaveMatBinary(file_name + "_oracle", cvColor);
		//flag_save_draw = false;


		lines.clear();
		UpdateDataFile(file_name);
		int si = file_name.size();
		// 58:57 → 1文字 59:57 
		number_file = file_name.substr(path_number);

		if (!bm.LoadMatBinary(file_name)) {

		}
		//bm.ImageGenerator(openFileResult.getPath() + ".jpg");
		if (!depthImage.load(file_name + "color.bmp")) {
			//ofLog(OF_LOG_NOTICE, "can't load image");
		}
		else {
			flag_depth_read = true;
			cv::Mat color = cv::imread(file_name + "color.bmp");
			cv::cvtColor(color, color, CV_BGR2RGB);
			depthImage.setFromPixels(color.ptr(), color.cols, color.rows, OF_IMAGE_COLOR, false);
		}
		//depthImage.mirror(false, true);
		flag_show_image = true;
		mat_color.release();
		buffer.release();
		buffer = cv::Mat::zeros(424, 512, CV_8SC1);
		mat_color = cv::imread(file_name + "color.bmp");
		//cv::medianBlur(mat_color, mat_color_rgb, 5);
		cv::medianBlur(mat_color, mat_color, 5);
		cv::cvtColor(mat_color, mat_color, CV_BGR2HSV);
		//cv:imshow("color", mat_color);

		flag_joint = true;
}
