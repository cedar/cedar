import sys
import os
from PyQt5 import QtCore, QtWidgets, QtGui

import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure
from mpl_toolkits.axes_grid1 import make_axes_locatable

# MatPlotLib Canvas: Widget used to show plots
class MPLCanvas(FigureCanvasQTAgg):
    def __init__(self, nbrOfSubplots=1, width=10, height=5, dpi=100):
        self.fig = Figure(dpi=dpi, figsize=(width, height))
        self.axes = [self.fig.add_subplot(101 + 10 * nbrOfSubplots + index) for index in range(nbrOfSubplots)]

        super(MPLCanvas, self).__init__(self.fig)

class TwoLineInputDialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.first = QtWidgets.QLineEdit(self)
        self.second = QtWidgets.QLineEdit(self)
        buttonBox = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Cancel | QtWidgets.QDialogButtonBox.Ok, self);

        layout = QtWidgets.QFormLayout(self)
        layout.addRow("Lower bound", self.first)
        layout.addRow("Upper bound", self.second)
        layout.addWidget(buttonBox)

        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)

    def getInputs(self):
        return (self.first.text(), self.second.text())

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.plotdata = []
        self.dimensions = []
        self.dim = []
        self.metadata = []
        self.valueRange_lower = None
        self.valueRange_upper = None
        self.cbar = None

        self.init_layout()

    # Defines layout of the main window
    def init_layout(self, nbrOfSubplots=1):
        splitter_main_widget = QtWidgets.QSplitter(self)

        vbox_widget = QtWidgets.QWidget(splitter_main_widget)
        vbox_widget.layout = QtWidgets.QVBoxLayout(vbox_widget)

        self.canvas = MPLCanvas(nbrOfSubplots)
        # Insert a default plot, to be removed
        for axes in self.canvas.axes:
            axes.plot([0,1,2,3,4], [10,1,20,3,40])
        
        vbox_widget.layout.addWidget(self.canvas)

        # Add slider to select timestep
        self.slider = QtWidgets.QSlider(QtCore.Qt.Orientation.Horizontal, self)
        self.slider.setPageStep(1)
        self.slider.setRange(1, 1)

        # Add label to the slider, to show details about selected timestep
        self.sliderValueLabel = QtWidgets.QLabel('Data point: 1', self)
        self.sliderValueLabel.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter |
                                QtCore.Qt.AlignmentFlag.AlignVCenter)
        self.sliderValueLabel.setMinimumWidth(80)
        
        # Align slider and label horizontally
        slider_hbox_widget = QtWidgets.QWidget(vbox_widget)
        slider_hbox_widget.layout = QtWidgets.QHBoxLayout(slider_hbox_widget)
        slider_hbox_widget.layout.addWidget(self.slider)
        slider_hbox_widget.layout.addSpacing(15)
        slider_hbox_widget.layout.addWidget(self.sliderValueLabel)
        vbox_widget.layout.addWidget(slider_hbox_widget)
        
        hbox_button_widget = QtWidgets.QWidget(self)
        hbox_button_widget.layout = QtWidgets.QHBoxLayout(hbox_button_widget)


        # Add button to set the value range
        buttonValRange = QtWidgets.QPushButton(self)
        buttonValRange.setText("Set value range")
        buttonValRange.clicked.connect(self.openValueRangeDialog)
        hbox_button_widget.layout.addWidget(buttonValRange)

        # Add button to load a file
        button = QtWidgets.QPushButton(self)
        button.setText("Load file")
        button.clicked.connect(self.openFileReadDialog)
        hbox_button_widget.layout.addWidget(button)

        vbox_widget.layout.addWidget(hbox_button_widget)

        splitter_main_widget.addWidget(vbox_widget)


        vbox_list_widget = QtWidgets.QWidget(splitter_main_widget)
        vbox_list_widget.layout = QtWidgets.QVBoxLayout(vbox_list_widget)


        self.list_view = QtWidgets.QListWidget(self)
        self.list_view.currentRowChanged.connect(self.listViewRowChanged)

        exportButton = QtWidgets.QPushButton(self)
        exportButton.setText("Export")
        exportButton.clicked.connect(self.openFileSaveDialog)


        vbox_list_widget.layout.addWidget(self.list_view)
        vbox_list_widget.layout.addWidget(exportButton)
        
        splitter_main_widget.addWidget(vbox_list_widget)

        self.setCentralWidget(splitter_main_widget)
        self.show()

    # Show saveFile dialog and export to file if successful
    def openFileSaveDialog(self):
        filename, success = QtWidgets.QFileDialog.getSaveFileName(None, "Export recordings",
                                                   "/home/lars/cedarRecordings", "CSV Files (*.csv);;All Files (*)")
        #directory = QtWidgets.QFileDialog.getExistingDirectory(None, "Export recordings", )
        if success:
            self.exportFiles(filename)

    # export selected timesteps to file
    def exportFiles(self, filename):
        filename = filename.strip()
        if filename[-4:] == ".csv":
            filename = filename[:-4]
        for index in range(len(self.filenames)):
            # prepare lines to write to file
            lines = [",".join(self.metadata[index]) + "\n"]
            for dataIndex in range(len(self.plotdata[index])):
                if self.list_view.item(dataIndex).checkState() == QtCore.Qt.CheckState.Checked:
                    lines.append(self.plotdata[index][dataIndex]) 
            
            # export
            finalFilename = filename
            if(len(self.filenames) > 1):
                finalFilename += "_" + self.filenames[index].split(os.path.sep)[-1]
            
            if not finalFilename[-4:] == ".csv":
                finalFilename = finalFilename + ".csv"

            with open(finalFilename, "w") as f:
                f.writelines(lines)

    # open the file dialog and load the file if successful
    def openFileReadDialog(self):
        files, success = QtWidgets.QFileDialog.getOpenFileNames(None, "Select recording",
                                                   "/home/lars/cedarRecordings", "CSV Files (*.csv);;All Files (*)")
        
        if success:
            self.loadFiles(files)

    # open a dialog to set the value range
    def openValueRangeDialog(self):
        valueRangeDialog = TwoLineInputDialog()
        if valueRangeDialog.exec():
            lower, upper = valueRangeDialog.getInputs()

            if(lower.strip() == ""):
                lower = None
            else:
                lower = float(lower)
            if(upper.strip() == ""):
                upper = None
            else:
                upper = float(upper)

            self.setValueRange(lower, upper)

    def setValueRange(self, lower, upper):
        self.valueRange_lower = lower
        self.valueRange_upper = upper
        self.repaint()

    def leftAppendString(self, string, digits):
        return "0" * (digits - len(string)) + string

    def listViewRowChanged(self, currentRow):
        if not currentRow + 1 == self.slider.value():
            self.slider.setValue(currentRow + 1) 

    def sliderValueChanged(self):
        if not self.slider.value() == self.list_view.currentRow() + 1:   
            self.list_view.setCurrentRow(self.slider.value() - 1)
        self.repaint()

    def loadFiles(self, filenames):
        self.filenames = filenames
        self.dimensions = []
        self.dim = []
        self.plotdata = []
        self.metadata = []
        for fileIndex, filename in enumerate(filenames):
            lines = []
            # Read all lines of the file
            with open(filename, "r") as f:
                lines = f.readlines()
            if len(lines) <= 0:
                #TODO add throw
                print(f"No data in file {filename}")
                return

            # Read the metadata in the first line
            metadata = lines[0].strip().split(",")
            if len(metadata) < 3:
                #TODO add throw
                print(f"Metadata in first line contains less data than expected ({filename})")
                return
            if metadata[0].strip() != "Mat":
                #TODO add throw
                print("First metadata element in first line was expected to be 'Mat' ({filename})")
                return
            self.metadata.append(metadata)#

            # Store the sizes of all dimensions
            dimensions = [int(size) for size in metadata[2:]]
            dim = len(dimensions)
            # One dimensional data is stored as two dimensional data with a 2nd dimension of size 1
            if dim == 2:
                if dimensions[1] == 1:
                    dim = 1
            
            
            self.dimensions.append(dimensions)
            self.dim.append(dim)

            # Save the plot data
            self.plotdata.append(lines[1:])
            if fileIndex > 0 and not len(lines) - 1 == len(self.plotdata[0]):
                # TODO ??
                print("Files have a different amount of timesteps")

        # Init layout and prepare slider
        self.init_layout(len(filenames))
        self.slider.setValue(0)
        self.slider.setRange(1, len(self.plotdata[0]))
        self.slider.valueChanged.connect(self.sliderValueChanged)     

        model = QtGui.QStandardItemModel(self.list_view)
        for index in range(len(self.plotdata[0])):
            time = self.plotdata[0][index].strip().split(",")[0]
            text = self.leftAppendString(str(index + 1), 2) + " - Time: " + time
            item = QtWidgets.QListWidgetItem(text)
            item.setCheckState(QtCore.Qt.CheckState.Unchecked)
            self.list_view.addItem(item)

        # Plot data at timestep 0
        self.repaint()

    def repaint(self):
        # Plot data at the selected timestep
        self.plot_at_index(self.slider.value() - 1)

    def plot_at_index(self, timestepIndex):
        for subplotIndex in range(len(self.plotdata)):
            if len(self.plotdata[subplotIndex]) <= timestepIndex:
                self.plot1D([], [], subplotIndex)
                continue

            data = self.plotdata[subplotIndex][timestepIndex].strip().split(",")
            if subplotIndex == 0:
                time = data[0]
                # Add timestep metadata to the label
                self.sliderValueLabel.setText("Data point: " + str(self.slider.value()))# + "\nTime: " + time)
            if self.dim[subplotIndex] == 1:
                # Prepare x and y arrays from data
                y = [float(y) for y in data[1:]]
                x = [x for x in range(len(y))]
                self.plot1D(x, y, subplotIndex)

            elif self.dim[subplotIndex] == 2:
                width = self.dimensions[subplotIndex][0]
                height = self.dimensions[subplotIndex][1]
                if len(data) - 1 != width * height:
                    #TODO add throw
                    print("len(data) -1 != width * height")
                    return
                # Prepare image from the data
                image = [[float(val) for val in data[(rowindex*width)+1:((rowindex+1)*width)+1]] for rowindex in range(height)]
                #TODO can this be replaced by a call to .transpose() ?
                image_transposed = [[row[i] for row in image] for i in range(len(image[0]))]
                self.plot2D(image_transposed, subplotIndex)
            else:
                print(f"Unsupported dimension: {self.dim[subplotIndex]}")
                return
    
    def plot1D(self, x, y, subplotIndex):
        self.canvas.axes[subplotIndex].cla()
        self.canvas.axes[subplotIndex].plot(x, y)
        self.canvas.axes[subplotIndex].set_ylim([self.valueRange_lower, self.valueRange_upper])
        self.canvas.axes[subplotIndex].set_title(f"{self.filenames[subplotIndex].split('/')[-1]}\nTime: {self.plotdata[subplotIndex][self.slider.value() - 1].strip().split(' ')[0]}", fontsize=8)
        self.canvas.draw()

    def plot2D(self, im, subplotIndex):
        self.canvas.axes[subplotIndex].cla()
        if not self.cbar == None:
            self.cbar.remove()
        ax = self.canvas.axes[subplotIndex].imshow(im, vmin = self.valueRange_lower, vmax = self.valueRange_upper)
        self.cbar = self.canvas.fig.colorbar(ax)
        self.canvas.axes[subplotIndex].set_title(f"{self.filenames[subplotIndex].split('/')[-1]}\nTime: {self.plotdata[subplotIndex][self.slider.value() - 1].strip().split(' ')[0]}", fontsize=8)
        self.canvas.draw()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()

    app.exec_()