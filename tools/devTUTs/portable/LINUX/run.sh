#!/bin/sh

cd compile/cedar/
git status
git pull
cd build
make cedar -j 10
cd ../../..
rm -rf release
mkdir release
cp -r compile/cedar release/cedar
rm -f release/AppRun
rm -f release/cedar.app
rm -rf release/cedar/bin/examples/
rm -rf release/cedar/bin/*Test
rm -f release/cedar/bin/cedar
mv release/cedar/bin/cedar-6.2.0 release/cedar/bin/cedar
rm -f release/cedar/*.*
rm -rf release/cedar/cedar/
rm -rf release/cedar/cmake/
rm -rf release/cedar/build/
rm -rf release/cedar/doc/
rm -rf release/cedar/examples/
rm -rf release/cedar/executables/
rm -rf release/cedar/templates/
rm -rf release/cedar/tools/
mkdir release/cedar/Cedar_Plugins/
mv release/cedar/resources/ release/cedar/Cedar_Resources/
mv release/cedar/tests/ release/cedar/Cedar_Tests/
cp -r /opt/qt512/plugins release/cedar/plugins
cp -P compile/libqglviewer/libQGLViewer-2.6.3/QGLViewer/libQGL*.so* release/cedar/lib/
cd release
wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage cedar/bin/cedar -always-overwrite -bundle-non-qt-libs -no-copy-copyright-files -no-translations -unsupported-allow-new-glibc -qmake=/opt/qt512/bin/qmake 
mv AppRun cedar.app
rm -f linuxdeployqt-continuous-x86_64.AppImage
cd ..
exit 0
