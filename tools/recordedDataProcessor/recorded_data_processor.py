#!/usr/bin/python

'''
========================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        recorded_data_processor.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 23

    Description: 

    Credits:

========================================================================================================================
'''

import matplotlib as mpl

mpl.use('WXAgg')

import matplotlib.pyplot as plt
import numpy as np
import os

try:
    import cPickle as pickle
except ImportError:
    import pickle

import rdp.plotfuncs
import rdp.images

import sys
import warnings

warnings.filterwarnings('ignore', category=UserWarning)

import wx

from functools import partial
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas
from matplotlib.ticker import ScalarFormatter 
from mpl_toolkits.mplot3d import Axes3D

# Prevent Eclipse warning from Axes3D import
Axes3D

#========================================================================================================================

class RDPGUIEvents():
    
    def evt_ok_btn(self, step_num_entry, step_size_entry, event, parent):
        
        great_parent = parent.parent
        # Get number of steps as well as step size from widgets
        great_parent.nstep = int(step_num_entry.GetValue())
        great_parent.step_size = int(step_size_entry.GetValue())

        great_parent.plot, parent.reduced_data = rdp.plotfuncs.plot_snapshot_sequence(start = great_parent.step, 
                                                                                                    step_size = great_parent.step_size, 
                                                                                                    steps = great_parent.nstep, 
                                                                                                    style = great_parent.style, 
                                                                                                    data = great_parent.data, 
                                                                                                    header = great_parent.header, 
                                                                                                    vmin = great_parent.vmin,
                                                                                                    vmax = great_parent.vmax,
                                                                                                    resolution = great_parent.resolution,
                                                                                                    surface_linewidth = great_parent.surface_linewidth,
                                                                                                    proj = great_parent.proj,
                                                                                                    linestyle = great_parent.line_style,
                                                                                                    proj_method = great_parent.proj_method,
                                                                                                    x_label = great_parent.x_label,
                                                                                                    y_label = great_parent.y_label,
                                                                                                    z_label = great_parent.z_label,
                                                                                                    file_name = great_parent.flist_sorted[great_parent.selection],
                                                                                                    file_directory = great_parent.dir,
                                                                                                    save_mode = great_parent.save_mode,
                                                                                                    color = great_parent.line_color,
                                                                                                    figure = None)
        
        # Close dialog
        parent.Hide()
        mpl.interactive(False)
        wx.CallAfter(parent.Destroy)

        
    def evt_cancel_button(self, event, parent):
        #Close dialog
        parent.Hide()
        wx.CallAfter(parent.Destroy)
    
    def evt_save_config_btn(self, event, parent):
        dlg = rdp.dlg.save_config_dlg(parent=parent, defaulDir=parent.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        save_object = parent.prepare_plot_configuration(directory=parent.dir,
                                                        x_label=parent.x_label,
                                                        y_label=parent.y_label, 
                                                        z_label=parent.z_label, 
                                                        marked=parent.marked, 
                                                        step=parent.step, 
                                                        slider_max=parent.slider_max, 
                                                        resolution=parent.resolution, 
                                                        vmax=parent.vmax, 
                                                        vmin=parent.vmin, 
                                                        proj=parent.proj, 
                                                        proj_method=parent.proj_method, 
                                                        style=parent.style, 
                                                        mode=parent.mode, 
                                                        labelling_mode=parent.labelling_mode, 
                                                        reduced_data=parent.reduced_data, 
                                                        header=parent.header, 
                                                        ndim=parent.ndim, 
                                                        selection=parent.selection,
                                                        sel_cbox_selection=parent.sel_cbox_selection,
                                                        proj_choice_time_course=parent.proj_choice_time_course, 
                                                        proj_choice_snapshot=parent.proj_choice_snapshot, 
                                                        line_color=parent.line_color, 
                                                        aux_line_color=parent.aux_line_color, 
                                                        marker_color=parent.marker_color, 
                                                        aux_marker_color=parent.aux_marker_color,
                                                        title=parent.title, 
                                                        axis_ticks=parent.axis_ticks, 
                                                        figure_azimuth=parent.figure_azimuth, 
                                                        figure_elevation=parent.figure_elevation, 
                                                        figure_distance=parent.figure_distance, 
                                                        surface_linewidth=parent.surface_linewidth,
                                                        flist_sorted=parent.flist_sorted,
                                                        mode_ch = parent.rdp_frame.frame.mode_cbox.GetItems(),
                                                        proj_ch=parent.rdp_frame.frame.proj_cbox.GetItems())
                
        file_path = dlg.GetPath()

        pickle.dump(save_object, open(file_path, 'wb'), 0)
        
        
    def evt_elevation_spn(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        parent.figure_elevation = value
        parent.figure.gca().elev = parent.figure_elevation
           
        wx.CallAfter(parent._update_plot)
        
        
    def evt_line_style_cbox(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        
        great_parent = parent.parent
        
        great_parent.line_style = value
        
        wx.CallAfter(great_parent._update_plot)
        
        
    def evt_azimuth_spn(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        parent.figure_azimuth = value      
        parent.figure.gca().azim = parent.figure_azimuth
        
        wx.CallAfter(parent._update_plot)
        

    def evt_distance_spn(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        parent.figure_distance = value        
                
        wx.CallAfter(parent._update_plot)
        
    
    def evt_close_figure(self, event, parent):
        
        parent.figure_canvas.mpl_disconnect(parent.figure_canvas_connection_id)
        parent.figure_canvas_connection_id = None
        parent.figure.clf()
        parent.plot = None
        parent.figure = None
        parent.colorbar = None
        parent.figure_canvas = None
        parent.control_plot_frame.Hide()
        parent.control_plot_frame = None
        
    
    def evt_add_figure_frame_btn(self, event, parent):
                        
        new_frame = RDPMainWindow(parent=parent.frame, title='', pos=None, size=None, style=None, name=None, directory=parent.dir)     
        new_frame.rdp_gui.step = parent.step   
        new_frame.Bind(wx.EVT_CLOSE, partial(self.evt_close_frame, parent=parent))
        
        new_frame_id = new_frame.GetId()
        parent.frame_ids.append(new_frame_id)
        
        wx.FindWindowById(new_frame_id).step = parent.step
        
     
    def evt_close_frame(self, event, parent):
        
        frame = event.GetEventObject()
        frame_id = frame.GetId()
        
        if frame_id in parent.frame_ids:
            parent.frame_ids.remove(frame_id)
        
        wx.FindWindowById(frame_id).Hide()
    
                            
    def evt_play_pause_btn(self, event, parent):
        parent._play_pause_btn(reverse=False)
        

    def evt_reverse_play_pause_btn(self, event, parent):
        parent._play_pause_btn(reverse=True)
        
            
    def evt_reset_btn(self, event, parent):
        '''Reset plot to default.'''
        
        for i in range(len(parent.frame_ids)):
            current_frame = parent._set_current_frame(i)
            current_frame.step = 0
            current_frame.nstep = 0
            current_frame.step_size = 0
            current_frame.save_mode = False
            
            if i == 0:
                current_frame.pos_slider.SetValue(current_frame.step)
                current_frame.time_stamp_display.SetLabel('')
                    
            if current_frame.control_plot_frame:
                
                try:
                    current_frame.control_plot_frame.resolution_spn.SetValue(current_frame.resolution)
                except AttributeError:
                    
                    pass
                try:
                    current_frame.control_plot_frame.linewidth_spn.SetValue(current_frame.surface_linewidth)
                except AttributeError:
                    pass
            
            current_frame._update_plot()
            wx.Yield()  

    
    def evt_increase_single_step_btn(self, event, parent):
        parent._move_single_step(increase=True)
        wx.Yield()
                    
    
    def evt_decrease_single_step_btn(self, event, parent):
        parent._move_single_step(increase=False)
        wx.Yield()
        
    
    def evt_axis_label(self, event, x_axis_label, y_axis_label, parent, z_axis_label=None):
        
        parent.x_label = x_axis_label.GetValue()
        parent.y_label = y_axis_label.GetValue()
        
        if z_axis_label != None:
            try:
                parent.z_label = z_axis_label.GetValue()
            except AttributeError:
                pass
                    
        wx.CallAfter(parent._update_plot)
        
    
    def evt_add_time_course(self, event, parent):
        '''Add time course to currently displayed plot.'''
        
        parent._add_time_course()
        
        
    def evt_resolution_spn(self, event, parent):
        
        parent.resolution = int(parent.control_plot_frame.resolution_spn.GetValue())
        wx.CallAfter(parent._update_plot)
        

    def evt_vmax_spn(self, event, parent):
        
        parent.vmax = parent.control_plot_frame.vmax_spn.GetValue()
        wx.CallAfter(parent._update_plot)

    
    def evt_vmin_spn(self, event, parent):
        
        parent.vmin = parent.control_plot_frame.vmin_spn.GetValue()
        wx.CallAfter(parent._update_plot)
        
    
    def evt_proj_cbox(self, event, parent):
        
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.proj = widget.GetValue()
        widget = great_parent.color_combobox_background(value=great_parent.proj, widget=widget)
        
        if ',' not in parent.parent.proj and parent.parent.mode != 'time course':
            parent.style_cbox.Disable()
            parent.proj_method_cbox.Disable()
            
            if 'wxMac' in wx.PlatformInfo:
                parent.style_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                parent.proj_method_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                
            else:
                parent.style_cbox.SetBackgroundColour('#FFFFFF')
                parent.proj_method_cbox.SetBackgroundColour('#FFFFFF')
            
            parent.style_cbox.SetValue('')
            parent.proj_method_cbox.SetValue('')
            
            if great_parent.proj != '':
                
                if great_parent.mode != 'snapshot sequence':
                    parent.panel.Hide()
                    RDPPlotFrame(parent=great_parent, panel=parent.panel) 
                
                if great_parent.style != 'heatmap' and great_parent.ndim[great_parent.selection] != 0:
                    try:    
                        great_parent._plot()
                    except AttributeError:
                        pass
                
            great_parent._update_plot()
            
        else:
            parent.style_cbox.Enable()
            parent.proj_method_cbox.Enable()
            
        if great_parent.proj == '':
            great_parent.proj_method = ''
            great_parent.style = ''
            
            parent.proj_method_cbox.Disable()
            parent.style_cbox.Disable()
            
            if 'wxMac' in wx.PlatformInfo:
                parent.proj_method_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                parent.style_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
            else:
                parent.proj_method_cbox.SetBackgroundColour('#FFFFFF')
                parent.style_cbox.SetBackgroundColour('#FFFFFF')
            
            parent.proj_method_cbox.SetValue(great_parent.proj_method)
            parent.style_cbox.SetValue(great_parent.style)
            
        
    def evt_reset_heatmap_boundaries_btn(self, event, parent):
        
        parent.vmin = parent.vmax = None
        
        parent.control_plot_frame.vmin_spn.SetValue(0.0000)
        parent.control_plot_frame.vmax_spn.SetValue(0.0000)
        
        wx.CallAfter(parent._update_plot)

    
    def evt_switch_btn(self, event, parent):  
        for i in range(len(parent.frame_ids)):
            current_frame = parent._set_current_frame(i)
            
            # Clear memory
            if current_frame.data is not None:
                del current_frame.data
                current_frame.data = None
            
            # Close open plots              
            if current_frame.figure:
                plt.close(current_frame.figure)
                current_frame.figure = None
                        
        frame = parent.panel.GetParent()
        browser_panel = RDPBrowserPanel(parent=frame)
        
        # Replace plot generation frame with file browser
        frame.SetSizer(browser_panel.main_sizer)
        
        # Layout
        wx.CallAfter(frame.Sizer.Fit, frame)
        wx.CallAfter(frame.Layout)

                    
    def evt_pos_slider(self, event, parent):
        
        step = parent.pos_slider.GetValue()
                
        for i in range(len(parent.frame_ids)):
            current_frame = parent._set_current_frame(i)
            current_frame.step = step
        
            if i == 0:
                # Set time code
                current_time_stamp = parent.time_stamps[current_frame.step]
                current_frame.time_stamp_display.SetLabel(current_time_stamp)
                
            # Update currently displayed plot
            current_frame._update_plot()
            wx.Yield()
        
                
    def evt_linewidth_spn(self, event, parent):
        widget = event.GetEventObject()
        parent.surface_linewidth = float(widget.GetValue() / 20.)
        wx.CallAfter(parent._update_plot)
        
        
    def evt_sel_cbox(self, event, parent):
        widget = event.GetEventObject()
        
        if 'RDPGUI' not in str(type(parent)):
            parent = parent.GetParent()
        
        parent.selection = widget.GetSelection()
        parent.sel_cbox_selection = widget.GetValue()
        parent.title = parent.sel_cbox_selection
        
        
        parent.rdp_frame = RDPSetupFrame(parent=parent, title=parent.sel_cbox_selection)
        parent._update_selection_data(parent.rdp_frame.frame)
        parent.rdp_frame.frame.panel.Hide()
        RDPPlotFrame(parent=parent, panel=parent.rdp_frame.frame.panel)
                
        # Case 1: selected file is an image
        if 'CV_8U' in parent.header_list[parent.selection] or 'CV_8UC3' in parent.header_list[parent.selection]:
            parent.rdp_frame.frame.style_ch = ['image']
            parent.rdp_frame.frame.style_cbox.Enable()
            parent.rdp_frame.frame.proj_cbox.Disable()
            parent.rdp_frame.frame.proj_method_cbox.Disable()
            
            if 'wxMac' in wx.PlatformInfo:
                parent.rdp_frame.frame.proj_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                parent.rdp_frame.frame.proj_method_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
            else:
                parent.rdp_frame.frame.proj_cbox.SetBackgroundColour('#FFFFFF')
                parent.rdp_frame.frame.proj_method_cbox.SetBackgroundColour('#FFFFFF')
            
            parent.rdp_frame.frame.style_cbox.SetValue('image')
            parent.rdp_frame.frame.style_cbox.SetSelection(0)
            parent.style = 'image'
            
            parent.rdp_frame.frame.panel.Hide()
            RDPPlotFrame(parent=parent, panel=parent.rdp_frame.frame.panel) 
            parent.rdp_frame.frame.Hide()
            parent._plot()
          
        else:
            parent.style_ch = ['', 'heatmap', 'surface', 'wireframe']
            parent.rdp_frame.frame.style_cbox.SetItems(parent.style_ch)
            parent.rdp_frame.frame.mode_cbox.Enable()
            
            # Initialize plot generation GUI      
            parent.rdp_frame.frame.style_cbox.SetItems(parent.style_ch)
        
                          
    def evt_save_csv_btn(self, event, parent):
                
        dlg_name = parent.dir + '/' + parent.flist_sorted[parent.selection].strip('.csv') + '-' + str(parent.mode) + '-1.csv'
        dlg = rdp.dlg.save_csv_dlg(parent, parent.dir, dlg_name)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        file_path = dlg.GetPath()
        np.savetxt(fname=file_path, X=parent.reduced_data, delimiter=',')
        

    def evt_save_plot_btn(self, event, parent):
        
        dlg = rdp.dlg.save_dlg(parent=parent, defaultDir=parent.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        file_path = dlg.GetPath()
                        
        if parent.mode != 'snapshot sequence':
            parent._plot(save=True, file_path=file_path)
            
            
    def evt_dir_btn(self, event, parent):
        
        RDPMainWindow(parent=None, title='Recorded Data Processor', pos=None, size=None, style=None, name=None, directory=parent.frame.dir)
        parent.frame.Destroy()
        
        
    def evt_load_config_btn(self, event, parent):
        
        try:
            rdp_gui = parent._initialize_rdp_gui()
        except AttributeError:
            new_frame = RDPMainWindow(parent=parent.frame, title='', pos=None, size=None, style=None, name=None, directory=parent.dir)   
            new_frame.rdp_gui.step = parent.step   
            new_frame.Bind(wx.EVT_CLOSE, partial(self.evt_close_frame, parent=parent))
        
            new_frame_id = new_frame.GetId()
            parent.frame_ids.append(new_frame_id)
            rdp_gui = new_frame.rdp_gui
            
        rdp_gui._load_plot_configuration()

        
    def evt_end_app(self, event):                
        sys.exit()
        
        
    def evt_sel_btn(self, event, parent):
        parent._initialize_rdp_gui()
        
        
    def evt_line_color_ctrl(self, event, parent):
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.aux_line_color = widget.GetColour()
        widget.SetColour(great_parent.aux_line_color)
        
        # Color conversion
        red = great_parent.aux_line_color.Red()/255.
        green = great_parent.aux_line_color.Green()/255.
        blue = great_parent.aux_line_color.Blue()/255.
        
        great_parent.line_color = [red, green, blue]
        
        # Update currently displayed plot
        if great_parent.figure and great_parent.ndim[great_parent.selection] != 0:
            wx.CallAfter(great_parent._update_plot)
            
            
    def evt_marker_color_ctrl(self, event, parent):
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.aux_marker_color = widget.GetColour()
        widget.SetColour(great_parent.aux_marker_color)
        
        # Color conversion
        red = great_parent.aux_marker_color.Red()/255.
        green = great_parent.aux_marker_color.Green()/255.
        blue = great_parent.aux_marker_color.Blue()/255.
        great_parent.marker_color = [red, green, blue]
        
        # Update currently displayed plot
        if great_parent.figure:
            wx.CallAfter(great_parent._update_plot)
    
    
    def evt_marked_check_box(self, event, parent):
        
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.marked = widget.GetValue()
        wx.CallAfter(great_parent._update_plot)
    
    
    def evt_plot_labelling_cbox(self, event, parent):
        
        widget = event.GetEventObject()
        parent.labelling_mode = widget.GetValue()
        
        parent._update_plot()
        parent._update_plot()
            
        
    def evt_axis_ticks_cbox(self, event, parent):
        
        widget = event.GetEventObject()
                
        if widget.GetValue() == 'on':
            parent.axis_ticks = True
        else:
            parent.axis_ticks = False
            
        parent._update_plot()
        parent._update_plot()
        
    
    def evt_mode_cbox(self, event, parent):
        
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.mode = widget.GetValue()
        widget = great_parent.color_combobox_background(value=great_parent.mode, widget=widget)

        '''
        if great_parent.figure:
            plt.close(great_parent.figure)
        '''
                    
        #else:
        if not great_parent.figure:
            great_parent.proj_method = ''
            great_parent.style = ''
            great_parent.proj = ''
            
            parent.proj_method_cbox.Disable()
            parent.style_cbox.Disable()
            
            if 'wxMac' in wx.PlatformInfo:
                parent.style_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                parent.proj_method_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
            else:
                parent.style_cbox.SetBackgroundColour('#FFFFFF')
                parent.proj_method_cbox.SetBackgroundColour('#FFFFFF')
            
            parent.proj_cbox.SetValue(great_parent.proj)
            parent.proj_method_cbox.SetValue(great_parent.proj_method)
            parent.style_cbox.SetValue(great_parent.style)
            
        if great_parent.ndim[great_parent.selection] == 0:    
                    
            if great_parent.mode != 'snapshot sequence':
                parent.panel.Hide()
                RDPPlotFrame(parent=great_parent, panel=parent.panel) 
                great_parent.rdp_frame.frame.Hide()
            
            great_parent._update_plot()
        
        # Fill projection combobox with the fitting options
        if great_parent.mode == 'time course':
            parent.proj_cbox.SetItems(great_parent.proj_choice_time_course)
            parent.proj_cbox.Enable()
        elif great_parent.mode == 'snapshot' or great_parent.mode == 'snapshot sequence':
            parent.proj_cbox.SetItems(great_parent.proj_choice_snapshot)
            parent.proj_cbox.Enable()
        else:
            parent.proj_cbox.SetItems([])
            
           
    def evt_proj_method_cbox(self, event, parent):
        
        widget = event.GetEventObject()
        great_parent = parent.parent
        great_parent.proj_method = widget.GetValue()
        widget = great_parent.color_combobox_background(value=great_parent.proj_method, widget=widget)
                            
        if great_parent.control_plot_frame:
            great_parent._update_plot()
            
            
    def evt_style_cbox(self, event, parent):
        
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.style = widget.GetValue()
        widget = great_parent.color_combobox_background(value=great_parent.style, widget=widget)
        
        if great_parent.style != 'image':
            parent.mode_cbox.Enable()
            parent.proj_cbox.Enable()
            parent.proj_method_cbox.Enable()
            
        else:
            great_parent.mode = parent.mode_cbox.GetValue()
            parent.mode_cbox.Disable()
            parent.proj_cbox.Disable()
            parent.proj_method_cbox.Disable()
            
            if 'wxMac' in wx.PlatformInfo:
                parent.proj_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                parent.proj_method_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
                parent.mode_cbox.GetChildren()[0].SetBackgroundColour('#FFFFFF')
            else:
                parent.proj_cbox.SetBackgroundColour('#FFFFFF')
                parent.proj_method_cbox.SetBackgroundColour('#FFFFFF')
                parent.mode_cbox.SetBackgroundColour('#FFFFFF')
            
        if great_parent.mode != 'snapshot sequence':
            parent.panel.Hide()
            RDPPlotFrame(parent=great_parent, panel=parent.panel) 
            great_parent._update_plot()
            
            
    def evt_resize_control_plot_panel(self, event, parent):
        
        pix = parent.control_plot_frame.control_plot_panel.GetClientSize()
        
        parent.figure.set_size_inches(pix[0]/parent.figure.get_dpi(),
                                    pix[1]/parent.figure.get_dpi())
        
        
        x,y = parent.control_plot_frame.GetSize()  
        
        parent.figure.set_size_inches(x/parent.figure.get_dpi(),
                                    y/parent.figure.get_dpi())
        
        parent.figure_canvas.SetSize((x-1, y-1))
        parent.figure_canvas.SetSize((x, y))
                        
        event.Skip()
                
                                   
    def evt_resize_canvas(self, event, parent):
        parent.figure_canvas.draw()
        
        
    def evt_change_plot_parameters(self, event, parent):
        
        parent.figure = plt.figure(str(parent.title))
        parent.plot = parent.figure.gca()
        
        if parent.style == 'surface' or parent.style == 'wireframe':
            parent.figure_azimuth = int(parent.figure.gca(projection='3d').azim)
            parent.figure_elevation = int(parent.figure.gca(projection='3d').elev)
        
        try:
            parent.control_plot_frame.azimuth_spn.SetValue(parent.figure_azimuth)
            parent.control_plot_frame.elevation_spn.SetValue(parent.figure_elevation)
        except AttributeError:
            pass    
        
#========================================================================================================================

class RDPStartupFrame(wx.Frame):
    def __init__(self, parent):
        
        rdp_gui_events = RDPGUIEvents()
        
        title = 'Recorded Data Processor'
                
        self.frame = wx.Frame(parent=None, title=title)
        self.frame.parent = parent
        self.panel = wx.Panel(parent=self.frame)
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        
        # search for standard cedarRecordings directory
        if 'HOME' in os.environ:
            self.frame.dir = os.environ['HOME']
        elif os.name == 'posix':
            self.frame.dir = os.path.expanduser("~/")
        elif os.name == 'nt':
            if 'HOMEPATH' in os.environ and 'HOMEDRIVE' in os.environ:
                self.frame.dir = os.environ['HOMEDRIVE'] + os.environ['HOMEPATH']
        else:
            self.frame.dir = os.environ['HOMEPATH']
        
        # Walk through directories until 'cedarRecordings' is found
        for (self.frame.dir, dirs, _) in os.walk(self.frame.dir):
            for j in range(len(dirs)):
                if dirs[j] == 'cedarRecordings':
                    new_dir = dirs[j]
                    self.frame.dir = os.path.join(self.frame.dir, new_dir)
                    break
            
            break
                
        dir_btn = wx.Button(parent=self.panel, id=-1, label='Please choose a directory containing cedar recordings')
        choose_txt2 = wx.StaticText(self.panel, -1, 'OR')
        load_btn = wx.Button(parent=self.panel, id=-1, label='load a saved plot configuration')
        
        dir_btn.Bind(wx.EVT_BUTTON, partial(rdp_gui_events.evt_dir_btn, parent=self))
        load_btn.Bind(wx.EVT_BUTTON, partial(rdp_gui_events.evt_load_config_btn, parent=self))
        
        self.main_sizer.Add(item=dir_btn, flag=wx.ALIGN_CENTER|wx.ALL, border=25)
        self.main_sizer.Add(item=choose_txt2, flag=wx.ALIGN_CENTER|wx.RIGHT|wx.LEFT, border=25)
        self.main_sizer.Add(item=load_btn, flag=wx.ALIGN_CENTER|wx.ALL, border=25)
        
        self.main_sizer.Fit(self.panel)
        self.frame.SetSizerAndFit(self.main_sizer)
        self.frame.CenterOnScreen()
        self.frame.Show()
        
        return
    
        
    def _initialize_rdp_gui(self):
        
        # Initialize plot generation GUI      
        rdp_gui = RDPGUI(parent=self.frame)
        self.frame.SetSizer(rdp_gui.main_sizer)
        self.frame.Sizer.Fit(self.frame)
        
        RDPMainWindow(parent=None, title='Recorded Data Processor', pos=None, size=None, style=None, name=None, directory=self.frame.dir)
        wx.CallAfter(self.frame.Hide)
        
        return rdp_gui
    
#========================================================================================================================

class RDPApp(wx.App):
    def OnInit(self):        
        RDPStartupFrame(parent=None)
                        
        return True
            
#========================================================================================================================

class RDPMainWindow(wx.Frame):
    def __init__(self, parent, title, pos, size, style, name, directory):
         
        self.rdp_gui_events = RDPGUIEvents()
        self.parent = parent
        self.dir = directory
                
        # initialize first frame
        if self.parent is None:
            wx.Frame.__init__(self, parent=None, title=title, style=wx.MINIMIZE_BOX|wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN)  
            self.rdp_browser = RDPBrowserPanel(parent=self)
            self.SetSizer(self.rdp_browser.main_sizer)
            self.Sizer.Fit(self)
            self.Show()
        
        # initialize frame for multi-plot purposes
        else:
            wx.Frame.__init__(self, parent=parent, title='', style=wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.FRAME_NO_TASKBAR|wx.CLIP_CHILDREN)
            self.dir = self.parent.dir  
            self.parent = parent
            self.rdp_gui = RDPGUI(parent=self)
            self.SetSizer(self.rdp_gui.main_sizer)
            self.Sizer.Fit(self)
            
            parent.rdp_browser.rdp_gui.sel_cbox.Reparent(self.rdp_gui)
            self.rdp_gui.sel_cbox.Reparent(parent.rdp_browser.rdp_gui)
            
        self.Bind(wx.EVT_CLOSE, self.rdp_gui_events.evt_end_app)
                        
        return
        
#========================================================================================================================

class RDPBrowserPanel(wx.Panel):
    def __init__(self, parent):
        
        rdp_gui_events = RDPGUIEvents()
        wx.Panel.__init__(self, parent)
        self.frame = parent
                
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.select_directory = wx.StaticText(self, -1, 'Select directory:')
        
        # File browser
        self.browser = wx.GenericDirCtrl(parent=self, dir=self.frame.dir, size=(325, 450))            
        self.sel_btn = wx.Button(parent=self, label = 'Select')
        self.load_config_btn = wx.Button(parent=self, label = 'Load configuration')
        self.sel_btn.Bind(wx.EVT_BUTTON, partial(rdp_gui_events.evt_sel_btn, parent=self))
        self.load_config_btn.Bind(wx.EVT_BUTTON, partial(rdp_gui_events.evt_load_config_btn, parent=self))
        
        # Main sizer
        self.main_sizer.Add(item=self.select_directory, proportion=0, flag=wx.TOP|wx.LEFT|wx.BOTTOM, border=10)
        self.main_sizer.Add(item=self.browser, proportion=0, flag=wx.LEFT|wx.BOTTOM|wx.RIGHT|wx.EXPAND, border=10)
        self.btn_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.btn_sizer.Add(item=self.sel_btn, proportion=0)
        self.btn_sizer.Add(item=self.load_config_btn, proportion=0)
        self.main_sizer.Add(item=self.btn_sizer, proportion=0, flag=wx.LEFT|wx.BOTTOM, border=10)
        
        # Layout
        self.SetSizer(self.main_sizer)
        self.Sizer.Fit(self)
        self.Fit()
        
        return
    
    
    def _initialize_rdp_gui(self):
        frame = self.GetParent()
        frame.dir = self.browser.GetPath()
        
        # Initialize plot generation GUI      
        self.rdp_gui = RDPGUI(parent=frame)
        frame.SetSizer(self.rdp_gui.main_sizer)
        frame.Sizer.Fit(frame)
        #wx.CallAfter(self.Hide)
        
        return self.rdp_gui
    
#========================================================================================================================

class RDPPlotFrame(wx.Frame):
    def __init__(self, parent, panel):
        
        self.parent = parent
        self.rdp_gui_events = RDPGUIEvents()
        
        # Initialize control plot frame
        if parent.control_plot_frame == None:
            parent.control_plot_frame = wx.Frame(parent=parent, id=-1, title=str(parent.title), style=wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN|wx.FRAME_NO_TASKBAR)
            parent.control_plot_frame.Bind(wx.EVT_CLOSE, partial(self.rdp_gui_events.evt_close_figure, parent=parent))
            
        parent.control_plot_frame.control_plot_panel = wx.Panel(parent=parent.control_plot_frame)
        self.notebook = wx.Notebook(parent.control_plot_frame.control_plot_panel)
        
        notebook_setup_panel = wx.Panel(parent=self.notebook)
        notebook_parameters_panel = wx.Panel(parent=self.notebook)
        notebook_export_panel = wx.Panel(parent=self.notebook)
        
        panel.Reparent(notebook_setup_panel)
        panel.Show()
        
        notebook_setup_panel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        notebook_setup_panel_sizer.Add(item=panel, proportion=1, flag=wx.ALIGN_LEFT|wx.RIGHT, border=10)
        
        notebook_parameters_panel_sizer = wx.FlexGridSizer(cols=2)
        
        notebook_export_panel_sizer = wx.BoxSizer(wx.HORIZONTAL)
                
        save_btn = wx.Button(notebook_export_panel, id=wx.ID_SAVE)
        save_csv_btn = wx.Button(notebook_export_panel, label = 'Save .csv')
        save_config_btn = wx.Button(notebook_export_panel, label = 'Save plot configuration')
        
        save_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_save_plot_btn, parent=parent))
        save_csv_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_save_csv_btn, parent=parent))
        save_config_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_save_config_btn, parent=parent))
        
        notebook_export_panel_sizer.Add(item=save_btn, proportion=0, flag=wx.ALIGN_BOTTOM|wx.BOTTOM|wx.LEFT, border=10)
        notebook_export_panel_sizer.Add(item=save_csv_btn, proportion=0, flag=wx.ALIGN_BOTTOM|wx.BOTTOM, border=10)
        notebook_export_panel_sizer.Add(item=save_config_btn, proportion=0, flag=wx.ALIGN_BOTTOM|wx.BOTTOM|wx.RIGHT, border=10)
        
        # Sizers
        #========================================================================================================================
        parent.control_plot_frame.main_sizer = wx.FlexGridSizer(cols=2)
        #========================================================================================================================
        
        # Labels
        #========================================================================================================================
        axes_heading = wx.StaticText(notebook_parameters_panel, -1, 'Axes labelling')
        axes_heading.SetFont(wx.Font(pointSize=10, family=wx.DEFAULT, style=wx.NORMAL, weight=wx.NORMAL))  
        x_axis_txt = wx.StaticText(notebook_parameters_panel, -1, 'X axis')
        y_axis_txt = wx.StaticText(notebook_parameters_panel, -1, 'Y axis')
                
        if parent.style != 'heatmap' and (parent.style == 'surface' or parent.style == 'wireframe'):
            z_axis_txt = wx.StaticText(notebook_parameters_panel, -1, 'Z axis')
        elif parent.style == 'heatmap':
            z_axis_txt = wx.StaticText(notebook_parameters_panel, -1, 'color bar')
        #========================================================================================================================
        
        x_axis_label = wx.TextCtrl(notebook_parameters_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        y_axis_label = wx.TextCtrl(notebook_parameters_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        z_axis_label = None 
        
        # Build sizers
        #========================================================================================================================
        
        notebook_parameters_panel_sizer.Add(axes_heading, 1, wx.ALIGN_LEFT|wx.ALL, border=10)
        notebook_parameters_panel_sizer.AddSpacer(5)
        notebook_parameters_panel_sizer.Add(x_axis_txt, 1, wx.ALIGN_LEFT|wx.LEFT, border=20)
        notebook_parameters_panel_sizer.Add(x_axis_label, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
        notebook_parameters_panel_sizer.Add(y_axis_txt, 1, wx.ALIGN_LEFT|wx.LEFT, border=20)
        notebook_parameters_panel_sizer.Add(y_axis_label, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
        
        # Plot is either 3-dimensional or a heatmap
        if parent.style == 'surface' or parent.style == 'wireframe' or parent.style == 'heatmap':
            z_axis_label = wx.TextCtrl(notebook_parameters_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
            notebook_parameters_panel_sizer.Add(z_axis_txt, 0, wx.ALIGN_LEFT|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(z_axis_label, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            z_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.rdp_gui_events.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label, parent=parent))
                                                        

        x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.rdp_gui_events.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label, parent=parent))
        y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.rdp_gui_events.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label, parent=parent))
            
        plot_labelling_tooltipstring = ('Choose the style in which the plot is to be labelled.\n'
                                        '=====================================\n'
                                        'Off:\t\tThere will be no labelling at all.\n'
                                        'LateX:\tFonts will be consistent with the one used in \t\t\tstandard LateX documents. Furthermore, the\n'
                                        '\t\taxis labels will accept input in LateX notation.\n'
                                        '\t\tRendering time will be severely impeded.\n'
                                        'Standard:\tThe plot will be labelled in a simple, sans-serif \t\t\tfont. Input in LateX notation will not be accepted.\n'
                                        '=====================================')
        
        plot_labelling_txt = wx.StaticText(notebook_parameters_panel, -1, 'Plot labelling')
        plot_labelling_cbox = wx.ComboBox(notebook_parameters_panel, size=x_axis_label.GetSize(), choices=parent.labelling_choices, value='off', style=wx.CB_READONLY)
        plot_labelling_txt.SetToolTipString(plot_labelling_tooltipstring)
        plot_labelling_cbox.SetToolTipString(plot_labelling_tooltipstring)
        plot_labelling_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_plot_labelling_cbox, parent=parent))
        
        notebook_parameters_panel_sizer.Add(item=plot_labelling_txt, proportion=1, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
        notebook_parameters_panel_sizer.Add(item=plot_labelling_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.RIGHT, border=20)
        
        axis_ticks_txt = wx.StaticText(notebook_parameters_panel, -1, 'Axis ticks\t')
        axis_ticks_cbox = wx.ComboBox(notebook_parameters_panel, size=x_axis_label.GetSize(), choices=['on', 'off'], style=wx.CB_READONLY)
        
        if parent.axis_ticks is not None:
            if parent.axis_ticks is False:
                axis_ticks_cbox.SetValue('off')
            else:
                axis_ticks_cbox.SetValue('on')
        else:
            if parent.style == 'image':
                axis_ticks_cbox.SetValue('off')
            else:
                axis_ticks_cbox.SetValue('on')
            
        axis_ticks_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_axis_ticks_cbox, parent=parent))
        
        notebook_parameters_panel_sizer.Add(item=axis_ticks_txt, proportion=1, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
        notebook_parameters_panel_sizer.Add(item=axis_ticks_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.RIGHT, border=20)
            
        if (parent.style == 'surface' or parent.style == 'wireframe' or parent.style == 'heatmap') and parent.style != 'heatmap':
            
            perspective_heading = wx.StaticText(notebook_parameters_panel, -1, 'Perspective')
            perspective_heading.SetFont(wx.Font(pointSize=10, family=wx.DEFAULT, style=wx.NORMAL, weight=wx.NORMAL))  
            parent.control_plot_frame.azimuth_label = wx.StaticText(notebook_parameters_panel, -1, 'Azimuth')
            parent.control_plot_frame.elevation_label = wx.StaticText(notebook_parameters_panel, -1, 'Elevation')
            parent.control_plot_frame.distance_label = wx.StaticText(notebook_parameters_panel, -1, 'Distance')
            
            parent.control_plot_frame.azimuth_spn = wx.SpinCtrl(notebook_parameters_panel, size=x_axis_label.GetSize(), 
                                                                      min=-180, max=180, initial=-60)
            parent.control_plot_frame.elevation_spn = wx.SpinCtrl(notebook_parameters_panel, size=x_axis_label.GetSize(),  
                                                                        min=-90, max=90, initial=30)
            parent.control_plot_frame.distance_spn = wx.SpinCtrl(notebook_parameters_panel, size=x_axis_label.GetSize(),  
                                                                       min=10, max=100, initial=10)
            parent.control_plot_frame.azimuth_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_azimuth_spn, parent=parent))
            parent.control_plot_frame.elevation_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_elevation_spn, parent=parent))
            parent.control_plot_frame.distance_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_distance_spn, parent=parent))
            
            notebook_parameters_panel_sizer.Add(item=perspective_heading, proportion=1, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
            notebook_parameters_panel_sizer.AddSpacer(5)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.azimuth_label, proportion=1, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.azimuth_spn, proportion=1, flag=wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.elevation_label, proportion=1, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.elevation_spn, proportion=1, flag=wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.distance_label, proportion=1, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.distance_spn, proportion=1, flag=wx.ALIGN_RIGHT|wx.RIGHT, border=20)
                        
        self._create_figure_canvas()
        
        style_heading = wx.StaticText(notebook_parameters_panel, -1, 'Style')
        style_heading.SetFont(wx.Font(pointSize=10, family=wx.DEFAULT, style=wx.NORMAL, weight=wx.NORMAL))
        notebook_parameters_panel_sizer.Add(item=style_heading, proportion=1, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
        notebook_parameters_panel_sizer.AddSpacer(1)
                                        
        # Style options for 3-dimensional plots                                                                    
        if parent.style == 'surface' or parent.style == 'wireframe':
            
            if parent.style == 'wireframe':
                line_color_label = wx.StaticText(notebook_parameters_panel, -1, label='Line color ')  
                self.line_color_ctrl = wx.ColourPickerCtrl(notebook_parameters_panel, -1, col=parent.aux_line_color)
                self.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, partial(self.rdp_gui_events.evt_line_color_ctrl, parent=self))
                notebook_parameters_panel_sizer.Add(line_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
                notebook_parameters_panel_sizer.Add(self.line_color_ctrl, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            
            elif parent.style == 'surface':
                linewidth_label = wx.StaticText(notebook_parameters_panel, -1, label='Line width ') 
                parent.control_plot_frame.linewidth_spn = wx.SpinCtrl(notebook_parameters_panel, size=x_axis_label.GetSize(), min=0, max=20)
                parent.control_plot_frame.linewidth_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_linewidth_spn, parent=parent))
                notebook_parameters_panel_sizer.Add(linewidth_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
                notebook_parameters_panel_sizer.Add(parent.control_plot_frame.linewidth_spn, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            
            resolution_label = wx.StaticText(notebook_parameters_panel, -1, 'Resolution ')
            parent.control_plot_frame.resolution_spn = wx.SpinCtrl(notebook_parameters_panel,  size=x_axis_label.GetSize(), min=1, max=100, value=str(parent.resolution))
            parent.control_plot_frame.resolution_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_resolution_spn, parent=parent))
        
            notebook_parameters_panel_sizer.Add(resolution_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(parent.control_plot_frame.resolution_spn, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
                                                        
        # Style options for line plots
        elif parent.style == '':
            line_style_options = ['solid', 'dashed', 'dash dot', 'dotted']
            line_style_label = wx.StaticText(notebook_parameters_panel, -1, label='Line style ')  
            self.line_style_cbox = wx.ComboBox(notebook_parameters_panel, size=x_axis_label.GetSize(), choices=line_style_options, value='solid', style=wx.CB_READONLY)
            self.line_style_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_line_style_cbox, parent=self))
            notebook_parameters_panel_sizer.Add(line_style_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(self.line_style_cbox, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            
        if parent.mode == 'time course':
            marker_color_label = wx.StaticText(notebook_parameters_panel, -1, label='Marker')
            self.marked_check_box = wx.CheckBox(notebook_parameters_panel, -1)
            self.marked_check_box.Bind(wx.EVT_CHECKBOX, partial(self.rdp_gui_events.evt_marked_check_box, parent=self))
            self.marker_color_ctrl = wx.ColourPickerCtrl(notebook_parameters_panel, -1, col=parent.aux_marker_color)
            self.marker_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, partial(self.rdp_gui_events.evt_marker_color_ctrl, parent=self))
            
            color_picker_sizer = wx.BoxSizer(wx.HORIZONTAL)
            color_picker_sizer.Add(item=self.marked_check_box, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
            color_picker_sizer.Add(item=self.marker_color_ctrl,proportion=1, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT)
                       
            notebook_parameters_panel_sizer.Add(marker_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(color_picker_sizer, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            
        # Plot options for heatmaps
        if parent.style == 'heatmap':
            
            heatmap_boundaries_heading = wx.StaticText(notebook_parameters_panel, -1, 'Heatmap boundaries')
            heatmap_boundaries_heading.SetFont(wx.Font(pointSize=10, family=wx.DEFAULT, style=wx.NORMAL, weight=wx.NORMAL))
                        
            # Controls for narrowing of depicted value range
            vmin_label = wx.StaticText(notebook_parameters_panel, -1, 'Minimum')
            vmax_label = wx.StaticText(notebook_parameters_panel, -1, 'Maximum')
            parent.control_plot_frame.vmin_spn = wx.SpinCtrl(notebook_parameters_panel, size=x_axis_label.GetSize(), min=-50, max=50, initial=0)
            parent.control_plot_frame.vmax_spn = wx.SpinCtrl(notebook_parameters_panel, size=x_axis_label.GetSize(),  min=-50, max=50, initial=0)
            
            # Set widgets to default values
            if parent.vmin is not None:
                parent.control_plot_frame.vmin_spn.SetValue(parent.vmin)
            else:
                parent.control_plot_frame.vmin_spn.SetValue(0.0)
                
            if parent.vmax is not None:
                parent.control_plot_frame.vmax_spn.SetValue(parent.vmax)
            else:
                parent.control_plot_frame.vmax_spn.SetValue(0.0)
            
            reset_heatmap_boundaries_btn = wx.Button(notebook_parameters_panel, label = 'Reset')
            reset_heatmap_boundaries_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_reset_heatmap_boundaries_btn, parent=parent))
        
            notebook_parameters_panel_sizer.Add(item=heatmap_boundaries_heading, proportion=1, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
            notebook_parameters_panel_sizer.AddSpacer(1)
            notebook_parameters_panel_sizer.Add(vmin_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(parent.control_plot_frame.vmin_spn, 2, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            notebook_parameters_panel_sizer.Add(vmax_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(parent.control_plot_frame.vmax_spn, 2, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            notebook_parameters_panel_sizer.Add(reset_heatmap_boundaries_btn, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.AddSpacer(5)
            
            parent.control_plot_frame.vmin_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_vmin_spn, parent=parent))
            parent.control_plot_frame.vmax_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_vmax_spn, parent=parent))
                                                    
        if parent.ndim[parent.selection] == 0:
            parent.control_plot_frame.sel_cbox = wx.ComboBox(notebook_parameters_panel, size=(150, 27), choices=parent.flist_sorted, value=parent.sel_cbox.GetValue(), style=wx.CB_READONLY)
            parent.control_plot_frame.sel_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_sel_cbox, parent=parent))
            
            add_time_course_btn = wx.Button(notebook_parameters_panel, label = 'Add time course')
            add_time_course_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_add_time_course, parent=parent))
            remove_time_course_btn = wx.Button(notebook_parameters_panel, label = 'Remove time course')
            
            #remove_time_course_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_add_time_course, parent=parent))
            line_color_label = wx.StaticText(notebook_parameters_panel, -1, label='Line color')
            
            self.line_color_ctrl = wx.ColourPickerCtrl(notebook_parameters_panel, -1, col=parent.aux_line_color)
            self.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, partial(self.rdp_gui_events.evt_line_color_ctrl, parent=self))
            sel_label = wx.StaticText(notebook_parameters_panel, -1, 'File')            

            notebook_parameters_panel_sizer.Add(line_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            notebook_parameters_panel_sizer.Add(self.line_color_ctrl, 1, wx.ALIGN_RIGHT)
            notebook_parameters_panel_sizer.Add(item=add_time_course_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.EXPAND)
            notebook_parameters_panel_sizer.Add(item=remove_time_course_btn, proportion=0, flag=wx.ALIGN_RIGHT|wx.EXPAND)
            notebook_parameters_panel_sizer.Add(item=sel_label, proportion=0, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            notebook_parameters_panel_sizer.Add(item=parent.control_plot_frame.sel_cbox, proportion=0, flag=wx.ALIGN_RIGHT)
            
        notebook_parameters_panel.SetSizerAndFit(notebook_parameters_panel_sizer)  
        notebook_setup_panel.SetSizerAndFit(notebook_setup_panel_sizer)
        notebook_export_panel.SetSizerAndFit(notebook_export_panel_sizer)
        
        self.notebook.AddPage(notebook_setup_panel, 'Setup')
        self.notebook.AddPage(notebook_parameters_panel, 'Plot parameters')
        self.notebook.AddPage(notebook_export_panel, 'Export')
        
        notebook_sizer = wx.BoxSizer()
        notebook_sizer.Add(self.notebook, 1, wx.EXPAND)
        
        parent.control_plot_frame.main_sizer.Add(item=parent.figure_canvas, proportion=0, flag=wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, border=10)
        parent.control_plot_frame.main_sizer.Add(item=notebook_sizer, proportion=0, flag=wx.ALIGN_RIGHT|wx.ALL|wx.EXPAND, border=10)
        
        #========================================================================================================================
        
        try:
            self.marked_check_box.SetValue(parent.marked)
        except AttributeError:
            pass
        
        x_axis_label.SetValue(parent.x_label)
        y_axis_label.SetValue(parent.y_label)
        
        try:
            z_axis_label.SetValue(parent.z_label)
        except AttributeError:
            pass
        try:
            parent.control_plot_frame.linewidth_spn.SetValue(parent.surface_linewidth*20.0)
        except AttributeError:
            pass
        try:
            plot_labelling_cbox.SetValue(parent.labelling_mode)
        except AttributeError:
            pass
        try:
            self.marker_color_ctrl.SetColour(parent.aux_marker_color)
        except AttributeError:
            pass
        try:
            self.line_color_ctrl.SetColour(parent.aux_line_color)
        except AttributeError:
            pass
                
        # Layout
        #========================================================================================================================
        parent.control_plot_frame.control_plot_panel.SetSizerAndFit(parent.control_plot_frame.main_sizer)
        parent.control_plot_frame.SetSizerAndFit(parent.control_plot_frame.main_sizer)
        parent.control_plot_frame.Show()
        
        return
    
     
    def _create_figure_canvas(self):
    
        self.parent.figure = plt.figure(str(self.parent.title))
        self.parent.figure_canvas = FigureCanvas(self.parent.control_plot_frame.control_plot_panel, wx.ID_ANY, self.parent.figure)    
        self.parent.figure_canvas.mpl_connect('button_release_event', partial(self.rdp_gui_events.evt_change_plot_parameters, parent=self.parent))
        

class RDPSetupFrame(wx.Frame):
    def __init__(self, parent, title):
        
        rdp_gui_events = RDPGUIEvents()
        self.frame = wx.Frame(parent=parent, title=title)
        self.frame.panel = wx.Panel(parent=self.frame)
        
        self.frame.parent = parent
        self.frame.dir = self.frame.parent.dir
        self.frame.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.frame.widget_sizer = wx.FlexGridSizer(cols=2)
        
        self.frame.sel_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY)
        self.frame.mode_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY)
                
        self.frame.proj_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY)
        self.frame.proj_method_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY, choices=self.frame.parent.proj_methods)
        self.frame.style_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY, choices=[])
        
        init_color = '#FFF85D'
        
        if 'wxMac' in wx.PlatformInfo:
            self.frame.mode_cbox.GetChildren()[0].SetBackgroundColour(init_color)
            self.frame.proj_cbox.GetChildren()[0].SetBackgroundColour(init_color)
            self.frame.proj_method_cbox.GetChildren()[0].SetBackgroundColour(init_color)
            self.frame.style_cbox.GetChildren()[0].SetBackgroundColour(init_color)
        else:
            self.frame.mode_cbox.SetBackgroundColour(init_color)
            self.frame.proj_cbox.SetBackgroundColour(init_color)
            self.frame.proj_method_cbox.SetBackgroundColour(init_color)
            self.frame.style_cbox.SetBackgroundColour(init_color)
        
        self.frame.sel_cbox.SetItems(parent.flist_sorted)
        self.frame.sel_cbox.SetValue(parent.sel_cbox_selection)
        self.frame.sel_cbox.Bind(wx.EVT_COMBOBOX, partial(rdp_gui_events.evt_sel_cbox, parent=self.frame))
        self.frame.mode_cbox.SetItems(parent.mode_ch)
        self.frame.mode_cbox.Bind(wx.EVT_COMBOBOX, partial(rdp_gui_events.evt_mode_cbox, parent=self.frame))
        self.frame.proj_cbox.Bind(wx.EVT_COMBOBOX, partial(rdp_gui_events.evt_proj_cbox, parent=self.frame))
        self.frame.proj_method_cbox.Bind(wx.EVT_COMBOBOX, partial(rdp_gui_events.evt_proj_method_cbox, parent=self.frame))
        self.frame.style_cbox.Bind(wx.EVT_COMBOBOX, partial(rdp_gui_events.evt_style_cbox, parent=self.frame))
        
        self.frame.sel_label = wx.StaticText(self.frame.panel, -1, 'File')
        self.frame.mode_label = wx.StaticText(self.frame.panel, -1, 'Plot mode')
        self.frame.proj_label = wx.StaticText(self.frame.panel, -1, 'Projection')
        self.frame.proj_method_label = wx.StaticText(self.frame.panel, -1, 'Projection method \t')
        self.frame.style_label = wx.StaticText(self.frame.panel, -1, 'Plot style')
        
        self.frame.widget_sizer.Add(self.frame.sel_label, proportion=0, flag=wx.ALIGN_LEFT|wx.TOP|wx.LEFT, border=15)
        self.frame.widget_sizer.Add(self.frame.sel_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.EXPAND|wx.TOP|wx.RIGHT, border=15)       
        self.frame.widget_sizer.Add(self.frame.mode_label, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT, border=15)
        self.frame.widget_sizer.Add(self.frame.mode_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.EXPAND|wx.RIGHT, border=15)
        self.frame.widget_sizer.Add(self.frame.proj_label, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT, border=15)
        self.frame.widget_sizer.Add(self.frame.proj_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.EXPAND|wx.RIGHT, border=15)
        self.frame.widget_sizer.Add(self.frame.proj_method_label, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT, border=15)
        self.frame.widget_sizer.Add(self.frame.proj_method_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.EXPAND|wx.RIGHT, border=15)
        self.frame.widget_sizer.Add(self.frame.style_label, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT|wx.BOTTOM, border=15)
        self.frame.widget_sizer.Add(self.frame.style_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.EXPAND|wx.BOTTOM|wx.RIGHT, border=15)
        
        self.frame.main_sizer.Add(self.frame.widget_sizer)
        self.frame.main_sizer.Fit(self.frame.panel)
        self.frame.panel.SetSizerAndFit(self.frame.main_sizer)
        
        return
                
