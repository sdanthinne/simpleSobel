#include "processor.hpp"
#include <iostream>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#define RED_CONSTANT 0.2126
#define GREEN_CONSTANT 0.7152 
#define BLUE_CONSTANT 0.0722
#define X_KERNEL {-1,0,1,-2,0,2,-1,0,1}
#define Y_KERNEL {-1,-2,-1,0,0,0,1,2,1}
#define KERNEL_SIZE 9
#define KERNEL_SIDE 3

//a vector magnitude approximation based on the max and min vector lengths. 
//for more accuracy, multiplying result by 15/16 would work
#define VEC_MAG(A,B)\
    ((A) > (B)) ? 15*((A) + ((B)>>1))/16 : 15*((B) + ((A)>>1))/16


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
long matValMult(int array1[], int array2[])
{
    long resultant=0;
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

int clamp(long value,int min,int max)
{
    return (value>max)? max : (value < min) ? min : value;
}
/**
 * Creates a sobel calculated frame from a grayscale image.
 */

Mat sobelFrameFromGrayScale(Mat frame)
{

   //currently this ignores the very edges of the image
   int photoKernel[9];
   for(int row=1;row<frame.rows-1;row++)
   {
       for(int col=1;col<frame.cols-1;col++)
       {
           //now we have each pixel location, so do the calculationa
           populatePhotoKernel(row,col,frame,photoKernel);
           Pixel * current = frame.ptr<Pixel>(row,col);
           //currently this maxes the result, but IDK if that is realyl what we want in this case. Looks more sobel-y without max
           current->x = 
               current->y = 
               current->z = 
               clamp(
                VEC_MAG(
                 matValMult(photoKernel,x_kernel),matValMult(photoKernel,y_kernel)),0,UCHAR_MAX);
           

       }
   }
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
