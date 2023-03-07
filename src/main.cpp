#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "multi_camera.hpp"

#include <lsl_c.h>

int
main(int argc, char **argv)
{
    std::string name = (argc>1)?argv[1]:"vid";
    MultiCam cameras;
    int n =argc-2;
    std::cout << "Number of cameras: " << n << std::endl;
    for(int i = 0; i < n; i++) cameras.addCamera(atoi(argv[i+2]), 640, 480);//1920, 1080);//3264, 2448);//1920, 1080);//640, 480);//3264, 2448);
    cameras.set_video_writer(name,10);

    
    for(;;)
    {
        for(int i = 0; i < n; i++)
        {
            cv::imshow("Warp" + std::to_string(i), cameras.getFrame(i, true));
            cameras.record_frame(i);
        }

        if(cv::waitKey(5) >= 0)
            break;
    }
    return 0;
}
