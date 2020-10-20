#include "sobel.hpp"

#include "reader.hpp"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#define NUMTHREADS 2
#define ROUNDS 2
using namespace std;
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
        startSobel(video);
    }
    return 0;
}
