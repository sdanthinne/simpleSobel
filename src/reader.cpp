#include "reader.hpp"
#include <ctime>
#include "sobel.hpp"
#define WINDOW_NAME "images"

using namespace std;
using namespace cv;

Mat getFrame(VideoCapture v);

void displayFrame(VideoCapture v)
{
    /*int key=0;
    Mat currFrame = getFrame(v);
    namedWindow(WINDOW_NAME, WINDOW_NORMAL);
    while(key!='q')
    {
        imshow(WINDOW_NAME,currFrame);
        resizeWindow(WINDOW_NAME,600,600);
        int time_init = clock();
        currFrame = sobel(getFrame(v));
        time_init = clock()-time_init;
        cout << "next frame loaded in: " << (float)time_init/CLOCKS_PER_SEC << "s" << endl;
        if((key=waitKey(1))=='q')
        {
            //quit
            destroyAllWindows();
            break;
        } 
    }*/
    startSobel(v);
}

/**
* Just opens a window and displays a frame. Nothing much
*/
void displayFrameMat(Mat frame)
{
    imshow(WINDOW_NAME,frame);
    resizeWindow(WINDOW_NAME,600,600);
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
