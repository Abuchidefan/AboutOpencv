#include "Camcalibration.h"


CCamcalibration::CCamcalibration()
{
}


CCamcalibration::~CCamcalibration()
{
}
void CCamcalibration::addPoints(TV2Dp &imageCornerts,TV3Dp &objectCornerts)
{
	objectPoints.push_back(objectCornerts);
	imagePoints.push_back(imageCornerts);
}
bool CCamcalibration::addOneChessBoardPoints(cv::Mat&image, cv::Size & boardSize)
{
	TV3Dp objectCornerts;
	TV2Dp imageCornerts;
	for (int i = 0; i < boardSize.height; i++)
		for (int j = 0; j < boardSize.width; j++)
			objectCornerts.push_back(cv::Point3f(i, j, 0.0f));
	bool isfind=cv::findChessboardCorners(image, boardSize, imageCornerts);
	img = image.clone();
    cv::drawChessboardCorners(img, boardSize, imageCornerts, isfind);
	if (!isfind)
		return false;
	cv::cornerSubPix(image, imageCornerts, cv::Size(5, 5), cv::Size(-1, -1)
		, cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1));
	if (imageCornerts.size() == boardSize.area())
	{
		addPoints(imageCornerts, objectCornerts);
		return true;
	}
	else
		return false;
}
double CCamcalibration::calibrate(cv::Size&imageSize)
{
	mustInitUndistort = true;
	std::vector<cv::Mat>rvecs, tvecs;
	
	double errorval = cv::calibrateCamera(objectPoints, imagePoints
		, imageSize, cameraMat, distCoeffs, rvecs, tvecs, flag);
    isCalibration=true;
	return errorval;
}
bool CCamcalibration::remap(cv::Mat&image)
{
	if (mustInitUndistort)
	{
		cv::initUndistortRectifyMap(cameraMat, distCoeffs, cv::Mat(), cv::Mat(),
			image.size(), CV_32FC1, map1, map2);
		mustInitUndistort = false;
	}
	cv::remap(image, img, map1, map2, cv::INTER_LINEAR);
	return true;
}


bool CCamcalibration::Calibration(std::vector<std::string>&filelist, int rows, int cols)
{
	for (auto it = filelist.begin(); it != filelist.end(); it++)
	{
        frame = cv::imread(*it, 0);
        if (frame.empty())
		{
			lastErr = "can not open " + *it;
			return false;
		}
        addOneChessBoardPoints(frame, cv::Size(rows, cols));
	}
    calibrate(frame.size());
	return true;
}
bool CCamcalibration::CalibrationRD(std::string filelist)
{
	std::ifstream fin;
	vfilelist.clear();
	fin.open(filelist);

	if (!fin.is_open())
	{
		lastErr = "can not open " + filelist;
		return false;
	}
    std::string path = filelist.substr(0, filelist.rfind('\\')+1);
	std::string file;
	while (fin >> file)
	{
		vfilelist.push_back(path + file);
	}
	fin.close();
	return true;
}

bool CCamcalibration::CalibrationOneByOne(int rows, int cols)
{
    if(vfilelist.empty())
    {
        lastErr = "No flie to read!";
        return false;
    }
    if (num > vfilelist.size()-1)
	{
        frame = cv::imread(vfilelist.at(num-1), 0);
        calibrate(frame.size());


        lastErr = "Calibration OK!";
        num=0;
		return false;
	}
    frame = cv::imread(vfilelist.at(num), 0);
    if (frame.empty())
	{
		lastErr = "can not open " + vfilelist.at(num);
		return false;
	}
	num++;
    addOneChessBoardPoints(frame, cv::Size(rows, cols));
	return true;
}
bool CCamcalibration::remapChess()
{
    if(vfilelist.empty())
    {
        lastErr = "No flie to remap!";
        return false;
    }
    if(!isCalibration)
    {
        lastErr = "Calibration first!";
        return false;
    }
    if (num > vfilelist.size()-1)
    {
        lastErr = "File is over!";
        return false;
    }
    frame = cv::imread(vfilelist.at(num), 0);
    if (frame.empty())
    {
        lastErr = "can not open " + vfilelist.at(num);
        return false;
    }
    num++;
    remap(frame);
    return true;
}

