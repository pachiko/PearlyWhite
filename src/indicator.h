#pragma once

#include <opencv2/core.hpp>


/// @brief Finds the angle to rotate based on the position of the indicator in the image
/// @param const cv::Mat&: The image containing the indicator
/// @return The cv::RotateFlags value for the required rotation. -1 means no rotation required.
int getImageRotation(const cv::Mat& );