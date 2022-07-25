#ifndef __MULTI_CAMERA_HPP__
#define __MULTI_CAMERA_HPP__

#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#include "camera.hpp"

class MultiCam
{
    public:
    MultiCam(){

          };
    ~MultiCam(){};

    void addCamera(int index = 0, int w = 640, int h = 360)
    {
        m_cameras.push_back(new Camera(index, w, h));
    }

    cv::Mat getFrame(unsigned index = 0, bool warp = false)
    {
        if(index >= m_cameras.size())
            return cv::Mat();
        return m_cameras[index]->getFrame(warp);
    }

    private:
    std::vector<Camera*> m_cameras;
};

#endif //__MULTI_CAMERA_HPP__