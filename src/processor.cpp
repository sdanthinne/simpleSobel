#include "processor.hpp"
#include <iostream>
#include <limits.h>
#include <math.h>
#include <pthread.h>
//#include <mutex>

#define RED_CONSTANT 0.2126
#define GREEN_CONSTANT 0.7152 
#define BLUE_CONSTANT 0.0722
#define X_KERNEL {-1,0,1,-2,0,2,-1,0,1}
#define Y_KERNEL {-1,-2,-1,0,0,0,1,2,1}
#define KERNEL_SIZE 9
#define KERNEL_SIDE 3
#define L1_AREA_SIZE 100
#define DIVISOR 4

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

Mat threadFrame[DIVISOR];
pthread_t thread[DIVISOR];
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;
Mat resultantMat;

//struct is passed through to each thread.
//struct threadInfo_s
//{
 //   int thread_number;
  //  Mat frame;
//};

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
 * populates the photo kernel from the frame around the top left
 * COULD BE SOURCE OF MEMORY OVER WRITE
 */
void populatePhotoKernel(int row, int col, Mat frame,int * photoKernel)
{
    for(int i=0;i<KERNEL_SIDE;i++)
    {
        //this I think is slow. 
        //May need performance optimization in the future.
        uint8_t * selected = frame.ptr<uint8_t>(
                row+i,
                col);

        //photoKernel[i] = selected.x;

        //(centerCol>frame.cols+)
        photoKernel[i*3] = selected[0];
        photoKernel[i*3+1] = selected[1];
        photoKernel[i*3+2] = selected[2];
    }
}

int clamp(long value,int min,int max)
{
    return (value>max)? max : (value < min) ? min : value;
}



Mat sobelFrameFromGrayScale(Mat frame,Mat outFrame)
{
    int photoKernel[9];
    for(int row=1;row<frame.rows-1;row++)
    {
        for(int col=1;col<frame.cols-1;col++)
        {
            //now we have each pixel location, so do the calculation
            populatePhotoKernel(row-1,col-1,frame,photoKernel);
            uint8_t * current = (outFrame).ptr<uint8_t>(row,col);
            *current = 
                clamp(
                    VEC_MAG(
                    matValMult(photoKernel,x_kernel),matValMult(photoKernel,y_kernel)),0,UCHAR_MAX);
           

        }
    }
    return outFrame;
}


/**
* This function is used to grayscale out a frame.
*/
Mat grayscaleFrame(Mat frame,Mat grayFrame,int thread_number)
{

    uchar * grayPointer;
    for(int i=0; i<frame.rows;i++)
    {
        Pixel * row_ptr = frame.ptr<Pixel>(i);
        grayPointer = grayFrame.ptr<uchar>(i);
        for(int j=0; j<frame.cols;j++)
        {
            
            float newC = row_ptr[j].x*BLUE_CONSTANT + row_ptr[j].y*GREEN_CONSTANT + row_ptr[j].z*RED_CONSTANT;
                                
            grayPointer[j] =(uint8_t)newC;
        }
    }
    cout << thread_number << endl;

    /*try{
    frame.forEach<Pixel>([&](Pixel &p, const int * position) ->  void {
        Mat grayNotPoint = (*grayFrame);
    });
    }catch (Exception ex)
    {
        cout << "grayscale ex" << endl;
    }*/
    return grayFrame;
}

Mat sobelFrame(Mat frame,Mat outFrame,Mat grayFrame, int thread_number)
{
    return sobelFrameFromGrayScale(grayscaleFrame(frame,grayFrame,thread_number),outFrame);
}

/**
 * divides parent mat into quadrants with overlap=2px
 * (assumes that video sizes are even.)
 * [0,1]
 * [2,3]
 */
Mat * split4FromParent(Mat parent,Mat * matCollection)
{
    int colSize = parent.cols/2;
    int rowSize = parent.rows/2;

    matCollection[0] = parent(Range(0,rowSize+1),Range(0,colSize+1));
    matCollection[1] = parent(Range(0,rowSize+1),Range(colSize-1,parent.cols));
    matCollection[2] = parent(Range(rowSize-1,parent.rows),Range(0,colSize+1));
    matCollection[3] = parent(Range(rowSize-1,parent.rows),Range(colSize-1,parent.cols));
    return matCollection;

}

/**
 * runs on thread PER FRAME to do the processing.
 */
void * launchThread(void * info)
{
    //sobelFrame(((threadInfo_s *)info)->frame,0,0);
    pthread_mutex_lock(&process_mutex);//we finished processing, write to the global obj
    //here we need to combine the rest
    //merge4ToParent(&resultantMat,(threadInfo_s *)info); 
    //cout << "resultSize: " << resultant.rows << "x"<< resultant.cols << endl;
    //((threadInfo_s *)info)->frame.copyTo(resultantMat);
    pthread_mutex_unlock(&process_mutex);
}



/**
 * Creates a sobel calculated frame from a grayscale image. 
 * for threading, it might be worth getting the grayscale calculation at the same time as the sobel calculation.
 */
Mat threadedSobelFrame(Mat frame)
{

    Mat matCollection[DIVISOR];
    split4FromParent(frame,matCollection);
    threadInfo_s info[DIVISOR];
    //slow, might just copy mat size
    frame.copyTo(resultantMat);
    void * threadStatus[DIVISOR];
    //following lines NEED TO BE MOVED
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);//make threads joinable

    //populate the threadInfo struct
    for(int i=0; i<DIVISOR;i++)
    {
        info[i].frame = matCollection[i];
        info[i].thread_number = i;
        
    }
    for(int i=0;i<DIVISOR;i++)
    {
        pthread_create(&thread[i],&attr,launchThread,(void *)&info[i]);
    }
    for(int i=0;i<DIVISOR;i++)
    {
        pthread_join(thread[i],&threadStatus[i]);
    }
    return frame;
}

/**
* Performs a sobel filter on a Mat frame
*/
Mat sobel(Mat frame)
{   
    
    cout << frame.rows << " x " << frame.cols << endl;
    return threadedSobelFrame(frame);
    //return sobelFrameFromGrayScale(grayscaleFrame(frame));
}
