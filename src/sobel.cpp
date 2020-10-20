/**
 * This file contains most of the thread management and process launching.
 */
#include "sobel.hpp"
#include "reader.hpp"
#include "processor.hpp"
#include <pthread.h>
#include <signal.h>
#include <ctime>
#define THREAD_COUNT 4

using namespace cv;
using namespace std;

pthread_attr_t attr;
int k;//our input key and stop condition
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

double approxRollingAverage (double avg, double new_sample,int rnumRounds) {

    avg -= avg / rnumRounds;
    avg += new_sample / rnumRounds;

    return avg;
}

void intHandler(int n)
{
    cout << "Average time for frame processing was: " << averageTime << endl;
    exit(0);

}

void setThreadOpt()
{
    pthread_attr_init(&attr);
    signal(SIGINT,intHandler);
    //Maybe unnecessary to have the re-joiable?
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
    pthread_barrier_init(&sobel_barrier,NULL,THREAD_COUNT+1);
}

/**
* main for each thread.
*/
void * threadedSobel(void * info)
{
   while(k!='q')
   {
        int thread_num =  ((threadInfo_s *)info) -> thread_number;
        //waitKey(0);
        sobelFrame(splitMats[thread_num],
                outSplitMats[thread_num],
                graySplitMats[thread_num]);
        pthread_barrier_wait(&sobel_barrier);//wait for every thread to finish processing
        pthread_barrier_wait(&sobel_barrier);//wait for the parent to give new jobs accordingly
   }
}

/**
* Launches sobel on the given VideoCapture object. Spawns all 4 processing threads
*/
void startSobel(VideoCapture v)
{
    inMat = getFrame(v);//get the initial frame
    split4FromParent(inMat,splitMats);
    setThreadOpt();//set the pthread options
    k=0;//input key
    double ttime = 0;
    averageTime = 40;
    numRounds = 1;
    outFrame = Mat(Size(inMat.cols,inMat.rows),CV_8UC1);

    grayFrame = Mat(Size(inMat.cols,inMat.rows),CV_8UC1);

    split4FromParent(outFrame,outSplitMats);

    split4FromParent(grayFrame,graySplitMats);
    //displayFrameMat(grayFrame);
    //waitKey(2000);

    threadInfo_s thread_infos[4];
    //launch the pthreads
    for(int i=0;i<THREAD_COUNT;i++)
    {
        thread_infos[i].thread_number = i;
        if(pthread_create(&threads[i],&attr,threadedSobel,(void*)&thread_infos[i])!=0)
        {
            cerr << "problem spawning threads" << endl;
            return;
        }
    }
    while((k=waitKey(1))!='q')
    {
	ttime = clock();
        pthread_barrier_wait(&sobel_barrier);
	ttime = ((float)(clock()-ttime))/CLOCKS_PER_SEC;
	cout << "time to sobel: " << ttime << "s" <<endl;
	averageTime = approxRollingAverage(averageTime,ttime,numRounds++);
        //here, we fill the next frame
        displayFrameMat(outFrame);
        inMat = getFrame(v);
        split4FromParent(inMat,splitMats);
        pthread_barrier_wait(&sobel_barrier);
    }
    //something strange going on at exit
    for(int i=0;i<THREAD_COUNT;i++)
    {
        if(pthread_join(threads[i],NULL)!=0)
        {
            cerr << "problem joining threads" << endl;
            return;
        }
    }

    pthread_barrier_destroy(&sobel_barrier);
}
