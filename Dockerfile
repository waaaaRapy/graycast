FROM ubuntu:latest

ENV MIRROR=http://ftp.jaist.ac.jp/pub/Linux/ubuntu/
RUN sed -i -re "s|http://.+/|${MIRROR}|" /etc/apt/sources.list

ENV TZ=Asia/Tokyo
RUN echo $TZ > /etc/timezone \
  && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime

RUN apt-get update \
  && apt-get install -y \
      gcc make git binutils libc6-dev \
      wabt gdb \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*
