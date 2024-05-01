#pragma once

#include <opencv2/core.hpp>

// Cut away the grid in the image and return the cropped image.
cv::Mat cropGrid(const cv::Mat& );

// Figures out what angle (cv::RotateFlags) should the image be rotated with.
int getImageRotation(const cv::Mat& );