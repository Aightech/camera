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
    ~MultiCam()
  {
    for(auto c : m_cameras)
      delete c;
    
  };

    void
    addCamera(int index = 0, int w = -1, int h = -1)
    {
        m_cameras.push_back(new Camera(index, w, h));
    }

    cv::Mat
    getFrame(unsigned index = 0, bool warp = false)
    {
        if(index >= m_cameras.size())
            return cv::Mat();
        return m_cameras[index]->getFrame(warp);
    }

    void
    set_video_writer(std::string name, int fps=10)
    {
        for(int i = 0; i < m_cameras.size(); i++)
            m_cameras[i]->set_video_writer(name + std::to_string(i),
                                           fps);
    }

    void record_frame(unsigned index = 0)
    {
        if(index >= m_cameras.size())
            return;
        m_cameras[index]->record_frame();
    }

    private:
    std::vector<Camera *> m_cameras;
};

#endif //__MULTI_CAMERA_HPP__
