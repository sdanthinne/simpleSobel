#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <opencv2/opencv.hpp>
typedef cv::Point3_<uint8_t> Pixel;

cv::Mat * grayscaleFrame(cv::Mat frame,cv::Mat * grayFrame);
cv::Mat * sobelFrameFromGrayScale(cv::Mat * frame,cv::Mat * outFrame);
cv::Mat sobel(cv::Mat frame);
cv::Mat * split4FromParent(cv::Mat parent, cv::Mat * matCollection);
cv::Mat * sobelFrame(cv::Mat  frame,cv::Mat * outFrame, cv::Mat * grayFrame);
struct threadInfo_s
{
    int thread_number;
    cv::Mat frame;
};
#endif
