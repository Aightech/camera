#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "strANSIseq.hpp"
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

class Camera : public virtual ESC::CLI
{
    public:
    Camera(int index = 0, int verbose = -1)
        : cap_index(index), CLI(verbose, "camera_" + std::to_string(index))
    {
        m_cap.open(cap_index); // //, cv::CAP_V4L);
        if(!m_cap.isOpened())
            throw std::string("Camera " + std::to_string(cap_index) +
                              " not found");

        m_cap.set(cv::CAP_PROP_FPS, 30);
        m_cap.set(cv::CAP_PROP_FOURCC,
                  cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

        m_imageSize = cv::Size(m_cap.get(cv::CAP_PROP_FRAME_WIDTH),
                               m_cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        m_fps = m_cap.get(cv::CAP_PROP_FPS);
        logln("Resolustion " + std::to_string(m_imageSize.width) + "*" +
                  std::to_string(m_imageSize.height) + " " +
                  std::to_string(m_fps) + "fps",
              true);
    };

    ~Camera()
    {
        if(m_cap.isOpened())
            m_cap.release();
        if(m_video != nullptr)
        {
            logln("Closing video writer " + std::to_string(wrap_index), true);
            m_video->release();
            //delete m_video;
        }
    };

    void
    set_resolution(int w = -1, int h = -1)
    {

        m_cap.set(cv::CAP_PROP_FPS, 30);
        m_cap.set(cv::CAP_PROP_FOURCC,
                  cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        if(w > 0)
            m_cap.set(cv::CAP_PROP_FRAME_WIDTH, w);
        if(h > 0)
            m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, h);
        w = m_cap.get(cv::CAP_PROP_FRAME_WIDTH);
        h = m_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        int fps = m_cap.get(cv::CAP_PROP_FPS);
        logln("Resolution " + std::to_string(m_imageSize.width) + "*" +
                  std::to_string(m_imageSize.height) + " " +
                  std::to_string(m_fps) + "fps",
              true);
    };

    void
    set_calibration(std::string path = "../docs/out_camera_data.xml")
    {
        m_fs.open(path, cv::FileStorage::READ);
        m_cameraMatrix = m_fs["camera_matrix"].mat();
        m_distCoeffs = m_fs["distortion_coefficients"].mat();

        cv::initUndistortRectifyMap(
            m_cameraMatrix, m_distCoeffs, cv::Mat(),
            cv::getOptimalNewCameraMatrix(m_cameraMatrix, m_distCoeffs,
                                          m_imageSize, 1, m_imageSize, 0),
            m_imageSize, CV_16SC2, m_map1, m_map2);
        m_calibrated = true;
        cv::remap(m_frame, m_frame, m_map1, m_map2, cv::INTER_LINEAR);

        logln("Camera " + std::to_string(cap_index) + " calibrated", true);
        m_fs.release();
    }

    void
    set_wrap(std::string path)
    {
        m_wrap_path = path;
        cv::Mat frame;
        m_cap.read(frame);

        m_dstTri[0] = cv::Point2f(0.f, 0.f);
        m_dstTri[1] = cv::Point2f(frame.cols - 1.f, 0.f);
        m_dstTri[2] = cv::Point2f(frame.cols - 1.f, frame.rows - 1.f);
        m_dstTri[3] = cv::Point2f(0.f, frame.rows - 1.f);

        m_fs.open(path, cv::FileStorage::READ);
        if(m_fs.isOpened())
        {
            logln("Wrap file found at " + path, true);
            int i = 0;
            cv::FileNode fn = m_fs["srcTri"];
            for(cv::FileNodeIterator v = fn.begin(); v != fn.end(); v++)
            {
                double x, y;
                cv::FileNodeIterator it = (*v).begin();
                x = (*it);
                it++;
                y = (*it);
                srcTri[i] = cv::Point2f(x, y);
                i++;
            }
            m_fs.release();
            save_wrap();
        }
        else
        {
            srcTri[0] = cv::Point2f(0.f, 0.f);
            srcTri[1] = cv::Point2f(frame.cols - 1.f, 0.f);
            srcTri[2] = cv::Point2f(frame.cols - 1.f, frame.rows - 1.f);
            srcTri[3] = cv::Point2f(0.f, frame.rows - 1.f);

            cv::namedWindow("Calib" + std::to_string(cap_index));
            cv::imshow("Calib" + std::to_string(cap_index), frame);
            logln("Waiting for user selection...", true);
            m_user_calib = true;

            cv::setMouseCallback(
                "Calib" + std::to_string(cap_index),
                [](int event, int x, int y, int flag, void *param)
                {
                    if(event == cv::EVENT_LBUTTONDOWN)
                    {
                        Camera *cam = ((Camera *)param);
                        int ind = cam->wrap_index;

                        cam->logln("Calibration pt" + std::to_string(ind) +
                                       ": [" + std::to_string(x) + " ; " +
                                       std::to_string(y) + " ]",
                                   true);
                        cam->srcTri[ind] = cv::Point2f(x, y);
                        cam->wrap_index = (ind + 1) % 4;
                        for(int i = 0; i < 4; i++)
                            cv::circle(cam->getFrame(), cam->srcTri[i], 5,
                                       cv::Scalar((63 * i) % 255,
                                                  (63 * i + 85) % 255,
                                                  (63 * i + 170) % 255),
                                       -1);
                        cv::imshow("Calib" + std::to_string(cam->cap_index),
                                   cam->getFrame());
                        if(ind == 3)
                        {
                            cam->save_wrap();
                            //close window
                            cv::destroyWindow("Calib" +
                                              std::to_string(cam->cap_index));
                        }
                    }
                },
                this);

            while(m_user_calib)
            {
                cv::imshow("Calib" + std::to_string(cap_index), getFrame());
                cv::waitKey(30);
            }
        }
    }

    void
    save_wrap(std::string path = "")
    {
        if(path == "")
            path = m_wrap_path;
        m_warpped_frame =
            cv::Mat::zeros(getFrame().rows, getFrame().cols, getFrame().type());
        m_warp_mat = cv::getPerspectiveTransform(srcTri, m_dstTri);

        m_fs.open(path, cv::FileStorage::WRITE);
        m_fs << "srcTri"
             << "[" << srcTri[0] << srcTri[1] << srcTri[2] << srcTri[3];
        m_fs.release();
        logln("Wrap saved to " + path, true);
        m_user_calib = false;
    };

    cv::Mat &
    getFrame(bool transformed = false)
    {
        m_cap.read(m_frame);
        if(m_frame.empty())
            throw std::string("Frame " + std::to_string(cap_index) +
                              " not found");
        if(m_calibrated)
            cv::remap(m_frame, m_frame, m_map1, m_map2, cv::INTER_LINEAR);

        if(transformed)
        {
            cv::warpPerspective(m_frame, m_warpped_frame, m_warp_mat,
                                m_imageSize);
            return m_warpped_frame;
        }
        else
        {
            return m_frame;
        }
    }

    void
    set_video_writer(std::string name, int fps = 30)
    {
        m_video = new cv::VideoWriter();
        bool o = m_video->open(name + ".avi",
                               cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                               FP_SUBNORMAL, m_imageSize);
        m_fps = fps;
        logln("Video writer set to " + name + ".avi", true);
    }

    void
    record_frame(bool transformed = false)
    {
        logln("Recording frame " + std::to_string(wrap_index), true);
        m_video->write(this->getFrame(transformed));
    }

    int wrap_index = 0;
    cv::Point2f srcTri[4];

    int cap_index;

    protected:
    cv::VideoCapture m_cap;
    cv::Mat m_frame;

    std::string m_calib_path;
    std::string m_wrap_path;
    bool m_user_calib = false;

    cv::FileStorage m_fs;
    cv::Mat m_cameraMatrix;
    cv::Mat m_distCoeffs;
    cv::Size m_imageSize;
    int m_fps;
    cv::Mat m_view, m_rview, m_map1, m_map2;
    bool m_calibrated = false;

    cv::Mat m_warp_mat;
    cv::Mat m_warpped_frame;

    cv::Point2f m_dstTri[4];

    cv::VideoWriter *m_video = nullptr;
};

#endif // __CAMERA_HPP__
