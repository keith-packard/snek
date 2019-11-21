# snek toolchain
#
# VERSION               0.1

FROM debian:unstable

RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
 && rm -rf /var/lib/apt/lists/*

RUN wget -O - https://keithp.com/archive/archive-key | \
    apt-key add - && echo 'deb   http://keithp.com/archive unstable/' > /etc/apt/sources.list.d/keithp.list

RUN apt-get update && apt-get install -y \
    libreadline-dev \
    gawk \
    lola \
    gcc-avr \
    avr-libc \
    python3-serial \
    gcc-arm-none-eabi \
    gcc-riscv64-unknown-elf \
    qemu-system-riscv32 \
    qemu-system-arm \
    picolibc-riscv64-unknown-elf \
    picolibc-arm-none-eabi \
    rsync \
    librsvg2-bin \
    asciidoctor \
    ruby-asciidoctor-pdf \
    coderay \
    gcc-mingw-w64-i686 \
    icoutils \
    nsis \
    icnsutils \
    genisoimage \
    python3-pip \
 && python3 -m pip install pynsist \
 && rm -rf /var/lib/apt/lists/*
