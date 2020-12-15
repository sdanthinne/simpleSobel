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
    
    int flags=0,opt;
    while((opt = getopt(argc, argv,"rvf:h"))!=-1)
    {
        switch(opt)
        {
            case 'r':
                cout << "Using reference" << endl;
                flags |= 1;
                break;
            case 'v':
                flags |= 2;
                break;
            case 'f':
                flags |= 4;
                videoTitle = optarg;
                break;
            case 'h':
                cout << "Usage: [executable] -[r(v|f:)] [f:filename]" << endl;
                return 0;
                break;
            default:
                break;
        }
    }
    /*if((flags&~(4+2))==0)
    {
        cout << "incorrect argument configuration"<< endl;
        return 1;
    }*/
    if(flags&4)
    {
        if(access(videoTitle,F_OK)==-1)
        {
            cout << "nonexistant file" << endl;
        }
        video.open(videoTitle);

    }
    if(flags&2)
    {
        cout << "using video feed 0" << endl;
        video.open(0);
    }
    
    if(video.isOpened())
    {
        if(flags&1)
        {
            startReferenceSobel(video);
        }else
        {
           startSobel(video);
        }
    }
    return 0;
}
