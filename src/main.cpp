#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "camera.hpp"

using namespace std;
int p[2] = {0, 0};
std::vector<cv::Point2f[4]> srcTri(2);

int coor[2 * 2 * 4] = {
    451, 257, 186, 292, 183, 74, 425, 98, 443, 287, 187, 281, 199, 124, 422, 67,
};
void
mouse_callback(int event, int x, int y, int flag, void *param)
{
    if(event == cv::EVENT_LBUTTONDOWN)
    {
        uint64_t ind = (uint64_t)param;
        srcTri[ind][p[ind]] = cv::Point2f(x, y);
        p[ind] = (p[ind] + 1) % 4;
        cout << " " << ind << " " << p << " " << x << " " << y << endl;
    }
}

int
main(int, char **)
{
    std::vector<Camera*> cameras;
    for(int i = 0; i < 2; i++)
    {
        cameras.push_back(new Camera(i*2, 640, 360));
        
        //cameras[i]->set_wrap(coor + i * 2 * 4);
    }

    for(;;)
    {
        // wait for a new
        for(int i = 0; i < 2; i++)
        {
            cv::imshow("Warp" + std::to_string(i), cameras[i]->getFrame(true));
        }
        //cv::hconcat(warps[0](cv::Rect(0, 0, 320, 360)), warps[1](cv::Rect(320, 0, 320, 360)), res);
        //cv::imshow("res", res);
        if(cv::waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}