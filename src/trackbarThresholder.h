#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


// Option for user to calibrate threshold value and type.
extern bool calibrate_threshold;


/// Implements image thresholding with the help of user-guided trackbars
/// Example: 
///		TrackbarThresholder indicatorFinder(230, 1, "Indicator Finder");
///		indicatorFinder.m_input = image;
///		indicatorFinder.threshold();
/// 	std::vector<std::vector<cv::Point>> contours;
///		cv::findContours(indicatorFinder.m_output, contours, cv::RetrievalModes::RETR_LIST, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
class TrackbarThresholder {
public:
	// The input image to perform thresholding
	cv::Mat m_input;

	// The output binary image of the thresholding
	cv::Mat m_output;

	/// @brief Default constructor
	TrackbarThresholder();

	/// @param thresholdValue: Threshold value
	/// @param thresholdType: Type of thresholding to use. 
	/// @param windowName: String for the window name. Used when user wants to calibrate threshold values.
	TrackbarThresholder(int thresholdValue, int thresholdType, const cv::String& windowName);

	/// @brief Performs thresholding. May spawn trackbars.
	void threshold();

private:
	/// Name of the thresholding value trackbar
	const cv::String thresholdName = "Threshold";

	/// Name of the thresholding type trackbar
	const cv::String typeName = "Type";

	/// Default name of the window for user-guided thresholding 
	const cv::String defaultWindowName = "Default";

	/// Threshold value
	int m_thresholdValue;

	/// Threshold type
	int m_thresholdType;

	/// Window name
	cv::String m_windowName = defaultWindowName;

	/// Maximum value for thresholding
	const int maxThresholdValue = 255;

	/// Maximum value for thresholding type
	/// Supports {THRESH_BINARY, THRESH_BINARY_INV, THRESH_TRUNC, THRESH_TOZERO, THRESH_TOZERO_INV }
	/// See cv::ThresholdTypes
	const int maxThresholdType = 4;

	/// @brief Performs actual thresholding.
	void doThreshold();

	/// @param thresholdValue: Threshold value
	/// @param thresholdType: Type of thresholding to use. 
	/// @param windowName: String for the window name. Used when user wants to calibrate threshold values.
	static void trackbarCallback(int, void*);
};