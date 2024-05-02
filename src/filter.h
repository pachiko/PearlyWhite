#pragma once

#include <opencv2/core.hpp>

// Cut away the grid in the image and return the cropped image.
cv::Mat cropGrid(const cv::Mat&);

// Fill a rounded rectangle for masking.
void fill_rounded_rectangle(cv::Mat&, const cv::Rect&, int);