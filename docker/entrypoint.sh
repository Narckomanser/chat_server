#!/usr/bin/env bash

set -eo pipefail

mkdir -p /app/build
cd /app/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

exec /app/build/chat_server --port "${PORT:-5555}"
