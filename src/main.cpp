#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "multi_camera.hpp"

#include <lsl_c.h>

int
main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " camera_index" << std::endl;
        return 0;
    }
    int index = atoi(argv[1]);
    Camera camera(index, 1);
    camera.set_resolution(640, 480);
    camera.set_wrap("wrap.xml");

    for(;;)
    {
        cv::imshow("Warp", camera.getFrame(true));
        if(cv::waitKey(5) >= 0)
            break;
    }

    //std::string name = (argc>1)?argv[1]:"vid";
    // MultiCam cameras;
    // int n =argc-2;
    // std::cout << "Number of cameras: " << n << std::endl;
    // for(int i = 0; i < n; i++) cameras.addCamera(atoi(argv[i+2]),1);//, 640, 480);//1920, 1080);//3264, 2448);//1920, 1080);//640, 480);//3264, 2448);
    // cameras.set_video_writer(name,10);

    
    // for(;;)
    // {
    //     for(int i = 0; i < n; i++)
    //     {
    //         cv::imshow("Warp" + std::to_string(i), cameras.getFrame(i, true));
    //         cameras.record_frame(i);
    //     }

    //     if(cv::waitKey(5) >= 0)
    //         break;
    // }
    return 0;
}
