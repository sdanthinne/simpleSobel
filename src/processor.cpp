#include "processor.hpp"
#include <iostream>
#define RED_CONSTANT 0.2126
#define GREEN_CONSTANT 0.7152 
#define BLUE_CONSTANT 0.0722
#define X_KERNEL {-1,0,1,-2,0,2,-1,0,1}
#define Y_KERNEL {-1,-2,-1,0,0,0,1,2,1}

using namespace cv;
using namespace std;

/**
* Performs a sobel filter on a Mat frame
*/
Mat sobel(Mat frame)
{
    return sobelFrameFromGrayScale(grayscaleFrame(frame));
}



Mat sobelFrameFromGrayScale(Mat frame)
{
   int totalIterate = frame.rows * frame.cols;
   cout << totalIterate << endl;
   return frame;
}
/**
* This function is used to grayscale out a frame.
*/
Mat grayscaleFrame(Mat frame)
{
    frame.forEach<Pixel>([&](Pixel &p, const int * position) ->  void {
        float newC = p.x*BLUE_CONSTANT + p.y*GREEN_CONSTANT + p.z*RED_CONSTANT;
        p.x = p.y = p.z = newC;
    });
    return frame;
}
