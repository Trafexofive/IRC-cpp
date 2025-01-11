FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libevent-dev \
    && rm -rf /var/lib/apt/lists/*


EXPOSE 22200

WORKDIR /app

COPY . .

RUN make run

# CMD ["./test/main.sh"]
