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
    for(int i = 0; i < 2; i++) cameras.addCamera(i *2, 640, 480);
    //for(int i = 0; i < 2; i++) cameras.set_video_writer(name,10);

    
    for(;;)
    {
        for(int i = 0; i < 2; i++)
        {
            cv::imshow("Warp" + std::to_string(i), cameras.getFrame(i, true));
            //cameras.record_frame(i);
        }

        if(cv::waitKey(5) >= 0)
            break;
    }
    return 0;
}