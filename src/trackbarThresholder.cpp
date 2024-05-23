#include "trackbarThresholder.h"

bool calibrate_threshold = false;

TrackbarThresholder::TrackbarThresholder() { }

TrackbarThresholder::TrackbarThresholder(int thresholdValue, int thresholdType, const cv::String& windowName) : 
	m_thresholdValue(thresholdValue), m_thresholdType(thresholdType), m_windowName(windowName) { }

void TrackbarThresholder::threshold() {
	if (calibrate_threshold) {
		cv::namedWindow(m_windowName, cv::WINDOW_AUTOSIZE);
		cv::createTrackbar(thresholdName, m_windowName, &m_thresholdValue, maxThresholdValue, trackbarCallback, this);
		cv::createTrackbar(typeName, m_windowName, &m_thresholdType, maxThresholdType, trackbarCallback, this);
		while (cv::waitKey(0) != 'n') {}
		cv::destroyWindow(m_windowName);
	} else {
		doThreshold();
	}
}

void TrackbarThresholder::doThreshold() {
	cv::threshold(m_input, m_output, m_thresholdValue, maxThresholdValue, m_thresholdType);
}

void TrackbarThresholder::trackbarCallback(int newValue, void* userData) {
	TrackbarThresholder* thresholder = (TrackbarThresholder*) userData;
	thresholder->doThreshold();
	cv::imshow(thresholder->m_windowName, thresholder->m_output);
}
