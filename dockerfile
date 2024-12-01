FROM debian:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    libc++-dev \
    libc++abi-dev \
    libssl-dev \
    && apt-get clean

COPY . /app
WORKDIR /app

RUN make

EXPOSE 6667

CMD ["./weusearch"]
