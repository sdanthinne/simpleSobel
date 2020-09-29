#include "reader.hpp"
#include "processor.hpp"
#define WINDOW_NAME "images"

using namespace std;
using namespace cv;

Mat getFrame(VideoCapture v);

void displayFrame(VideoCapture v)
{
    int key=0;
    Mat currFrame = getFrame(v);
    namedWindow(WINDOW_NAME, WINDOW_NORMAL);
    while(key!='q')
    {
        imshow(WINDOW_NAME,currFrame);
        resizeWindow(WINDOW_NAME,600,600);
        currFrame = sobel(getFrame(v));
        cout << "next frame loaded" << endl;
        if((key=waitKey(0))=='q')
        {
            //quit
            destroyAllWindows();
            break;
        }else if(key=='n'){
            //go to the next frame
            currFrame = sobel(getFrame(v));
        }
    }
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
