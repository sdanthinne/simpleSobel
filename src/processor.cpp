/**
 * File: processor.cpp
 *
 * Description: Contains all functions relating to the processing of each Mat/SubMat for a 
 * sobel algorithm.
 *
 * Author: Erin Clark and Sebastien Danthinne
 *
 * Revisions:
 * 
 */
#include "processor.hpp"
#include <iostream>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <arm_neon.h>

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
#define VEC_MAG(A,B)\
    ((A) > (B)) ? 15*((A) + ((B)>>1))/16 : 15*((B) + ((A)>>1))/16


using namespace cv;
using namespace std;


int8_t x_kernel[] = X_KERNEL;
int8_t y_kernel[] = Y_KERNEL;

Mat threadFrame[DIVISOR];
pthread_t thread[DIVISOR];
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;
Mat resultantMat;

/*-----------------------------------------------------------------------------
 * Function: matValMult
 *
 * Description: performs the actual multiplication of two 1d KERNEL_SIZE arrays
 *
 * array1[]: int []: first array being multiplied
 * array2[]: int []: second array being multiplied
 *
 * return: long: sum of the product of the two arrays
 *---------------------------------------------------------------------------*/

int matValMult(int8_t array1[], int8_t array2[])
{
    int resultant=0;
    int8_t arrayR[9];
    int8x16_t v1, v2;
    v1 = vld1q_s8(array1);
    v2 = vld1q_s8(array2);
    v2 = vmulq_s8(v1,v2);
    
    //temp1 = vget_low_s8(vR);
    //temp2 = vget_high_s8(vR);
    //temp3 = vadd_s8(temp1,temp2);//after this we now have a 8x8 that has it added together
    vst1q_s8(arrayR,v2);

    for(int i=0; i<KERNEL_SIZE;i++)
    {
        resultant += arrayR[i];
    }
    return resultant;
}

/*-----------------------------------------------------------------------------
 * Function: populatePhotoKernel
 *
 * Description: Populates the photo kernel of size KERNEL_SIZE from the 
 *  top-right with col and row as the x and y coordinates of the top-right 
 *  of the resultant array
 *
 * row: int : the reference y value of where the kernel should start
 * col: int : the reference x value of where the kernel should start
 * frame: cv::Mat : the frame in which to pull the values from
 * photoKernel: int * : the reference to a KERNEL_SIZE array where the 
 *  resultant pixels will be populated
 *
 * return: void:
 *---------------------------------------------------------------------------*/

void populatePhotoKernel(int row, int col, Mat frame,int8_t * photoKernel)
{
    for(int i=0;i<KERNEL_SIDE;i++)
    {
        //Describes 
        uint8_t * selected = frame.ptr<uint8_t>(
                row+i,
                col);

        photoKernel[i*3] = selected[0];
        photoKernel[i*3+1] = selected[1];
        photoKernel[i*3+2] = selected[2];
    }
}

/*-----------------------------------------------------------------------------
 * Function: clamp
 *
 * Description: clamps the value within the min and max values 
 *  (supports up to int sizes)
 *
 * value: long: the value that needs to be clamped
 * min: int: the min value of the clamp
 * max: int: the max value of the clamp
 *
 * return: int: the clamped value
 *---------------------------------------------------------------------------*/
int clamp(long value,int min,int max)
{
    return (value>max)? max : (value < min) ? min : value;
}


/*-----------------------------------------------------------------------------
 * Function: sobelFrameFromGrayScale
 *
 * Description: performs a sobel filter on the frame inFrame and places it in 
 *  the outFrame. outFrame MUST already have memory allocated for the Mat value
 *  (assumes that the input frame is already grayscale)
 *
 * inFrame: cv::Mat: the input frame for the filter to be applied
 * outFrame: cv::Mat: the output frame for which the resulting filtered frame
 *  will be put. Should already have memory allocated for it.
 *
 * return: cv::Mat: the same reference to outFrame
 *---------------------------------------------------------------------------*/
Mat sobelFrameFromGrayScale(Mat inFrame,Mat outFrame)
{

    int8_t photoKernel[9];
    for(int row=1;row<inFrame.rows-1;row++)
    {
        for(int col=1;col<inFrame.cols-1;col++)
        {
            //now we have each pixel location, so do the calculation
            populatePhotoKernel(row-1,col-1,inFrame,photoKernel);
            uint8_t * current = (outFrame).ptr<uint8_t>(row,col);
            *current = 
                clamp(
                    VEC_MAG(
                    matValMult(photoKernel,x_kernel),matValMult(photoKernel,y_kernel)),0,UCHAR_MAX);
           

        }
    }
    return outFrame;
}


