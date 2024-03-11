#!/bin/bash

# Find the absolute path of the script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check if version and Qt path parameters are provided
if [ "$#" -ne 2 ]; then
    echo "Version number and path to Qt installation are required."
    echo "Usage: ./build.sh [version] [path-to-qt]"
    echo ""
    echo "Example: ./build.sh 1.0.4 /Users/kalle/Qt/6.53/macos"
    echo ""
    exit 1
fi

BUILD_VERSION="$1"
QT_DIR="$2"

BUILD_DIR="$SCRIPT_DIR/build"
RELEASE_DIR="$SCRIPT_DIR/releases"
PUBLISH_DIR="$SCRIPT_DIR/publish"

echo ""
echo "Compiling Velopack Qt sample..."

# Remove build directory if it exists
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
fi

# Create build directory to run cmake in
mkdir "$BUILD_DIR"

# Navigate to the build directory
cd "$BUILD_DIR" || exit

cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PUBLISH_DIR -DCMAKE_PREFIX_PATH=$QT_DIR ..

if [ $? -ne 0 ] ; then
  echo "I couldn't run cmake. Exiting." 
  cd $SCRIPT_DIR
  exit 1
fi

ninja
ninja install

echo ""
echo "Building Velopack Qt Sample Release v$BUILD_VERSION"
vpk pack --packId appVelopackQtSample \
    --mainExe appVelopackQtSample \
    --packTitle VelopackQtSample \
    -v $BUILD_VERSION \
    -o "$RELEASE_DIR" \
    -p "$PUBLISH_DIR/appVelopackQtSample.app" \
    -i "$SCRIPT_DIR/artwork/DefaultApp.icns"