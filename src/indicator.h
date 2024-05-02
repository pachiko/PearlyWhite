#pragma once

#include <opencv2/core.hpp>


// Figures out what angle (cv::RotateFlags) should the image be rotated with.
int getImageRotation(const cv::Mat& );