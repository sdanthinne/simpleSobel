#include "reader.hpp"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    cv::VideoCapture video = readVideo("../res/test.avi");
    if(video.isOpened())
    {
        displayFrame(video);
    }
    return 0;
}
