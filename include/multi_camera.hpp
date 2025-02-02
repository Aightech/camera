#ifndef __MULTI_CAMERA_HPP__
#define __MULTI_CAMERA_HPP__

#include "strANSIseq.hpp"
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "camera.hpp"

class MultiCam : public virtual ESC::CLI
{
    public:
    MultiCam(int verbose = -1) : CLI(verbose, "multi_cam"){};
    ~MultiCam()
    {
        for(auto c : m_cameras) delete c;
    };

    void
    addCamera(int index = 0, int w = -1, int h = -1)
    {
        logln("Adding camera " + std::to_string(index), true);
        m_cameras.push_back(new Camera(index, m_verbose-1)); //, w, h));
        if(w > 0 && h > 0)
        {
            logln("Setting resolution to " + std::to_string(w) + "x" + std::to_string(h), true);
            m_cameras.back()->set_resolution(w, h);
        }
    }

    cv::Mat
    getFrame(unsigned index = 0, bool warp = false)
    {
        if(index >= m_cameras.size())
            return cv::Mat();
        return m_cameras[index]->getFrame(warp);
    }

    void
    set_video_writer(std::string name, int fps = 10)
    {
        for(int i = 0; i < m_cameras.size(); i++)
            m_cameras[i]->set_video_writer(name + std::to_string(i), fps);
    }

    void
    record_frame(unsigned index = 0)
    {
        if(index >= m_cameras.size())
            return;
        m_cameras[index]->record_frame();
    }

    Camera* get_cam(int index)
    {
        if(index >= m_cameras.size())
            throw std::string("Camera index out of range");
        return m_cameras[index];
    }

    public:
    std::vector<Camera *> m_cameras;
};

#endif //__MULTI_CAMERA_HPP__
