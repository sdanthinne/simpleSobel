#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <opencv2/opencv.hpp>
typedef cv::Point3_<uint8_t> Pixel;

cv::Mat * split4FromParent(cv::Mat parent, cv::Mat * matCollection);
cv::Mat  sobelFrame(cv::Mat  frame,cv::Mat  outFrame, cv::Mat  grayFrame);
cv::Mat sobelReference(cv::Mat inframe, cv::Mat outFrame);

struct DimRef {
    int startx; //the stop left value where this chunk starts
    int starty;
    int endx;
    int endy;//bottom right values where the chunk ends
} DimRef;

#endif
