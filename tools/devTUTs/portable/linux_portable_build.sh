#!/bin/sh
# !!!SET cedar.conf CEDAR_PORTABLE AND CEDAR_MAC_BUNDLE_PATHS
# Define paths for easy editing
PYTHONQT_LIB_PATH="/usr/local/lib/pythonqt/lib"
PYTHON_LIB_PATH="/usr/lib/x86_64-linux-gnu"
QT_PATH="/usr/lib/x86_64-linux-gnu/qt5"
#ur corresponding python_version
PYTHON_VERSION=3.6.9
PYTHON_TARBALL_URL="https://www.python.org/ftp/python/$PYTHON_VERSION/Python-$PYTHON_VERSION.tgz"
PYTHON_INSTALL_DIR="$(pwd)/release/cedar/python"

# Check if --no-python is passed as argument
NO_PYTHON=0
if [ "$1" = "--no-python" ]; then
  NO_PYTHON=1
fi

# Determine the number of CPU cores, subtract one
NUM_CORES=$(nproc --all)
NUM_CORES=$((NUM_CORES - 1))

cd compile/cedar/
git status
git pull
cd build
make cedar -j $NUM_CORES
cd ../../..
rm -rf release
mkdir release
cp -r compile/cedar release/cedar
# Removing unnecessary files and folders
rm -f release/AppRun
rm -f release/cedar.app
rm -rf release/cedar/bin/examples/
rm -rf release/cedar/bin/*Test
rm -f release/cedar/bin/cedar
rm -f release/cedar/*.*
rm -rf release/cedar/cedar/
rm -rf release/cedar/cmake/
rm -rf release/cedar/build/
rm -rf release/cedar/doc/
rm -rf release/cedar/examples/
rm -rf release/cedar/executables/
rm -rf release/cedar/templates/
rm -rf release/cedar/tools/
mv release/cedar/bin/cedar-6.2.0 release/cedar/bin/cedar
mkdir release/cedar/Cedar_Plugins/
mv release/cedar/resources/ release/cedar/Cedar_Resources/
mv release/cedar/tests/ release/cedar/Cedar_Tests/

cp -P $PYTHONQT_LIB_PATH/*.so* release/cedar/lib/
cp -P $PYTHON_LIB_PATH/libpython3.6*.so* release/cedar/lib/
cp -r $QT_PATH/plugins release/cedar/plugins
cp -P compile/libqglviewer/libQGLViewer-2.6.3/QGLViewer/libQGL*.so* release/cedar/lib/

# Only execute Python-related steps if NO_PYTHON is not set
if [ $NO_PYTHON -eq 0 ]; then
  # Download, configure, and install Python
  wget $PYTHON_TARBALL_URL
  tar xzf Python-$PYTHON_VERSION.tgz
  cd Python-$PYTHON_VERSION
  ./configure --prefix=$PYTHON_INSTALL_DIR
  make install -j $NUM_CORES
  cd ..
  rm -r Python-$PYTHON_VERSION*
  echo -e '#!/bin/bash\n\nCEDAR_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"\n\nexport PYTHONHOME="${CEDAR_DIR}/cedar/python"\n\n"${CEDAR_DIR}/cedar.app"' > release/start_cedar.sh && chmod +x release/start_cedar.sh
fi

cd release
wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage cedar/bin/cedar -always-overwrite -bundle-non-qt-libs -no-copy-copyright-files -no-translations -unsupported-allow-new-glibc -qmake=$QT_PATH/bin/qmake
mv AppRun cedar.app
rm -f linuxdeployqt-continuous-x86_64.AppImage
cd ..
exit 0
