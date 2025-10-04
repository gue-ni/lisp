#!/bin/bash
set -e
set -x

# Detect OS and architecture
OS="$(uname | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"

# Map uname output to common arch names
if [ "$ARCH" = "x86_64" ]; then
    ARCH="amd64"
elif [ "$ARCH" = "aarch64" ]; then
    ARCH="arm64"
fi

echo "Install lisp for ${OS} ${ARCH}"

RELEASE="lisp-${OS}-${ARCH}.tar.gz"

BINARY_URL="https://www.jakobmaier.at/files/lisp/release/latest/${RELEASE}"

# Temp directory
TMPDIR="$(mktemp -d)"
echo "Downloading lisp interpreter to ${TMPDIR}..."

curl -L "${BINARY_URL}" -o "${TMPDIR}/lisp.tar.gz"

# Extract
tar -xzf "${TMPDIR}/lisp.tar.gz" -C "${TMPDIR}"

BINDIR=/usr/local/bin/

# Install binary (requires sudo)
echo "Installing Lisp interpreter to ${BINDIR} …"
sudo cp -v $TMPDIR/lisp $BINDIR
sudo chmod +x $BINDIR/lisp

# Cleanup
rm -rf "$TMPDIR"

# Check for readline
if ! ldconfig -p | grep -q readline; then
  echo "Warning: readline library not found."
  echo "On Debian/Ubuntu: sudo apt install libreadline-dev"
  echo "On Fedora: sudo dnf install readline-devel"
else
  echo "Found readline."
fi

