#include "trackbarThresholder.h"


TrackbarThresholder::TrackbarThresholder() { }

TrackbarThresholder::TrackbarThresholder(int thresholdValue, int thresholdType, const cv::String& windowName) : 
	m_thresholdValue(thresholdValue), m_thresholdType(thresholdType), m_windowName(windowName) { }

void TrackbarThresholder::threshold() {
	cv::namedWindow(m_windowName, cv::WINDOW_AUTOSIZE);
	cv::createTrackbar(thresholdName, m_windowName, &m_thresholdValue, maxThresholdValue, trackbarCallback, this);
	cv::createTrackbar(typeName, m_windowName, &m_thresholdType, maxThresholdType, trackbarCallback, this);
	while(cv::waitKey(0) != 'n') { }
	cv::destroyWindow(m_windowName);
}

void TrackbarThresholder::trackbarCallback(int newValue, void* userData) {
	TrackbarThresholder* thresholder = (TrackbarThresholder*) userData;
	cv::threshold(thresholder->m_input, thresholder->m_output, thresholder->m_thresholdValue, thresholder->maxThresholdValue, thresholder->m_thresholdType);
	cv::imshow(thresholder->m_windowName, thresholder->m_output);
}
