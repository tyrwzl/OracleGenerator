#pragma once

#ifndef binary_mat_hpp
#define binary_mat_hpp

#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

class BinaryMat
{
private:
	const int depthWidth = 512;
	const int depthHeight = 424;
	cv::Mat depthData;

public:
	BinaryMat();
	~BinaryMat();

	cv::Mat GetDepthData();
	int GetDepthWidth();
	int GetDepthHeight();

	bool LoadMatBinary(const std::string& filename);
	bool SaveMatBinary(const std::string& filename, const cv::Mat& out_mat);
	void ImageGenerator(const std::string& filename);
};

#endif /* binary_loader_hpp */


