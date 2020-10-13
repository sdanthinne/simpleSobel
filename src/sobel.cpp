/**
 * This file contains most of the thread management and process launching.
 */
#include "sobel.hpp"
#include "reader.hpp"
#include "processor.hpp"
#include <pthread.h>

#define THREAD_COUNT 4

using namespace cv;
using namespace std;

pthread_attr_t attr;
int k;//our input key and stop condition
Mat inMat;
Mat outMat;
Mat splitMats[4];
pthread_barrier_t sobel_barrier;
pthread_t threads[THREAD_COUNT];

void setThreadOpt()
{
    pthread_attr_init(&attr);
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
        outMat = sobelFrame(splitMats[((threadInfo_s *)info) -> thread_number]);
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
        pthread_barrier_wait(&sobel_barrier);
        //here, we fill the next frame
        displayFrameMat(outMat);
        inMat = getFrame(v);
        split4FromParent(inMat,splitMats);
        pthread_barrier_wait(&sobel_barrier);
    }
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
