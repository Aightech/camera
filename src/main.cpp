#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "multi_camera.hpp"


int
main(int, char **)
{
    MultiCam cameras;
    for(int i = 0; i < 2; i++)
        cameras.addCamera(i*2, 640, 360);

    for(;;)
    {
        for(int i = 0; i < 2; i++)
            cv::imshow("Warp" + std::to_string(i), cameras.getFrame(i,true));
        cv::imshow("Res", cameras.getFrame(0,true)+cameras.getFrame(1,true));
        if(cv::waitKey(5) >= 0)
            break;
    }
    return 0;
}