/*-----------------------------------------------------------------------------
 * Function: grayscaleFrame
 *
 * Description: performs a grayscale filter on the inFrame and applies it 
 *  to the grayFrame
 *
 * inFrame: cv::Mat: the input frame for the filter to be applied
 * grayFrame: cv::Mat: the output frame for which the resulting filtered frame
 *  will be put. Should already have memory allocated for it.
 *
 * return: cv::Mat: the same reference to grayFrame
 *---------------------------------------------------------------------------*/
Mat grayscaleFrame(Mat inFrame,Mat grayFrame)
{

    uchar * grayPointer;
    for(int i=0; i<inFrame.rows;i++)
    {
        Pixel * row_ptr = inFrame.ptr<Pixel>(i);
        grayPointer = grayFrame.ptr<uchar>(i);

        int counter=0;
        for(int j=0; j<inFrame.cols;j++)
        {
            
            float newC = row_ptr[j].x*BLUE_CONSTANT + row_ptr[j].y*GREEN_CONSTANT + row_ptr[j].z*RED_CONSTANT;
                                
            grayPointer[j] =(uint8_t)newC;

            //counter++;
        }

    }
    return grayFrame;
}


/*-----------------------------------------------------------------------------
 * Function: grayScaleRowNEON
 * 
 * Description: takes the next 4 values and grayscales them all at once
 */
void grayScaleRowNEON(Pixel * start,uchar* grayPointer)
{
    float32x4_t constant;
    uint8x8x3_t vI;

    uint32x4_t vPF;
    float32x4_t rvF,gvF,bvF;
    //constant = vdupq_n_f32(rgbToGrayScale[i]);

    vI = vld3_u8((uint8_t *)(start));

    rvF = vcvtq_f32_u32(vmovl_u16(vget_low_u8(vmovl_u8(vI.val[0]))));//this gets the lower 4 values in a 16x4, then moves it to a 32x4(red
    gvF = vcvtq_f32_u32(vmovl_u16(vget_low_u8(vmovl_u8(vI.val[1]))));//this gets the lower 4 values in a 16x4, then moves it to a 32x4(green
    bvF = vcvtq_f32_u32(vmovl_u16(vget_low_u8(vmovl_u8(vI.val[2]))));//this gets the lower 4 values in a 16x4, then moves it to a 32x4(blue


    //vF = vcvtq_f32_u32(vPF);
    
    rvF = vmulq_n_f32(rvF,RED_CONSTANT);
    gvF = vmulq_n_f32(gvF,GREEN_CONSTANT);
    bvF = vmulq_n_f32(bvF,BLUE_CONSTANT);

    rvF = vaddq_f32(rvF,gvF);
    rvF = vaddq_f32(rvF,bvF);

    uint16x4_t out = vmovn_u32(vcvtq_u32_f32(rvF));//16x4
    
    //need a better solution than this
    grayPointer[0] = vget_lane_u16(out,0);
    grayPointer[1] = vget_lane_u16(out,1);
    grayPointer[2] = vget_lane_u16(out,2);
    grayPointer[3] = vget_lane_u16(out,3);

    //vst1q_u8(grayPointer,vmovn_u16(  vmovn_u32(vcvtq_u32_f32(rvF))));//16x4
    

}

/*-----------------------------------------------------------------------------
 * Function: sobelFrame
 *
 * Description: performs a complete sobel filter on a given frame from color
 *
 * inFrame: cv::Mat: the input frame for the filter to be applied
 * outFrame: cv::Mat: the output frame for which the resulting filtered frame
 *  will be put. Should already have memory allocated for it.
 * grayFrame: cv::Mat: the grayscale intermidiate frame for which the resulting
 *  grayscale'd frame will reside. Should already have memory allocated for it.
 *
 * return: cv::Mat: the same reference to outFrame
 *---------------------------------------------------------------------------*/
Mat sobelFrame(Mat inFrame,Mat outFrame,Mat grayFrame)
{
    return sobelFrameFromGrayScale(grayscaleFrame(inFrame,grayFrame),outFrame);
}

/*-----------------------------------------------------------------------------
 * Function: split4FromParent
 *
 * Description: splits a parent mat into quadrants with a 2px overlap of each.
 *  Assumes that video sizes are even. Order is as follows:
 *  [0,1]
 *  [2,3]
 *
 * parent: cv::Mat: the Mat to be split up into children
 * matCollection: cv::Mat*: a reference to an array of 4 Mat objects to be 
 *  populated with sub-Mats of the parent
 *
 * return: cv::Mat: the pointer to the first element of the 4 array
 *---------------------------------------------------------------------------*/
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
