#pragma once
#include <opencv2/core.hpp>

/// @brief Calculate the histogram of the image
/// @param The input image
/// @return The histogram plotted as a bar graph
cv::Mat getHistogram(const cv::Mat&);
