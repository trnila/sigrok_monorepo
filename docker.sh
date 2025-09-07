#!/usr/bin/env bash
set -ex

docker build -t xxx .
docker run -v $PWD/install/cache:/root/.cache -v $PWD:/work -w /work -it xxx
