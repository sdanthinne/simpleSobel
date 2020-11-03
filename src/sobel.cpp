/*******************************************************************************
 * File: soble.cpp
 *
 * Description: This file contains the thread management and process launching. 
 *
 * Author: Sebastien Danthinne and Erin Rylie Clark
 * ****************************************************************************/
#include "sobel.hpp"
#include "reader.hpp"
#include "processor.hpp"
#include <pthread.h>
#include <signal.h>
#include <ctime>
#include <perfmon/pfmlib.h>

#define THREAD_COUNT 4

using namespace cv;
using namespace std;

pthread_attr_t attr;
int k; /* our input key and stop condition */
Mat inMat;
Mat outFrame;
Mat grayFrame;
Mat splitMats[4];
Mat outSplitMats[4];
Mat graySplitMats[4];
pthread_barrier_t sobel_barrier;
pthread_t threads[THREAD_COUNT];
double averageTime;
int numRounds;
pthread_mutex_t lock; 
/* Calculate the average time to compute sobel filter */
double approxRollingAverage (double avg, double new_sample,int rnumRounds) {
    avg -= avg / rnumRounds;
    avg += new_sample / rnumRounds;
    return avg;
}

void intHandler(int n) /* print the average time when the program is exited */
{
    cout << "Average time for frame processing was: " << averageTime << endl;
    exit(0);
}

void setThreadOpt() /* Set to run the final function on exit */
{
    pthread_attr_init(&attr);
    signal(SIGINT,intHandler);
    //Maybe unnecessary to have the re-joiable?
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
    pthread_barrier_init(&sobel_barrier,NULL,THREAD_COUNT+1);
}

/*------------------------------------------------------------------------------
 * Function: threadedSobel
 * Description: Giving each thread their portion of the frame to process. This
 *    is basically the main function for each thread.
 * param: info - the number of each thread
 * -----------------------------------------------------------------------------*/
void * threadedSobel(void * info)
{

    pfm_pmu_encode_arg_t raw;
    memset(&raw,0,sizeof(raw));

    if(pfm_initialize()!=PFM_SUCCESS)
    {
        pthread_mutex_lock(&lock);
        cout << "init PFM failed" << endl;
        pthread_mutex_unlock(&lock); 
    }
     


   while(k!='q')
   {
        //int thread_info = pfm_get_event_attr_info(int idx, int attr, PFM_OS_PERF_EVENT, &info);


        int ret = pfm_get_os_event_encoding("CPU_CYCLES",PFM_PLM0,PFM_OS_NONE,&raw);
        if(ret != PFM_SUCCESS)
        {
            cout<< "bad event get" << endl;
        }
        uint64_t cycles = raw.codes[0];

        int thread_num =  *((int *)info);

        sobelFrame(splitMats[thread_num],
                outSplitMats[thread_num],
                graySplitMats[thread_num]);

        memset(&raw,0,sizeof(raw));
        ret = pfm_get_os_event_encoding("CPU_CYCLES",PFM_PLM0,PFM_OS_NONE,&raw);
        if(ret != PFM_SUCCESS)
        {
            cout<< "bad event get" << endl;
        }
        cycles = raw.codes[0] - cycles;

        pthread_mutex_lock(&lock);
        cout << "number of cycles for quadrant: " << cycles << endl;
        pthread_mutex_unlock(&lock);

        pthread_barrier_wait(&sobel_barrier);
	    /* wait for every thread to finish processing */
        pthread_barrier_wait(&sobel_barrier);
	    /* wait for the parent to give new jobs accordingly */
   }
}

/*------------------------------------------------------------------------------
 * Function: startSobel
 * Description: startSoble initializes the Mat objects, splits them all into
 *    four pieces and then waits for each thread to finish applying the sobel
 *    filter to its piece of the frame. The function will then grab the next
 *    frame, split it again, and send the threads off. Repeat until video is
 *    over.
 * param: v - video to process
 * -----------------------------------------------------------------------------*/
void startSobel(VideoCapture v)
{
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
    } 
    inMat = getFrame(v); /* get the initial frame */
    split4FromParent(inMat,splitMats); /* Separate the frame into four quads */
    setThreadOpt(); /*set the pthread options */
    k=0;//input key
    
    /* Next 3 lines are for determining timing */
    double ttime = 0;
    averageTime = 40;
    numRounds = 1;

    /* Initialize the Mat objects to contain the same rows and cols as the frame
     * from the video. outFrame contains the sobel image, grayFrame contains the
     * gray image */
    outFrame = Mat(Size(inMat.cols,inMat.rows),CV_8UC1);
    grayFrame = Mat(Size(inMat.cols,inMat.rows),CV_8UC1);

    /* Split each Mat into four quads to give to each threaded process */
    split4FromParent(outFrame,outSplitMats);
    split4FromParent(grayFrame,graySplitMats);

    /* Tell each thread what number it is */
    int thread_infos[4] = {0, 1, 2, 3};
    for(int i=0;i<THREAD_COUNT;i++)
    {
        if(pthread_create(&threads[i],&attr,threadedSobel,(void*)&thread_infos[i])!=0)
        { /* spawn the threads and give it the function to start executing */
            cerr << "problem spawning threads" << endl;
            return;
        }
    }

    /* The following loop follows the process of waiting for each
     * thread to hit its barrier after finishing applying the sobel filter,
     * displaying the frame from the parent thread, splitting the next 
     * frame into four parts, and sending the child threads to apply the 
     * filter again. */
    while((k=waitKey(1))!='q') 
    {
	    ttime = clock(); /* time how long it takes to get each sobel frame */
        pthread_barrier_wait(&sobel_barrier); 
	    /* wait for each thread to finish applying sobel filter */
	    /* Next 3 lines for getting timing information */

	    ttime = ((float)(clock()-ttime))/CLOCKS_PER_SEC;
	    cout << "time to sobel: " << ttime << "s" <<endl;
	    averageTime = approxRollingAverage(averageTime,ttime,numRounds++);
        /* Now display the frame and give each thread a new frame */
        displayFrameMat(outFrame); /* Display sobel frame */
        inMat = getFrame(v); /* Get the next frame in the video */
        if(inMat.empty())
            break;
        split4FromParent(inMat,splitMats); /* split the frame into quads */
        pthread_barrier_wait(&sobel_barrier); /* Allow the threads to work */
    }
    /* Join the threads back together when the video is done, or the user
     * presses q to quit */
    /*for(int i=0;i<THREAD_COUNT;i++)
    {
        if(pthread_join(threads[i],NULL)!=0)
        {
            cerr << "problem joining threads" << endl;
            return;
        }
    }*/
    /* Destroy the threads when done */
    //pthread_barrier_destroy(&sobel_barrier);
}
