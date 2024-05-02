#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "histogram.h"
#include "filter.h"


cv::Mat getHistogram(const cv::Mat& image) {
	const int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange[] = { range };

	cv::Mat mask = cv::Mat::zeros(cv::Size(image.cols,image.rows), CV_8UC1);
	cv::Rect r{0, 0, image.cols, image.rows};
	fill_rounded_rectangle(mask, r, 100);

	cv::Mat hist;
	cv::calcHist(&image, 
		1, // nImages
		0, // channels used to compute histogram
		mask, // mask
		hist,
		1, // histogram dimensionality
		&histSize, // histogram sizes in each dimension
		histRange // boundary of histogram in each dimension
	);

	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	cv::normalize(hist, hist,
		0, // lower bound
		histImage.rows, // upper bound
		cv::NORM_MINMAX, // normalize with limits
		-1, // output type == input type
		cv::Mat() // mask
	);

	// Draw histogram as bar graph
	for (int i = 1; i < histSize; i++) {
		float binVal = hist.at<float>(i - 1);
		cv::rectangle(histImage, 
			cv::Point(bin_w * (i - 1), hist_h - binVal),
			cv::Point(bin_w * i, hist_h),
			cv::Scalar(255, 255, 255),
			-1);
	}

	return histImage;
}