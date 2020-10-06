#include "processor.hpp"
#include <iostream>
#include <limits.h>
#include <math.h>
#include <thread>
#include <mutex>

#define RED_CONSTANT 0.2126
#define GREEN_CONSTANT 0.7152 
#define BLUE_CONSTANT 0.0722
#define X_KERNEL {-1,0,1,-2,0,2,-1,0,1}
#define Y_KERNEL {-1,-2,-1,0,0,0,1,2,1}
#define KERNEL_SIZE 9
#define KERNEL_SIDE 3
#define L1_AREA_SIZE 100

//a vector magnitude approximation based on the max and min vector lengths. 
//for more accuracy, multiplying result by 15/16 would work
#define VEC_MAG(A,B)\
    ((A) > (B)) ? 15*((A) + ((B)>>1))/16 : 15*((B) + ((A)>>1))/16
/**
*Implementation of threads - I think we want to spawn threads, 
* perform sobel on the a section of the image with size around the l1 (per core) (32KB) (faster memory access)
* cache. We need to be able to calculate convolutions on every pixel, so actual selection will 
* overlap with other threads' selections (border pixel conv. not calculated, overlap takes care of it)
* 
*/

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
* This function performs sobel on a 100x100 area with 
* x1,y1 being the right left corner of this region
*  size of the L1 cache(per core)
*/
Mat sobelThreadArea(Mat frame,int x1,int y1)
{
    //pass
    for(int i=0;i<x1;i++)
    {
        for(int j=0; j<y1;j++)
	{
		//return nothing
	}
    }
    return frame;
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
        Pixel selected;
        try {

            selected = frame.at<Pixel>(centerRow+i/KERNEL_SIDE,centerCol+i%KERNEL_SIDE);
            if(centerCol==0)
            {
                //cout << selected << endl;
            }
        } catch(Exception ex){
            selected = Pixel(0,0,0);
            cout << "blank pixel" << endl;
        }
        photoKernel[i] = selected.x;
    }
}

int clamp(long value,int min,int max)
{
    return (value>max)? max : (value < min) ? min : value;
}
/**
 * Creates a sobel calculated frame from a grayscale image. 
 * for threading, it might be worth getting the grayscale calculation at the same time as the sobel calculation.
 */

Mat threadedSobelFrameFromGrayScale(Mat frame)
{
    //we are using the referenceMat to be sure that we are not going to have any read-write issues WRT the 
    //sobel due to overwrites.
    /*Mat referenceMat;
    frame.copyTo(resultantMat);
    int size = frame.rows*frame.cols;
    for(int i=0;i<frame.rows)
    {
	//pass
    }*/
    return frame;
}

Mat sobelFrameFromGrayScale(Mat frame)
{
    cout << frame.rows << " x " << frame.cols << endl;
    Mat resultantMat;
    frame.copyTo(resultantMat);
    int photoKernel[9];
    for(int row=0;row<frame.rows;row++)
    {
        for(int col=0;col<frame.cols;col++)
        {
            //now we have each pixel location, so do the calculation
            populatePhotoKernel(row,col,frame,photoKernel);
            Pixel * current = resultantMat.ptr<Pixel>(row,col);
            //currently this maxes the result, but IDK if that is realyl what we want in this case. Looks more sobel-y without max
            current->x = 
                current->y = 
                current->z = 
                clamp(
                    VEC_MAG(
                    matValMult(photoKernel,x_kernel),matValMult(photoKernel,y_kernel)),0,UCHAR_MAX);
           

        }
    }
    return resultantMat;
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
