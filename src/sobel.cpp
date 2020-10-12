#include "sobel.hpp"
#include <pthread.h>
#include "reader.hpp"
#include "processor.hpp"


using namespace cv;


pthread_attr_t attr;
int k;
Mat inMat;
Mat outMat;

void setThreadOpt()
{
    pthread_attr_init(&attr);
    //Maybe unnecessary to have the re-joiable?
    pthread_attr_setdetatchstate(&attr,PTHREAD_CREATE_JOINABLE);
}

/**
* main for each thread.
*/
void * threadedSobel(void * info)
{
   while(k!='q')
   {
        //do our processing in here
   }
}

/**
* Launches sobel on the given VideoCapture object. Spawns all 4 processing threads
*/
void startSobel(VideoCapture v)
{
    Mat frame = getFrame(v);//get the initial frame
    setThreadOpt();//set the pthread options
    k=0;
    //launch the pthreads
    for(
    while((k=waitKey(1))!='q')
    {
        
    }
}
