FROM ubuntu:18.04
LABEL maintainer='Bartosz Piekny (bartosz.piekny@gmail.com)'

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends\
        git \
        cmake \
        curl \
        build-essential \
        libboost-all-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        openssh-client \
        ca-certificates

RUN git clone https://github.com/zeromq/libzmq.git && \
    cd libzmq && git checkout v4.3.0 && \
    mkdir build && cd build && \
    cmake .. && make -j install

RUN git clone https://github.com/zeromq/cppzmq.git && \
    cd cppzmq && git checkout v4.7.1 && \
    mkdir build && cd build && \
    cmake -DCPPZMQ_BUILD_TESTS=OFF .. && make -j install

COPY . /app/src/qos_server/
WORKDIR /app/src/qos_server/

RUN mkdir build && cd build  \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && cmake --build . -- -j4; \
    fi

CMD /app/src/qos_server/build/qos_server
