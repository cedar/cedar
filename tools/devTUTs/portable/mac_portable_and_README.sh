#!/bin/bash


#!!!READ
#Frederik Bendel, frederik.bendel@ruhr-uni-bochum.de
#Hey, you, who wants to build that nice portable.
# Set CEDAR_PORTABLE and BUNDLE_MAC_PATHS TO 1
  #Build in Release Mode
  #
  #Run this script from bin Folder
#
#
# This script tries to fix the inclusion of PythonQT. It tries to make sure to:
# - that all libraries in App/Contents/Frameworks AND executables in App/Contents/MacOs point to the libPythonQt... in the same folder.
# - also that they point to the correct python framework. as for now it is copied from the framework in MacOs /Library, therefore "/Library/Frameworks/Python.framework/Versions/3.9/lib/libpython3.9.dylib" is the path
# - basics: Every reference given by otool -l <LibName> should point to a RELATIVE PATH something as @executable_path/../
# - you can fix NAMES with: install_name_tool -change old_name new_name library_to_edit
# - you can change paths with: 1. -install_name_tool -delete_rpath "old_path" library_to_edit 2. -install_name_tool -add_rpath "old_path" library_to_edit


# Define app name
APP_NAME="cedar"

# Run macdeployqt
/usr/local/Cellar/qt5/5.15.2/clang_64/bin/macdeployqt $APP_NAME.app -always-overwrite

#Download macdeployqtfix
curl -o macdeployqtfix.py https://raw.githubusercontent.com/arl/macdeployqtfix/master/macdeployqtfix.py

# Apply macdeployqtfix with Python 2.7!!! brew install python@2
/usr/local/bin/python2.7 macdeployqtfix.py $APP_NAME.app/Contents/MacOS/cedar /usr/local/Cellar/qt5/5.15.2/clang_64

# Copy PythonQt libraries
cp /Users/cedarmac/CLionProjects/pythonqt/lib/libPythonQt_QtAll-Qt5-Python3.9.3.dylib $APP_NAME.app/Contents/Frameworks
cp /Users/cedarmac/CLionProjects/pythonqt/lib/libPythonQt-Qt5-Python3.9.3.dylib $APP_NAME.app/Contents/Frameworks

# Copy Python libraries
cp /Library/Frameworks/Python.framework/Versions/3.9/lib/libpython3.9.dylib $APP_NAME.app/Contents/Frameworks
cp -R /Library/Frameworks/Python.framework $APP_NAME.app/Contents/Frameworks/Python.framework

# Copy gcc libraries
cp /usr/local/opt/gcc/lib/gcc/11/libgcc_s.1.1.dylib $APP_NAME.app/Contents/Frameworks

# Copy Resources and create Pluginordner
cp -R ../resources $APP_NAME.app/Contents/Cedar_Resources
mkdir $APP_NAME.app/Contents/Cedar_Plugins

# Define QT_PATH
QT_PATH="/usr/local/opt/qt@5/lib"

# Define FRAMEWORKS
FRAMEWORKS=("QtPrintSupport" "QtSvg" "QtOpenGL" "QtMultimediaWidgets" "QtQuickWidgets" "QtWidgets" "QtMultimedia" "QtQuick" "QtGui" "QtSql" "QtXmlPatterns" "QtQmlModels" "QtQml" "QtNetwork" "QtXml" "QtCore")

# Define LIBRARIES
LIBRARIES=("libPythonQt_QtAll-Qt5-Python3.9.3.dylib" "libPythonQt-Qt5-Python3.9.3.dylib" "libcedardyn.6.2.0.dylib" "libcedarunits.6.2.0.dylib" "libcedardev.6.2.0.dylib" "libcedarproc.6.2.0.dylib" "libcedaraux.6.2.0.dylib")

# Update install names for each library
for LIB in ${LIBRARIES[@]}; do
    for FRAMEWORK in ${FRAMEWORKS[@]}; do
        install_name_tool -change $QT_PATH/$FRAMEWORK.framework/Versions/5/$FRAMEWORK @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK $APP_NAME.app/Contents/Frameworks/$LIB
    done
    install_name_tool -change libPythonQt_QtAll-Qt5-Python3.9.3.dylib @executable_path/../Frameworks/libPythonQt_QtAll-Qt5-Python3.9.3.dylib $APP_NAME.app/Contents/Frameworks/$LIB
    install_name_tool -change libPythonQt-Qt5-Python3.9.3.dylib @executable_path/../Frameworks/libPythonQt-Qt5-Python3.9.3.dylib $APP_NAME.app/Contents/Frameworks/$LIB
    install_name_tool -change @rpath/Python3.framework/Versions/3.9/Python3 @executable_path/../Frameworks/Python.framework/Versions/3.9/Python $APP_NAME.app/Contents/Frameworks/$LIB
    install_name_tool -delete_rpath "/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/3.9/include/python3.9" $APP_NAME.app/Contents/Frameworks/$LIB
    install_name_tool -delete_rpath "/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/3.9/lib" $APP_NAME.app/Contents/Frameworks/$LIB
    install_name_tool -add_rpath "@executable_path/../Frameworks/Python.framework/Versions/3.9/include/python3.9" $APP_NAME.app/Contents/Frameworks/$LIB
    install_name_tool -add_rpath "@executable_path/../Frameworks/Python.framework/Versions/3.9/lib" $APP_NAME.app/Contents/Frameworks/$LIB
done

EXECUTABLES=("cedar" "cedar-6.2.0")

for exe in ${EXECUTABLES[@]}; do
  install_name_tool -delete_rpath "/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/3.9/include/python3.9" $APP_NAME.app/Contents/MacOS/$exe
  install_name_tool -delete_rpath "/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/3.9/lib" $APP_NAME.app/Contents/MacOS/$exe
  install_name_tool -add_rpath "@executable_path/../Frameworks/Python.framework/Versions/3.9/include/python3.9" $APP_NAME.app/Contents/MacOS/$exe
  install_name_tool -add_rpath "@executable_path/../Frameworks/Python.framework/Versions/3.9/lib" $APP_NAME.app/Contents/MacOS/$exe
  install_name_tool -change libPythonQt_QtAll-Qt5-Python3.9.3.dylib @executable_path/../Frameworks/libPythonQt_QtAll-Qt5-Python3.9.3.dylib $APP_NAME.app/Contents/MacOS/$exe
  install_name_tool -change libPythonQt-Qt5-Python3.9.3.dylib @executable_path/../Frameworks/libPythonQt-Qt5-Python3.9.3.dylib $APP_NAME.app/Contents/MacOS/$exe
  install_name_tool -change @rpath/Python3.framework/Versions/3.9/Python3 @executable_path/../Frameworks/Python.framework/Versions/3.9/Python $APP_NAME.app/Contents/MacOS/$exe
done

# Run macdeployqt
/usr/local/Cellar/qt5/5.15.2/clang_64/bin/macdeployqt $APP_NAME.app -always-overwrite

# Apply macdeployqtfix with Python 2.7!!! brew install python@2
/usr/local/bin/python2.7 macdeployqtfix.py $APP_NAME.app/Contents/MacOS/cedar /usr/local/Cellar/qt5/5.15.2/clang_64

