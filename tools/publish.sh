#!/bin/bash
set -e
#set -x

# Detect OS and architecture
OS="$(uname | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"

# Map uname output to common arch names
if [ "$ARCH" = "x86_64" ]; then
    ARCH="amd64"
elif [ "$ARCH" = "aarch64" ]; then
    ARCH="arm64"
fi

WORKSPACE=$(pwd)
BUILD_TYPE=Release

# Build
rm -rf $WORKSPACE/build
cmake -B $WORKSPACE/build -DCMAKE_BUILD_TYPE=Release
cmake --build $WORKSPACE/build --parallel --target lisp --config $BUILD_TYPE

# Create bundle
TMPDIR=$(mktemp -d)
RELEASE="lisp-${OS}-${ARCH}.tar.gz"

cp -v build/src/lisp $TMPDIR
cp -v -r stdlib $TMPDIR

cd $TMPDIR
tar -czvf $RELEASE lisp stdlib

# Upload to release server
SERVER=root@www.jakobmaier.at
RELEASE_LOCATION=/var/www/files/lisp/release/latest/
scp $RELEASE $SERVER:$RELEASE_LOCATION

# Cleanup
rm -rf $TMPDIR

echo "Success"
