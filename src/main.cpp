#include "sobel.hpp"
#include <opencv2/videoio.hpp>
#include "reader.hpp"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#define NUMTHREADS 2
#define ROUNDS 2
using namespace std;
int main(int argc, char** argv)
{
    char * videoTitle;
    cv::VideoCapture video;
    if(argc == 2)
    {
        int titleLength = strlen(argv[1]);
        videoTitle = argv[1];
        if(strcmp(argv[1],"-v")==0)
        {
            cout << "using Video feed 0" << endl;
            video.open(0);
        }
        else if(strcmp((videoTitle+titleLength-4),".avi")!=0)
        {
            cout << "invalid option type" << endl;
            return 1;
        }else if(access(videoTitle,F_OK)==-1)
        {
            cout << "that file does not exist" << endl;
            return 1;
        }else 
        {
            video.open(videoTitle);
        }
    }else
    {
        return 1;
    }
    if(video.isOpened())
    {
        startSobel(video);
    }
    return 0;
}
