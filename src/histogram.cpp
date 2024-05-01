#include "histogram.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


void showHistogram(const cv::Mat& image) {
	const int histSize = 50;
	float range[] = { 0, 256 };
	const float* histRange[] = { range };

	cv::Mat hist;
	cv::calcHist(&image, 
		1, // nImages
		0, // channels used to compute histogram
		cv::Mat(), // mask
		hist,
		1, // histogram dimensionality
		&histSize, // histogram sizes in each dimension
		histRange // boundary of histogram in each dimension
	);

	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

	cv::normalize(hist, hist,
		0, // lower bound
		histImage.rows, // upper bound
		cv::NORM_MINMAX, // normalize with limits
		-1, // output type == input type
		cv::Mat() // mask
	);

	// Draw histogram as line graph
	for (int i = 1; i < histSize; i++) {
		float binVal = hist.at<float>(i - 1);

		cv::rectangle(histImage, 
			cv::Point(bin_w * (i - 1), hist_h - binVal),
			cv::Point(bin_w*i, 0),
			cv::Scalar(0, 0, 0),
			-1);
	}

	cv::imshow("Histogram", histImage);
}