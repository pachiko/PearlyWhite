#pragma once

#include <opencv2/core.hpp>

/// @brief Removes the axis and numbers from the image matrix
/// @param The original image
/// @return the image without the axis and numbers
cv::Mat cropGrid(const cv::Mat&);

/// @brief Fills a rounded rectangle on an empty image. This image/mask is used for histogram counting.
/// @param cv::Mat&: The blank image
/// @param const cv::Rect&: Rectangle
/// @param int: The radius of the rounded corners for the rectangle
void fill_rounded_rectangle(cv::Mat&, const cv::Rect&, int);