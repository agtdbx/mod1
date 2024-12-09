#!/bin/sh


MESON_CONFIG_DIR=build
MESON_BUILD_DIR=release
EXECUTABLE_NAME=mod1
DEFAULT_MAP=../map/testmap.mod1

if [ ! -d build ]
then
	meson $MESON_CONFIG_DIR --prefix=$PWD/$MESON_BUILD_DIR --bindir="" --libdir=""
fi

ninja -C $MESON_CONFIG_DIR
ninja install -C $MESON_CONFIG_DIR >/dev/null
