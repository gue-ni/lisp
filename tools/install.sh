#!/bin/bash
set -e

# Detect OS and architecture
OS="$(uname | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"

# Map uname output to common arch names
if [ "$ARCH" = "x86_64" ]; then
    ARCH="amd64"
elif [ "$ARCH" = "aarch64" ]; then
    ARCH="arm64"
fi

RELEASE="redstart-${OS}-${ARCH}.tar.gz"

BINARY_URL="https://www.jakobmaier.at/files/redstart/release/latest/${RELEASE}"

# Temp directory
TMPDIR="$(mktemp -d)"
echo "Downloading interpreter to ${TMPDIR}..."

curl -L "${BINARY_URL}" -o "${TMPDIR}/redstart.tar.gz"

# Extract
tar -xzf "${TMPDIR}/redstart.tar.gz" -C "${TMPDIR}"

LIBDIR=~/.local/share/redstart/lib/
BINDIR=~/.local/bin/

# Install startup script
if [ ! -e ~/.profile.lsp ]; then
  cp $TMPDIR/stdlib/profile.lsp ~/.profile.lsp
fi

# Install redstart libraries (required sudo)
mkdir -p $LIBDIR
cp $TMPDIR/stdlib/* $LIBDIR

# Install binary (requires sudo)
mkdir -p $BINDIR
cp $TMPDIR/redstart $BINDIR
chmod +x $BINDIR/redstart

# Cleanup
rm -rf "$TMPDIR"

# Check for readline
if ! ldconfig -p | grep -q readline; then
  echo "Warning: readline library not found."
  echo "On Debian/Ubuntu: sudo apt install libreadline-dev"
  echo "On Fedora: sudo dnf install readline-devel"
fi

if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
  echo "Warning: $HOME/.local/bin is not in your PATH" >&2
  echo "Add this line to your shell startup file (e.g. ~/.bashrc):"
  echo "export PATH=\"\$HOME/.local/bin:\$PATH\""
fi

echo "Successfully installed redstart to ${BINDIR}"
