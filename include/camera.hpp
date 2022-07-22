#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

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

class Camera
{
    public:
    Camera(int index = 0, int w = 640, int h = 360) : cap_index(index)
    {
        m_cap.open(cap_index, cv::CAP_ANY);
        if(!m_cap.isOpened())
            std::cout << "Camera " << cap_index << " not found" << std::endl;

        m_cap.set(cv::CAP_PROP_FRAME_WIDTH, w);
        m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, h);
        m_cap.read(frame);
        this->set_calibration();

        m_dstTri[0] = cv::Point2f(0.f, 0.f);
        m_dstTri[1] = cv::Point2f(frame.cols - 1.f, 0.f);
        m_dstTri[2] = cv::Point2f(frame.cols - 1.f, frame.rows - 1.f);
        m_dstTri[3] = cv::Point2f(0.f, frame.rows - 1.f);


        m_fs.open("../docs/wrap" + std::to_string(index) + ".xml",
                  cv::FileStorage::READ);


        if(m_fs.isOpened())
        {
            int i=0;
            cv::FileNode fn = m_fs["srcTri"];
            for(cv::FileNodeIterator v = fn.begin(); v!=fn.end();v++)
            {
                double x,y;
                cv::FileNodeIterator it = (*v).begin();
                x = (*it);
                it++;
                y = (*it);
                srcTri[i] = cv::Point2f(x,y);
                std::cout << x << " " << y << std::endl;
                i++;    
            }
            m_fs.release();
        }
        else
        {
            srcTri[0] = cv::Point2f(0.f, 0.f);
            srcTri[1] = cv::Point2f(frame.cols - 1.f, 0.f);
            srcTri[2] = cv::Point2f(frame.cols - 1.f, frame.rows - 1.f);
            srcTri[3] = cv::Point2f(0.f, frame.rows - 1.f);

            cv::namedWindow("Calib" + std::to_string(cap_index));
        cv::imshow("Calib" + std::to_string(cap_index), frame);

        cv::setMouseCallback(
            "Calib" + std::to_string(cap_index),
            [](int event, int x, int y, int flag, void *param)
            {
                if(event == cv::EVENT_LBUTTONDOWN)
                {
                    Camera *cam = ((Camera *)param);
                    int ind = cam->wrap_index;
                    std::cout << "cam" << cam->cap_index << " " << ind << ": ["
                              << x << " ; " << y << " ]" << std::endl;
                    cam->srcTri[ind] = cv::Point2f(x, y);
                    cam->wrap_index = (ind + 1) % 4;
                    for(int i = 0; i < 4; i++)
                        cv::circle(cam->frame, cam->srcTri[i], 5,
                                   cv::Scalar((63 * i) % 255,
                                              (63 * i + 85) % 255,
                                              (63 * i + 170) % 255),
                                   -1);

                    cv::imshow("Calib" + std::to_string(cam->cap_index),
                               cam->frame);
                    if(ind == 3)
                        cam->setup_wrap();
                }
            },
            this);

        }

        this->setup_wrap();

        
    };

    ~Camera(){};

    // void
    // test(int event, int x, int y, int flag, void *param)
    // {
    //     if(event == cv::EVENT_LBUTTONDOWN)
    //     {
    //         uint64_t ind = (uint64_t)param;
    //         m_srcTri[ind][p[ind]] = cv::Point2f(x, y);
    //         p[ind] = (p[ind] + 1) % 4;
    //         cout << " " << ind << " " << p << " " << x << " " << y << endl;
    //     }
    // }

    void
    set_calibration()
    {
        m_fs.open("../docs/out_camera_data.xml", cv::FileStorage::READ);
        m_cameraMatrix = m_fs["camera_matrix"].mat();
        m_distCoeffs = m_fs["distortion_coefficients"].mat();
        m_imageSize = cv::Size(640, 360);
        cv::initUndistortRectifyMap(
            m_cameraMatrix, m_distCoeffs, cv::Mat(),
            cv::getOptimalNewCameraMatrix(m_cameraMatrix, m_distCoeffs,
                                          m_imageSize, 1, m_imageSize, 0),
            m_imageSize, CV_16SC2, m_map1, m_map2);
        m_calibrated = true;
        cv::remap(frame, frame, m_map1, m_map2, cv::INTER_LINEAR);

        std::cout << "Camera " << cap_index << " calibrated" << std::endl;
        m_fs.release();
    }

    void
    setup_wrap()
    {
        m_warpped_frame = cv::Mat::zeros(frame.rows, frame.cols, frame.type());
        m_warp_mat = cv::getPerspectiveTransform(srcTri, m_dstTri);

        m_fs.open("../docs/wrap" + std::to_string(cap_index) + ".xml",
                  cv::FileStorage::WRITE);
        m_fs << "srcTri" << "[" << srcTri[0] << srcTri[1] << srcTri[2] << srcTri[3];
        m_fs.release();
    };

    cv::Mat &
    getFrame(bool transformed = false)
    {
        m_cap.read(frame);
        if(frame.empty())
            std::cout << "Frame " << cap_index << " not found" << std::endl;
        if(m_calibrated)
            cv::remap(frame, frame, m_map1, m_map2, cv::INTER_LINEAR);

        if(transformed)
        {
            cv::warpPerspective(frame, m_warpped_frame, m_warp_mat,
                                m_imageSize);
            return m_warpped_frame;
        }
        else
        {
            return frame;
        }
    }

    int wrap_index = 0;
    cv::Point2f srcTri[4];
    cv::Mat frame;
    int cap_index;

    private:
    cv::VideoCapture m_cap;

    cv::FileStorage m_fs;
    cv::Mat m_cameraMatrix;
    cv::Mat m_distCoeffs;
    cv::Size m_imageSize;
    cv::Mat m_view, m_rview, m_map1, m_map2;
    bool m_calibrated = false;

    cv::Mat m_warp_mat;
    cv::Mat m_warpped_frame;

    cv::Point2f m_dstTri[4];
};

#endif // __CAMERA_HPP__