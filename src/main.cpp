#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "trackbarThresholder.h"




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

	cv::imshow("Image", image);
	cv::waitKey(0);

	return 0;
}


