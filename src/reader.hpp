#ifndef READER_H
#define READER_H
#include <opencv2/imgcodecs.hpp>
#include <string.h>
void displayImage(cv::Mat m);
void createDisplayFrame();
cv::Mat readImage(std::string fileName);
cv::VideoCapture readVideo(std::string fileName);
cv::Mat getFrame(cv::VideoCapture video);
#endif
