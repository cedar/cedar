
brew install qt@5.15.2 boost-python3 opencv gawk
ln -s /usr/local/Cellar/qt5/5.15.2/bin/qmake /usr/local/bin/qmake

installpath="/usr/local/lib"

python=$(python3 -V)
IFS=" "
read -a python <<< "$python"
python=${python[1]}
IFS="."
read -a python <<< "$python"
py_major=${python[0]}
py_minor=${python[1]}
pythonv=${py_major}.${py_minor}

python3 -m pip install python-config

cd $installpath
sudo rm -r pythonqt
git clone https://github.com/MeVisLab/pythonqt.git
cd pythonqt/build
changeLine=$(sudo awk '/unix:PYTHON_VERSION=/{ print NR; exit }' python.prf)
echo "line to change: ${changeLine}"
#sudo sed -i ''"${changeLine}"'s/.*/  unix:PYTHON_VERSION='"${pythonv}"'' python.prf
awk 'NR=='"${changeLine}"' {$0="  unix:PYTHON_VERSION='"${pythonv}"'"} 1' python.prf > python1.prf
cp python1.prf python.prf
rm python1.prf
changeLine=$(sudo awk '/System\/Library/{ print NR; exit }' python.prf)
echo "line to change: ${changeLine}"
awk 'NR=='"${changeLine}"' {$0="   INCLUDEPATH += /Library/Frameworks/Python.framework/Headers"} 1' python.prf > python1.prf
cp python1.prf python.prf
rm python1.prf
changeLine=$(sudo awk '/-framework Python/{ print NR; exit }' python.prf)
echo "line to change: ${changeLine}"
awk 'NR=='"${changeLine}"' {$0="    LIBS += -F/Library/Frameworks -framework Python"} 1' python.prf > python1.prf

cp python1.prf python.prf
rm python1.prf
echo "Changed PythonQt PYTHON_VERSION to ${pythonv}"
cd ../src
changeLine=$(awk '/#undef _POSIX_THREADS/{ print NR; exit }' PythonQtPythonInclude.h)
if [ $py_minor -gt 6 -a $py_major -eq 3 ]
  then  
    awk 'NR=='"${changeLine}"' {$0="'//'#undef _POSIX_THREADS"} 1' PythonQtPythonInclude.h > PythonQtPythonInclude1.h
  else
    awk 'NR=='"${changeLine}"' {$0="#undef _POSIX_THREADS"} 1' PythonQtPythonInclude.h  > PythonQtPythonInclude1.h
fi
cp PythonQtPythonInclude1.h PythonQtPythonInclude.h
rm PythonQtPythonInclude1.h
echo -e "Necessary changes made. \nNow building PythonQt" 
cd ..
qmake
make all &&
echo -e "Successfully built PythonQt. \nIn CEDAR.CONF do the following changes: \n" &&
echo "SET (CEDAR_INCLUDE_PYTHON   1)" 
echo "SET (PYTHON_MAJOR_VERSION   ${py_major})" &&
echo "SET (PYTHON_MINOR_VERSION   ${py_minor})" &&
echo "SET (CEDAR_USE_PYTHONQT     1)" &&
echo "SET (PYTHONQT_PATH     '${installpath}/pythonqt')"
