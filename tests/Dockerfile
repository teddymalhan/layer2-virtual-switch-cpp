FROM alpine:latest

# Install dependencies
RUN apk add --no-cache \
    build-base \
    cmake \
    git \
    linux-headers \
    iproute2 \
    tcpdump \
    bash

# Set working directory
WORKDIR /app

# Copy source (exclude build directory)
COPY CMakeLists.txt Makefile .clang-format .cursorrules ./
COPY cmake/ ./cmake/
COPY include/ ./include/
COPY src/ ./src/
COPY test/ ./test/
COPY reference/ ./reference/

# Build
RUN mkdir -p build && \
    cd build && \
    rm -rf CMakeCache.txt CMakeFiles/ && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DProject_ENABLE_UNIT_TESTING=OFF && \
    cmake --build . && \
    ls -lh vport vswitch && \
    echo "Build complete!"

ENTRYPOINT ["/bin/sh"]
