FROM debian:unstable AS download-tarballs

RUN apt-get update && \
    apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
    build-essential \
    libreadline-dev \
    avr-libc \
    gcc-avr \
    gcc-arm-none-eabi \
    gcc-riscv64-unknown-elf \
    picolibc-arm-none-eabi \
    picolibc-riscv64-unknown-elf \
    python3 \
    lola \
    gawk \
    asciidoctor \
    asciidoctor-pdf \
    ruby-coderay \
    gcc-arm-linux-gnueabi \
    libc6-dev-armel-cross \
    qemu-system-misc \
    qemu-system-arm \
    black \
    python3-pip \
    python3-serial \
    nsis \
    rsync \
    genisoimage \
    zip \
    gcc-mingw-w64 \
    librsvg2-bin \
    icoutils \
    icnsutils \
    black && \
    pip3 install --break-system-packages pynsist
