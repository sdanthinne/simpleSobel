#include "processor.hpp"
#include <iostream>
#define RED_CONSTANT 0.2126
#define GREEN_CONSTANT 0.7152 
#define BLUE_CONSTANT 0.0722
#define X_KERNEL {-1,0,1,-2,0,2,-1,0,1}
#define Y_KERNEL {-1,-2,-1,0,0,0,1,2,1}
#define KERNEL_SIZE 9
#define KERNEL_SIDE 3

using namespace cv;
using namespace std;

int x_kernel[] = X_KERNEL;
int y_kernel[] = Y_KERNEL;
/**
* Performs a sobel filter on a Mat frame
*/
Mat sobel(Mat frame)
{   
    return sobelFrameFromGrayScale(grayscaleFrame(frame));
}

/**
 * performs the actual multiplication
 */
double matValMult(int array1[], int array2[])
{
    double resultant=0;
    for(int i=0; i<KERNEL_SIZE;i++)
    {
        resultant += array1[i] * array2[i];
    }
    return resultant;
}

/**
 * populates the photo kernel from the frame around the center coords
 */
void populatePhotoKernel(int centerRow, int centerCol, Mat frame,int * photoKernel)
{
    for(int i=0;i<KERNEL_SIZE;i++)
    {
        photoKernel[i] = frame.at<Pixel>(centerRow+i/KERNEL_SIDE,centerCol+i%KERNEL_SIDE).x;
    }
}

/**
 * Creates a sobel calculated frame from a grayscale image.
 */

Mat sobelFrameFromGrayScale(Mat frame)
{
   int totalIterate = frame.rows * frame.cols;

   //currently this ignores the very edges of the image
   int photoKernel[9];
   for(int row=1;row<frame.rows-1;row++)
   {
       for(int col=1;col<frame.cols-1;col++)
       {
           //now we have each pixel location, so do the calculationa
           populatePhotoKernel(row,col,frame,photoKernel);
           Pixel * current = frame.ptr<Pixel>(row,col);

           current->x = current->y = current->z = matValMult(photoKernel,x_kernel)+matValMult(photoKernel,y_kernel);
           

       }
   }
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
