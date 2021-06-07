FROM ubuntu
RUN apt update && \
    apt install -y gcc make git binutils libc6-dev