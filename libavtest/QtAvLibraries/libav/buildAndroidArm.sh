#!/bin/bash
ARC=arm
NDK=/usr/local/android-ndk
PLATFORM=$NDK/platforms/android-18/arch-$ARC
PREBUILT=$NDK/toolchains/$ARC-linux-androideabi-4.8/prebuilt/darwin-x86_64

. build

buildAndroid