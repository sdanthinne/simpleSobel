/*****************************************************************************
 * File: reader.cpp
 *
 * Description: This file has functions to display the sobel frame and to
 *     get the next frame out of the video. 
 *
 * Author: Sebastien Danthinne and Erin Rylie Clark
 * ***************************************************************************/
#include "reader.hpp"
#include <ctime>
#include "sobel.hpp"
#define WINDOW_NAME "images"

using namespace std;
using namespace cv;

/* Global variable for getting frame from video */
Mat getFrame(VideoCapture v);

/*------------------------------------------------------------------------------
 * Function: displayFrameMat
 * Description: this function takes in the frame with the sobel filter already
 *     applied, creats a window to display it in, and displays it.
 *     We also currently have this function printing out the resolution of the
 *     frame.
 * param: frame - the Mat object containing the frame to display
 * -----------------------------------------------------------------------------*/
void displayFrameMat(Mat frame)
{
    namedWindow(WINDOW_NAME,WINDOW_NORMAL);
    cout << frame.cols << " x " << frame.rows << endl;
    imshow(WINDOW_NAME,frame);
    resizeWindow(WINDOW_NAME,600,600);
}

/*------------------------------------------------------------------------------
 * Function: getFrame 
 * Description: this function just takes in the video, grabs the next frame, and
 *     returns a Mat object containing that frame
 * param: video - the video to get the frame from
 * return: m - the Mat object containing the frame to be returned
 * -----------------------------------------------------------------------------*/
Mat getFrame(VideoCapture video)
{
   Mat m;
   video >> m; /* Get the next frame available */

   return m;
}
