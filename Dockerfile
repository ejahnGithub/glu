# Simple build - just works
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Just install the absolute essentials
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy just what we need for the simple test
COPY tools/simple-test ./simple-test/

# Build the simple test tool
RUN cd simple-test && \
    cmake . && \
    make simple-test

# Run the simple test
CMD ["./simple-test/simple-test"]
