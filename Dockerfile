# Super minimal and fast build
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install minimal dependencies - just what we need
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    cmake \
    make \
    llvm-16-dev \
    clang-16 \
    && rm -rf /var/lib/apt/lists/* \
    && ln -sf /usr/bin/clang-16 /usr/bin/clang++ \
    && ln -sf /usr/bin/clang-16 /usr/bin/clang

WORKDIR /app
COPY . .

# Minimal build - just the compiler binary
RUN cmake -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DFROM_SOURCE=0 \
    && cmake --build build --target gluc -j$(nproc) \
    && strip build/tools/gluc/gluc

# Final tiny image
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    libc6 \
    && rm -rf /var/lib/apt/lists/*
COPY --from=0 /app/build/tools/gluc/gluc /usr/local/bin/
CMD ["gluc", "--help"]
