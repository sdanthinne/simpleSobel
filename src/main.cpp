#include "reader.hpp"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#define NUMTHREADS 2
#define ROUNDS 2
using namespace std;
/*
pthread_barrier_t barrier;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

int input=ROUNDS;

void * thread_program(void * threadid)
{
    //do some computation
    int result =100;
    while(result!=0)
    {
        result = input*3;
        pthread_mutex_lock(&mymutex);
        cout <<  result << " inc "<< threadid << endl;
        pthread_mutex_unlock(&mymutex);
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(NULL);
    //wait
    return 0;
}


int main()
{
    pthread_t threads[NUMTHREADS];

    pthread_barrier_init(&barrier,NULL,NUMTHREADS);
    for(int i=0;i<NUMTHREADS;i++)
    {
        pthread_create(&threads[i],NULL,thread_program,(void*)i);
    }
    for(int i=0;i<ROUNDS;i++)
    {
        //following line is probably problematic 
        //due to it decrementing the thread counter in addition to all of the other threads.
        pthread_mutex_lock(&mymutex);
        cout << "finished waiting" << endl;
        input--;
        pthread_mutex_unlock(&mymutex);
    }
    pthread_barrier_destroy(&barrier);
    return 0;
}
*/
int main(int argc, char** argv)
{
    char * videoTitle;
    if(argc == 2)
    {
        int titleLength = strlen(argv[1]);
        videoTitle = argv[1];
        if(strcmp((videoTitle+titleLength-4),".avi")!=0)
        {
            cout << "invalid file type" << endl;
            return 1;
        }else if(access(videoTitle,F_OK)==-1)
        {
            cout << "that file does not exist" << endl;
            return 1;
        }
    }else
    {
        return 1;
    }
    cv::VideoCapture video = readVideo(videoTitle);
    if(video.isOpened())
    {
        displayFrame(video);
    }
    return 0;
}
