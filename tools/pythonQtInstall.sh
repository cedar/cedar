installpath="/usr/local/lib"
if [[ $# -eq 0 ]]
  then
    echo "Using default path: ${installpath}"
  else
    installpath=$1
    echo "Using selected path: ${installpath}"
fi
python=$(python3 -V)
IFS=" "
read -a python <<< "$python"
python=${python[1]}
IFS="."
read -a python <<< "$python"
py_major=${python[0]}
py_minor=${python[1]}
pythonv=${py_major}.${py_minor}
echo "PythonVersion: ${pythonv}"
requiredPkgs=('make' 'qtmultimedia5-dev' 'libqt5multimediawidgets5' 'libqt5multimedia5-plugins libqt5multimedia5' 'qtdeclarative5-dev' 'libqt5svg5-dev' 'libqt5xmlpatterns5-dev' 'qttools5-dev' 'qtbase5-private-dev' 'qt5-default' 'python-dev' 'pip' 'python3-pip' 'git' 'libboost-python-dev')
for REQUIRED_PKG in ${requiredPkgs[@]};
do
  PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
  echo Checking for $REQUIRED_PKG: $PKG_OK
  if [ "" = "$PKG_OK" ];  then
    echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."
    sudo apt-get --yes install $REQUIRED_PKG
  fi
done
python3 -m pip install python-config
cd $installpath
sudo rm pythonqt -r
sudo git clone https://github.com/MeVisLab/pythonqt.git
cd pythonqt/build
changeLine=$(sudo awk '/unix:PYTHON_VERSION=/{ print NR; exit }' python.prf)
echo "line to change: ${changeLine}"
#sudo sed -i ''"${changeLine}"'s/.*/  unix:PYTHON_VERSION='"${pythonv}"'' python.prf
sudo awk 'NR=='"${changeLine}"' {$0="  unix:PYTHON_VERSION='"${pythonv}"'"} 1' python.prf > python1.prf
sudo cp python1.prf python.prf
sudo rm python1.prf
echo "Changed PythonQt PYTHON_VERSION to ${pythonv}"
cd ../src
changeLine=$(sudo awk '/#undef _POSIX_THREADS/{ print NR; exit }' PythonQtPythonInclude.h)
if [ $py_minor -gt 6 -a $py_major -eq 3 ]
  then  
    sudo awk 'NR=='"${changeLine}"' {$0="'//'#undef _POSIX_THREADS"} 1' PythonQtPythonInclude.h > PythonQtPythonInclude1.h
  else
    sudo awk 'NR=='"${changeLine}"' {$0="#undef _POSIX_THREADS"} 1' PythonQtPythonInclude.h  > PythonQtPythonInclude1.h
fi
sudo cp PythonQtPythonInclude1.h PythonQtPythonInclude.h
sudo rm PythonQtPythonInclude1.h
echo -e "Necessary changes made. \nNow building PythonQt" 
cd ..
sudo qmake
sudo make all &&
echo -e "Successfully built PythonQt. \nIn CEDAR.CONF do the following changes: \n" &&
echo "SET (CEDAR_INCLUDE_PYTHON   1)" 
echo "SET (PYTHON_MAJOR_VERSION   ${py_major})" &&
echo "SET (PYTHON_MINOR_VERSION   ${py_minor})" &&
echo "SET (CEDAR_USE_PYTHONQT     1)" &&
echo "SET (PYTHONQT_PATH     '${installpath}/pythonqt')"
