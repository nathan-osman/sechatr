## sechatr

I had originally written an application in Go that allowed users to install a UserScript in their browser and see when other users were typing messages and how far they had read.

Unfortunately, for various technical reasons, the program started running into some rather serious technical issues. Rather than trying to fight my way through (what appears to be) a very difficult and unforgiving websocket package, I decided to rewrite the entire thing in C++ with Qt. This is that project.

Install it here: https://sechat.quickmediasolutions.com

### Building

The application requires the following:

- C++ compiler with C++11 support
- CMake 3.2+
- Qt 5.4+
- [QHttpEngine](https://github.com/nitroshare/qhttpengine) *

Note that QHttpEngine 1.0.0 has not been released yet. You will need to build it from master.

Because the application uses CMake, building sechatr is quite straightforward:

    mkdir build
    cd build
    cmake ..
    make

### Running in Docker

It is probably simpler to use Docker to build and run the application. Assuming Docker is installed, you can simple run:

docker build -t sechatr .
