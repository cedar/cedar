import sys
from PyQt5 import QtCore, QtWidgets

import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure


# MatPlotLib Canvas: Widget used to show plots
class MPLCanvas(FigureCanvasQTAgg):
    def __init__(self, width=10, height=5, dpi=100):
        self.fig = Figure(dpi=dpi, figsize=(width, height))
        self.axes = self.fig.add_subplot(111)
        
        super(MPLCanvas, self).__init__(self.fig)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.init_layout()

    # Defines layout of the main window
    def init_layout(self):
        vbox_widget = QtWidgets.QWidget(self)
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
        
        # Add button to load a file
        button = QtWidgets.QPushButton(self)
        button.setText("Load file")
        button.clicked.connect(self.openFileDialog)
        vbox_widget.layout.addWidget(button)

        self.setCentralWidget(vbox_widget)
        self.show()

    # open the file dialog and load the file if successful
    def openFileDialog(self):
        file, success = QtWidgets.QFileDialog.getOpenFileName(None, "Select recording",
                                                   "/home/lars/cedarRecordings", "CSV Files (*.csv);;All Files (*)")
        if success:
            self.loadFile(file)

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

        # Plot data at timestep 0
        self.plot_at_index(0)
    
    def sliderValueChanged(self):
        # Plot data at the selected timestep
        self.plot_at_index(self.slider.value()) 

    def plot_at_index(self, index):
        if(len(self.plotdata) <= index):
            #TODO add throw
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
        self.canvas.draw()

    def plot2D(self, im):
        self.canvas.axes.cla()
        self.canvas.axes.imshow(im)
        self.canvas.draw()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()

    app.exec_()