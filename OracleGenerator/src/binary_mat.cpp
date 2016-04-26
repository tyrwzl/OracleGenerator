//
//  binary_mat.cpp
//  mySketch
//
//  Created by Takahiro on 2015/12/29.
//
//

#include "binary_mat.hpp"

BinaryMat::BinaryMat()
{
}

BinaryMat::~BinaryMat()
{
}

cv::Mat BinaryMat::GetDepthData()
{
	return depthData;
}

int BinaryMat::GetDepthWidth()
{
	return depthWidth;
}

int BinaryMat::GetDepthHeight()
{
	return depthHeight;
}

// Load cv::Mat as binary
bool BinaryMat::LoadMatBinary(const std::string& filename)
{
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		return false;
	}

	int rows, cols, type;
	ifs.read((char*)(&rows), sizeof(int));
	if (rows == 0) {
		return true;
	}
	ifs.read((char*)(&cols), sizeof(int));
	ifs.read((char*)(&type), sizeof(int));

	depthData.release();
	depthData.create(rows, cols, type);
	ifs.read((char*)(depthData.data), depthData.elemSize() * depthData.total());

	/*
	cv::namedWindow("depth", CV_WINDOW_AUTOSIZE);
	cv::resizeWindow("depth", depthWidth, depthHeight);
	cv::imshow("depth", depthData);
	*/
	return true;
}

// Save cv::Mat as binary
bool BinaryMat::SaveMatBinary(const std::string& filename, const cv::Mat& out_mat)
{
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		return false;
	}
	if (out_mat.empty()) {
		int s = 0;
		ofs.write((const char*)(&s), sizeof(int));
		return true;
	}
	int type = out_mat.type();
	ofs.write((const char*)(&out_mat.rows), sizeof(int));
	ofs.write((const char*)(&out_mat.cols), sizeof(int));
	ofs.write((const char*)(&type), sizeof(int));
	ofs.write((const char*)(out_mat.data), out_mat.elemSize() * out_mat.total());

	return true;
}

void BinaryMat::ImageGenerator(const std::string& filename)
{

	cv::imwrite(filename, depthData);
}
