#ifndef READER_H
#define READER_H
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string.h>
void displayFrame(cv::VideoCapture video);
void createDisplayFrame();
cv::Mat readImage(std::string fileName);
cv::VideoCapture readVideo(std::string fileName);
cv::Mat getFrame(cv::VideoCapture video);
void displayFrameMat(cv::Mat frame);
#endif
