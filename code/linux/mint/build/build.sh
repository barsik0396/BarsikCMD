#!/bin/bash
# Usage: ./build.sh 0.1.0

VERSION=$1

if [ -z "$VERSION" ]; then
  echo "Error: version not specified"
  echo "Usage: $0 <version>"
  exit 1
fi

PKG_DIR="barsik-cmd_${VERSION}_amd64"

mkdir -p "$PKG_DIR/DEBIAN"
mkdir -p "$PKG_DIR/usr/local/bin"

if [ ! -f "$PKG_DIR/DEBIAN/control" ]; then
  cat > "$PKG_DIR/DEBIAN/control" << CONTROL
Package: barsik-cmd
Version: $VERSION
Architecture: amd64
Maintainer: Your Name <your@email.com>
Description: BarsikCMD - CLI utility
CONTROL
else
  sed -i "s/^Version: .*/Version: $VERSION/" "$PKG_DIR/DEBIAN/control"
fi

# Check binary exists
BINARY="$PKG_DIR/usr/local/bin/barsik-cmd"
if [ ! -f "$BINARY" ]; then
  echo "Error: binary not found at $BINARY"
  exit 1
fi

chmod 755 "$BINARY"

dpkg-deb --build "$PKG_DIR"
echo "Done: ${PKG_DIR}.deb"