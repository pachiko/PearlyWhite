#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "trackbarThresholder.h"


cv::Mat cropGrid(const cv::Mat& );
int findLargestContour(const std::vector<std::vector<cv::Point>>& );
int getImageRotation(const cv::Mat& );


int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << " Usage: " << argv[0] << " ImageToLoadAndDisplay\n";
		return -1;
	}

	cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
	if (image.empty()) {
		std::cout << "Could not open or find the image\n";
		return -1;
	}

	cv::Mat cropped = cropGrid(image);
	int rot = getImageRotation(cropped);
	if (rot >= 0) {
		cv::rotate(image, image, rot);
	}

	cv::imshow("Image", image);
	cv::waitKey(0);

	return 0;
}


// Cut away the grid in the image and return the cropped image.
// The numbers on the grid creates many false positives when finding the indicator.
cv::Mat cropGrid(const cv::Mat& image) {
	TrackbarThresholder gridCropping(230, 1, "Grid Removal");
	gridCropping.m_input = image;
	gridCropping.threshold();

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(gridCropping.m_output, contours, cv::RetrievalModes::RETR_LIST, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

	int maxContourIdx = findLargestContour(contours);
	if (maxContourIdx < 0) {
		std::cout << "Could not extract image without grid\n";
		return cv::Mat{};
	}

	cv::Rect r = cv::boundingRect(contours[maxContourIdx]);

	return image(cv::Range(r.y, r.y + r.height), cv::Range(r.x, r.x + r.width));
}


// Finds the largest contour in the vector. Approximates by using bounding rect.
int findLargestContour(const std::vector<std::vector<cv::Point>>& contours) {
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


// Calculates the distance ratio of a value 'x' from the 'edge'. Also returns a boolean to indicate 'x' is on the lower half of 'edge'.
inline std::pair<double, bool> fromEdge(double x, double edge) {
	if (x > edge / 2) return std::make_pair((edge - x) / edge, false);
	else return std::make_pair(x / edge, true);
}

// Determines the rotation given the booleans 'left' and 'top'.
int findAngle(bool left, bool top) {
	if (left && top) return -1;
	if (left && !top) return cv::RotateFlags::ROTATE_90_CLOCKWISE;
	if (!left && top)  return cv::RotateFlags::ROTATE_90_COUNTERCLOCKWISE;
	return cv::RotateFlags::ROTATE_180;
}

// Figures out what angle (radians) should the image be rotated with.
int getImageRotation(const cv::Mat& image) {
	TrackbarThresholder indicatorFinder(230, 1, "Indicator Finder");
	indicatorFinder.m_input = image;
	indicatorFinder.threshold();

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(indicatorFinder.m_output, contours, cv::RetrievalModes::RETR_LIST, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
	cv::Point size = image.size();

	for (size_t i = 0; i < contours.size(); i++) {
		cv::Rect r = cv::boundingRect(contours[i]);

		//cv::Mat test = image.clone();
		//cv::Scalar col(0, 0, 0);
		//cv::rectangle(test, r, col);
		//cv::imshow("Test", test);
		//cv::waitKey(0);

		// Aspect ratio should not be +/- 10% more than a perfect square.
		double ar = r.width / static_cast<double>(r.height);
		if (ar > 1.1 || ar < 0.9) continue;

		// Should not be too big or too small 
		double areaRatio = r.area() / static_cast<double>(size.x * size.y);
		if (areaRatio > 0.002 || areaRatio < 0.001) continue;

		cv::Point center{ r.x + r.width / 2, r.y + r.height / 2 };
		auto xEdge = fromEdge(center.x, size.x);
		auto yEdge = fromEdge(center.y, size.y);
		
		// Indicator should not be more than 20% away
		if (xEdge.first > 0.2 || yEdge.first > 0.2) continue;


		return findAngle(xEdge.second, yEdge.second);
	}
	
	return -1;
}

