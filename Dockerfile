FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-sqlite \
    libqt6sql6-sqlite \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN qmake6 server.pro && make clean && make

EXPOSE 33333

CMD ["./tcp_server"]