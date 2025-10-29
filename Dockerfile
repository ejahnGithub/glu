# Super minimal and fast build
FROM alpine:3.18

# Install minimal dependencies
RUN apk add --no-cache \
    g++ \
    cmake \
    make \
    llvm18-dev \
    clang18 \
    musl-dev \
    && ln -sf /usr/bin/clang-18 /usr/bin/clang++ \
    && ln -sf /usr/bin/clang-18 /usr/bin/clang

WORKDIR /app
COPY . .

# Minimal build - just the compiler binary
RUN cmake -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DFROM_SOURCE=0 \
    && cmake --build build --target gluc -j$(nproc) \
    && strip build/tools/gluc/gluc

# Final tiny image
FROM alpine:3.18
RUN apk add --no-cache libstdc++ musl
COPY --from=0 /app/build/tools/gluc/gluc /usr/local/bin/
CMD ["gluc", "--help"]
