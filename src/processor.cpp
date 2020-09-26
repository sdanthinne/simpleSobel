#include "processor.hpp"
#include <iostream>
#define RED_CONSTANT 0.2126
#define GREEN_CONSTANT 0.7152 
#define BLUE_CONSTANT 0.0722
using namespace cv;
using namespace std;
int colorRemoval();
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
