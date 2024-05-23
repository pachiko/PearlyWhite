#include <iostream>
#include <opencv2/imgproc.hpp>

#include "indicator.h"
#include "trackbarThresholder.h"


/// Constants for checking if contour is the indicator
/// Minimum and maximum aspect ratio
const float minAr = 0.9, maxAr = 1.1;
/// Minimum and maximum area ratio (contour area/image area)
const float minAreaRatio = 0.001, maxAreaRatio = 0.002;
/// Maximum distance ratio from the closest edge
const float maxEdgeDistRatio = 0.2;

/// @brief Calculates the distance of a coordinate 'x' from it's closest edge, which can be '0' or 'edge'
///			If x is closer to 0, the distance, d is (x - 0); else, it is (edge - x)
/// @param x: The input coordinate
/// @param edge: The maximum edge coordinate
/// @return A pair containing the distance ratio (d/edge) and a flag indicating where x is closer to 0 or not
inline std::pair<double, bool> fromEdge(double x, double edge) {
	if (x > edge / 2) return std::make_pair((edge - x) / edge, false);
	else return std::make_pair(x / edge, true);
}

/// @brief Finds the angle to rotate based on the position of the indicator
/// @param left: The indicator is on the left
/// @param top: The indicator is at the top
/// @return The cv::RotateFlags value for the required rotation. -1 means no rotation required.
int findAngle(bool left, bool top) {
	if (left && top) return -1;
	if (left && !top) return cv::RotateFlags::ROTATE_90_CLOCKWISE;
	if (!left && top)  return cv::RotateFlags::ROTATE_90_COUNTERCLOCKWISE;
	return cv::RotateFlags::ROTATE_180;
}

int getImageRotation(const cv::Mat& image) {
	TrackbarThresholder indicatorFinder(230, 1, "Indicator Finder");
	indicatorFinder.m_input = image;
	indicatorFinder.threshold();

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(indicatorFinder.m_output, contours, cv::RetrievalModes::RETR_LIST, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
	cv::Point size = image.size();

	for (size_t i = 0; i < contours.size(); i++) {
		cv::Rect r = cv::boundingRect(contours[i]);

		// Check aspect ratio. Needs to be close to a square
		double ar = r.width / static_cast<double>(r.height);
		if (ar > maxAr || ar < minAr) continue;

		// Check area ratio. Cannot be too small or too big relative to entire image.
		double areaRatio = r.area() / static_cast<double>(size.x * size.y);
		if (areaRatio > maxAreaRatio || areaRatio < minAreaRatio) continue;

		// Check distance ratio from closest edges. Cannot be too far.
		cv::Point center{ r.x + r.width / 2, r.y + r.height / 2 };
		auto xEdge = fromEdge(center.x, size.x);
		auto yEdge = fromEdge(center.y, size.y);
		if (xEdge.first > maxEdgeDistRatio || yEdge.first > maxEdgeDistRatio) continue;

		return findAngle(xEdge.second, yEdge.second);
	}
	
	return -1;
}