#========================================================================================================================

class RDPGUI(wx.Panel):
    
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        
        self.rdp_gui_events = RDPGUIEvents()
        self.frame = parent        
        self.control_plot_frame = None
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.dir = self.frame.dir            
        self.figure_size = None
        self.x_label = ' ' 
        self.y_label = ' ' 
        self.z_label = ' ' 
        self.marked = False
        self.nstep = 0
        self.step = 0
        self.step_size = 0
        self.slider_max = 0
        self.resolution = 5
        self.vmin = None
        self.vmax = None
        self.proj = None
        self.proj_method = 'average'
        self.style = ' '
        self.mode = ' '
        self.labelling_mode = 'off'
        self.flist = [record_file for record_file in os.listdir(self.dir) if record_file.lower().endswith('.csv') or record_file.lower().endswith('.data')]
        self.flist_sorted = np.asarray(rdp.datatools.sort_alphnum(self.flist))
        self.data = None
        self.reduced_data = None
        self.header = None
        self.header_list = []
        self.ndim = []
        self.time_stamps = []
        self.selection = None
        self.proj_choice_time_course = None
        self.proj_choice_snapshot = None
        self.plot = None
        self.line_color = '#FF9600'
        self.aux_line_color = '#FF9600'
        self.marker_color = '#FF9600'
        self.aux_marker_color = '#FF9600'
        self.save_mode = False
        self.frame_ids = []
        self.frame_ids.append(self.frame.GetId())
        self.figure = None
        self.figure_canvas = None
        self.figure_canvas_connection_id = None
        self.title = ' ' 
        self.figure_resize = None
        self.colorbar = None
        self.axis_ticks = None
        self.sel_cbox_selection = ' ' 
        self.rdp_frame = None
        self.line_style = 'solid'
        self.lines = []
        
        # Plot modes
        #self.mode_ch = [' ', 'snapshot', 'snapshot sequence', 'time course']
        self.mode_ch = [' ', 'snapshot', 'time course']
        self.multi_mode_ch = [' ', 'snapshot', 'time course']
        
        # Projection choices for time course plot mode
        self.proj_ch = [' ', 'x_1', 'x_2', 'x_3', 'x_4', 'x_5']
        
        # Projection choices for snapshot/snapshot sequence plot modes
        self.proj_ch_step = [' ', 'x_1', 'x_2', 'x_3', 'x_4', 'x_5', 
                             'x_1, x_2', 'x_1, x_3', 'x_1, x_4', 'x_1, x_5', 'x_2, x_3', 
                             'x_2, x_4', 'x_2, x_5', 'x_3, x_4', 'x_3, x_5', 'x_4, x_5']
        self.style_ch = [' ', 'heatmap', 'surface', 'wireframe']
        self.proj_methods = [' ', 'average', 'maximum', 'sum']
        
        self.labelling_choices = ['off', 'LateX', 'Standard']
        
        self.figure_azimuth = None
        self.figure_elevation = None
        self.figure_distance = None
        
        self.surface_linewidth = 0
        
        # Labels
        #========================================================================================================================
        self.sel_label = wx.StaticText(self, -1, 'File\t\t')
                
        if self.frame.parent is None:
            self.time_stamp_display = wx.StaticText(self, -1, '-')
            self.time_stamp_label = wx.StaticText(self, -1, 'Time stamp t \t')
            
        #========================================================================================================================
        
        self.line_1 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        self.line_2 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        self.line_4 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        
        # Control widgets
        #========================================================================================================================
        self.sel_cbox = wx.ComboBox(self, size=(200, 27), choices = self.flist_sorted, value = ' ', style = wx.CB_READONLY) 
                
        if self.frame.parent is None:
            self.pos_slider = wx.Slider(self, value=0, minValue = 0, maxValue = 1, style = wx.SL_LABELS|wx.SL_AUTOTICKS)
            
        #========================================================================================================================

        for i in range(len(self.flist_sorted)):    
                self.header_list.append(rdp.datatools.get_header(csv_f=self.dir + '/' + self.flist_sorted[i]))
                self.ndim.append(rdp.datatools.get_dimension(self.header_list[i]))
        
        # Sizers
        #========================================================================================================================
        self.pos_slider_sizer = wx.BoxSizer(wx.VERTICAL)
        self.player_caption_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.time_stamp_sizer = wx.BoxSizer(wx.HORIZONTAL)        
        self.plot_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.line_sizer_1 = wx.BoxSizer(wx.HORIZONTAL)
        self.line_sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        self.line_sizer_4 = wx.BoxSizer(wx.HORIZONTAL)
        self.player_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.player_time_stamp_sizer = wx.BoxSizer(wx.VERTICAL)
        self.btn_sizer = wx.BoxSizer(wx.VERTICAL)
        self.axes_grid_sizer = wx.FlexGridSizer(cols=2)
        self.cbox_grid_sizer = wx.FlexGridSizer(cols=2)
        self.directory_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.selection_sizer = wx.BoxSizer(wx.VERTICAL)
        
        if self.frame.parent is None:
            play = rdp.images.play.GetImage()
            self.play_bitmap = wx.BitmapFromImage(play)
            reverse_play = rdp.images.reverse_play.GetImage()
            self.reverse_play_bitmap = wx.BitmapFromImage(reverse_play)
            pause = rdp.images.pause.GetImage()
            self.pause_bitmap = wx.BitmapFromImage(pause)
            reset = rdp.images.reset.GetImage()
            self.reset_bitmap = wx.BitmapFromImage(reset)
            decrease_single_step = rdp.images.decrease_single_step.GetImage()
            self.decrease_single_step_bitmap = wx.BitmapFromImage(decrease_single_step)
            increase_single_step = rdp.images.increase_single_step.GetImage()
            self.increase_single_step_bitmap = wx.BitmapFromImage(increase_single_step)
            
        # Buttons
        #========================================================================================================================
        self.load_config_btn = wx.Button(self, label = 'Load configuration')
        
        if self.frame.parent is None:
            self.switch_btn = wx.Button(self, label = 'Switch directory')
            self.add_figure_frame_btn = wx.Button(self, label = '+', size=(27,27))

            # Player buttons
            #========================================================================================================================
            self.play_pause_btn = wx.BitmapButton(self, -1, bitmap=self.play_bitmap)
            self.reverse_play_pause_btn = wx.BitmapButton(self, -1, bitmap=self.reverse_play_bitmap)
            self.reset_btn = wx.BitmapButton(self, -1, bitmap=self.reset_bitmap)
            self.decrease_single_step_btn = wx.BitmapButton(self, -1, bitmap=self.decrease_single_step_bitmap)
            self.increase_single_step_btn = wx.BitmapButton(self, -1, bitmap=self.increase_single_step_bitmap)
            
            # Player button tooltips
            #========================================================================================================================
            self.play_pause_btn.SetToolTipString('Starts and pauses the Plot animation.')
            self.reverse_play_pause_btn.SetToolTipString('Starts and pauses the reverse Plot animation.')
            self.reset_btn.SetToolTipString('Resets the plot to the first recorded time step.')
            self.increase_single_step_btn.SetToolTipString('Updates the plot by increasing the time slice index by 1.')
            self.decrease_single_step_btn.SetToolTipString('Updates the plot by decreasing the time slice index by 1.')
        
        # Build partial sizers
        #========================================================================================================================
            
        # Slider and player functionalities
        #========================================================================================================================
        self.player_time_stamp_sizer.Add(self.player_caption_sizer, 0, wx.ALIGN_LEFT)
        
        if self.frame.parent is None:
            
            self.pos_slider_sizer.Add(self.pos_slider, 1, wx.EXPAND|wx.ALIGN_CENTER)
            
            self.time_stamp_sizer.Add(self.time_stamp_label, 1, wx.ALIGN_LEFT)
            self.time_stamp_sizer.Add(self.time_stamp_display, 1, wx.ALIGN_RIGHT)
        
            self.player_sizer.Add(self.decrease_single_step_btn, 1, wx.ALIGN_CENTER)
            self.player_sizer.Add(self.reverse_play_pause_btn, 1, wx.ALIGN_CENTER)
            self.player_sizer.Add(self.play_pause_btn, 1, wx.ALIGN_CENTER)
            self.player_sizer.Add(self.increase_single_step_btn, 1, wx.ALIGN_CENTER)
            self.player_sizer.Add(self.reset_btn, 1, wx.ALIGN_CENTER)
        
            self.player_time_stamp_sizer.Add(self.pos_slider_sizer, 2, wx.EXPAND|wx.ALIGN_CENTER)
            self.player_time_stamp_sizer.Add(self.player_sizer, 2, wx.ALIGN_CENTER)
            self.player_time_stamp_sizer.AddSpacer(10)
            self.player_time_stamp_sizer.Add(self.time_stamp_sizer, 1, wx.ALIGN_LEFT)
        
        #========================================================================================================================
                
        self.directory_sizer.Add(self.sel_cbox, 1, wx.EXPAND)
        
        try:
            self.directory_sizer.Add(self.add_figure_frame_btn, 0, wx.EXPAND)
        except AttributeError:
            pass
        
        self.selection_sizer.Add(self.directory_sizer, 1, wx.EXPAND)

        
        # Selection ComboBoxes
        #========================================================================================================================
        self.cbox_grid_sizer.Add(self.sel_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        self.cbox_grid_sizer.Add(self.selection_sizer, 1, wx.ALIGN_RIGHT)        
        #========================================================================================================================
        
        # Buttons
        #========================================================================================================================
        self.plot_sizer.Add(item=self.load_config_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.EXPAND)
        
        if self.frame.parent == None:
            self.plot_sizer.Add(item=self.switch_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.EXPAND)
            
        self.btn_sizer.Add(item=self.plot_sizer, proportion=1, flag=wx.ALIGN_LEFT|wx.EXPAND)
        #========================================================================================================================
        
        # Dividing lines
        #========================================================================================================================
        self.line_sizer_1.Add(item=self.line_1, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        self.line_sizer_2.Add(item=self.line_2, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        self.line_sizer_4.Add(item=self.line_4, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        #========================================================================
                
        # Build main sizerPlot 
        #========================================================================================================================
        self.main_sizer.Add(self.line_sizer_1, 0, wx.ALIGN_CENTER_HORIZONTAL)
        self.main_sizer.Add(item=self.cbox_grid_sizer, proportion=0, flag=wx.EXPAND|wx.ALIGN_CENTER|wx.LEFT|wx.RIGHT|wx.TOP, border=10)
 
        self.main_sizer.Add(self.line_sizer_2, 0, wx.ALIGN_CENTER_HORIZONTAL)
        self.main_sizer.Add(item=self.player_time_stamp_sizer, proportion=0, flag=wx.ALIGN_CENTER|wx.LEFT|wx.RIGHT|wx.EXPAND, border=10)
            
        self.main_sizer.Add(self.line_sizer_4, 0, wx.ALIGN_CENTER_HORIZONTAL)
            
        self.main_sizer.Add(self.btn_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT|wx.BOTTOM|wx.TOP, border=10)
                
        # Event handling
        #========================================================================================================================
        self.sel_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_sel_cbox, parent=self))
        self.load_config_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_load_config_btn, parent=self))
        
        if self.frame.parent is None:
            self.pos_slider.Bind(wx.EVT_COMMAND_SCROLL_THUMBTRACK, partial(self.rdp_gui_events.evt_pos_slider, parent=self))
            self.pos_slider.Bind(wx.EVT_COMMAND_SCROLL_CHANGED, partial(self.rdp_gui_events.evt_pos_slider, parent=self))       
            self.play_pause_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_play_pause_btn, parent=self))
            self.reverse_play_pause_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_reverse_play_pause_btn, parent=self))
            self.increase_single_step_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_increase_single_step_btn, parent=self))
            self.decrease_single_step_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_decrease_single_step_btn, parent=self))
            self.reset_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_reset_btn, parent=self))
            self.switch_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_switch_btn, parent=self))
                    
        if self.frame.parent is None:
            self.add_figure_frame_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_add_figure_frame_btn, parent=self))
        #========================================================================================================================
        
        # Layout
        #========================================================================================================================
        self.SetSizerAndFit(self.main_sizer)

        #========================================================================================================================
                          
        return
    
    
    def color_combobox_background(self, value, widget):
        if value == '' or value == ' ':
            color =  '#FFF85D'
        else:
            color = '#FFFFFF'
            
        if 'wxMac' in wx.PlatformInfo:
            widget.GetChildren()[0].SetBackgroundColour(color)
        else:
            widget.SetBackgroundColour(color)
            
        return widget
    
    
    def prepare_plot_configuration(self, directory, x_label, y_label, z_label, marked, step, slider_max, resolution, vmax, vmin, proj, 
                                   proj_method, style, mode, labelling_mode, reduced_data, header, ndim, selection, sel_cbox_selection,
                                   proj_choice_time_course, proj_choice_snapshot, line_color, aux_line_color, marker_color, aux_marker_color, title, 
                                   axis_ticks, figure_azimuth, figure_elevation, figure_distance, surface_linewidth, flist_sorted, mode_ch, proj_ch):
        
        
        save_object = [432, directory, x_label, y_label, z_label, marked, step, slider_max, resolution, vmax, vmin, proj, 
                       proj_method, style, mode, labelling_mode, reduced_data, header, ndim, selection, sel_cbox_selection,
                       proj_choice_time_course, proj_choice_snapshot, line_color, aux_line_color, marker_color, aux_marker_color, title, 
                       axis_ticks, figure_azimuth, figure_elevation, figure_distance, surface_linewidth, flist_sorted, mode_ch, proj_ch]
                        
        return save_object
    

    def _load_plot_configuration(self):
        
        dlg = LoadConfigDialog(self, self.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        file_path = dlg.GetPath()
        loaded_object = pickle.load(open(file_path, 'rb'))
        
        if loaded_object[0] == 432:
            self.dir = loaded_object[1]
            self.x_label = loaded_object[2] 
            self.y_label = loaded_object[3]
            self.z_label = loaded_object[4]  
            self.marked = loaded_object[5]  
            self.step = loaded_object[6]  
            self.slider_max = loaded_object[7]  
            self.resolution = loaded_object[8]  
            self.vmax = loaded_object[9]  
            self.vmin = loaded_object[10]  
            self.proj = loaded_object[11]  
            self.proj_method = loaded_object[12]  
            self.style = loaded_object[13]  
            self.mode = loaded_object[14]  
            self.labelling_mode = loaded_object[15]  
            self.reduced_data = loaded_object[16]  
            self.header = loaded_object[17]  
            self.ndim = loaded_object[18]  
            self.selection = loaded_object[19] 
            self.sel_cbox_selection = loaded_object[20]
            self.proj_choice_time_course = loaded_object[21]  
            self.proj_choice_snapshot = loaded_object[22]  
            self.line_color = loaded_object[23]  
            self.aux_line_color = loaded_object[24]  
            self.marker_color = loaded_object[25] 
            self.aux_marker_color = loaded_object[26] 
            self.title = loaded_object[27]  
            self.axis_ticks = loaded_object[28]  
            self.figure_azimuth = loaded_object[29]  
            self.figure_elevation = loaded_object[30]  
            self.figure_distance = loaded_object[31]  
            self.surface_linewidth = loaded_object[32]  
            self.flist_sorted = loaded_object[33] 
            self.mode_ch = loaded_object[34]
            self.proj_ch = loaded_object[35]
                        
        else:
            print 'File format cannot be read.'
            return
        
        self.data, self.time_stamps = RDPPlot().get_data(csv_f=self.dir + '/' + self.flist_sorted[self.selection], header=self.header) 
        self.sel_cbox.SetItems(self.flist_sorted)
        self.sel_cbox.SetValue(self.sel_cbox_selection)
        
        try:
            self.pos_slider.SetMax(self.slider_max)
            self.pos_slider.SetValue(self.step)
            self.time_stamp_display.SetLabel(str(self.time_stamps[self.step]))
        except AttributeError:
            pass
        
        self.rdp_frame = RDPFrame(parent=self, title=self.sel_cbox_selection) 
        self.rdp_frame.frame.proj_cbox.SetItems(self.proj_ch)
        self.rdp_frame.frame.proj_cbox.SetValue(self.proj)
        self.rdp_frame.frame.proj_method_cbox.SetValue(self.proj_method)
        self.rdp_frame.frame.mode_cbox.SetItems(self.mode_ch)
        self.rdp_frame.frame.mode_cbox.SetValue(self.mode)
        self.rdp_frame.frame.style_cbox.SetItems(self.style_ch)
        self.rdp_frame.frame.style_cbox.SetValue(self.style)
        
        self.rdp_frame.frame.mode_cbox.Enable()
        self.rdp_frame.frame.style_cbox.Enable()
        self.rdp_frame.frame.proj_cbox.Enable()
        self.rdp_frame.frame.proj_method_cbox.Enable()
        
        init_color = '#FFFFFF'
        
        if 'wxMac' in wx.PlatformInfo:
            self.rdp_frame.frame.mode_cbox.GetChildren()[0].SetBackgroundColour(init_color)
            self.rdp_frame.frame.proj_cbox.GetChildren()[0].SetBackgroundColour(init_color)
            self.rdp_frame.frame.proj_method_cbox.GetChildren()[0].SetBackgroundColour(init_color)
            self.rdp_frame.frame.style_cbox.GetChildren()[0].SetBackgroundColour(init_color)
        else:
            self.rdp_frame.frame.mode_cbox.SetBackgroundColour(init_color)
            self.rdp_frame.frame.proj_cbox.SetBackgroundColour(init_color)
            self.rdp_frame.frame.proj_method_cbox.SetBackgroundColour(init_color)
            self.rdp_frame.frame.style_cbox.SetBackgroundColour(init_color)
        
        if self.mode != 'snapshot sequence':
            self.rdp_frame.frame.panel.Hide()
            RDPPlotFrame(parent=self, panel=self.rdp_frame.frame.panel)
        
        self._update_plot()
        
             
    def _play_pause_btn(self, reverse):
                
        if reverse == False:
            active_btn = self.play_pause_btn
            active_bitmap = self.play_bitmap
            inactive_btn = self.reverse_play_pause_btn
            loop_range = range(self.step, self.slider_max+1)
            
        else:
            active_btn = self.reverse_play_pause_btn
            active_bitmap = self.reverse_play_bitmap
            inactive_btn = self.play_pause_btn
            loop_range = range(self.step, -1, -1)
        
        if active_btn.GetBitmapLabel() == active_bitmap:
            active_btn.SetBitmapLabel(self.pause_bitmap)
            
            if self.figure:
                
                for i in loop_range:
                    
                    if active_btn.GetBitmapLabel() == active_bitmap:
                        break
                    else:
                        for j in range(len(self.frame_ids)):
                            current_frame = self._set_current_frame(j)
                            current_frame.step = i
                            current_frame._update_plot()
                            wx.Yield()
                                                               
                        inactive_btn.Disable()
                        self.increase_single_step_btn.Disable()
                        self.decrease_single_step_btn.Disable()
                        self.reset_btn.Disable()
                        self.pos_slider.SetValue(min(self.slider_max, self.step))
                        self.time_stamp_display.SetLabel(str(self.time_stamps[self.pos_slider.GetValue()]))
                        
            active_btn.SetBitmapLabel(active_bitmap)
            
        else:
            active_btn.SetBitmapLabel(active_bitmap)
        
        inactive_btn.Enable()
        self.increase_single_step_btn.Enable()
        self.decrease_single_step_btn.Enable()
        self.reset_btn.Enable()
        
        
    def _move_single_step(self, increase):
        
        if increase is False:
            self.step -= 1
                        
            try:
                self.pos_slider.SetValue(max(0, self.step))
            except AttributeError:
                pass
            
        else:
            self.step += 1
                        
            try:
                self.pos_slider.SetValue(min(self.slider_max, self.step))
            except AttributeError:
                pass
            
        try:
            self.time_stamp_display.SetLabel(str(self.time_stamps[self.pos_slider.GetValue()]))
        except AttributeError:
            pass
            
        if self.step == 0:
            
            try:
                self.time_stamp_display.SetLabel(' ')
            except AttributeError:
                pass
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.step = self.step
            current_frame._update_plot()
            wx.Yield()
            
    
    def _update_plot(self):
                
        if self.figure_canvas:
            if self.ndim[self.selection] == 0:
                plt.cla()
            else:
                try:
                    self.figure.clf()
                except AttributeError:
                    pass
        
        try:
            self._plot()    
        except AttributeError:
            pass
        
        wx.YieldIfNeeded()
            
                   
    def _add_time_course(self): 
        self._plot()

              
    def _update_selection_data(self, frame):
        '''Reset the control panel and update it with the newly selected data.'''
        
        self.data = None    
        self.reduced_data = None
                        
        if len(self.frame_ids) == 1 and self.frame.parent is None:
            if self.ndim[self.selection] == 0:
                frame.mode_cbox.SetItems(['time course'])
            else:
                frame.mode_cbox.SetItems(self.mode_ch)
        else:
            if self.ndim[self.selection] == 0:
                frame.mode_cbox.SetItems(['time course'])
            else:
                frame.mode_cbox.SetItems(self.multi_mode_ch)
                                                
        if self.frame.parent is None:
            # Reset control panel
            self.slider_max = 0
            self.step = 0
            self.pos_slider.SetValue(self.step)
            self.time_stamp_display.SetLabel('-')
            self.pos_slider.Disable()
            self.play_pause_btn.Disable()
            self.reverse_play_pause_btn.Disable()
            self.reset_btn.Disable()
            self.decrease_single_step_btn.Disable()
            self.increase_single_step_btn.Disable()
        else:
            for i in range(len(self.frame_ids)):
                current_frame = self._set_current_frame(i)
                current_frame.step = self.step
            
        frame.mode_cbox.Disable()
        
        # Generate adequate options for the projection combobox, depending on data dimensionality and plot mode
        self.proj_choice_time_course = self.proj_ch[:self.ndim[self.selection]+1]
        self.proj_choice_snapshot = rdp.plotfuncs.build_proj_ch_step(ndim=self.ndim[self.selection], temp_proj_ch_step=self.proj_ch_step)   
        
        # Get data and data header
        self.header = rdp.datatools.get_header(csv_f=self.dir + '/' + self.flist_sorted[self.selection])        
        temp_data = rdp.datatools.get_data(csv_f=self.dir + '/' + self.flist_sorted[self.selection], header=self.header)
        
        if 'CV_8U' in self.header or 'CV_8UC3' in self.header:
            self.axis_ticks = False
        
        # Enable slider and player buttons
        if self.frame.parent is None:
            self.pos_slider.Enable()
            self.play_pause_btn.Enable()
            self.reverse_play_pause_btn.Enable()
            self.reset_btn.Enable()
            self.decrease_single_step_btn.Enable()
            self.increase_single_step_btn.Enable()
        
        # Update GUI elements with loaded data
        self.data = temp_data[0]
        self.time_stamps = temp_data[1]
        self.slider_max = len(self.time_stamps)-1
        
        if self.frame.parent is None:
            self.pos_slider.SetMax(self.slider_max)

    
    def _set_current_frame(self, i):
        
        if i == 0:
            current_frame = self
        else:
            current_frame = wx.FindWindowById(self.frame_ids[i]).rdp_gui
                            
        return current_frame
    
    
    def enforce_labelling_mode(self, plot, labelling_mode, style):
        
        # Prevent scientific notation and offset
        formatter = ScalarFormatter()
        formatter.set_scientific(False) 
        formatter.set_useOffset(False)
        
        plot.yaxis.set_major_formatter(formatter)
        plot.yaxis.set_major_formatter(formatter)
        
        try:
            plot.zaxis.set_major_formatter(formatter)
        except AttributeError:
            pass
        
        if labelling_mode == 'off':
            plt.rcdefaults()
            plot.axes.set_xticklabels([])
            plot.axes.set_yticklabels([])
            
            if self.style == 'surface' or self.style == 'wireframe':
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
            if self.axis_ticks == False:
                plot.tick_params(axis='both', which='both', bottom='off', top='off', left='off', right='off')
                    
            else:
                plot.tick_params(axis='both', which='both', bottom='on', top='on', left='on', right='on')
        except AttributeError:
            pass
                
        return plot

    
    def _plot(self, save=False, file_path=None):
        '''Build plot(s) to either visualize or save as pdf file'''
        
        if save is True and self.mode != 'snapshot sequence':
            rdp.plotfuncs.save_plot(plot=self.plot, plot_mode=self.mode, file_name=self.flist_sorted[self.selection], file_directory=self.dir, figure=self.figure, file_path=file_path)
            self._update_plot()
            
        else:         
                       
            if self.mode != 'snapshot sequence':
                if self.figure is None:
                    self.figure = plt.figure(str(self.title))
                                                                       
            if self.mode == 'snapshot' or self.style == 'image':

                try:
                    self.plot, self.reduced_data = rdp.plotfuncs.plot_snapshot(step = self.step, 
                                                                  style = self.style, 
                                                                  data = self.data, 
                                                                  header = self.header, 
                                                                  vmin = self.vmin,
                                                                  vmax = self.vmax,
                                                                  resolution = self.resolution, 
                                                                  surface_linewidth = self.surface_linewidth,
                                                                  proj = self.proj,
                                                                  linestyle = self.line_style,
                                                                  proj_method = self.proj_method,
                                                                  color = self.line_color,
                                                                  figure = self.figure,
                                                                  title = self.title)
                                        
                    if self.style == 'heatmap' or self.style == 'surface' or self.style == 'wireframe':
                        self.figure.gca().invert_yaxis()
                                                    
                except IndexError:
                    dlg = wx.MessageDialog(parent=None, 
                                           message = 'The specified time slice does not exist.', 
                                           caption = 'An Error has occurred.', 
                                           style = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                    
                    dlg.ShowModal()
                    dlg.Hide()
                    wx.CallAfter(dlg.Destroy)
                
            elif self.mode == 'snapshot sequence':
                self.save_mode = save
                dlg = SnapshotSequenceDialog(self, -1, 'Options')
                dlg.ShowModal()
                dlg.Hide()
                wx.CallAfter(dlg.Destroy)
                        
            elif self.mode == 'time course':
                try:
                    self.plot = rdp.plotfuncs.plot_time_course(data = self.data, 
                                                                     header = self.header, 
                                                                     vmin = self.vmin,
                                                                     vmax = self.vmax,
                                                                     resolution = self.resolution,
                                                                     surface_linewidth = self.surface_linewidth,
                                                                     plot = self.plot,
                                                                     linestyle=self.line_style,
                                                                     proj = self.proj,
                                                                     proj_method = self.proj_method,
                                                                     color = self.line_color,
                                                                     step = self.step, 
                                                                     marker = self.marked, 
                                                                     style = self.style,
                                                                     marker_color = self.marker_color,
                                                                     figure = self.figure,
                                                                     title = self.title)
                    
                    if self.ndim[self.selection] == 0:
                        self.lines.append(self.plot.get_lines()[-1])
                        
                    if self.style == 'heatmap' or self.style == 'surface' or self.style == 'wireframe':
                        self.figure.gca().invert_yaxis()
                    
                    if self.x_label == ' ' or self.x_label == ' ' :
                        self.x_label = 'Time'
                        try:
                            self.plot.set_xlabel(self.x_label)
                        except AttributeError:
                            pass
                        
                except UnboundLocalError:
                    dlg = wx.MessageDialog(parent = None, 
                                           message = 'It is not possible to build a time course out of 2-dimensional time slices.', 
                                           caption = 'The attempted operation is not possible.', 
                                           style = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
                    dlg.ShowModal()
                    dlg.Hide()
                    wx.CallAfter(dlg.Destroy)
            
            try:
                rdp.plotfuncs.label_axis(plot=self.plot, x_label=self.x_label, y_label=self.y_label, z_label=self.z_label)
            except AttributeError:
                pass
                            
            if self.style != 'heatmap':
                if self.figure_azimuth is not None and self.figure_elevation is not None:
                    self.figure.gca(projection='3d').view_init(elev=self.figure_elevation, azim=self.figure_azimuth)
            
                if self.figure_distance is not None:
                    self.figure.gca(projection='3d').dist = self.figure_distance
                    
            if save is False and self.mode != 'snapshot sequence':
                self.plot = self.enforce_labelling_mode(plot=self.plot, labelling_mode=self.labelling_mode, style=self.style)
                
                try:
                    self.figure_canvas.draw()
                except AttributeError:
                    pass
                            
#========================================================================================================================

if __name__ == '__main__':
    app = RDPApp()
    app.MainLoop()
