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

    File:        datatools.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 30

    Description: 

    Credits:

========================================================================================================================
'''

import csv
import numpy as np
import re
import wx
import rdp
import recorded_data_processor

try:
    import cPickle as pickle
except ImportError:
    import pickle

from dlg import progress_dlg

def prepare_plot_configuration(directory, x_label, y_label, z_label, marked, step, slider_max, stride, vmax, vmin, proj, 
                               proj_method, style, mode, labelling_mode, reduced_data, header, ndim, selection, sel_cbox_selection,
                               proj_choice_time_course, proj_choice_snapshot, line_color, aux_line_color, marker_color, aux_marker_color, title, 
                               axis_ticks, figure_azimuth, figure_elevation, figure_distance, surface_linewidth, flist_sorted, mode_ch, proj_ch, 
                               line_style, surface_cmap):
    
    
    save_object = [691, directory, x_label, y_label, z_label, marked, step, slider_max, stride, vmax, vmin, proj, 
                   proj_method, style, mode, labelling_mode, reduced_data, header, ndim, selection, sel_cbox_selection,
                   proj_choice_time_course, proj_choice_snapshot, line_color, aux_line_color, marker_color, aux_marker_color, title, 
                   axis_ticks, figure_azimuth, figure_elevation, figure_distance, surface_linewidth, flist_sorted, mode_ch, proj_ch, 
                   line_style, surface_cmap]
    
    return save_object


def get_csv_header(csv_f):
    '''Gets header from given csv file.'''
    
    csv_file = open(csv_f, 'rb')   
    reader = csv.reader(csv_file)
    header = reader.next()
    csv_file.close()

    if csv_f.endswith('.data'):
        header = header[:-1]

    return header  


def get_csv_data(csv_f, header):
    '''Gets data and time codes from given csv file.'''
    time_stamps = []
    data = None
    count = 0
    csv_file = open(csv_f, 'rb')
    csv_file2 = open(csv_f, 'rb')
    
    reader = csv.reader(csv_file)
            
    row_count = len(list(csv_file2))
    csv_file2.close()
    
    # skip header
    next(reader, None)
    
    # Progress bar
    dlg = progress_dlg(None, -1, 'Loading data', '', row_count)
    
    # build time_stamps list and data matrix
    #========================================================================================================================
    for row in reader:
        
        # Update time_stamps list
        time_stamps.append(row[0])
        row.pop(0)
        
        # Matrix generation
        if data is None:
            data = np.zeros((row_count-1, len(row)))
        
        # Fill current data row and update progress bar
        data[count] = row
        count += 1
        dlg.Update(count)
    #========================================================================================================================
                           
    dlg.Hide()
    wx.CallAfter(dlg.Destroy)
    csv_file.close()
                                                    
    return data, time_stamps


def get_dimension(header):
    '''Return the dimensionality of the data belonging to the given header.'''
    
    ndim = len(header[2:])
            
    for i in range(1, len(header[2:])+1):
        
        if int(header[-i]) == 1:
            ndim -= 1
        elif int(header[-i]) != 1:
            break
       
    return ndim


def sort_alphnum(unsorted):
    '''Sort given list alphanumerically.'''
    conv = lambda text: int(text) if text.isdigit() else text
    alphnum_key = lambda key: [conv(c) for c in re.split('([0-9]+)', key)]
    
    return sorted(unsorted, key=alphnum_key)


def load_plot_configuration(parent):
    
    dlg = rdp.dlg.load_config_dlg(parent, parent.dir)
    
    if dlg.ShowModal() == wx.ID_CANCEL:
        return
    
    file_path = dlg.GetPath()
    loaded_object = pickle.load(open(file_path, 'rb'))
            
    aux_frame = parent
    
    while aux_frame.GetParent() != None:
        aux_frame = aux_frame.GetParent()

    aux_frame = aux_frame.GetChildren()[1]
            
    parent.dir = loaded_object[1]
    parent.x_label = loaded_object[2] 
    parent.y_label = loaded_object[3]
    parent.z_label = loaded_object[4]  
    parent.marked = loaded_object[5]  
    parent.step = loaded_object[6]  
    parent.slider_max = loaded_object[7]  
    parent.stride = loaded_object[8]  
    parent.vmax = loaded_object[9]  
    parent.vmin = loaded_object[10]  
    parent.proj = loaded_object[11]  
    parent.proj_method = loaded_object[12]  
    parent.style = loaded_object[13]  
    parent.mode = loaded_object[14]  
    parent.labelling_mode = loaded_object[15]  
    parent.reduced_data = loaded_object[16]  
    parent.header = loaded_object[17]  
    parent.ndim = loaded_object[18]  
    parent.selection = loaded_object[19] 
    parent.sel_cbox_selection = loaded_object[20]
    parent.proj_choice_time_course = loaded_object[21]  
    parent.proj_choice_snapshot = loaded_object[22]  
    parent.line_color = loaded_object[23]  
    parent.aux_line_color = loaded_object[24]  
    parent.marker_color = loaded_object[25] 
    parent.aux_marker_color = loaded_object[26] 
    parent.title = loaded_object[27]  
    parent.axis_ticks = loaded_object[28]  
    parent.figure_azimuth = loaded_object[29]  
    parent.figure_elevation = loaded_object[30]  
    parent.figure_distance = loaded_object[31]  
    parent.surface_linewidth = loaded_object[32]  
    parent.flist_sorted = loaded_object[33] 
    parent.mode_ch = loaded_object[34]
    parent.proj_ch = loaded_object[35]
        
    if loaded_object[0] == 537:
        parent.line_style = loaded_object[36]
        
    elif loaded_object[0] == 691:
        parent.line_style = loaded_object[36]
        parent.surface_cmap = loaded_object[37]
                    
    else:
        dlg = wx.MessageDialog(parent = None, 
                   message = 'RDP does not recognize the internal structure of the chosen file.', 
                   caption = 'The chosen file cannot be read.', 
                   style = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
        
        dlg.ShowModal()
        dlg.Hide()
        wx.CallAfter(dlg.Destroy)
        return
    
    parent.data, parent.time_stamps = rdp.datatools.get_csv_data(csv_f=parent.dir + '/' + parent.flist_sorted[parent.selection], header=parent.header) 
            
    try:
        aux_frame.pos_slider.SetMax(parent.slider_max)
        aux_frame.pos_slider.SetValue(parent.step)
        aux_frame.time_stamp_display.SetLabel(str(parent.time_stamps[parent.step]))
    except AttributeError:
        pass
    
    parent.rdp_frame = recorded_data_processor.RDPSetupFrame(parent=parent, title=parent.sel_cbox_selection)
    parent.rdp_frame.frame.proj_cbox.SetItems(parent.proj_ch)
    parent.rdp_frame.frame.proj_cbox.SetValue(str(parent.proj))
    parent.rdp_frame.frame.proj_method_cbox.SetValue(parent.proj_method)
    parent.rdp_frame.frame.mode_cbox.SetItems(parent.mode_ch)
    parent.rdp_frame.frame.mode_cbox.SetValue(parent.mode)
    parent.rdp_frame.frame.style_cbox.SetItems(parent.style_ch)
    parent.rdp_frame.frame.style_cbox.SetValue(parent.style)
    
    parent.rdp_frame.frame.mode_cbox.Enable()
    parent.rdp_frame.frame.style_cbox.Enable()
    parent.rdp_frame.frame.proj_cbox.Enable()
    parent.rdp_frame.frame.proj_method_cbox.Enable()
    parent.rdp_frame.frame.panel.Hide()
    recorded_data_processor.RDPPlotFrame(parent=parent, panel=parent.rdp_frame.frame.panel)
    rdp.plotfuncs.update_plot(parent=parent)
