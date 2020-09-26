#include "reader.hpp"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    cv::VideoCapture video = readVideo("../res/test.avi");
    displayImage(getFrame(video));
    return 0;
}
