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

    File:        plotfuncs.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 30

    Description: 

    Credits:

========================================================================================================================
'''
import matplotlib as mpl
mpl.use('WXAgg')

import math
import matplotlib.pyplot as plt
import numpy as np
import os
import re
import wx

import datatools
import guifuncs

from matplotlib.collections import PolyCollection
from matplotlib.lines import Line2D
from scipy import ndimage


def build_proj_ch_step(ndim, temp_proj_ch_step):
    '''Build the various projection choices for snapshot plots.'''
    
    # Empty (default) selection option
    proj_ch_step = []
    
    if ndim != 1:
        for j in range(ndim+1):
            for k in range(len(temp_proj_ch_step)):
                
                # temp_proj_ch_step[k] contains 1 axis
                if 'x_'+ str(j) == temp_proj_ch_step[k]:
                    proj_ch_step.append(temp_proj_ch_step[k])
                    
                # temp_proj_ch_step[k] contains 2 axes
                elif 'x_'+ str(j) in temp_proj_ch_step[k]:
                    count = 0
                    for l in range(len(re.findall('\d', temp_proj_ch_step[k]))):
                        if int(re.findall('\d', temp_proj_ch_step[k])[l]) <= ndim:
                            count += 1
                            if count == 2 and temp_proj_ch_step[k] not in proj_ch_step:
                                proj_ch_step.append(temp_proj_ch_step[k])
        
        # sort list by length of entries
        proj_ch_step.sort(key=lambda t: len(t))
    
    return proj_ch_step


def project(mode, steps, data, header, proj, proj_method='average'):
    '''Project data onto the given axis.'''
    
    ndim = datatools.get_dimension(header)
    X = np.zeros(ndim)

    for i in range(ndim):
        X[i] = int(header[i+2])
        
        if proj == 'x_' + str(i+1): 
            col = (ndim-1) - i
                        
    X = X[::-1]
    new_X = None
    
    for i in range(steps):
        try:
            aux_col = col
            aux_X = np.reshape(data[i], X)
            j = 0
                        
            while np.ndim(aux_X) > 1:
                if j != aux_col:   
                                            
                    # dimensionality reduction along j
                    if proj_method == 'maximum':
                        aux_X = np.maximum.reduce(array=aux_X, axis=j)
                        
                    else:
                        div = aux_X.shape[j]
                        aux_X = np.add.reduce(array=aux_X, axis=j)
                        
                        if proj_method == 'average':                        
                            aux_X = np.true_divide(aux_X, div)
                    
                    if j < aux_col:
                        aux_col -= 1
    
                else:
                    j += 1
                    continue
                        
            if new_X is None:
                new_X = np.zeros((steps, aux_X.shape[0]))
            
            new_X[i] = aux_X
    
            X_1,X_2 = np.mgrid[:steps, :X[col]]
            Z = new_X
        
        except UnboundLocalError:
            raise UnboundLocalError()
            
    
    return X_1, X_2, Z


def project2D(step, data, header, proj, proj_method='average'):
    '''Project data onto 2 given axes.'''
    
    ndim = datatools.get_dimension(header)
    X = np.zeros(ndim)
    col = np.zeros(2)
    
    # Extract dimensions on which data is to be projected from proj tuple
    proj = proj.split(',')
    proj[0] = proj[0].strip()
    proj[1] = proj[1].strip()
          
    for i in range(ndim):
        X[i] = int(header[i+2])
        
        if   proj[0] == 'x_' + str(i+1): 
            col[0] = i
        elif proj[1] == 'x_' + str(i+1): 
            col[1] = i
            
    X = X[::-1]
    try:
        aux_X = np.reshape(data[step], X)
    except IndexError:
        raise IndexError
        
    aux_col = col
    j = 0
    
    for i in range(aux_col.shape[0]):
        aux_col[i] = math.fabs((ndim-1)-aux_col[i])
        
    while np.ndim(aux_X) > 2:
        if j != aux_col[0] and j != aux_col[1]:
            # dimensionality reduction along j
            if proj_method == 'maximum':
                aux_X = np.maximum.reduce(array=aux_X, axis=j)
                
            else:
                div = aux_X.shape[j]
                aux_X = np.add.reduce(array=aux_X, axis=j)
                if proj_method == 'average':
                    aux_X = np.true_divide(aux_X, div)
            
            if j < aux_col[0]: 
                aux_col[0] -= 1
            if j < aux_col[1]: 
                aux_col[1] -= 1
                
        else:
            j += 1
    
    X_1, X_2 = np.mgrid[:aux_X.shape[0], :aux_X.shape[1]]
    Z = aux_X
    
    return X_1, X_2, Z

    
def set_marker(step, data, plot, style, marker_color='#FF9600'):
    '''Mark the given time slice in a time course plot.'''
                
    #========================================================================================================================
                
    if style == '' or style == 'heatmap':
        # Mark step with line
        plot.axvline(x=step, color=marker_color)
        
    else:
        min_data = 9999999
        max_data = -9999999
    
        # set marker size
        #========================================================================================================================
        for x in np.nditer(data):
            if x > max_data:
                max_data = x
        
        for x in np.nditer(data):
            if x < min_data:
                min_data = x
                
        xs = np.arange(0, data.shape[1], 0.1)
        ys = np.zeros(xs.shape[0])
        ys.fill(min_data)
        ys[0], ys[-1] = max_data, max_data
            
        v = []
        v.append(list(zip(xs, ys)))
        
        if style == 'wireframe':
            cross = PolyCollection(v, facecolors=marker_color, closed=False)
        elif style == 'surface':
            cross = PolyCollection(v, facecolors=marker_color, closed=False)
        
        cross.set_alpha(0.25)
        
        # Mark x with either blue (wireframe) or red (surface) plane
        plot.add_collection3d(cross, zs=step, zdir='x')
                
    return plot

        
def initialize_3D_plot(mode=None, figure=None, title=''):
    
    if mode == 'snapshot sequence':
        fig = plt.figure()
        fig.canvas.set_window_title(str(title))
        fig.canvas.supports_blit = True
        
    else:
        if figure is None:
            fig = plt.figure()
            fig.canvas.set_window_title(str(title))        
            fig.canvas.supports_blit = True
        else:
            fig = figure
    
    plot = fig.add_subplot(1, 1, 1, projection='3d')
                        
    # make grid background transparent
    plot.w_xaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
    plot.w_yaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
    plot.w_zaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
            
    return plot


def process_image(data, header, step):
    '''Convert data read from cedar-recorded csv file into a numpy array fit for matplotlib plotting.'''
    
    img_data = data[step]
    x_1 = int(header[2])
    x_2 = int(header[3])
    
    #RGB image
    #========================================================================================================================
    if (img_data.shape[0]/x_1)/x_2 == 3:

        img_array = np.zeros((x_2, x_1, 3), 'uint8')
        img_array_red = np.reshape(img_data[2::3], (x_2, x_1))
        img_array_green = np.reshape(img_data[1::3], (x_2, x_1))
        img_array_blue = np.reshape(img_data[0::3], (x_2, x_1))
        
        img_array[..., 0] = img_array_red
        img_array[..., 1] = img_array_green
        img_array[..., 2] = img_array_blue
        
        img_array = ndimage.rotate(img_array, 90)
        
        return img_array
    #========================================================================================================================

    #Greyscale image
    #========================================================================================================================
    elif (img_data.shape[0]/x_1)/x_2 == 1:
        img_array = np.reshape(img_data, (x_2, x_1))
        img_array = ndimage.rotate(img_array, 90)

        return img_array


def plot_snapshot(step, data, vmin, vmax, stride, header, style, surface_linewidth, surface_cmap, mode=' ', proj='', linestyle='solid', proj_method='average', color='#FF9600', figure=None, title=None):        
    ndim = datatools.get_dimension(header)
    steps = data.shape[0]
            
    if linestyle == 'dash dot':
        linestyle = '-.'
                    
    if figure == None:
        fig = plt.figure()
    else:
        fig = figure
                
    if style == 'image':
        try:
            image = process_image(data=data, header=header, step=step)                
            plot = plt.imshow(image, origin='lower', rasterized=True)
            
            return plot, data
            
        except TypeError:
            dlg = wx.MessageDialog(parent = None, 
                                   message = 'The plot was not updated.', 
                                   caption = ' ' , 
                                   style = wx.OK|wx.ICON_INFORMATION|wx.CENTER|wx.STAY_ON_TOP)
            dlg.ShowModal()
            dlg.Hide()
            wx.CallAfter(dlg.Destroy)
    
    else:                    
        if ndim == 1:                
            plot = fig.add_subplot(1,1,1)
            plot.plot(data[step], color=color, linestyle=linestyle)
        
        elif ndim != 1:
            if proj != ' ':
                                
                if ',' in proj:
                    
                    try:
                        X_1, X_2, data = project2D(step=step, data=data, header=header, proj=proj, proj_method=proj_method)
                        
                    except IndexError:
                        dlg = wx.MessageDialog(parent = None, 
                                               message = 'The ' + str(step) + '. snapshot does not exist.', 
                                               caption = 'An Error has occurred.', 
                                               style = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                        
                        dlg.ShowModal()
                        dlg.Hide()
                        wx.CallAfter(dlg.Destroy)
                        
                    if style != 'heatmap':
                        plot = initialize_3D_plot(mode=mode, figure=figure, title=title)
                        
                        if style == 'surface':
                            plot.plot_surface(X_1, X_2, data,rstride=stride, cstride=stride,cmap=surface_cmap, alpha=0.5, linewidth=surface_linewidth, rasterized=True)
                        elif style == 'wireframe':
                            plot.plot_wireframe(X_1,X_2, data, rstride=stride,cstride=stride, color=color, rasterized=True)
                        
                    elif style == 'heatmap':
                        plot = plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode, figure=figure)
                                                
                elif ',' not in proj:
                    
                    try:
                        data = project(mode=mode, steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)[2]    
                    except UnboundLocalError:
                        raise UnboundLocalError
                    
                    if mode == 'snapshot sequence':
                        fig = plt.figure()
                    else:
                        fig = figure
                
                    plot = fig.gca()
                    
                    try:
                        plot.plot(data[step], color=color, linestyle=linestyle)
                    except:
                        fig = plt.figure()
                        plot = fig.gca()
                        plot.plot(data[step], color=color, linestyle=linestyle)
                                        
            elif proj == ' ':
                
                if ndim == 2:
                    x_1 = int(header[2])
                    x_2 = int(header[3])                    
                    
                    if style != 'image':
                        data = np.reshape(data[step], (x_2, x_1))
                        X_1, X_2 = np.mgrid[:x_2, :x_1]
                
                    if style != 'heatmap' and style != 'image':
                        plot = initialize_3D_plot(mode=mode, figure=figure, title=title)
        
                        if style == 'surface': 
                            plot.plot_surface(X_1,X_2,data,rstride=stride, cstride=stride,cmap='coolwarm', alpha=0.5, linewidth=surface_linewidth, rasterized=False)
                        elif style == 'wireframe':
                            plot.plot_wireframe(X_1,X_2,data, rstride=stride,cstride=stride, color=color)
                        
                    elif style == 'heatmap':
                        plot = plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode, figure=figure)
        
        try:
            return plot, data

        except UnboundLocalError:
            pass
        
    
def plot_time_course(data, header, vmin, vmax, stride, surface_linewidth, surface_cmap, proj, proj_method, style, linestyle='solid', color='#FF9600', plot=None, marker=False, step=None, 
                     marker_color=None, figure=None, title=None):
    
    ndim = datatools.get_dimension(header)
    steps = data.shape[0]
    fig = figure
    plot = fig.gca()

    
    if ndim == 0:
        x_values = np.arange(0, steps)
        x_values.shape = data.shape
        plot.plot(x_values, data, linestyle=linestyle, color=color)
        plt.ylim(float(1.2*(data.min())), float(1.2*(data.max())))
        
    if ndim != 0:
        if ndim == 1:
            x = int(header[2])
            X_1,X_2 = np.mgrid[:steps, :x]
        
        elif ndim >= 2:
            try:
                X_1, X_2, data = project(mode='time course', steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)
            
            except UnboundLocalError:
                raise UnboundLocalError
            
        if style != 'heatmap':
            plot = initialize_3D_plot(figure=figure, title=title)
            
            if style == 'surface':   
                plot.plot_surface(X_1, X_2, data, rstride=stride, cstride=stride, cmap=surface_cmap, alpha=0.5, linewidth=surface_linewidth, rasterized=True)
            elif style == 'wireframe': 
                plot.plot_wireframe(X_1, X_2, data, rstride=stride, cstride=stride, color=color, rasterized=True)
        
        elif style == 'heatmap':
            plot = plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, figure=figure)
    
    # mark the selected time-slice if true                              
    if marker == True:
        plot = set_marker(step=step, data=data, plot=plot, style=style, marker_color=marker_color)
          
    return plot


def plot_heatmap(X_1, X_2, data, vmin, vmax, mode=None, figure=None):
    '''Plot data as either one heatmap or a sequence of heatmaps.'''
    
    # Set plot minimum/maximum either to given values or to data minimum/maximum
    if vmax is None:
        vmax = data.max()
    if vmin is None:
        vmin = data.min()
            
    # Either plot each plot in a separate figure (snapshot sequence) or overwrite existing figure with new plot (otherwise)        
    if mode == 'snapshot sequence':
        fig = plt.figure()
    else:
        fig = figure
    
    try:
        plot = fig.gca()
        heatmap_collection = plt.pcolormesh(X_1, X_2, data, cmap='RdYlBu_r', vmin=vmin, vmax=vmax, rasterized=True)
        plot.add_collection(heatmap_collection)
        
        plot.set_ylim([X_2.min(), X_2.max()])
        plot.set_xlim([X_1.min(), X_1.max()])
                    
        return plot
    
    except AttributeError:
        pass
                    

def label_axis(plot, x_label, y_label, z_label=None):
    '''Adds axis labels to an existing plot.'''
    
    try:
        colorbar = None
    except UnboundLocalError:
        pass
    
    # Plot is no heatmap
    plot.set_xlabel(x_label)
    plot.set_ylabel(y_label)
    
    # Plot is in 3D
    if 'Axes3DSubplot' in str(type(plot)):
        plot.set_zlabel(z_label)
    
    # Plot is a heatmap
    elif 'matplotlib.axes.AxesSubplot' in str(type(plot)):
        try:
            colorbar = plt.colorbar(ax=plot)        
            colorbar.set_label(z_label)
                
        except RuntimeError:
            pass
    
    return


def save_plot(plot, plot_mode, file_name, file_directory, save_mode='single', plot_number=0, figure=None, file_path=None):
    '''Saves either a plot or a sequence of plots to one/several svg files.'''
    
    if file_path == None:
        count = 1       
        file_path_partial = None
        
        if plot_mode == 'snapshot sequence': 
            plot_mode = 'snapshot_sequence'
        if plot_mode == 'time course':
            plot_mode = 'time_course'
                    
        # Snapshot or time course
        if save_mode == 'single':
            file_path = file_directory + '/' + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(count) + '.svg'
            
            while os.path.exists(file_path):
                count += 1
                file_path = file_directory + '/' + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(count) + '.svg'
            
        # Snapshot Sequence
        elif save_mode == 'sequence':
            sequence_number = 1
            file_path_partial = file_directory + '/' + file_name.strip('.csv') + '_sequence_' + str(sequence_number) + '/'
            
            if not os.path.exists(file_path_partial):
                os.mkdir(file_path_partial)
                
            elif os.path.exists(file_path_partial):
                
                while os.path.exists(file_path_partial):
                    sequence_number += 1
                    file_path_partial = file_directory + '/' + file_name.strip('.csv') + '_sequence_' + str(sequence_number) + '/'
                
                # If the plot is the first one of a new sequence generate new folder
                if plot_number == 0:
                    os.mkdir(file_path_partial)
                # Else fall back to the last one
                else:
                    file_path_partial = file_directory + '/' + file_name.strip('.csv') + '_sequence_' + str(sequence_number-1) + '/'
                                       
            file_path = file_path_partial + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(count) + '.svg'
                                
            while os.path.exists(file_path):
                if file_path_partial is not None:
                    count += 1
                    file_path = file_path_partial + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(count) + '.svg'
    try:
        figure.savefig(file_path, dpi=400, bbox_inches='tight', pad_inches=0.2, transparent=True)
    except RuntimeError:
        figure.savefig(file_path, dpi=400, bbox_inches='tight', pad_inches=0.2, transparent=True)


def update_plot(parent):
                      
    if parent.ndim[parent.selection] == 0:
        plt.cla()
    
    else:
        try:
            parent.figure.clf()
        except AttributeError:
            pass
    
    plot(parent=parent)    

    
def plot(parent, save=False, file_path=None):
    '''Build plot(s) to either visualize or save as pdf file'''
     
    if save is True:
        save_plot(plot=parent.plot, plot_mode=parent.mode, file_name=parent.flist_sorted[parent.selection], file_directory=parent.dir, figure=parent.figure, file_path=file_path)
        update_plot(parent=parent)
    
    else:         
        
        if parent.figure == None:
            parent.figure = plt.figure(str(parent.title))
                                                                   
        if parent.mode == 'snapshot' or parent.style == 'image':

            try:
                parent.plot, parent.reduced_data = plot_snapshot(step = parent.step, 
                                                              style = parent.style, 
                                                              data = parent.data, 
                                                              header = parent.header, 
                                                              vmin = parent.vmin,
                                                              vmax = parent.vmax,
                                                              stride = parent.stride, 
                                                              surface_linewidth = parent.surface_linewidth,
                                                              surface_cmap=parent.surface_cmap,
                                                              proj = parent.proj,
                                                              linestyle = parent.line_style,
                                                              proj_method = parent.proj_method,
                                                              color = parent.line_color,
                                                              figure = parent.figure,
                                                              title = parent.title)
                                                
            except IndexError:
                dlg = wx.MessageDialog(parent=None, 
                                       message = 'The specified time slice does not exist.', 
                                       caption = 'An Error has occurred.', 
                                       style = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                
                dlg.ShowModal()
                dlg.Hide()
                wx.CallAfter(dlg.Destroy)
                                
        elif parent.mode == 'time course':
            parent.plot = plot_time_course(data = parent.data, 
                                           header = parent.header, 
                                           vmin = parent.vmin,
                                           vmax = parent.vmax,
                                           stride = parent.stride,
                                           surface_linewidth = parent.surface_linewidth,
                                           surface_cmap=parent.surface_cmap,
                                           plot = parent.plot,
                                           linestyle=parent.line_style,
                                           proj = parent.proj,
                                           proj_method = parent.proj_method,
                                           color = parent.line_color,
                                           step = parent.step, 
                                           marker = parent.marked, 
                                           style = parent.style,
                                           marker_color = parent.marker_color,
                                           figure = parent.figure,
                                           title = parent.title)
            
            if parent.ndim[parent.selection] == 0:

                for i in range(len(parent.lines)):
                    if parent.lines[i].get_data() != parent.plot.get_lines()[-1].get_data():                   
                        parent.lines.append(parent.plot.get_lines()[-1])
            
            if parent.x_label == '' or parent.x_label == ' ':
                parent.x_label = 'Time'
                try:
                    parent.plot.set_xlabel(parent.x_label)
                except AttributeError:
                    pass
                                
        if parent.style == 'heatmap' or parent.style == 'surface' or parent.style == 'wireframe':
            parent.figure.gca().invert_yaxis()
                        
        try:
            label_axis(plot=parent.plot, x_label=parent.x_label, y_label=parent.y_label, z_label=parent.z_label)
        except AttributeError:
            pass
                        
        if parent.style != 'heatmap':
            if parent.figure_azimuth is not None and parent.figure_elevation is not None:
                parent.figure.gca(projection='3d').view_init(elev=parent.figure_elevation, azim=parent.figure_azimuth)
                
            if parent.figure_distance is not None:
                parent.figure.gca(projection='3d').dist = parent.figure_distance
                
        if save == False:
            parent.plot = guifuncs.enforce_labelling_mode(plot=parent.plot, labelling_mode=parent.labelling_mode, style=parent.style, axis_ticks=parent.axis_ticks)
            try:
                parent.figure_canvas.draw()
            except AttributeError:
                pass
            

def add_time_course(parent, ydata, linestyle, color, plot):
    x_values = np.arange(0, ydata[0].shape[0])
    x_values.shape = ydata[0].shape
    new_line = Line2D(x_values, ydata[0], linestyle=linestyle, color=color)
    plot.add_line(new_line)
