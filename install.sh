#!/usr/bin/env bash
set -ex

ROOT="$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)"
PREFIX="$ROOT/install"

install_with_launcher() {
    binary="$1"
    cat > ~/.local/bin/"$binary" <<EOF
#!/usr/bin/env bash
export LD_LIBRARY_PATH=$PREFIX/lib:/usr/local/Qt-6.7.2/lib/:\$LD_LIBRARY_PATH
export PATH=$PREFIX/bin:\$PATH
export SIGROK_FIRMWARE_DIR=$PREFIX/share/sigrok-firmware/
./install/bin/"$binary"
EOF
    chmod a+x ~/.local/bin/"$binary"
}

install_with_launcher pulseview
install_with_launcher sigrok-cli