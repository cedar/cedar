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

    File:        recorded_data_processor.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 30

    Description: 

    Credits:

========================================================================================================================
'''

import matplotlib as mpl
mpl.use('WXAgg')

import matplotlib.pyplot as plt
import numpy as np

try:
    import cPickle as pickle
except ImportError:
    import pickle

import rdp.plotfuncs
import rdp.guifuncs
import rdp.images

import sys
import warnings

warnings.filterwarnings('ignore', category=UserWarning)

import wx

from functools import partial
from mpl_toolkits.mplot3d import Axes3D

# Prevent Eclipse warning from Axes3D import
Axes3D

#========================================================================================================================

class RDPGUIEvents():
      
    def evt_cancel_button(self, event, parent):
        #Close dialog
        parent.Hide()
        wx.CallAfter(parent.Destroy)
    
    def evt_save_config_btn(self, event, parent):
        dlg = rdp.dlg.save_config_dlg(parent=parent, defaultDir=parent.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        save_object = rdp.datatools.prepare_plot_configuration(directory=parent.dir,
                                                               x_label=parent.x_label,
                                                               y_label=parent.y_label, 
                                                               z_label=parent.z_label, 
                                                               marked=parent.marked, 
                                                               step=parent.step, 
                                                               slider_max=parent.slider_max, 
                                                               stride=parent.stride, 
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
                                                               proj_ch=parent.rdp_frame.frame.proj_cbox.GetItems(),
                                                               line_style=parent.line_style,
                                                               surface_cmap=parent.surface_cmap)
                
        file_path = dlg.GetPath()

        pickle.dump(save_object, open(file_path, 'wb'), 0)
        
    
    def evt_elevation_spn(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        parent.figure_elevation = value
        parent.figure.gca().elev = parent.figure_elevation
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        
        
    def evt_line_style_cbox(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        great_parent = parent.parent
        great_parent.line_style = value
        wx.CallAfter(rdp.plotfuncs.update_plot, great_parent)
        
        
    def evt_azimuth_spn(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        parent.figure_azimuth = value      
        parent.figure.gca().azim = parent.figure_azimuth
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        

    def evt_distance_spn(self, event, parent):
        widget = event.GetEventObject()
        value = widget.GetValue()
        parent.figure_distance = value        
                
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
                
    
    def evt_add_figure_frame_btn(self, event, parent):
        
        aux_frame = parent
        
        # Find uppermost RDPGUI
        while aux_frame.GetParent() != None:
            aux_frame = aux_frame.GetParent()

        aux_frame = aux_frame.GetChildren()[1]
        
        new_frame = RDPMainWindow(parent=parent.frame, title='', pos=None, size=None, style=None, name=None, directory=parent.dir)     
        new_frame.rdp_gui.step = parent.step   
        new_frame.Bind(wx.EVT_CLOSE, partial(self.evt_close_frame, parent=parent))
        new_frame_id = new_frame.GetId()
        aux_frame.frame_ids.append(new_frame_id)
        wx.FindWindowById(new_frame_id).step = aux_frame.step
        
     
    def evt_close_frame(self, event, parent):
        
        frame = event.GetEventObject()
        frame_great_parent = frame.GetParent().GetParent()
        
        # Get ID of active event objects great parent/main window
        frame_id = frame_great_parent.GetId()
        aux_frame = frame
        
        # Find uppermost RDPGUI
        while aux_frame.GetParent() != None:
            aux_frame = aux_frame.GetParent()

        aux_frame = aux_frame.GetChildren()[1]
                        
        # Remove ID of active event object from ID list
        if frame_id in aux_frame.frame_ids:
            aux_frame.frame_ids.remove(frame_id)
                
        frame.Hide()
        
                            
    def evt_play_pause_btn(self, event, parent):
        rdp.guifuncs.play_pause_btn(parent=parent, reverse=False)
        
        
    def evt_reverse_play_pause_btn(self, event, parent):
        rdp.guifuncs.play_pause_btn(parent=parent, reverse=True)
        
            
    def evt_reset_btn(self, event, parent):
        '''Reset plot to default.'''
        
        for i in range(len(parent.frame_ids)):
            current_frame = rdp.guifuncs.set_current_frame(parent=parent, i=i)
            current_frame.step = 0
            current_frame.nstep = 0
            current_frame.step_size = 0
            current_frame.save_mode = False
            
            if i == 0:
                current_frame.pos_slider.SetValue(current_frame.step)
                current_frame.time_stamp_display.SetLabel('')
                    
            if current_frame.control_plot_frame:
                
                try:
                    current_frame.control_plot_frame.stride_spn.SetValue(current_frame.stride)
                except AttributeError:
                    
                    pass
                try:
                    current_frame.control_plot_frame.linewidth_spn.SetValue(current_frame.surface_linewidth)
                except AttributeError:
                    pass
            
            rdp.plotfuncs.update_plot(parent=current_frame)
            wx.Yield()  

    
    def evt_increase_single_step_btn(self, event, parent):
        rdp.guifuncs.move_single_step(parent=parent, increase=True)
        wx.Yield()
                    
    
    def evt_decrease_single_step_btn(self, event, parent):
        rdp.guifuncs.move_single_step(parent=parent, increase=False)
        wx.Yield()
        
    
    def evt_axis_label(self, event, x_axis_label, y_axis_label, parent, z_axis_label=None):
        
        parent.x_label = x_axis_label.GetValue()
        parent.y_label = y_axis_label.GetValue()
        
        if z_axis_label != None:
            try:
                parent.z_label = z_axis_label.GetValue()
            except AttributeError:
                pass
                    
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        
    
    def evt_add_time_course(self, event, parent):
        '''Add time course to currently displayed plot.'''
        
        rdp.plotfuncs.add_time_course(parent=parent, ydata=parent.data, color=parent.line_color, linestyle=parent.line_style, plot=parent.plot)
        
        
    def evt_stride_spn(self, event, parent):
        
        parent.stride = int(parent.control_plot_frame.stride_spn.GetValue())
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        

    def evt_vmax_spn(self, event, parent):
        
        parent.vmax = parent.control_plot_frame.vmax_spn.GetValue()
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)

    
    def evt_vmin_spn(self, event, parent):
        
        parent.vmin = parent.control_plot_frame.vmin_spn.GetValue()
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        
    
    def evt_proj_cbox(self, event, parent):
        
        if 'RDPGUI' not in str(type(parent)):
            parent = parent.GetParent()
        
        widget = event.GetEventObject()
        parent.proj = widget.GetValue()
        
        # Case 1: A 1-dimensional snapshot is to be plotted => no 3-dimensional plot is needed
        if ',' not in parent.proj and parent.mode != 'time course':
            parent.style = ''
            parent.rdp_frame.frame.style_cbox.Disable()
            
            if parent.ndim[parent.selection] == 1:
                parent.rdp_frame.frame.proj_method_cbox.Disable()
            else:
                parent.rdp_frame.frame.proj_method_cbox.Enable()
                         
            parent.rdp_frame.frame.style_cbox.SetValue('')
            parent.rdp_frame.frame.proj_method_cbox.SetValue('')
            
            if parent.proj != '':
                parent.rdp_frame.frame.panel.Hide()
                RDPPlotFrame(parent=parent, panel=parent.rdp_frame.frame.panel) 
                
                if parent.style != 'heatmap' and parent.ndim[parent.selection] != 0:
                    try:    
                        rdp.plotfuncs.plot(parent=parent)
                    except AttributeError:
                        pass
                        
        # Case 2: 3-dimensional plot or heatmap 
        else:
            parent.rdp_frame.frame.style_cbox.Enable()
            
            if parent.ndim[parent.selection] != 2:
                parent.rdp_frame.frame.proj_method_cbox.Enable()
            else:
                parent.rdp_frame.frame.proj_method_cbox.Disable()
                parent.rdp_frame.frame.proj_method_cbox.SetValue('')
                parent.proj_method = ''
            
        if parent.proj == '':
            parent.proj_method = ''
            parent.style = ''
            
            parent.rdp_frame.frame.proj_method_cbox.Disable()
            parent.rdp_frame.frame.style_cbox.Disable()            
            parent.rdp_frame.frame.proj_method_cbox.SetValue(parent.proj_method)
            parent.rdp_frame.frame.style_cbox.SetValue(parent.style)
            
        rdp.plotfuncs.update_plot(parent=parent)
            
        
    def evt_reset_heatmap_boundaries_btn(self, event, parent):
        
        parent.vmin = parent.vmax = None
        
        parent.control_plot_frame.vmin_spn.SetValue(0.0000)
        parent.control_plot_frame.vmax_spn.SetValue(0.0000)
        
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)

    
    def evt_switch_btn(self, event, parent):
        
        for i in range(len(parent.frame_ids)):
            current_frame = rdp.guifuncs.set_current_frame(parent=parent, i=i)
            
            # Clear memory
            if current_frame.data is not None:
                del current_frame.data
                current_frame.data = None
            
            # Close open plots              
            if current_frame.figure:
                plt.close(current_frame.figure)
                current_frame.figure = None
                        
        frame = parent.GetParent()
        browser_panel = RDPBrowserPanel(parent=frame)
                
        # Replace plot generation frame with file browser
        frame.SetSizer(browser_panel.main_sizer)
        
        # Layout
        wx.CallAfter(frame.Sizer.Fit, frame)
        wx.CallAfter(frame.Layout)

                    
    def evt_pos_slider(self, event, parent):
        
        step = parent.pos_slider.GetValue()
                
        for i in range(len(parent.frame_ids)):
            current_frame = rdp.guifuncs.set_current_frame(parent=parent, i=i)
            current_frame.step = step
        
            if i == 0:
                # Set time code
                current_time_stamp = parent.time_stamps[current_frame.step]
                current_frame.time_stamp_display.SetLabel(current_time_stamp)
                
            # Update currently displayed plot
            rdp.plotfuncs.update_plot(parent=current_frame)
            wx.Yield()
        
                
    def evt_linewidth_spn(self, event, parent):
        widget = event.GetEventObject()
        parent.surface_linewidth = float(widget.GetValue() / 20.)
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        
        
    def evt_surface_cmap_cbox(self, event, parent):
        widget = event.GetEventObject()
                
        if widget.GetValue() == 'grayscale':
            parent.surface_cmap = 'binary'
        elif widget.GetValue() == 'color':
            parent.surface_cmap = 'coolwarm'
        wx.CallAfter(rdp.plotfuncs.update_plot, parent)
        
    
    def evt_add_time_course_sel_cbox(self, event, parent):
        widget = event.GetEventObject()
        
        if 'RDPGUI' not in str(type(parent)):
            parent = parent.GetParent()
                    
        recorded_file = parent.dir + '/' + widget.GetValue()
        
        header = rdp.datatools.get_csv_header(csv_f=recorded_file)
        data = rdp.datatools.get_csv_data(csv_f=recorded_file, header=header)
        rdp.plotfuncs.add_time_course(parent=parent, ydata=data, linestyle=parent.line_style, color=parent.line_color, plot=parent.plot)
        rdp.plotfuncs.update_plot(parent=parent)
        
        
    def evt_sel_cbox(self, event, parent):
        widget = event.GetEventObject()
                
        if 'RDPGUI' not in str(type(parent)):
            parent = parent.GetParent()
                    
        parent.selection = widget.GetSelection()
        parent.sel_cbox_selection = widget.GetValue()
        parent.title = parent.sel_cbox_selection
        current_header = parent.header_list[parent.selection]
        parent.control_plot_frame.SetTitle(parent.sel_cbox_selection)
                     
        # Case 1: selected file is an image
        if 'CV_8U' in current_header or 'CV_8UC3' in current_header:
            parent.style = 'image'
            parent.rdp_frame.frame.style_cbox.SetItems(['image'])
            parent.rdp_frame.frame.style_cbox.SetValue('image')
            
        # Case 2: selected file contains 0-dimensional data    
        elif rdp.datatools.get_dimension(current_header) == 0:
            parent.mode = 'time course'
            parent.rdp_frame.frame.mode_cbox.SetItems(['time course'])
            parent.rdp_frame.frame.mode_cbox.SetValue('time course')
        
        # Case 3: selected file contains 1 or more-dimensional data and is no image   
        else:
            parent.style_ch = ['heatmap', 'surface', 'wireframe']
            parent.mode_ch = ['snapshot', 'time course']
            parent.rdp_frame.frame.mode_cbox.SetItems(parent.mode_ch)
            parent.rdp_frame.frame.style_cbox.SetItems(parent.style_ch)
            parent.rdp_frame.frame.mode_cbox.Enable()
        
        rdp.guifuncs.update_selection_data(parent=parent, frame=parent.rdp_frame.frame)
        RDPPlotFrame(parent=parent, panel=parent.rdp_frame.frame.panel) 
        
        try:
            rdp.plotfuncs.update_plot(parent=parent)
        except AttributeError:
            pass

        
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
        rdp.plotfuncs.plot(parent=parent, save=True, file_path=file_path)
            
            
    def evt_dir_btn(self, event, parent):
        RDPMainWindow(parent=None, title='Recorded Data Processor', pos=None, size=None, style=None, name=None, directory=parent.frame.dir)
        parent.frame.Destroy()
        
        
    def evt_load_config_btn(self, event, parent):
        
        try:
            rdp_gui = parent._initialize_rdp_gui()
            
        except AttributeError:
            new_frame = RDPMainWindow(parent=parent, title='', pos=None, size=None, style=None, name=None, directory=parent.frame.dir)  
            new_frame.rdp_gui.step = parent.step   
            new_frame.Bind(wx.EVT_CLOSE, partial(self.evt_close_frame, parent=parent))
        
            new_frame_id = new_frame.GetId()
            parent.frame_ids.append(new_frame_id)
            rdp_gui = new_frame.rdp_gui
                
        rdp.datatools.load_plot_configuration(parent=rdp_gui)

        
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
        #if great_parent.figure and great_parent.ndim[great_parent.selection] != 0:
        wx.CallAfter(rdp.plotfuncs.update_plot, great_parent)
            
            
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
            wx.CallAfter(rdp.plotfuncs.update_plot, great_parent)
    
    
    def evt_marked_check_box(self, event, parent):
        
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.marked = widget.GetValue()
        wx.CallAfter(rdp.plotfuncs.update_plot, great_parent)
    
    
    def evt_plot_labelling_cbox(self, event, parent):
        
        widget = event.GetEventObject()
        parent.labelling_mode = widget.GetValue()
        
        rdp.plotfuncs.update_plot(parent=parent)
        rdp.plotfuncs.update_plot(parent=parent)
            
        
    def evt_axis_ticks_cbox(self, event, parent):
        
        widget = event.GetEventObject()
                
        if widget.GetValue() == 'on':
            parent.axis_ticks = True
        else:
            parent.axis_ticks = False
            
        rdp.plotfuncs.update_plot(parent=parent)
        rdp.plotfuncs.update_plot(parent=parent)
        
    
    def evt_mode_cbox(self, event, parent):
        
        if 'RDPGUI' not in str(type(parent)):
            parent = parent.GetParent()
        
        widget = event.GetEventObject()
                        
        if widget.GetValue() == 'time course' and ',' in str(parent.proj):
            dlg = wx.MessageDialog(parent = None, 
                                   message = 'It is not possible to build a time course out of 2-dimensional time slices.', 
                                   caption = 'The attempted operation is not possible.', 
                                   style = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
            
            dlg.ShowModal()
            dlg.Hide()
            wx.CallAfter(dlg.Destroy)
            
            widget.SetValue('snapshot')
            parent.mode = widget.GetValue()
        
        else:
            parent.mode = widget.GetValue()
            parent.proj_method = ''
            parent.style = ''
            parent.proj = ''
            parent.rdp_frame.frame.proj_method_cbox.Disable()
            parent.rdp_frame.frame.style_cbox.Disable()                
            parent.rdp_frame.frame.proj_method_cbox.SetValue(parent.proj_method)
            parent.rdp_frame.frame.style_cbox.SetValue(parent.style)
            parent.rdp_frame.frame.panel.Hide()
            RDPPlotFrame(parent=parent, panel=parent.rdp_frame.frame.panel) 
            parent.rdp_frame.frame.Hide()
        
        if parent.ndim[parent.selection] == 1:
            rdp.plotfuncs.update_plot(parent=parent)
            if parent.mode == 'time course':
                parent.rdp_frame.frame.style_cbox.Enable()
        else:
            # Fill projection combobox with the fitting options
            if parent.mode == 'time course':
                parent.rdp_frame.frame.proj_cbox.SetItems(parent.proj_choice_time_course)
                parent.rdp_frame.frame.proj_cbox.Enable()
            elif parent.mode == 'snapshot':
                parent.rdp_frame.frame.proj_cbox.SetItems(parent.proj_choice_snapshot)
                parent.rdp_frame.frame.proj_cbox.Enable()
            else:
                parent.rdp_frame.frame.proj_cbox.SetItems([])
            
        
    def evt_proj_method_cbox(self, event, parent):
        
        widget = event.GetEventObject()
        great_parent = parent.parent
        great_parent.proj_method = widget.GetValue()
        
        if great_parent.style != 'image':
            parent.style_cbox.Enable()
                            
        parent.panel.Hide()
        RDPPlotFrame(parent=great_parent, panel=parent.panel) 
        rdp.plotfuncs.update_plot(parent=great_parent)
            
            
    def evt_style_cbox(self, event, parent):
        
        great_parent = parent.parent
        widget = event.GetEventObject()
        great_parent.style = widget.GetValue()
        
        if great_parent.mode != 'image':
            if great_parent.ndim[great_parent.selection] != 1:
                parent.mode_cbox.Enable()
                parent.proj_cbox.Enable()
                if great_parent.ndim[great_parent.selection] != 2:
                    parent.proj_method_cbox.Enable()
            
        else:
            great_parent.mode = parent.mode_cbox.GetValue()
            parent.mode_cbox.Disable()
            parent.proj_cbox.Disable()
            parent.proj_method_cbox.Disable()

        parent.panel.Hide()
        RDPPlotFrame(parent=great_parent, panel=parent.panel) 
        rdp.plotfuncs.update_plot(parent=great_parent)
            
            
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
        self.frame.dir = rdp.guifuncs.search_homedir()
               
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
        if self.parent == None:
            wx.Frame.__init__(self, parent=None, title=title, style=wx.MINIMIZE_BOX|wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN)  
            self.rdp_browser = RDPBrowserPanel(parent=self)
            self.main_sizer = self.rdp_browser.main_sizer
        
        # initialize frame for multi-plot purposes
        else:
            wx.Frame.__init__(self, parent=parent, title='', style=wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.FRAME_NO_TASKBAR|wx.CLIP_CHILDREN)
            #self.dir = self.parent.dir
            self.parent = parent
            self.rdp_gui = RDPGUI(parent=self)
            self.main_sizer = self.rdp_gui.main_sizer
        
        self.menu_file = wx.Menu()
        self.menu_bar = wx.MenuBar()
        self.menu_bar.Append(self.menu_file, 'File')
        self.SetMenuBar(self.menu_bar)
        self.menu_bar.Hide()    
        self.SetSizer(self.main_sizer)
        self.Sizer.Fit(self)
        
        if self.parent == None:
            self.Bind(wx.EVT_CLOSE, self.rdp_gui_events.evt_end_app)
            self.Show()
                        
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
        self.sel_btn.Bind(wx.EVT_BUTTON, partial(rdp_gui_events.evt_sel_btn, parent=self))
        
        # Main sizer
        self.main_sizer.Add(item=self.select_directory, proportion=0, flag=wx.TOP|wx.LEFT|wx.BOTTOM, border=10)
        self.main_sizer.Add(item=self.browser, proportion=0, flag=wx.LEFT|wx.BOTTOM|wx.RIGHT|wx.EXPAND, border=10)
        self.btn_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.btn_sizer.Add(item=self.sel_btn, proportion=0)
        self.main_sizer.Add(item=self.btn_sizer, proportion=0, flag=wx.ALIGN_RIGHT|wx.BOTTOM|wx.RIGHT, border=10)
        
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
        wx.CallAfter(self.Hide)
        
        return self.rdp_gui
    
#========================================================================================================================

class RDPPlotFrame(wx.Frame):
    def __init__(self, parent, panel):
        
        self.parent = parent
        self.rdp_gui_events = RDPGUIEvents()
        
        # Initialize control plot frame
        if parent.control_plot_frame == None:
            parent.control_plot_frame = wx.Frame(parent=parent, id=-1, title=str(parent.title), style=wx.MINIMIZE_BOX|wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN)
            parent.control_plot_frame.Bind(wx.EVT_CLOSE, partial(self.rdp_gui_events.evt_close_frame, parent=parent))
        
        parent.control_plot_frame.panel = wx.Panel(parent=parent.control_plot_frame)
        self.notebook = wx.Notebook(parent.control_plot_frame.panel)
        
        notebook_setup_panel = wx.Panel(parent=self.notebook)
        notebook_parameters_panel = wx.Panel(parent=self.notebook)
        notebook_export_panel = wx.Panel(parent=self.notebook)
        
        panel.Reparent(notebook_setup_panel)
        panel.Show()
        
        add_figure_frame_btn = wx.Button(notebook_setup_panel, label = 'Add plot frame')
        add_figure_frame_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_add_figure_frame_btn, parent=parent))        
        notebook_setup_panel_sizer = wx.BoxSizer(wx.VERTICAL)
        notebook_setup_panel_sizer.Add(item=panel, proportion=1, flag=wx.ALIGN_LEFT|wx.RIGHT, border=10)
        notebook_setup_panel_sizer.Add(item=add_figure_frame_btn, proportion=0, flag=wx.ALIGN_BOTTOM|wx.ALIGN_RIGHT|wx.ALL, border=10)
        
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
        
        axes_label_tooltip_string = 'Enter axes labels here. Press Enter to update the plot.'
        
        # Build sizers
        #========================================================================================================================
        notebook_parameters_panel_sizer.Add(axes_heading, 1, wx.ALIGN_LEFT|wx.ALL, border=10)
        notebook_parameters_panel_sizer.AddSpacer(5)
        notebook_parameters_panel_sizer.Add(x_axis_txt, 1, wx.ALIGN_LEFT|wx.LEFT, border=20)
        notebook_parameters_panel_sizer.Add(x_axis_label, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
        notebook_parameters_panel_sizer.Add(y_axis_txt, 1, wx.ALIGN_LEFT|wx.LEFT, border=20)
        notebook_parameters_panel_sizer.Add(y_axis_label, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
        
        x_axis_txt.SetToolTipString(axes_label_tooltip_string)
        x_axis_label.SetToolTipString(axes_label_tooltip_string)
        y_axis_txt.SetToolTipString(axes_label_tooltip_string)
        y_axis_label.SetToolTipString(axes_label_tooltip_string)
        
        # Plot is either 3-dimensional or a heatmap
        if parent.style == 'surface' or parent.style == 'wireframe' or parent.style == 'heatmap':
            z_axis_label = wx.TextCtrl(notebook_parameters_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
            notebook_parameters_panel_sizer.Add(z_axis_txt, 0, wx.ALIGN_LEFT|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(z_axis_label, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
            z_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.rdp_gui_events.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label, parent=parent))
            z_axis_txt.SetToolTipString(axes_label_tooltip_string)
            z_axis_label.SetToolTipString(axes_label_tooltip_string)
            
            # Options for axis ticks only available in 2D plots, considering that they do not seem to work correctly for 3D plots
            if parent.style == 'heatmap':
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
        
        try:
            notebook_parameters_panel_sizer.Add(item=axis_ticks_txt, proportion=1, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(item=axis_ticks_cbox, proportion=1, flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.RIGHT, border=20)
        except UnboundLocalError:
            pass
        
        if (parent.style == 'surface' or parent.style == 'wireframe') and parent.style != 'heatmap':
            
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
        
        self.parent.figure_canvas = rdp.guifuncs.create_figure_canvas(figure=self.parent.figure, title=self.parent.title, control_plot_panel=self.parent.control_plot_frame.panel)        
        self.parent.figure_canvas.mpl_connect('button_release_event', partial(self.rdp_gui_events.evt_change_plot_parameters, parent=self.parent))
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
                
                surface_cmap_label = wx.StaticText(notebook_parameters_panel, -1, label='Color map ')
                parent.control_plot_frame.surface_cmap_cbox = wx.ComboBox(notebook_parameters_panel, size=x_axis_label.GetSize(), choices=['color', 'grayscale'])
                if parent.surface_cmap == 'Greys':
                    parent.control_plot_frame.surface_cmap_cbox.SetValue('grayscale')
                elif parent.surface_cmap == 'coolwarm':
                    parent.control_plot_frame.surface_cmap_cbox.SetValue('color')
                    
                parent.control_plot_frame.surface_cmap_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_surface_cmap_cbox, parent=parent))
                
                notebook_parameters_panel_sizer.Add(surface_cmap_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
                notebook_parameters_panel_sizer.Add(parent.control_plot_frame.surface_cmap_cbox, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
                notebook_parameters_panel_sizer.Add(linewidth_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
                notebook_parameters_panel_sizer.Add(parent.control_plot_frame.linewidth_spn, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)

            stride_label = wx.StaticText(notebook_parameters_panel, -1, 'Stride')
            parent.control_plot_frame.stride_spn = wx.SpinCtrl(notebook_parameters_panel,  size=x_axis_label.GetSize(), min=1, max=100, value=str(parent.stride))
            parent.control_plot_frame.stride_spn.Bind(wx.EVT_SPINCTRL, partial(self.rdp_gui_events.evt_stride_spn, parent=parent))
        
            notebook_parameters_panel_sizer.Add(stride_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(parent.control_plot_frame.stride_spn, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
                                                        
        # Style options for line plots
        elif parent.style == '' or parent.style == ' ':
            line_style_options = ['solid', 'dashed', 'dash dot', 'dotted']
            line_style_label = wx.StaticText(notebook_parameters_panel, -1, label='Line style ')  
            self.line_style_cbox = wx.ComboBox(notebook_parameters_panel, size=x_axis_label.GetSize(), choices=line_style_options, value='solid', style=wx.CB_READONLY)
            self.line_style_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_line_style_cbox, parent=self))
            notebook_parameters_panel_sizer.Add(line_style_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=20)
            notebook_parameters_panel_sizer.Add(self.line_style_cbox, 1, wx.ALIGN_RIGHT|wx.RIGHT, border=20)
        
        if parent.mode == 'time course' or parent.ndim[parent.selection] == 0:
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
            '''
            parent.rdp_frame.sel_cbox = wx.ComboBox(notebook_parameters_panel, size=(150, 27), choices=parent.flist_sorted, style=wx.CB_READONLY)
            parent.rdp_frame.sel_cbox.Bind(wx.EVT_COMBOBOX, partial(self.rdp_gui_events.evt_add_time_course_sel_cbox, parent=parent))
            add_time_course_btn = wx.Button(notebook_parameters_panel, label = 'Add time course')
            add_time_course_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_add_time_course, parent=parent))
            remove_time_course_btn = wx.Button(notebook_parameters_panel, label = 'Remove time course')
            remove_time_course_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_add_time_course, parent=parent))
            '''
            
            line_color_label = wx.StaticText(notebook_parameters_panel, -1, label='Line color')
            self.line_color_ctrl = wx.ColourPickerCtrl(notebook_parameters_panel, -1, col=parent.aux_line_color)
            self.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, partial(self.rdp_gui_events.evt_line_color_ctrl, parent=self))
            notebook_parameters_panel_sizer.Add(line_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            notebook_parameters_panel_sizer.Add(self.line_color_ctrl, 1, wx.ALIGN_RIGHT)
            
            '''
            sel_label = wx.StaticText(notebook_parameters_panel, -1, 'File')
            notebook_parameters_panel_sizer.Add(item=add_time_course_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.EXPAND)
            notebook_parameters_panel_sizer.Add(item=remove_time_course_btn, proportion=0, flag=wx.ALIGN_RIGHT|wx.EXPAND)
            notebook_parameters_panel_sizer.Add(item=sel_label, proportion=0, flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            notebook_parameters_panel_sizer.Add(item=parent.rdp_frame.sel_cbox, proportion=0, flag=wx.ALIGN_RIGHT)
            '''
            
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
        parent.control_plot_frame.panel.SetSizerAndFit(parent.control_plot_frame.main_sizer)
        parent.control_plot_frame.SetSizerAndFit(parent.control_plot_frame.main_sizer)
        parent.control_plot_frame.Show()
        
        return
        

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
        self.frame.proj_cbox.Disable()
        self.frame.proj_method_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY, choices=self.frame.parent.proj_methods)
        self.frame.style_cbox = wx.ComboBox(self.frame.panel, style = wx.CB_READONLY, choices=parent.style_ch)        
        self.frame.sel_cbox.SetItems(parent.flist_sorted)
        self.frame.sel_cbox.SetValue(parent.sel_cbox_selection)
        self.frame.mode_cbox.SetItems(parent.mode_ch)
        self.frame.sel_cbox.Bind(wx.EVT_COMBOBOX, partial(rdp_gui_events.evt_sel_cbox, parent=self.frame))
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
        
        self.frame.mode_cbox.Disable()
        self.frame.proj_method_cbox.Disable()
        self.frame.style_cbox.Disable()
        
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
        
        rdp.guifuncs.initialize_start_values(parent=self)
        self.frame.menu_file = wx.Menu()
        menu_load = self.frame.menu_file.Append(1, 'Load configuration')
        menu_switch = self.frame.menu_file.Append(2, 'Switch directory')
        self.frame.menu_file.AppendSeparator()
        menu_exit = self.frame.menu_file.Append(3, 'Exit')
        self.frame.menu_bar = wx.MenuBar()
        self.frame.menu_bar.Append(self.frame.menu_file, 'File')
        self.frame.SetMenuBar(self.frame.menu_bar)
                
        # Labels
        #========================================================================================================================
                
        if self.frame.parent is None:
            self.time_stamp_display = wx.StaticText(self, -1, '-')
            self.time_stamp_label = wx.StaticText(self, -1, 'Time stamp t \t')
            
        #========================================================================================================================
        
        self.line = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        
        # Control widgets
        #========================================================================================================================
                
        if self.frame.parent is None:
            self.pos_slider = wx.Slider(self, value=0, minValue = 0, maxValue = 1, style = wx.SL_LABELS|wx.SL_AUTOTICKS)
            
        #========================================================================================================================

        for i in range(len(self.flist_sorted)):
            if i == 0:
                self.ndim = []
            
            self.header_list.append(rdp.datatools.get_csv_header(csv_f=self.dir + '/' + self.flist_sorted[i]))
            self.ndim.append(rdp.datatools.get_dimension(self.header_list[i]))
        
        # Sizers
        #========================================================================================================================
        self.pos_slider_sizer = wx.BoxSizer(wx.VERTICAL)
        self.player_caption_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.time_stamp_sizer = wx.BoxSizer(wx.HORIZONTAL)        
        self.line_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.player_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.player_time_stamp_sizer = wx.BoxSizer(wx.VERTICAL)
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
        
        if self.frame.parent is None:
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
                    
        # Build main sizer
        #========================================================================================================================
        self.line_sizer.Add(item=self.line, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        self.main_sizer.Add(self.line_sizer, 0, wx.ALIGN_CENTER_HORIZONTAL)
        self.main_sizer.Add(item=self.player_time_stamp_sizer, proportion=0, flag=wx.ALIGN_CENTER|wx.LEFT|wx.RIGHT|wx.EXPAND, border=10)
                    
        # Event handling
        #========================================================================================================================
        if self.frame.parent is None:
            self.frame.Bind(wx.EVT_MENU, partial(self.rdp_gui_events.evt_load_config_btn, parent=self), menu_load)
            self.frame.Bind(wx.EVT_MENU, partial(self.rdp_gui_events.evt_switch_btn, parent=self), menu_switch)
            self.frame.Bind(wx.EVT_MENU, self.rdp_gui_events.evt_end_app, menu_exit)
            self.pos_slider.Bind(wx.EVT_COMMAND_SCROLL_THUMBTRACK, partial(self.rdp_gui_events.evt_pos_slider, parent=self))
            self.pos_slider.Bind(wx.EVT_COMMAND_SCROLL_CHANGED, partial(self.rdp_gui_events.evt_pos_slider, parent=self))       
            self.play_pause_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_play_pause_btn, parent=self))
            self.reverse_play_pause_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_reverse_play_pause_btn, parent=self))
            self.increase_single_step_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_increase_single_step_btn, parent=self))
            self.decrease_single_step_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_decrease_single_step_btn, parent=self))
            self.reset_btn.Bind(wx.EVT_BUTTON, partial(self.rdp_gui_events.evt_reset_btn, parent=self))
                  
        # Layout
        #========================================================================================================================
        self.SetSizerAndFit(self.main_sizer)
        self.rdp_frame = RDPSetupFrame(parent=self, title=self.sel_cbox_selection)
        RDPPlotFrame(parent=self, panel=self.rdp_frame.frame.panel)
        
        return
                        
#========================================================================================================================

if __name__ == '__main__':
    app = RDPApp()
    app.MainLoop()