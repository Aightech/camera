# camera

This module defines a C++ class Camera that provides an interface to capture video frames from a camera. The class constructor takes an optional camera index, width and height of the captured frames. The captured frames are read from the camera using OpenCV's VideoCapture class. The captured frames can be processed using OpenCV's image processing functions. The class also provides a method to set the camera resolution. Additionally, the class implements a method to calibrate the camera using mouse clicks on the image to define a perspective transformation. Finally, the module provides a simple example program that uses the Camera class to capture video from a camera and display the captured frames.

# Requirements
- CMake
- OpenCV
- 
### Submodules dependencies
#### libraries
- [strANSIseq](lib/strANSIseq/README.md)
#### Tools
- [built_lsl](tool_lib/built_lsl/README.md)

# Building source code

To build the project run:
```bash
cd camera
mkdir build && cd build
cmake .. && make
```

# Demonstration app

When the project have been built, you can run:
```bash
./camera -h
```
to get the demonstration app usage.

# Example
Open the ![main.cpp](cpp:src/main.cpp) file to get an example how to use the lib.


# Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.