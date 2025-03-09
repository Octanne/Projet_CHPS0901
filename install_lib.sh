#!/bin/bash

LIB_DIR="$(pwd)/libs"
SRC_DIR="$LIB_DIR/sources"
INSTALL_DIR="$LIB_DIR/install"

mkdir -p "$SRC_DIR"
mkdir -p "$INSTALL_DIR"

read -p "Do you only want to setup LD_LIBRARY_PATH just to run the compiled app ? (y/N)" choice
[[ "$choice" == "y" ]] && echo "Type : export LD_LIBRARY_PATH=\"$INSTALL_DIR/lib64:\$LD_LIBRARY_PATH\"" && exit 1

read -p "You need to load ninja, cmake and meson first have you done it ? (y/N)" choice
[[ "$choice" != "y" ]] && exit 1 

# Function to check if a library is installed
check_installed() {
    local lib_name=$1
    local lib_path="$INSTALL_DIR/lib/lib$lib_name.so"
    [[ -f "$lib_path" ]] && return 0 || return 1
}

# Function to ask user for installation
ask_install() {
    local lib_name=$1
    if check_installed "$lib_name"; then
        read -p "$lib_name is already installed. Do you want to reinstall it? (y/N): " choice
        [[ "$choice" != "y" ]] && return 1
    else
        read -p "Do you want to install $lib_name? (y/N): " choice
        [[ "$choice" == "y" ]] && return 0 || return 1
    fi
}

# Install GLU
if ask_install "GLU"; then
    echo "Installing GLU..."
    cd "$SRC_DIR"
    wget -q https://gitlab.freedesktop.org/mesa/glu/-/archive/glu-9.0.3/glu-glu-9.0.3.tar.gz -O glu.tar.gz
    tar -xzf glu.tar.gz
    cd glu-glu-9.0.3
    meson setup build --prefix="$INSTALL_DIR"
    ninja -C build
    ninja -C build install
    echo "GLU installed in $INSTALL_DIR"
fi

# Install GLEW
if ask_install "GLEW"; then
    echo "Installing GLEW..."
    cd "$SRC_DIR"
    wget -q https://downloads.sourceforge.net/project/glew/glew/2.2.0/glew-2.2.0.tgz
    tar -xzf glew-2.2.0.tgz
    cd glew-2.2.0
    make
    make GLEW_DEST="$INSTALL_DIR" install
    echo "GLEW installed in $INSTALL_DIR"
fi

# Install GLFW
if ask_install "glfw"; then
    echo "Installing GLFW..."
    cd "$SRC_DIR"
    wget -q https://codeload.github.com/glfw/glfw/tar.gz/refs/tags/3.4 -O glfw-3.4.tar.gz
    tar -xzf glfw-3.4.tar.gz
    cd glfw-3.4
    cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DBUILD_SHARED_LIBS="ON" .
    make
    make install
    echo "GLFW installed in $INSTALL_DIR"
fi

if ask_install "GLM"; then
    echo "Installing GLM..."
    cd "$SRC_DIR"
    wget -q https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz -O glm.tar.gz
    tar -xzf glm.tar.gz
    cd glm-1.0.1
    mkdir -p "$INSTALL_DIR/include"
    cp -r glm "$INSTALL_DIR/include/"
    echo "GLM installed in $INSTALL_DIR/include"
fi

echo "Installation process completed! To use these libraries, add the following to your compilation flags:"
echo "  -I$INSTALL_DIR/include -L$INSTALL_DIR/lib64 -lGLEW -lGL -lglfw"
echo "export LD_LIBRARY_PATH=\"$INSTALL_DIR/lib64:\$LD_LIBRARY_PATH\""

