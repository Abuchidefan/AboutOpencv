#pragma once
#ifndef CCAMCALIBRATION_H
#define CCAMCALIBRATION_H

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdio.h>


class CCamcalibration
{
	typedef std::vector<cv::Point3f> TV3Dp;
	typedef std::vector<cv::Point2f> TV2Dp;
	typedef std::vector<TV3Dp> TVobp;
	typedef std::vector<TV2Dp> TVigp;
private:
	TVobp objectPoints;
	TVigp imagePoints;
	cv::Mat cameraMat;
	cv::Mat distCoeffs;
	int flag;
	std::string lastErr;
	cv::Mat img;
	cv::Mat frame;
	cv::Mat map1,map2;
	bool mustInitUndistort = false;
	bool isCalibration = false;
	bool addOneChessBoardPoints(cv::Mat&image, cv::Size & boardSize);
	void addPoints(TV2Dp &imageCornerts, TV3Dp &objectCornerts);
	double calibrate(cv::Size&imageSize);
	std::vector<std::string>vfilelist;
	int num = 0;
public:
	CCamcalibration();
	~CCamcalibration();
	std::string getLastError(){ return lastErr; }

	bool Calibration(std::vector<std::string>&filelist,int rows,int cols);

    bool CalibrationRD(std::string filelist);
	bool CalibrationOneByOne(int rows, int cols);
    bool remapChess();
    bool remap(cv::Mat&image);

	const cv::Mat & getsrc(){ return frame; }
	const cv::Mat & getdst(){ return img; }
};
#endif

