#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Starting Toolchain Installation..."

#Detect the Operating System
OS="$(uname -s)"
case "${OS}" in
    Linux*)                     HOST_OS="Linux";;
    Darwin*)                    HOST_OS="Mac";;
    CYGWIN*|MINGW*|MSYS*|UCRT*) HOST_OS="Windows";;
    *)                          HOST_OS="UNKNOWN:${OS}"
esac

echo "Detected Operating System: $HOST_OS"

install_mac() {
    echo "Checking for Homebrew..."
    if ! command -v brew &> /dev/null; then
        echo "Homebrew is required but not installed. Please install it from https://brew.sh/"
        exit 1
    fi

    echo "Installing toolchain via Homebrew..."
    brew update

    brew install arm-none-eabi-gcc openocd cmake make
}

install_linux() {
    echo "Detecting Linux package manager..."
    if command -v apt-get &> /dev/null; then
        echo "Install toolchain via APT..."
        sudo apt-get update
        audo apt-get install -y gcc-arm-none-eabi gdb-multiarch binutils-arm-none-eabi openoce cmake make build-essential
    fi
}

install_windows() {
    echo "Checking for MSYS2/Pacman..."
    if ! command -v pacman &> /dev/null; then
        echo "Pacman not found. This script requires MSYS2 UCRT64 on Windows."
        exit 1
    fi

    echo "Installing toolchain via MSYS2 UCRT64..."

    pacman -S --noconfirm \
        mingw-w64-ucrt-x86_64-arm-none-eabi-gcc \
        mingw-w64-ucrt-x86_64-arm-none-eabi-gdb \
        mingw-w64-ucrt-x86_64-arm-none-eabi-binutils \
        mingw-w64-ucrt-x86_64-arm-none-eabi-newlib \
        mingw-w64-ucrt-x86_64-openocd \
        mingw-w64-ucrt-x86_64-cmake \
        mingw-w64-ucrt-x86_64-make
}

# Execute corresponding function based on OS
case "$HOST_OS" in
    "Mac")
        install_mac
        ;;
    "Linux")
        install_linux
        ;;
    "Windows")
        install_windows
        ;;
    *)
        echo "Unsupported OS: $HOST_OS"
        exit 1
        ;;
esac

echo "========================================"
echo "Installation Complete!"
echo "Verify your installation by running:"
echo "  arm-none-eabi-gcc --version"
echo "  openocd --version"
echo "========================================"