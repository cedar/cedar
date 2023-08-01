#!/bin/sh

# Define paths for easy editing
PYTHONQT_LIB_PATH="/usr/local/lib/pythonqt/lib"
PYTHON_LIB_PATH="/usr/lib/x86_64-linux-gnu"
QMAKE_PATH="/usr/lib/x86_64-linux-gnu/qt5/bin/qmake"
#ur corresponding python_version
PYTHON_TARBALL_URL="https://www.python.org/ftp/python/3.6.9/Python-3.6.9.tgz"
PYTHON_INSTALL_DIR="$(pwd)/release/cedar/python"


cd compile/cedar/
git status
git pull
cd build
make cedar -j 10
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
cp -r $PYTHON_LIB_PATH/qt5/plugins release/cedar/plugins
cp -P compile/libqglviewer/libQGLViewer-2.6.3/QGLViewer/libQGL*.so* release/cedar/lib/

# Download, configure, and install Python
wget $PYTHON_TARBALL_URL
tar xzf Python-3.6.9.tgz
cd Python-3.6.9
./configure --prefix=$PYTHON_INSTALL_DIR
make install
rm -rf Python-3.6.9
rm -f Python-3.6.9.tgz
cd ..

cd release
wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage cedar/bin/cedar -always-overwrite -bundle-non-qt-libs -no-copy-copyright-files -no-translations -unsupported-allow-new-glibc -qmake=$QMAKE_PATH
mv AppRun cedar.app
rm -f linuxdeployqt-continuous-x86_64.AppImage
echo -e '#!/bin/bash\n\nexport PYTHONHOME="./cedar/python"\n\n./cedar.app' > start_cedar.sh
cd ..
exit 0
