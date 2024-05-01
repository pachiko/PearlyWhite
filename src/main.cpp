#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "inputParser.h"
#include "indicator.h"
#include "trackbarThresholder.h"


int main(int argc, char** argv) {
	InputParser input(argc, argv);

	if (argc < 2 || argc > 3) {
		std::cout << " Usage: " << "PearlyWhite.exe "
			<< "[--calibrate]"
			<< "ImageToLoadAndDisplay\n";
		return -1;
	}

	std::string filename = argv[1];
	cv::Mat image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	if (image.empty()) {
		std::cout << "Could not open or find the image: " << filename << "\n";
		return -1;
	}

	if (input.cmdOptionExists("--calibrate")) {
		calibrate_threshold = true;
	}

	cv::Mat cropped = cropGrid(image);
	int rot = getImageRotation(cropped);
	if (rot >= 0) {
		cv::rotate(image, image, rot);
	}

	cv::imshow(filename, image);
	cv::waitKey(0);

	return 0;
}



