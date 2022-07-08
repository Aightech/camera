#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
int p[2] = {0,0};
std::vector<cv::Point2f[4]> srcTri(2);

int coor[2*2*4] = {222, 135, 421, 93, 417, 283, 178, 272, 148, 70, 389, 100, 431, 245, 136, 275};

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
    cv::FileStorage fs("../docs/out_camera_data.xml", cv::FileStorage::READ);
    cv::Mat cameraMatrix = fs["camera_matrix"].mat();
    cv::Mat distCoeffs = fs["distortion_coefficients"].mat();
    cv::Size imageSize(640, 360);
    cv::Mat view, rview, map1, map2;

    cv::initUndistortRectifyMap(
        cameraMatrix, distCoeffs, cv::Mat(),
        cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1,
                                      imageSize, 0),
        imageSize, CV_16SC2, map1, map2);

    int n = 2;
    std::vector<cv::Mat> frames(n), warps(n);
    std::vector<cv::VideoCapture> caps(n);
    for(int i = 0; i < n; i++)
    {
        caps[i].open(i * 2, cv::CAP_ANY);
        if(!caps[i].isOpened())
        {
            cout << "Camera " << i << " not found" << endl;
            return -1;
        }
        caps[i].set(cv::CAP_PROP_FRAME_WIDTH, 640);
        caps[i].set(cv::CAP_PROP_FRAME_HEIGHT, 360);
        caps[i].read(frames[i]);
        cv::namedWindow("Frame " + std::to_string(i));
        cv::setMouseCallback("Frame " + std::to_string(i), mouse_callback, (void*)(uint64_t)i);
        for(int j = 0; j < 4; j++)
        {
            srcTri[i][j] = cv::Point2f(coor[i*2*4+j*2], coor[i*2*4+j*2+1]);
        }
        warps[i] = cv::Mat::zeros(frames[i].rows, frames[i].cols,
                                              frames[i].type());
    }

    cv::Point2f dstTri[4];
    dstTri[0] = cv::Point2f(0.f, 0.f);
    dstTri[1] = cv::Point2f(frames[0].cols - 1.f, 0.f);
    dstTri[2] = cv::Point2f(frames[0].cols - 1.f, frames[0].rows - 1.f);
    dstTri[3] = cv::Point2f(0.f, frames[0].rows - 1.f);
    cv::Mat res;
    for(;;)
    {
        // wait for a new
        for(int i = 0; i < n; i++)
        {
            caps[i].read(frames[i]);
            if(frames[i].empty())
            {
                cout << "Frame " << i << " not found" << endl;
                return -1;
            }
            remap(frames[i], frames[i], map1, map2, cv::INTER_LINEAR);
            cv::imshow("Frame " + std::to_string(i), frames[i]);
            cv::Mat warp_mat = cv::getPerspectiveTransform(srcTri[i], dstTri);
            cv::warpPerspective(frames[i], warps[i], warp_mat, warps[i].size());
            cv::imshow("Warp"+ std::to_string(i), warps[i]);
        }
        cv:hconcat(warps[1](cv::Rect(0,0,320,360)), warps[0](cv::Rect(320,0,320,360)), res);
        cv::imshow("res", res);
        if(cv::waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}