FROM debian:unstable AS download-tarballs

RUN apt-get update && \
    apt-get install -y wget gnupg && \
    wget -O - https://keithp.com/archive/archive-key | apt-key add - && \
    echo "deb http://keithp.com/archive unstable/" > /etc/apt/sources.list.d/keithp.list && \
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
    ruby-asciidoctor-pdf \
    ruby-coderay \
    gcc-arm-linux-gnueabi \
    libc6-dev-armel-cross \
    qemu-system-arm=1:5.0-5.1 \
    qemu-system-misc=1:5.0-5.1 \
    black

RUN mkdir snek
COPY ./ snek/

WORKDIR ./snek
