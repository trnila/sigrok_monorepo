#!/usr/bin/env bash
set -ex

for repo in libserialport libsigrok libsigrokdecode pulseview sigrok-cli sigrok-firmware; do
	git subtree pull --prefix="$repo" https://github.com/sigrokproject/"$repo" master
done
