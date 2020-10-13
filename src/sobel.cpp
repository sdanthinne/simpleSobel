/**
 * This file contains most of the thread management and process launching.
 */
#include "sobel.hpp"
#include <pthread.h>
#include "reader.hpp"
#include "processor.hpp"

#define THREAD_COUNT 4

using namespace cv;
using namespace std;

pthread_attr_t attr;
int k;//our input key and stop condition
Mat inMat;
Mat outMat;
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
        //do our processing in here
        
        pthread_barrier_wait(&sobel_barrier);
        pthread_barrier_wait(&sobel_barrier);
   }
}

/**
* Launches sobel on the given VideoCapture object. Spawns all 4 processing threads
*/
void startSobel(VideoCapture v)
{
    inMat = getFrame(v);//get the initial frame
    
    setThreadOpt();//set the pthread options
    k=0;
    //launch the pthreads
    for(int i=0;i<THREAD_COUNT;i++)
    {
        if(pthread_create(&threads[i],&attr,threadedSobel,NULL)!=0)
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
        pthread_barrier_wait(&sobel_barrier);
    }
    pthread_barrier_destroy(&sobel_barrier);
}
