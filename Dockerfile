FROM ubuntu:24.04
RUN apt update && apt install -y wget build-essential python3 git libftdi-dev
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.27.0/bazelisk-linux-amd64 -O /usr/local/bin/bazel
RUN chmod a+x /usr/local/bin/bazel
