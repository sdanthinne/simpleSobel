#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <opencv2/opencv.hpp>
typedef cv::Point3_<uint8_t> Pixel;

cv::Mat grayscaleFrame(cv::Mat frame);
cv::Mat sobelFrameFromGrayScale(cv::Mat frame);
cv::Mat sobel(cv::Mat frame);

#endif
