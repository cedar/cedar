#!/usr/bin/python

'''
========================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        guifuncs.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 30

    Description: 

    Credits:

========================================================================================================================
'''
import matplotlib as mpl
mpl.use('WXAgg')

import os
import matplotlib.pyplot as plt
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas
from matplotlib.ticker import ScalarFormatter 
import numpy as np

import wx
import rdp.datatools
import rdp.plotfuncs

def search_homedir():

    # search for standard cedarRecordings directory
    if 'HOME' in os.environ:
        directory = os.environ['HOME']
    elif os.name == 'posix':
        directory = os.path.expanduser("~/")
    elif os.name == 'nt':
        if 'HOMEPATH' in os.environ and 'HOMEDRIVE' in os.environ:
            directory = os.environ['HOMEDRIVE'] + os.environ['HOMEPATH']
    else:
        directory = os.environ['HOMEPATH']
    
    cedar_settings_file = directory + '/.cedar/auxiliariesSettings'
    
    f = open(cedar_settings_file)
    flist = f.readlines()
    
    for i in range(len(flist)):
        if 'recorder output directory' in flist[i]:
            aux = str(flist[i].replace('\/','/').strip(' ').split(' ')[-1:])
            directory = aux.strip('[').strip(']').strip('\'\"').strip('\",\\n')
            
    f.close()
    
    return directory


def create_figure_canvas(figure, title, control_plot_panel):

    if figure == None:
        figure = plt.figure(str(title))
        
    figure_canvas = FigureCanvas(control_plot_panel, wx.ID_ANY, figure)
    
    return figure_canvas


def enforce_labelling_mode(plot, labelling_mode, style, axis_ticks):
        
    # Prevent scientific notation and offset
    formatter = ScalarFormatter()
    formatter.set_scientific(False)
    formatter.set_useOffset(False)
            
    try:
        plot.yaxis.set_major_formatter(formatter)
        plot.yaxis.set_major_formatter(formatter)
        plot.zaxis.set_major_formatter(formatter)
    except AttributeError:
        pass
    
    if labelling_mode == 'off':
        plot.axes.set_xticklabels([])
        plot.axes.set_yticklabels([])
        
        if style == 'surface' or style == 'wireframe':
            try:
                plot.axes.set_zticklabels([])
            except AttributeError:
                pass
    
    elif labelling_mode == 'Standard':
        plt.rcdefaults()
    
    else:
        plt.rc('text', usetex=True)
        mpl.rcParams['text.latex.unicode']=True
        plt.rc('font', family='serif')
        
    try:
        if axis_ticks == False:
            plt.tick_params(axis='both', which='both', bottom='off', top='off', left='off', right='off')
                
        else:
            plt.tick_params(axis='both', which='both', bottom='on', top='on', left='on', right='on')
    except AttributeError:
        pass
    
    return plot


def play_pause_btn(parent, reverse):
    
    if reverse == False:
        active_btn = parent.play_pause_btn
        active_bitmap = parent.play_bitmap
        inactive_btn = parent.reverse_play_pause_btn
        loop_range = range(parent.step, parent.slider_max+1)
        
    else:
        active_btn = parent.reverse_play_pause_btn
        active_bitmap = parent.reverse_play_bitmap
        inactive_btn = parent.play_pause_btn
        loop_range = range(parent.step, -1, -1)
    
    if active_btn.GetBitmapLabel() == active_bitmap:
        active_btn.SetBitmapLabel(parent.pause_bitmap)
        
        if parent.figure:
            
            for i in loop_range:
                
                if active_btn.GetBitmapLabel() == active_bitmap:
                    break
                else:
                    for j in range(len(parent.frame_ids)):
                        current_frame = set_current_frame(parent=parent, i=j)
                        current_frame.step = i
                        rdp.plotfuncs.update_plot(parent=current_frame)
                        wx.Yield()
                                                           
                    inactive_btn.Disable()
                    parent.increase_single_step_btn.Disable()
                    parent.decrease_single_step_btn.Disable()
                    parent.reset_btn.Disable()
                    parent.pos_slider.SetValue(min(parent.slider_max, parent.step))
                    parent.time_stamp_display.SetLabel(str(parent.time_stamps[parent.pos_slider.GetValue()]))
                    
        active_btn.SetBitmapLabel(active_bitmap)
        
    else:
        active_btn.SetBitmapLabel(active_bitmap)
    
    inactive_btn.Enable()
    parent.increase_single_step_btn.Enable()
    parent.decrease_single_step_btn.Enable()
    parent.reset_btn.Enable()
    
    
def move_single_step(parent, increase):
    
    if increase is False:
        parent.step -= 1
                    
        try:
            parent.pos_slider.SetValue(max(0, parent.step))
        except AttributeError:
            pass
        
    else:
        parent.step += 1
                    
        try:
            parent.pos_slider.SetValue(min(parent.slider_max, parent.step))
        except AttributeError:
            pass
        
    try:
        parent.time_stamp_display.SetLabel(str(parent.time_stamps[parent.pos_slider.GetValue()]))
    except AttributeError:
        pass
        
    if parent.step == 0:
        
        try:
            parent.time_stamp_display.SetLabel(' ')
        except AttributeError:
            pass
    
    for i in range(len(parent.frame_ids)):
        current_frame = set_current_frame(parent=parent, i=i)
        current_frame.step = parent.step
        rdp.plotfuncs.update_plot(parent=current_frame)
        wx.Yield()
        
        
