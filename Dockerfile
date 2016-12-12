FROM nathanosman/qt
MAINTAINER Nathan Osman <nathan@quickmediasolutions.com>

# Install Qt's websocket library & QHttpEngine (from PPA)
RUN \
    echo "deb http://ppa.launchpad.net/george-edison55/nitroshare-dev/ubuntu xenial main" >> /etc/apt/sources.list && \
    apt-key adv --keyserver keyserver.ubuntu.com --recv-keys B118CD3C377DF930EDD06C67084ECFC5828AB726 && \
    apt-get update && \
    apt-get install -y libqt5websockets5-dev libqhttpengine-dev && \
    rm -rf /var/lib/apt/lists/*

# Copy the source code to the container
COPY CMakeLists.txt /root/
COPY src/ /root/

# Configure and build the project
RUN \
    cd /root && \
    cmake . && \
    make && \
    make install

# Set the command for running the application
ENTRYPOINT sechatr

# Expose the two ports that the application uses
EXPOSE 8000
EXPOSE 8001
