cd cedar
git status
git pull
cd build
cmake ..
make cedar -j $NUM_CORES
sudo make install DESTDIR=AppDir

wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage

./linuxdeploy-x86_64.AppImage --appdir AppDir -e Cedar -i ../doc/resources/images/cedar_logo.png -d ../doc/resources/cedar.desktop --plugin qt --output appimage
