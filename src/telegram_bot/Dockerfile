FROM ubuntu:18.04

RUN apt-get -qq update && \
    apt-get -qq install -y g++ make binutils cmake libssl-dev libboost-system-dev libcurl4-openssl-dev zlib1g-dev git

WORKDIR /home
RUN git clone https://github.com/reo7sp/tgbot-cpp
WORKDIR /home/tgbot-cpp


RUN cmake .
RUN make -j4
RUN make install