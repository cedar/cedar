import sys
from PyQt5 import QtCore, QtWidgets, QtGui

import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure
from mpl_toolkits.axes_grid1 import make_axes_locatable


# MatPlotLib Canvas: Widget used to show plots
class MPLCanvas(FigureCanvasQTAgg):
    def __init__(self, width=10, height=5, dpi=100):
        self.fig = Figure(dpi=dpi, figsize=(width, height))
        self.axes = self.fig.add_subplot(111)
        
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
        self.valueRange_lower = None
        self.valueRange_upper = None
        self.cbar = None
        self.init_layout()

    # Defines layout of the main window
    def init_layout(self):
        splitter_main_widget = QtWidgets.QSplitter(self)

        vbox_widget = QtWidgets.QWidget(splitter_main_widget)
        vbox_widget.layout = QtWidgets.QVBoxLayout(vbox_widget)

        self.canvas = MPLCanvas()

        # Insert a default plot, to be removed
        self.canvas.axes.plot([0,1,2,3,4], [10,1,20,3,40])
        vbox_widget.layout.addWidget(self.canvas)

        # Add slider to select timestep
        self.slider = QtWidgets.QSlider(QtCore.Qt.Orientation.Horizontal, self)
        self.slider.setPageStep(1)
        self.slider.setRange(1, 1)

        # Add label to the slider, to show details about selected timestep
        self.sliderValueLabel = QtWidgets.QLabel('Data point: 1\nTime: 0 s', self)
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
        file, success = QtWidgets.QFileDialog.getSaveFileName(None, "Export recordings",
                                                   "/home/lars/cedarRecordings", "CSV Files (*.csv);;All Files (*)")
        if success:
            self.exportFile(file)

    # export selected timesteps to file
    def exportFile(self, filename):
        # prepare lines to write to file
        lines = [",".join(self.metadata) + "\n"]
        for index in range(len(self.plotdata)):
            if self.list_view.item(index).checkState() == QtCore.Qt.CheckState.Checked:
                lines.append(self.plotdata[index]) # str(index + 1) + ": " + 
        
        # export
        with open(filename, "w") as f:
            f.writelines(lines)

    # open the file dialog and load the file if successful
    def openFileReadDialog(self):
        file, success = QtWidgets.QFileDialog.getOpenFileName(None, "Select recording",
                                                   "/home/lars/cedarRecordings", "CSV Files (*.csv);;All Files (*)")
        if success:
            self.loadFile(file)

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

    def loadFile(self, filename):
        lines = []
        # Read all lines of the file
        with open(filename, "r") as f:
            lines = f.readlines()
        if len(lines) <= 0:
            #TODO add throw
            return

        # Read the metadata in the first line
        self.metadata = lines[0].strip().split(",")
        if len(self.metadata) < 3:
            #TODO add throw
            return
        if self.metadata[0].strip() != "Mat":
            #TODO add throw
            return
        # Store the sizes of all dimensions
        self.dimensions = [int(size) for size in self.metadata[2:]]
        self.dim = len(self.dimensions)
        # One dimensional data is stored as two dimensional data with a 2nd dimension of size 1
        if self.dim == 2:
            if self.dimensions[1] == 1:
                self.dim = 1

        # Save the plot data
        self.plotdata = lines[1:]

        # Init layout and prepare slider
        self.init_layout()
        self.slider.setValue(0)
        self.slider.setRange(1, len(self.plotdata))
        self.slider.valueChanged.connect(self.sliderValueChanged)     

        model = QtGui.QStandardItemModel(self.list_view)
        for index in range(len(self.plotdata)):
            time = self.plotdata[index].strip().split(",")[0]
            text = self.leftAppendString(str(index + 1), 2) + " - Time: " + time
            item = QtWidgets.QListWidgetItem(text)
            item.setCheckState(QtCore.Qt.CheckState.Unchecked)
            self.list_view.addItem(item)

        # Plot data at timestep 0
        self.repaint()

    def repaint(self):
        # Plot data at the selected timestep
        self.plot_at_index(self.slider.value() - 1) 

    def plot_at_index(self, index):
        if(len(self.plotdata) <= index):
            return
        
        data = self.plotdata[index].strip().split(",")
        time = data[0]
        # Add timestep metadata to the label
        self.sliderValueLabel.setText("Data point: " + str(self.slider.value()) + "\nTime: " + time)
        
        if self.dim == 1:
            # Prepare x and y arrays from data
            y = [float(y) for y in data[1:]]
            x = [x for x in range(len(y))]
            self.plot1D(x, y)

        elif self.dim == 2:
            width = self.dimensions[0]
            height = self.dimensions[1]
            if len(data) - 1 != width * height:
                #TODO add throw
                print("len(data) -1 != width * height")
                return
            # Prepare image from the data
            image = [[float(val) for val in data[(rowindex*width)+1:((rowindex+1)*width)+1]] for rowindex in range(height)]
            #TODO can this be replaced by a call to .transpose() ?
            image_transposed = [[row[i] for row in image] for i in range(len(image[0]))]
            self.plot2D(image_transposed)
    
    def plot1D(self, x, y):
        self.canvas.axes.cla()
        self.canvas.axes.plot(x, y)
        self.canvas.axes.set_ylim([self.valueRange_lower, self.valueRange_upper])
        self.canvas.draw()

    def plot2D(self, im):        
        self.canvas.axes.cla()
        if not self.cbar == None:
            self.cbar.remove()
        ax = self.canvas.axes.imshow(im, vmin = self.valueRange_lower, vmax = self.valueRange_upper)
        self.cbar = self.canvas.fig.colorbar(ax)
        self.canvas.draw()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()

    app.exec_()