#include <iostream>
#include <opencv2/imgproc.hpp>

#include "filter.h"
#include "trackbarThresholder.h"

 // #define DEBUG_MASK

/// @brief Finds the main dental contour. That's the largest one.
/// @param contours: The vector of contours, where each contour is represented as a vector of points.
/// @return The index of the contour. If not found, returns -1.
int findDentalContour(const std::vector<std::vector<cv::Point>>& contours) {
	double maxArea = -1;
	int maxContourIdx = -1;

	for (size_t i = 0; i < contours.size(); i++) {
		cv::Rect r = cv::boundingRect(contours[i]);
		double area = r.width * r.height;
		if (area > maxArea) {
			maxArea = area;
			maxContourIdx = i;
		}
	}

	return maxContourIdx;
}

cv::Mat cropGrid(const cv::Mat& image) {
	TrackbarThresholder gridCropping(230, 1, "Grid Removal");
	gridCropping.m_input = image;
	gridCropping.threshold();

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(gridCropping.m_output, contours, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

	int dentalContourIdx = findDentalContour(contours);
	if (dentalContourIdx < 0) {
		std::cout << "Could not extract image without grid\n";
		return image;
	}
	
	cv::Rect r = cv::boundingRect(contours[dentalContourIdx]);
	return image(cv::Range(r.y, r.y + r.height), cv::Range(r.x, r.x + r.width));
}

/**
 * Draws and fills a rectangle with rounded corners
 * @author K
 */
void fill_rounded_rectangle(cv::Mat& src, const cv::Rect& r, int cornerRadius) {
	/* corners:
	 * p1 - p2
	 * |     |
	 * p4 - p3
	 */
	cv::Point p1 = r.tl();
	cv::Point p2 = { r.x + r.width, r.y };
	cv::Point p3 = r.br();
	cv::Point p4 = { r.x, r.y + r.height };

	const cv::Scalar lineColor(255, 255, 255);
	const int thickness = 1;
	const int lineType = 8;

	// draw straight lines
	line(src, cv::Point(p1.x + cornerRadius, p1.y), cv::Point(p2.x - cornerRadius, p2.y), lineColor, thickness, lineType);
	line(src, cv::Point(p2.x, p2.y + cornerRadius), cv::Point(p3.x, p3.y - cornerRadius), lineColor, thickness, lineType);
	line(src, cv::Point(p4.x + cornerRadius, p4.y), cv::Point(p3.x - cornerRadius, p3.y), lineColor, thickness, lineType);
	line(src, cv::Point(p1.x, p1.y + cornerRadius), cv::Point(p4.x, p4.y - cornerRadius), lineColor, thickness, lineType);

	// draw arcs
	cv::Size rad(cornerRadius, cornerRadius);
	ellipse(src, p1 + cv::Point(cornerRadius, cornerRadius), rad, 180.0, 0, 90, lineColor, thickness, lineType);
	ellipse(src, p2 + cv::Point(-cornerRadius, cornerRadius), rad, 270.0, 0, 90, lineColor, thickness, lineType);
	ellipse(src, p3 + cv::Point(-cornerRadius, -cornerRadius), rad, 0.0, 0, 90, lineColor, thickness, lineType);
	ellipse(src, p4 + cv::Point(cornerRadius, -cornerRadius), rad, 90.0, 0, 90, lineColor, thickness, lineType);

	// choose arbitrary starting point for fill
	cv::Point fillFrom = { (r.x + r.width) / 2, (r.y + r.height) / 2 };
	floodFill(src, fillFrom, lineColor);

#ifdef DEBUG_MASK
	cv::imshow("mask", src);
	cv::waitKey(0);
	cv::destroyWindow("mask");
#endif
}