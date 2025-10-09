FROM debian:bookworm AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git libboost-all-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j && \
    strip build/apps/chat_server/chat_server || true

FROM debian:bookworm-slim
RUN useradd -m runner
USER runner
WORKDIR /app

COPY --from=build /_old/src/build/apps/chat_server/chat_server /usr/local/bin/chat_server

EXPOSE 5555

ENV PORT=5555
ENTRYPOINT ["/usr/local/bin/chat_server"]
CMD []
