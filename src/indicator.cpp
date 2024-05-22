#include <iostream>
#include <opencv2/imgproc.hpp>

#include "indicator.h"
#include "trackbarThresholder.h"


// Constants for checking if contour is the indicator
const float minAr = 0.9, maxAr = 1.1;
const float minAreaRatio = 0.001, maxAreaRatio = 0.002;
const float maxEdgeDistRatio = 0.2;


// Calculates the distance ratio of a value 'x' from the 'edge' or from 0, whichever is nearer.
// Also returns a boolean to indicate 'x' is closer to 0 than the 'edge'.
inline std::pair<double, bool> fromEdge(double x, double edge) {
	if (x > edge / 2) return std::make_pair((edge - x) / edge, false);
	else return std::make_pair(x / edge, true);
}


// Determines the rotation given the booleans 'left' and 'top',
// which indicate the location of the indicator.
int findAngle(bool left, bool top) {
	if (left && top) return -1;
	if (left && !top) return cv::RotateFlags::ROTATE_90_CLOCKWISE;
	if (!left && top)  return cv::RotateFlags::ROTATE_90_COUNTERCLOCKWISE;
	return cv::RotateFlags::ROTATE_180;
}


// Figures out what angle (cv::RotateFlags) should the image be rotated with.
// Performs tests on aspect ratio, area and distance from image border for each contour.
// If all tests have passed, return the required rotation.
int getImageRotation(const cv::Mat& image) {
	TrackbarThresholder indicatorFinder(230, 1, "Indicator Finder");
	indicatorFinder.m_input = image;
	indicatorFinder.threshold();

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(indicatorFinder.m_output, contours, cv::RetrievalModes::RETR_LIST, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
	cv::Point size = image.size();

	for (size_t i = 0; i < contours.size(); i++) {
		cv::Rect r = cv::boundingRect(contours[i]);

		// Aspect ratio should not be +/- 10% more than a perfect square.
		double ar = r.width / static_cast<double>(r.height);
		if (ar > maxAr || ar < minAr) continue;

		// Should not be too big or too small 
		double areaRatio = r.area() / static_cast<double>(size.x * size.y);
		if (areaRatio > maxAreaRatio || areaRatio < minAreaRatio) continue;

		cv::Point center{ r.x + r.width / 2, r.y + r.height / 2 };
		auto xEdge = fromEdge(center.x, size.x);
		auto yEdge = fromEdge(center.y, size.y);
		
		// Indicator should not be more than 20% away
		if (xEdge.first > maxEdgeDistRatio || yEdge.first > maxEdgeDistRatio) continue;

		return findAngle(xEdge.second, yEdge.second);
	}
	
	return -1;
}
