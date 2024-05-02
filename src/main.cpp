#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "filter.h"
#include "indicator.h"
#include "trackbarThresholder.h"
#include "histogram.h"


// cv::CommandLineParser syntax: name, default value, description
// @ is a positional arg and <none> enforces a user-provided value
const std::string keys =
"{help h usage ? | | prints this message }"
"{@image| <none> | path to image file }"
"{calibrate | | use trackbar to calibrate image thresholding }"
;


int main(int argc, char** argv) {
	cv::CommandLineParser parser(argc, argv, keys);
	parser.about("PearlyWhite v1.0.0\nThis program displays an auto-rotated dental image along with its histogram\n");

	if (parser.has("help"))	{
		parser.printMessage();
		return 0;
	}

	std::string filename = parser.get<std::string>("@image");
	if (!parser.check()) {
		parser.printErrors();
		parser.printMessage();
		return -1;
	}

	cv::Mat image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	if (image.empty()) {
		std::cout << "Could not open or find the image: " << filename << "\n";
		return -1;
	}

	calibrate_threshold = parser.has("calibrate");
	
	cv::Mat cropped = cropGrid(image);
	int rot = getImageRotation(cropped);
	if (rot >= 0) {
		cv::rotate(image, image, rot);
	}

	cv::imshow(filename, image);
	cv::Mat histImage = getHistogram(cropped);
	cv::imshow("Histogram", histImage);
	cv::waitKey(0);
	cv::destroyWindow(filename);
	cv::destroyWindow("Histogram");

	return 0;
}



