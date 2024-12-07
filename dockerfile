FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libevent-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN make

CMD ["./launsh.sh"]