def set_current_frame(parent, i):
    if i == 0:
        current_frame = parent
    else:
        current_frame = wx.FindWindowById(parent.frame_ids[i]).rdp_gui
                        
    return current_frame


def initialize_start_values(parent):
    
    parent.control_plot_frame = None
    parent.main_sizer = wx.BoxSizer(wx.VERTICAL)
    parent.dir = parent.frame.dir            
    parent.figure_size = None
    parent.x_label = '' 
    parent.y_label = '' 
    parent.z_label = '' 
    parent.marked = False
    parent.nstep = 0
    parent.step = 0
    parent.step_size = 0
    parent.slider_max = 0
    parent.stride = 5
    parent.vmin = None
    parent.vmax = None
    parent.proj = None
    parent.proj_method = 'average'
    parent.style = ''
    parent.mode = ''
    parent.labelling_mode = 'off'
    parent.flist = [record_file for record_file in os.listdir(parent.dir) if record_file.lower().endswith('.csv') or record_file.lower().endswith('.data')]
    parent.flist_sorted = np.asarray(rdp.datatools.sort_alphnum(parent.flist))
    parent.data = None
    parent.reduced_data = None
    parent.header = None
    parent.header_list = []
    parent.ndim = ['']
    parent.time_stamps = []
    parent.selection = 0
    parent.proj_choice_time_course = None
    parent.proj_choice_snapshot = None
    parent.plot = None
    parent.line_color = '#FF9600'
    parent.aux_line_color = '#FF9600'
    parent.marker_color = '#FF9600'
    parent.aux_marker_color = '#FF9600'
    parent.save_mode = False
    parent.frame_ids = []
    parent.frame_ids.append(parent.frame.GetId())
    parent.figure = None
    parent.figure_canvas = None
    parent.figure_canvas_connection_id = None
    parent.title = '' 
    parent.figure_resize = None
    parent.colorbar = None
    parent.axis_ticks = None
    parent.sel_cbox_selection = '' 
    parent.rdp_frame = None
    parent.line_style = 'solid'
    parent.lines = []
    
    # Plot modes
    parent.mode_ch = ['snapshot', 'time course']
    
    # Projection choices for time course plot mode
    parent.proj_ch = ['x_1', 'x_2', 'x_3', 'x_4', 'x_5']
    
    # Projection choices for snapshot/snapshot sequence plot modes
    parent.proj_ch_step = ['x_1', 'x_2', 'x_3', 'x_4', 'x_5', 
                         'x_1, x_2', 'x_1, x_3', 'x_1, x_4', 'x_1, x_5', 'x_2, x_3', 
                         'x_2, x_4', 'x_2, x_5', 'x_3, x_4', 'x_3, x_5', 'x_4, x_5']
    parent.style_ch = ['heatmap', 'surface', 'wireframe']
    parent.proj_methods = ['average', 'maximum', 'sum']
    
    parent.labelling_choices = ['off', 'LateX', 'Standard']
    parent.figure_azimuth = None
    parent.figure_elevation = None
    parent.figure_distance = None
    parent.surface_linewidth = 0
    parent.surface_cmap = 'coolwarm'
    
    
def update_selection_data(parent, frame):
    '''Reset the control panel and update it with the newly selected data.'''
    
    parent.data = None
    parent.reduced_data = None
    parent.figure_canvas = None
               
    #else:
    for i in range(len(parent.frame_ids)):
        current_frame = rdp.guifuncs.set_current_frame(parent=parent, i=i)
        current_frame.step = parent.step
                
    # Generate adequate options for the projection combobox, depending on data dimensionality and plot mode
    parent.proj_choice_time_course = parent.proj_ch[:parent.ndim[parent.selection]]
    parent.proj_choice_snapshot = rdp.plotfuncs.build_proj_ch_step(ndim=parent.ndim[parent.selection], temp_proj_ch_step=parent.proj_ch_step)   
    
    # Get data and data header
    recorded_file = parent.dir + '/' + parent.flist_sorted[parent.selection]
    parent.header = rdp.datatools.get_csv_header(csv_f=recorded_file)        
    temp_data = rdp.datatools.get_csv_data(csv_f=recorded_file, header=parent.header)
    
    # If current selection is an image
    if 'CV_8U' in parent.header or 'CV_8UC3' in parent.header:
        parent.axis_ticks = False
    
    # Enable slider and player buttons
    if parent.frame.parent == None:
        parent.pos_slider.Enable()
        parent.play_pause_btn.Enable()
        parent.reverse_play_pause_btn.Enable()
        parent.reset_btn.Enable()
        parent.decrease_single_step_btn.Enable()
        parent.increase_single_step_btn.Enable()
    
    # Update GUI elements with loaded data
    parent.data = temp_data[0]
    parent.time_stamps = temp_data[1]
    parent.slider_max = len(parent.time_stamps)-1
    
    if parent.frame.parent is None:
        parent.pos_slider.SetMax(parent.slider_max)
