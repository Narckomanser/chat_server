FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update  \
    && apt-get install -y \
      build-essential  \
      cmake  \
      pkg-config \
      libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && cmake .. && cmake --build .

FROM ubuntu:24.04 AS runtime
RUN apt-get update &&  \
    apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /app/build/chat_server /app/chat_server

COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

EXPOSE 5555
CMD ["/app/chat_server", "--port", "5555"]


