#include "reader.hpp"

#define WINDOW_NAME "images"

using namespace std;
using namespace cv;
void displayImage(Mat m)
{
   namedWindow(WINDOW_NAME, WINDOW_NORMAL);
   imshow(WINDOW_NAME,m);
   resizeWindow(WINDOW_NAME,600,600);
   waitKey(0);
}

Mat readImage(std::string fileName)
{
   return imread(fileName,IMREAD_COLOR);
}

void createDisplayFrame()
{
   namedWindow(WINDOW_NAME, WINDOW_NORMAL);
   waitKey(0);
}

VideoCapture readVideo(std::string fileName)
{
   VideoCapture v = VideoCapture(fileName);
   //if(!v.open(fileName,4)){
   //   cerr << "there was a problem with opening the video file" << endl;
   //}
   return v;
}

/**
 * gets the next available frame from a VC
 */

Mat getFrame(VideoCapture video)
{
   Mat m;
   video >> m;
   return m;
}
