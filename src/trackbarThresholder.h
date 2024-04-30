#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// Class to implement thresholding using a trackbar
class TrackbarThresholder {
public:
	cv::Mat m_input;
	cv::Mat m_output;

	TrackbarThresholder();
	TrackbarThresholder(int thresholdValue, int thresholdType, const cv::String& windowName);

	void threshold();

private:
	const cv::String thresholdName = "Threshold";
	const cv::String typeName = "Type";
	const cv::String defaultWindowName = "Default";

	int m_thresholdValue;
	int m_thresholdType;
	cv::String m_windowName = defaultWindowName;

	const int maxThresholdValue = 255;
	const int maxThresholdType = 4;

	static void trackbarCallback(int, void*);
};