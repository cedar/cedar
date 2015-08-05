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
    Date:        2015 08 05

    Description: 

    Credits:

========================================================================================================================
'''

import csv
import matplotlib as mpl

mpl.use('WXAgg')

import matplotlib.pyplot as plt
import math
import numpy as np
import os

try:
    import cPickle as pickle
except ImportError:
    import pickle
    
import re
import sys
import warnings

warnings.filterwarnings('ignore', category=UserWarning)

import wx

from functools import partial
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas
from matplotlib.collections import PolyCollection
from mpl_toolkits.mplot3d import Axes3D
from scipy import ndimage

from wx.lib.embeddedimage import PyEmbeddedImage

# Prevent Eclipse warning from Axes3D import
Axes3D


#========================================================================================================================

class RDPImageFiles():
    play = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAABmJLR0QA/wD/AP+gvaeTAAAA"
        "CXBIWXMAAA3XAAAN1wFCKJt4AAAAB3RJTUUH3gsMCgc1zYOOZAAAAYtJREFUSMfVlr9Kw1AU"
        "h79IRaVDpGv/7DrqA5R2an2AtKPabi66iJMgCKUPoIMpFnwBnyAVHAxdCw7tmOozdLE5Tleu"
        "IbVJGhF/ELiE3PNx7sk5vwt/rK3fDH4GiPY8AcdpAvYDAAF8bX0LbK8KeVRBe72etNttyWaz"
        "EgK2gc2kEAGk2WyKrsFgIJVKJQx2ERdQVJv7/b74vv8FUevJZCLVajUIegd2o0IO1MbRaCRh"
        "UrDhcCilUilYs6sokBMFmc1m8pMUrNvtBrNyl0Gu1cdxNJ1OpVAo6KApsL4IcgNIJpOJBVFZ"
        "1et1/fheF0HuATFNU+JKgSzL0jM6CoM8AJLL5SSJFEjLpq8Cr2mQD4D5fB6/uUQwDAPLstQr"
        "A3hOvSa1Wi1STRL9XZ7nST6f12vhAZnU+qTT6QT75GXZ0dajdrzrulIsFoMdf5nK7BqPx/qg"
        "VMHfgJ3YU7jRaHzLwHEcKZfLYVP4fCU/sW1bWq3WIj+5AzaS+sneEme8Acw0LPg0AHGAw395"
        "W0ldn0BVyth0l+oCAAAAAElFTkSuQmCC")
    
    reverse_play = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAABmJLR0QA/wD/AP+gvaeTAAAA"
        "CXBIWXMAAA3XAAAN1wFCKJt4AAAAB3RJTUUH3gsMDiYjssGGCgAAAZJJREFUSMfVlr1OwlAY"
        "ht+aGjUMNaxQdh31AkiZqBdQGFXYXHQxTiYmJoQL0MESSbwBrwBMHGxYSRxgbPUaWKSvgx7z"
        "WfkptQw8SZMOJ+9zzvn6nVNgxdhaVvAxgCcAFM9ZGsHbAG5FaBiREMB+0vBNAG40MJPJsF6v"
        "s9VqSeljEsFFNNyyLHa7XUqq1aocE5tdAO8yvFQqcTgckiTDMPwRhGHIdrstJWYcwZXc80Kh"
        "wF6v9ydc0u/3peRgnsCTs282mzPDFaPRSEpOpoWvAwjUwHw+zyAIuAhCcj1N8qq2x7btWLOP"
        "ouu6ktxMEhypWTiOk0hAkoZhKMn9JElbrSKpgCSz2aySPKjgNSF5BqABgOM40DQNX1u8GOPx"
        "WL1+zK1JuVxeSk0AQAfgq9rkcjn6vp/616V4kX3SaDRS7RPJpex40zTped4iHW/HreMOgDcp"
        "syyLg8EgtbNLch49hYvFIjudzq+VVCqVRKewZAPA3aT7pFar0XXdf98nEuP785x1M+6leccf"
        "AuhEBKcr+bcSi09pHsrYk6RaDwAAAABJRU5ErkJggg==")

    pause = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAABmJLR0QA/wD/AP+gvaeTAAAA"
        "CXBIWXMAAA3XAAAN1wFCKJt4AAAAB3RJTUUH3gsMCg4cXvOtQQAAAZVJREFUSMftlrFKw0AY"
        "x3+apKHNkgZbULC+g7jqoLM46STio5S+gauCk4u4+ARSirMPIPUBpKlZpZfzXO7CeYZ6wbUH"
        "JV/z+933vyxfAqvVYK0tYXvAKZACz8AdoJy9J8Chrh+ASZPwS0DqpuZ37Ti3DpfAeZOQKaA6"
        "nY7Kssw0+QIGmg/0f5VlmWq328aZ+gasAwJQo9FIjcdj+7T72jkw9yaTiRoOh4Yv9P5fDd3V"
        "B0KAfr9Pr9ez2aa+blXyTycCNnxCMlN0u13SNLVZ6uF0fUKiqogiWq2WzVoeTuQTElZFGBJF"
        "UR3zcfxDwjCsO6WPszQksBsEQVDHqptBELghgU9IWRVliZTSZtJ1pJSUZVnn+IUIIRBC2Ew0"
        "cJaGCLuBc8qygeMfslgs6piPszRkboqiKCiKoo75OH+GlAB5npPnuc3enSuz2Yz5fG4/xYfv"
        "kHwDVBzHKkkSe5Rva75jpnCSJCqOY+O8Nhn1F/pp7Al85Tg3Dhf6JdfozbgLnAEd4Al4rNl7"
        "DBwBn8A98LL62Pj3+gaJWayoxGFTWwAAAABJRU5ErkJggg==")
    
    reset = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAABmJLR0QA/wD/AP+gvaeTAAAA"
        "CXBIWXMAAA3XAAAN1wFCKJt4AAAAB3RJTUUH3gsMChUs0RxWdwAAAVJJREFUSMftlU8rRVEU"
        "xX+PnvSSCBk8ysSEqUyRkie9MhTyAYyMDQ3uV8BIRCiGeogv4BsYGZAXI5MXRUzOqd3u3Hv3"
        "yZ+BrDp1T3ets/a6Z7cv/BU0R/LbgTLwDrx+ZyGzwBHwBHyI9QgcAjNfObwPuFQHp60LlzAK"
        "o0DdaOBX3enMCbRBDZgGio7TAlSAc8V7sCY6E6IGsJDDX3Q8WVAmJlRlS8b0y0o3nkXeFcTT"
        "yHusCe1OFlHeRTXSZE7dTRBtKnJHpEm30pdCpH5BeAMKkSZNbhL4M4JdVlKVdImEFvQqfat0"
        "92gA92I/DxwAa0aTMfF8B7ykETdFJT56YjAoANdCu6G/o5zIzynv8rAKjIj9dojUCVylzKSt"
        "HINh1yief5JFHgSOAyb7OSZTgnsL9FiiTwI3QpgYWncd2AOGYnq+CKy4H1XCD2PAjfh//B4+"
        "Ac1ehBpa7nlwAAAAAElFTkSuQmCC")
    
    decrease_single_step = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAABmJLR0QA/wD/AP+gvaeTAAAA"
        "CXBIWXMAAA3XAAAN1wFCKJt4AAAAB3RJTUUH3gsMDgUx/9GAIwAAApVJREFUSMftlr9rE3EU"
        "wD9p7pJeQiEXgjgpsUinCsGhBKSDCEVRdFEEnbs0dOg5ODi0iLi0HVrORfsXOOrgErRFSBBs"
        "lw4NtdJNtGjaEiO5u/Tr4F37clxjdO6DL9z7ft/7ft77/njfgxP5B4mH9FHgAXAV6AdqPc6T"
        "A64D94AzwCbgRhneB9qAEu1Fl4k14BbwJsLvHdAX5fQJUIZhqGw2Gxgf+JFJiQG3/SzlxCqR"
        "SEj9ZhjQBziAmp6eVisrK9J4VNhdAKpyYtM0lWVZqlqtqv39fZVOp4OxZ2HIqcDJtm21sbEh"
        "IXcBHXgcBAKobDarZmdnVaPRUFLGxsYCv3WZAYAZdGQyGUzTlAGcB94DjwBd0zSmpqbY2trC"
        "sizS6XRHtMPDw8HnOX9p0fwOPRhJJBLoui79HgIpgEKhwNLSEoVC4djTkM/ng08DOA186RMn"
        "5Q9N19E0TfqlACYmJqhUKl0BALlcTqqZyEw0TeuAxGIxFhYWKJVKPV0YwzA6VLkncQmJx4/u"
        "6NDQUM+AYLmFJCWkHfR6nke7fahSq9VYXFzsGeI4jlRbEnJYAlzXxXWPKoJSisnJSUqlEq1W"
        "66+QZrMp1V8S4slMJARoAti2TbFYZHV1tStkZ2dHqj8iM3EcJwx5CnwAWFtbY2RkBMuy2N3d"
        "jYRsb2/L4L5JyPdgpF6vU6/XpV8NuAQ8AVzP85ifn2dwcJC5uTkajUYHZH398KJ/9m9+R+1y"
        "ATUzM6OWl5e71a6KrF0DAwNqfHxclctltbe3p1KpVDBmR2W6CahkMimL3AFwNqIK3wA+hqtw"
        "qF2LgtzxD4A0fN5lj2PAFeB1xHvyVr4nsZDjRR/WD5SBV+F17fIyXgbywFfgJfDz5Mfjv+Q3"
        "/MwNiDUDxl8AAAAASUVORK5CYII=")
    
    increase_single_step = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAABmJLR0QA/wD/AP+gvaeTAAAA"
        "CXBIWXMAAA3XAAAN1wFCKJt4AAAAB3RJTUUH3gsMChYCJucIewAAAqBJREFUSMftlTFME2EU"
        "x3+1d72ct/QqUScNEsMiJl3sYlw0ISEh6oBxYCQxkYaBWxwcSIxxoSwFF90Jzg5OaooJTYOQ"
        "mA40iuKIWEpKRdtr/Vz6lcflLLDzkkvue/d/7/+9d9/3f3Bix7BoYO0Ao8A94AywCewdIU8E"
        "uAs8BIaAX8D3MOAp4D2gxNMC3gB3AKMLycuQuNEw4G0NisViKhCkgBIw0t61tAvAX0AlEgll"
        "27bGfwkjeQ4ox3FUtVpV+XxeeZ6nXNcNkuWBqyLuhv6Wy+XU1NSUxjXa3TlgRUANDg4qabVa"
        "TU1PT6tEIiGJGsATwATua//a2pqam5uTuLMIpghwCWBgYODgSXAcPM9jfX2dyclJDMOgnfwx"
        "8AG4rLGu6xKPx2W4K0nOAzZAb29v6J+Nx+NkMhkKhQLJZFK7rwGP9MI0TWKxmAwzJUmHvqen"
        "p+tZTSaTLC0tMT4+rl2n9YthGJimKeGGJLG117btQy+FZVnMzs6SzWaJRPYPm2EYup2hlVja"
        "Gyi3q6XTafr7+/dvdjQaJIlKkrr2NhqNI5Nks1lKpVJn3Wq1aDabEtKSJL+1d2/vcBWp1+uk"
        "02kmJiZQSnX8vu/j+76E+gip2Nbera2trgQrKyuMjY2xurra2Zf++b7vBytpykp+aCHc2NgI"
        "Tb6zs4PneaRSKUlQAJ7JSgLt9iWJAr4CFIvFA8lrtRqZTIa+vj5mZmb0Tn3gKXC9rWkAVCoV"
        "KpWKDC8TUNYccGVxcZFqtcry8jILCwvMz8+zu7srA/PAA+BTe70pW10ul2WrtoMdGQpRXvl8"
        "BIZDVPiiVmHHcZRlWRr/+X/z5F3IXHgN3ApJLu1FIK7ZHnwdYQxOxhHgHPANeAv8POJkHAZu"
        "An+AV+3KT+z49g/v5g/7k3CcCwAAAABJRU5ErkJggg==")
    
#========================================================================================================================

class Progress(wx.ProgressDialog):
    '''Simple progress bar'''
    def __init__(self, parent, _, title, message, maximum):
        wx.ProgressDialog.__init__(self, title, message=message, maximum=maximum, parent=parent, style=wx.PD_ELAPSED_TIME|wx.PD_REMAINING_TIME|wx.STAY_ON_TOP)
        
        return
        
#========================================================================================================================

class SaveDialog(wx.FileDialog):
    def __init__(self, parent, defaultDir):
        
        wildcard = 'Scalable Vector Graphic (*.svg)|*.svg|'  \
                   'Portable Document Format (*.pdf)|*.pdf|' \
                   'Encapsulated PostScript (*.eps)|*.eps|'  \
                   'All files (*.*)|*.*'
        
        wx.FileDialog.__init__(self, parent=parent, message='Save plot', wildcard=wildcard, style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_PREVIEW, defaultDir=defaultDir)
        
        return
    
#========================================================================================================================

class WriteCSVDialog(wx.FileDialog):
    def __init__(self, parent, defaultDir, defaultFile):
        
        wildcard = 'Comma-separated Values (*.csv)|*.csv|'  \
                   'All files (*.*)|*.*'
        
        wx.FileDialog.__init__(self, parent=parent, message='Write .csv file', wildcard=wildcard, style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_PREVIEW, defaultDir=defaultDir)
        
        return
    
    
class SaveConfigDialog(wx.FileDialog):
    def __init__(self, parent, defaultDir):
        
        wildcard = 'Pickle (*.p)|*.p|'  \
                   'All files (*.*)|*.*'
        
        wx.FileDialog.__init__(self, parent=parent, message='Save Plot configuration', wildcard=wildcard, style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_PREVIEW, defaultDir=defaultDir)
        
        return
    
    
class LoadConfigDialog(wx.FileDialog):
    def __init__(self, parent, defaultDir):
        
        wildcard = 'Pickle files (*.p)|*.p|'  \
                   'All files (*.*)|*.*'
        
        wx.FileDialog.__init__(self, parent=parent, message='Load Plot configuration', wildcard=wildcard, style=wx.FD_OPEN|wx.FD_PREVIEW, defaultDir=defaultDir)
        
        return
    

class SnapshotSequenceDialog(wx.Dialog):
    def __init__(self, parent, _, title):
        mpl.interactive(True)
        wx.Dialog.__init__(self, parent=parent, id=_, title=title)
        
        self.parent = parent
        top_sizer = wx.BoxSizer(wx.VERTICAL)        
        btn_sizer = wx.BoxSizer(wx.HORIZONTAL)
        ok_btn = wx.Button(self, wx.ID_OK, 'OK')
        cancel_btn = wx.Button(self, wx.ID_CANCEL, 'Cancel')
        
        btn_sizer.Add(ok_btn, 1, wx.BOTTOM, border=10)
        btn_sizer.Add(cancel_btn, 1, wx.BOTTOM, border=10)
        
        step_txt = wx.StaticText(self, -1, 'Step options')
        step_num = wx.StaticText(self, -1, 'Number of snapshots')
        step_num_control = wx.SpinCtrl(self, -1, min=0, max=100)
        step_size = wx.StaticText(self, -1, 'Distance between snapshots \t')
        step_size_control = wx.SpinCtrl(self, -1, min=0, max=100)
        step_sizer = wx.FlexGridSizer(rows=2, cols=2)
        step_sizer.SetFlexibleDirection(wx.BOTH)
        
        step_sizer.Add(step_num, 1, wx.ALIGN_LEFT)
        step_sizer.Add(step_num_control, 1, wx.ALIGN_RIGHT|wx.EXPAND)
        step_sizer.Add(step_size, 1, wx.ALIGN_LEFT)
        step_sizer.Add(step_size_control, 1, wx.ALIGN_RIGHT|wx.EXPAND)
        
        # self.parent.proj consists of 2 axes => resulting plot will be 3D
        if len(self.parent.proj) >= 8:
            axes_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
        else:
            axes_grid_sizer = wx.FlexGridSizer(rows=2, cols=2) 
        
        axes_grid_sizer.SetFlexibleDirection(wx.BOTH)
        axes_label_sizer = wx.BoxSizer(wx.VERTICAL)
        min_max_sizer = wx.FlexGridSizer(rows=2, cols=2)         
        min_max_sizer.SetFlexibleDirection(wx.BOTH)
        
        label_axes_txt = wx.StaticText(self, -1, 'Axis labels')
        x_axis_txt = wx.StaticText(self, -1, 'X axis \t')
        y_axis_txt = wx.StaticText(self, -1, 'Y axis \t')
        
        x_axis_label = wx.TextCtrl(self, -1, style=wx.TE_PROCESS_ENTER)
        y_axis_label = wx.TextCtrl(self, -1, style=wx.TE_PROCESS_ENTER)
                
        x_axis_label.SetValue(parent.x_label)
        y_axis_label.SetValue(parent.y_label)
        
        x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
        y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
        
        axes_grid_sizer.Add(x_axis_txt, 0, wx.ALIGN_LEFT|wx.EXPAND)
        axes_grid_sizer.Add(x_axis_label, 1, wx.ALIGN_RIGHT|wx.EXPAND)
        axes_grid_sizer.Add(y_axis_txt, 0, wx.ALIGN_LEFT|wx.EXPAND)
        axes_grid_sizer.Add(y_axis_label, 1, wx.ALIGN_RIGHT)
        
        # self.parent.proj consists of 2 axes => resulting plot will be 3D
        if len(self.parent.proj) >= 8:
            # Change Z axis text depending on plot mode
            
            if parent.style != 'heatmap':
                z_axis_txt = wx.StaticText(self, -1, 'Z axis \t')
            else:
                z_axis_txt = wx.StaticText(self, -1, 'color bar \t')
            
            z_axis_label = wx.TextCtrl(self, -1, style=wx.TE_PROCESS_ENTER)
            z_axis_label.SetValue(self.parent.z_label)
            
            z_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
        
            axes_grid_sizer.Add(z_axis_txt, 0, wx.ALIGN_LEFT|wx.EXPAND)
            axes_grid_sizer.Add(z_axis_label, 1, wx.ALIGN_RIGHT)
        
        axes_label_sizer.Add(axes_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT)
        axes_label_sizer.AddSpacer((10,10))
        top_sizer.Add(item=label_axes_txt, proportion=0, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
        top_sizer.Add(axes_label_sizer, 0, wx.ALIGN_CENTER|wx.TOP, border=10)
        
        top_sizer.Add(item=step_txt, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.TOP|wx.LEFT, border=10)
        top_sizer.Add(step_sizer, 0, wx.ALIGN_CENTER|wx.ALL, border=20)
        top_sizer.Add(btn_sizer, 0, wx.ALIGN_CENTER|wx.RIGHT|wx.LEFT, border=20)
        
        # Event handling
        ok_btn.Bind(wx.EVT_BUTTON, lambda evt, step_num_control=step_num_control, step_size_control=step_size_control: 
                    self.evt_ok_btn(step_num_control, step_size_control, evt))
        
        cancel_btn.Bind(wx.EVT_BUTTON, self.evt_cancel_button)
        
        # Layout
        self.SetSizer(top_sizer)
        top_sizer.Fit(self)
        self.Center(wx.CENTER_ON_SCREEN)
        
    
    def evt_ok_btn(self, step_num_entry, step_size_entry, event):
        
        # Get number of steps as well as step size from widgets
        self.parent.nstep = int(step_num_entry.GetValue())
        self.parent.step_size = int(step_size_entry.GetValue())

        self.parent.plot, self.reduced_data = self.parent.frame.rdp_plot.plot_snapshot_sequence(start = self.parent.step, 
                                                                             step_size = self.parent.step_size, 
                                                                             steps = self.parent.nstep, 
                                                                             style = self.parent.style, 
                                                                             data = self.parent.data, 
                                                                             header = self.parent.header, 
                                                                             vmin = self.parent.vmin,
                                                                             vmax = self.parent.vmax,
                                                                             resolution = self.parent.resolution,
                                                                             surface_linewidth = self.parent.surface_linewidth,
                                                                             proj = self.parent.proj,
                                                                             proj_method = self.parent.proj_method,
                                                                             x_label = self.parent.x_label,
                                                                             y_label = self.parent.y_label,
                                                                             z_label = self.parent.z_label,
                                                                             file_name = self.parent.flist_sorted[self.parent.selection],
                                                                             file_directory = self.parent.dir,
                                                                             save_mode = self.parent.save_mode,
                                                                             color = self.parent.line_color,
                                                                             figure = None)
        
        # Close dialog
        self.Hide()
        mpl.interactive(False)
        wx.CallAfter(self.Destroy)

        
    def evt_cancel_button(self, event):
        #Close dialog
        self.Hide()
        wx.CallAfter(self.Destroy)

#========================================================================================================================

class RDPApp(wx.App):
    def OnInit(self):
        self.main_window = RDPMainWindow(parent=None, title='Recorded Data Processor', pos=None, size=None, style=None, name=None)
        self.SetTopWindow(self.main_window)
                
        return True
            
#========================================================================================================================

class RDPMainWindow(wx.Frame):
    def __init__(self, parent, title, pos, size, style, name):
        
        self.parent = parent
        self.rdp_plot = RDPPlot()
        self.rdp_image_files = RDPImageFiles()

        # search for standard cedarRecordings directory
        if 'HOME' in os.environ:
            self.dir = os.environ['HOME']
        elif os.name == 'posix':
            self.dir = os.path.expanduser("~/")
        elif os.name == 'nt':
            if 'HOMEPATH' in os.environ and 'HOMEDRIVE' in os.environ:
                self.dir = os.environ['HOMEDRIVE'] + os.environ['HOMEPATH']
        else:
            self.dir = os.environ['HOMEPATH']
        
        # Walk through directories until 'cedarRecordings' is found
        for (self.dir, dirs, _) in os.walk(self.dir):
            for j in range(len(dirs)):
                if dirs[j] == 'cedarRecordings':
                    new_dir = dirs[j]
                    self.dir = os.path.join(self.dir, new_dir)
                    break
            
            break
        
        # initialize first frame
        if self.parent is None:
            wx.Frame.__init__(self, parent=None, title=title, style=wx.MINIMIZE_BOX|wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN)  
            
            self.rdp_browser = RDPBrowserPanel(parent=self)
            self.SetSizer(self.rdp_browser.main_sizer)
            self.Bind(wx.EVT_CLOSE, self.evt_end_app)
            self.Sizer.Fit(self)
            self.Show()
        
        # initialize frame for multi-plot purposes
        else:
            wx.Frame.__init__(self, parent=parent, title=' ', style=wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.FRAME_NO_TASKBAR|wx.CLIP_CHILDREN)
                        
            self.rdp_image_files = RDPImageFiles()
            self.dir = self.parent.dir
            self.parent = parent
            self.rdp_gui = RDPGUI(parent=self)
            self.SetSizer(self.rdp_gui.main_sizer)
            self.Sizer.Fit(self)
            self.Show()
            
        return
            
            
    def evt_end_app(self, event):                
        sys.exit()
        
#========================================================================================================================

class RDPBrowserPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.frame = parent
                
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.select_directory = wx.StaticText(self, -1, 'Select directory:')
        
        # File browser
        self.browser = wx.GenericDirCtrl(parent=self, dir=self.frame.dir, size=(325, 450))            
        self.sel_btn = wx.Button(parent=self, label = 'Select')
        self.load_config_btn = wx.Button(parent=self, label = 'Load configuration')
        self.sel_btn.Bind(wx.EVT_BUTTON, self.evt_sel_btn)
        self.load_config_btn.Bind(wx.EVT_BUTTON, self.evt_load_config_btn)
        
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
    
    
    def evt_load_config_btn(self, event):
        rdp_gui = self._initialize_rdp_gui()
        rdp_gui._load_plot_configuration()
        
    
    def evt_sel_btn(self, event):
        '''Switch browser panel with control panel for plot generation/manipulation'''
                      
        self._initialize_rdp_gui()
        
        
    def _initialize_rdp_gui(self):
        frame = self.GetParent()
        frame.dir = self.browser.GetPath()
        
        # Initialize plot generation GUI      
        rdp_gui = RDPGUI(parent=frame)
        frame.SetSizer(rdp_gui.main_sizer)
        frame.Sizer.Fit(frame)
        wx.CallAfter(self.Hide)
        
        return rdp_gui
    
        
#========================================================================================================================

class RDPPlotFrame(wx.Frame):
    def __init__(self, parent):
        
        self.parent = parent
        
        # Initialize control plot frame if none is present
        if parent.control_plot_frame is None:
            parent.control_plot_frame = wx.Frame(parent=parent, id=-1, title=str(parent.title), style=wx.MAXIMIZE_BOX|wx.RESIZE_BORDER|
                                                 wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN|wx.FRAME_NO_TASKBAR)
        
        parent.control_plot_frame.control_plot_panel = wx.PyPanel(parent=parent.control_plot_frame, id=-1)
        parent.control_plot_frame.Bind(wx.EVT_CLOSE, parent.evt_close_figure)
        
        save_btn = wx.Button(parent.control_plot_frame.control_plot_panel, wx.ID_SAVE, label = 'Save')
        write_csv_btn = wx.Button(parent.control_plot_frame.control_plot_panel, label = 'Write .csv')
        save_config_btn = wx.Button(parent.control_plot_frame.control_plot_panel, label = 'Save plot configuration')
        save_btn.Bind(wx.EVT_BUTTON, parent.evt_save_plot_btn)
        write_csv_btn.Bind(wx.EVT_BUTTON, parent.evt_write_csv_btn)
        save_config_btn.Bind(wx.EVT_BUTTON, parent.evt_save_config_btn)
        
        # Sizers
        #========================================================================================================================
        top_sizer = wx.BoxSizer(wx.VERTICAL)
        parent.control_plot_frame.main_sizer = wx.FlexGridSizer(rows=1, cols=2)
        parent.control_plot_frame.main_sizer.SetFlexibleDirection(wx.BOTH)
                
        if 'Axes3DSubplot' in str(type(parent.plot)) or parent.style == 'heatmap':
            axes_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
        else:
            axes_grid_sizer = wx.FlexGridSizer(rows=2, cols=2)
        
        axes_grid_sizer.SetFlexibleDirection(wx.BOTH)
        axes_label_sizer = wx.FlexGridSizer(rows=4, cols=1)
        axes_label_sizer.SetFlexibleDirection(wx.BOTH)
        min_max_sizer = wx.FlexGridSizer(rows=2, cols=2)         
        min_max_sizer.SetFlexibleDirection(wx.BOTH)
        #========================================================================================================================
        
        # Labels
        #========================================================================================================================
        label_axes_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Axis labels')
        x_axis_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'X axis \t')
        y_axis_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Y axis \t')
                
        if parent.style != 'heatmap' and 'Axes3DSubplot' in str(type(parent.plot)):
            z_axis_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Z axis \t')
        elif parent.style == 'heatmap':
            z_axis_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'color bar \t')
        #========================================================================================================================
            
        x_axis_label = wx.TextCtrl(parent.control_plot_frame.control_plot_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        y_axis_label = wx.TextCtrl(parent.control_plot_frame.control_plot_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)        
        
        # Build sizers
        #========================================================================================================================
        axes_grid_sizer.Add(x_axis_txt, 0)
        axes_grid_sizer.Add(x_axis_label, 1)
        axes_grid_sizer.Add(y_axis_txt, 0)
        axes_grid_sizer.Add(y_axis_label, 1)
        
        # Plot is either 3-dimensional or a heatmap
        if 'Axes3DSubplot' in str(type(parent.plot)) or parent.style == 'heatmap':
            z_axis_label = wx.TextCtrl(parent.control_plot_frame.control_plot_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
            axes_grid_sizer.Add(z_axis_txt, 0)
            axes_grid_sizer.Add(z_axis_label, 1)
            x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            z_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            
            if parent.style != 'heatmap':
                plot_parameter_sizer = wx.BoxSizer(wx.VERTICAL)
                plot_parameter_content_sizer = wx.FlexGridSizer(rows=3, cols=2)
                plot_parameter_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Plot parameters')
                
                parent.control_plot_frame.azimuth_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Azimuth\t')
                parent.control_plot_frame.elevation_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Elevation\t')
                parent.control_plot_frame.distance_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Distance\t')
                
                parent.control_plot_frame.azimuth_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, 
                                                                          min=-180, max=180, initial=-60)
                parent.control_plot_frame.elevation_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, 
                                                                            min=-90, max=90, initial=30)
                parent.control_plot_frame.distance_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, 
                                                                           min=10, max=100, initial=10)
                parent.control_plot_frame.azimuth_spn.Bind(wx.EVT_SPINCTRL, parent.evt_azimuth_spn)
                parent.control_plot_frame.elevation_spn.Bind(wx.EVT_SPINCTRL, parent.evt_elevation_spn)
                parent.control_plot_frame.distance_spn.Bind(wx.EVT_SPINCTRL, parent.evt_distance_spn)
                
                plot_parameter_content_sizer.Add(item=parent.control_plot_frame.azimuth_label, proportion=1)
                plot_parameter_content_sizer.Add(item=parent.control_plot_frame.azimuth_spn, proportion=0)
                plot_parameter_content_sizer.Add(item=parent.control_plot_frame.elevation_label, proportion=1)
                plot_parameter_content_sizer.Add(item=parent.control_plot_frame.elevation_spn, proportion=0)
                plot_parameter_content_sizer.Add(item=parent.control_plot_frame.distance_label, proportion=1)
                plot_parameter_content_sizer.Add(item=parent.control_plot_frame.distance_spn, proportion=0)
                
                plot_parameter_sizer.Add(item=plot_parameter_label)
                plot_parameter_sizer.Add(item=plot_parameter_content_sizer)
                
                top_sizer.Add(item=plot_parameter_sizer)
            
        else:
            x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
            y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
        
        plot_labelling_sizer = wx.BoxSizer(wx.HORIZONTAL)
        plot_labelling_tooltipstring = ('Choose the style in which the plot is to be labelled.\n'
                                        '=====================================\n'
                                        'Off:\t\tThere will be no labelling at all.\n'
                                        'LateX:\tFonts will be consistent with the one used in \t\t\tstandard LateX documents. Furthermore, the\n'
                                        '\t\taxis labels will accept input in LateX notation.\n'
                                        '\t\tRendering time will be severely impeded.\n'
                                        'Standard:\tThe plot will be labelled in a simple, sans-serif \t\t\tfont. Input in LateX notation will not be accepted.\n'
                                        '=====================================')
        
        plot_labelling_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Plot Labelling\t')
        plot_labelling_cbox = wx.ComboBox(parent.control_plot_frame.control_plot_panel, choices=parent.labelling_choices, value='Off', style=wx.CB_READONLY)
        plot_labelling_txt.SetToolTipString(plot_labelling_tooltipstring)
        plot_labelling_cbox.SetToolTipString(plot_labelling_tooltipstring)
        plot_labelling_cbox.Bind(wx.EVT_COMBOBOX, self.evt_plot_labelling_cbox)
        
        plot_labelling_sizer.Add(item=plot_labelling_txt, proportion=1, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        plot_labelling_sizer.Add(item=plot_labelling_cbox, proportion=2, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        
        if 'Axes3D' not in str(type(parent.plot)):
            axis_ticks_sizer = wx.BoxSizer(wx.HORIZONTAL)
            axis_ticks_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Axis ticks\t')
            axis_ticks_cbox = wx.ComboBox(parent.control_plot_frame.control_plot_panel, choices=['On', 'Off'], style=wx.CB_READONLY)
            
            if parent.axis_ticks is not None:
                if parent.axis_ticks is False:
                    axis_ticks_cbox.SetValue('Off')
                else:
                    axis_ticks_cbox.SetValue('On')
            else:
                if parent.style == 'image':
                    axis_ticks_cbox.SetValue('Off')
                else:
                    axis_ticks_cbox.SetValue('On')
                
            axis_ticks_cbox.Bind(wx.EVT_COMBOBOX, self.evt_axis_ticks_cbox)
            axis_ticks_sizer.Add(item=axis_ticks_txt, proportion=1, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            axis_ticks_sizer.Add(item=axis_ticks_cbox, proportion=2, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        
        axes_label_sizer.Add(item=plot_labelling_sizer, proportion=1, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.RIGHT, border=10)
        
        if 'Axes3D' not in str(type(parent.plot)):
            axes_label_sizer.Add(item=axis_ticks_sizer, proportion=1, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.RIGHT|wx.BOTTOM, border=10) 
        
        axes_label_sizer.Add(item=label_axes_txt, proportion=1, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.RIGHT|wx.BOTTOM, border=10)
        axes_label_sizer.Add(item=axes_grid_sizer, proportion=1, flag=wx.EXPAND|wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)
        
        self._create_figure_canvas()
        #parent.figure_canvas.Bind(wx.EVT_SIZE, parent.evt_resize_control_plot_panel)
        
        top_sizer.Add(item=axes_label_sizer, proportion=0, flag=wx.EXPAND|wx.RIGHT|wx.LEFT, border=10)
        
        if parent.style == 'heatmap':
            line1 = wx.StaticLine(parent.control_plot_frame.control_plot_panel, -1, style=wx.LI_HORIZONTAL)
            top_sizer.Add(line1, 0, wx.ALIGN_CENTER|wx.RIGHT|wx.LEFT|wx.BOTTOM, border=5)
            heatmap_boundaries_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Heatmap boundaries')
            
            # Controls for narrowing of depicted value range
            vmin_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Minimum \t')
            vmax_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Maximum \t')
                    
            parent.control_plot_frame.vmin_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, min=-50, max=50, initial=0)
            parent.control_plot_frame.vmax_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, min=-50, max=50, initial=0)
            #parent.control_plot_frame.vmax_spn.SetDigits(5)
            #parent.control_plot_frame.vmin_spn.SetDigits(5)
            
            # Set widgets to default values
            if parent.vmin is not None:
                parent.control_plot_frame.vmin_spn.SetValue(parent.vmin)
            else:
                parent.control_plot_frame.vmin_spn.SetValue(0.0)
                
            if parent.vmax is not None:
                parent.control_plot_frame.vmax_spn.SetValue(parent.vmax)
            else:
                parent.control_plot_frame.vmax_spn.SetValue(0.0)
            
            reset_heatmap_boundaries_btn = wx.Button(parent.control_plot_frame.control_plot_panel, label = 'Reset')
            reset_heatmap_boundaries_btn.Bind(wx.EVT_BUTTON, parent.evt_reset_heatmap_boundaries_btn)
        
            min_max_sizer.Add(vmin_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=10)
            min_max_sizer.Add(parent.control_plot_frame.vmin_spn, 2, wx.ALIGN_RIGHT|wx.RIGHT, border=10)
            min_max_sizer.Add(vmax_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=10)
            min_max_sizer.Add(parent.control_plot_frame.vmax_spn, 2, wx.ALIGN_RIGHT|wx.RIGHT, border=10)

            top_sizer.Add(heatmap_boundaries_txt, 0, flag=wx.ALIGN_LEFT|wx.BOTTOM|wx.LEFT, border=10)
            top_sizer.Add(min_max_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)
            top_sizer.Add(reset_heatmap_boundaries_btn, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
            
            
            if parent.mode == 'time course':
                marker_color_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, label='Marker')
                self.marked_check_box = wx.CheckBox(parent.control_plot_frame.control_plot_panel, -1)
                self.marker_color_ctrl = wx.ColourPickerCtrl(parent.control_plot_frame.control_plot_panel, -1, col=parent.aux_marker_color)
                self.marker_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_marker_color_ctrl)
                
                color_picker_sizer = wx.BoxSizer(wx.HORIZONTAL)
                color_picker_sizer.Add(self.marked_check_box, flag=wx.ALIGN_CENTER_VERTICAL)
                color_picker_sizer.Add(self.marker_color_ctrl)
            
                plot_control_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
                plot_control_grid_sizer.Add(marker_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
                plot_control_grid_sizer.Add(color_picker_sizer, 1, wx.ALIGN_RIGHT)
                top_sizer.Add(plot_control_grid_sizer, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
                self.marked_check_box.Bind(wx.EVT_CHECKBOX, self.evt_marked_check_box)
                
            #top_sizer.Add(item=save_btn, proportion=0, flag=wx.ALIGN_CENTER|wx.TOP, border=25)
            
            parent.control_plot_frame.vmin_spn.Bind(wx.EVT_SPINCTRL, parent.evt_vmin_spn)
            
            parent.control_plot_frame.vmax_spn.Bind(wx.EVT_SPINCTRL, parent.evt_vmax_spn)
        
        elif parent.style == 'wireframe' or parent.style == 'surface':
            
            visual_controls_txt = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Visual controls')
            plot_control_sizer = wx.BoxSizer(wx.VERTICAL)
            
            if parent.style == 'wireframe':
                if parent.mode == 'time course':
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
                else:
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
            
            elif parent.style == 'surface':
                if parent.mode == 'time course':
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
                else:
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=2, cols=2)
            
            plot_control_grid_sizer.SetFlexibleDirection(wx.BOTH)
                        
            if parent.style == 'wireframe':
                line_color_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, label='Line color ')  
                self.line_color_ctrl = wx.ColourPickerCtrl(parent.control_plot_frame.control_plot_panel, -1, col=parent.aux_line_color)
                self.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_line_color_ctrl)
                plot_control_grid_sizer.Add(line_color_label, 1, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
                plot_control_grid_sizer.Add(self.line_color_ctrl, 1, wx.ALIGN_CENTER)
            
            elif parent.style == 'surface':
                linewidth_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, label='Line width ') 
                parent.control_plot_frame.linewidth_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, -1, min=0, max=20)
                parent.control_plot_frame.linewidth_spn.Bind(wx.EVT_SPINCTRL, parent.evt_linewidth_spn)
                plot_control_grid_sizer.Add(linewidth_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
                plot_control_grid_sizer.Add(parent.control_plot_frame.linewidth_spn, 1, wx.ALIGN_RIGHT)
            
            if parent.mode == 'time course':
                marker_color_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, label='Marker')
                self.marked_check_box = wx.CheckBox(parent.control_plot_frame.control_plot_panel, -1)
                self.marker_color_ctrl = wx.ColourPickerCtrl(parent.control_plot_frame.control_plot_panel, -1, col=parent.aux_marker_color)
                self.marker_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_marker_color_ctrl)
                
                color_picker_sizer = wx.BoxSizer(wx.HORIZONTAL)
                color_picker_sizer.Add(self.marked_check_box, flag=wx.ALIGN_CENTER_VERTICAL)
                color_picker_sizer.Add(self.marker_color_ctrl)
                           
                plot_control_grid_sizer.Add(marker_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
                plot_control_grid_sizer.Add(color_picker_sizer, 1, wx.ALIGN_RIGHT)
                self.marked_check_box.Bind(wx.EVT_CHECKBOX, self.evt_marked_check_box)
                    
            resolution_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, 'Resolution ')
            parent.control_plot_frame.resolution_spn = wx.SpinCtrl(parent.control_plot_frame.control_plot_panel, min=1, max=100, value=str(parent.resolution))
            parent.control_plot_frame.resolution_spn.Bind(wx.EVT_SPINCTRL, parent.evt_resolution_spn)
                        
            plot_control_grid_sizer.Add(resolution_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            plot_control_grid_sizer.Add(parent.control_plot_frame.resolution_spn, 1, wx.ALIGN_RIGHT)
            
            plot_control_sizer.Add(item=visual_controls_txt, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.BOTTOM|wx.TOP, border=10)
            plot_control_sizer.Add(item=plot_control_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)    
            top_sizer.Add(item=plot_control_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT, border=10)
            #top_sizer.Add(item=save_btn, proportion=0, flag=wx.ALIGN_CENTER|wx.TOP|wx.LEFT|wx.RIGHT, border=25)
                        
        if parent.ndim[parent.selection] == 0 and parent.mode == 'time course':
            parent.control_plot_frame.sel_cbox = wx.ComboBox(parent.control_plot_frame.control_plot_panel, choices=parent.flist_sorted, value=parent.sel_cbox.GetValue(), style=wx.CB_READONLY)
            parent.control_plot_frame.sel_cbox.Bind(wx.EVT_COMBOBOX, parent.evt_sel_cbox)
            line2 = wx.StaticLine(parent.control_plot_frame.control_plot_panel, -1, style=wx.LI_HORIZONTAL)
            top_sizer.Add(line2, 0, wx.ALIGN_CENTER|wx.EXPAND|wx.RIGHT|wx.LEFT|wx.BOTTOM, border=5)
            multi_plot_btn = wx.Button(parent.control_plot_frame.control_plot_panel, label = 'Add time course', size=(100,30))
            multi_plot_btn.Bind(wx.EVT_BUTTON, parent.evt_add_time_course)
            line_color_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, label='Line color \t')
            
            self.line_color_ctrl = wx.ColourPickerCtrl(parent.control_plot_frame.control_plot_panel, -1, col=parent.aux_line_color)
            self.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_line_color_ctrl)
            
            marker_color_label = wx.StaticText(parent.control_plot_frame.control_plot_panel, -1, label='Marker')
            self.marked_check_box = wx.CheckBox(parent.control_plot_frame.control_plot_panel, -1)
            self.marker_color_ctrl = wx.ColourPickerCtrl(parent.control_plot_frame.control_plot_panel, -1, col=parent.aux_marker_color)
            self.marker_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_marker_color_ctrl)
            
            color_picker_sizer = wx.BoxSizer(wx.HORIZONTAL)
            color_picker_sizer.Add(self.marked_check_box, flag=wx.ALIGN_CENTER_VERTICAL)
            color_picker_sizer.Add(self.marker_color_ctrl)
            
            plot_control_grid_sizer = wx.BoxSizer(wx.HORIZONTAL)
            plot_control_grid_sizer.Add(marker_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            plot_control_grid_sizer.Add(color_picker_sizer, 1, wx.ALIGN_RIGHT)
            self.marked_check_box.Bind(wx.EVT_CHECKBOX, self.evt_marked_check_box)
            
            line_sizer = wx.BoxSizer(wx.HORIZONTAL)
            line_sizer.Add(line_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            line_sizer.Add(self.line_color_ctrl, 1, wx.ALIGN_RIGHT)
            
            top_sizer.Add(item=parent.control_plot_frame.sel_cbox, proportion=0, flag=wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND, border=10)
            top_sizer.Add(item=line_sizer, proportion=0, flag=wx.LEFT|wx.RIGHT, border=10)
            top_sizer.Add(item=plot_control_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)
            top_sizer.Add(item=multi_plot_btn, proportion=0, flag=wx.LEFT|wx.RIGHT, border=10)
        
        button_sizer = wx.BoxSizer(wx.HORIZONTAL)
        button_sizer.Add(item=save_btn, proportion=0, flag=wx.ALIGN_CENTER|wx.TOP, border=5)
        button_sizer.Add(item=write_csv_btn, proportion=0, flag=wx.ALIGN_CENTER|wx.TOP, border=5)
        button_sizer.Add(item=save_config_btn, proportion=0, flag=wx.ALIGN_CENTER|wx.TOP, border=5)
        top_sizer.Add(item=button_sizer, proportion=0, flag=wx.ALIGN_CENTER|wx.TOP|wx.LEFT|wx.RIGHT, border=25) 
        parent.control_plot_frame.main_sizer.Add(item=parent.figure_canvas, proportion=1, flag=wx.EXPAND)
        parent.control_plot_frame.main_sizer.Add(item=top_sizer, proportion=1, flag=wx.EXPAND)
        
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
        
        return
    
    
    def evt_line_color_ctrl(self, event):
        widget = event.GetEventObject()
        self.parent.aux_line_color = widget.GetColour()
        widget.SetColour(self.parent.aux_line_color)
        
        # Color conversion
        red = self.parent.aux_line_color.Red()/255.
        green = self.parent.aux_line_color.Green()/255.
        blue = self.parent.aux_line_color.Blue()/255.
        
        self.parent.line_color = [red, green, blue]
        
        # Update currently displayed plot
        if self.parent.figure and self.parent.ndim[self.parent.selection] != 0:
            wx.CallAfter(self.parent._update_plot)
            wx.Yield()
            
    
    def evt_marker_color_ctrl(self, event):
        
        self.parent.aux_marker_color = self.marker_color_ctrl.GetColour()
        self.marker_color_ctrl.SetColour(self.parent.aux_marker_color)
        
        # Color conversion
        red = self.parent.aux_marker_color.Red()/255.
        green = self.parent.aux_marker_color.Green()/255.
        blue = self.parent.aux_marker_color.Blue()/255.
        self.parent.marker_color = [red, green, blue]
        
        # Update currently displayed plot
        if self.parent.figure:
            wx.CallAfter(self.parent._update_plot)
            wx.Yield()
    
    
    def evt_marked_check_box(self, event):
        
        widget = event.GetEventObject()
        self.parent.marked = widget.GetValue()
        wx.CallAfter(self.parent._update_plot)
    
    
    def evt_plot_labelling_cbox(self, event):
        parent = self.parent
        widget = event.GetEventObject()
        parent.labelling_mode = widget.GetValue()
        
        parent._update_plot()
        parent._update_plot()
            
        
    def evt_axis_ticks_cbox(self, event):
        parent = self.parent
        widget = event.GetEventObject()
                
        if widget.GetValue() == 'On':
            parent.axis_ticks = True
        else:
            parent.axis_ticks = False
            
        parent._update_plot()
        
     
    def _create_figure_canvas(self):
    
        self.parent.figure = plt.figure(str(self.parent.title))
        
        if self.parent.figure_canvas == None:
            self.parent.figure_canvas = FigureCanvas(self.parent.control_plot_frame.control_plot_panel, wx.ID_ANY, self.parent.figure)
            self.parent.figure_canvas.mpl_connect('button_release_event', self.parent.evt_change_plot_parameters)
        wx.Yield()
            
            
#========================================================================================================================
        
class RDPGUI(wx.Panel):
    
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
                        
        self.frame = parent        
        self.control_plot_frame = None
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.dir = self.GetParent().dir            
        self.figure_size = None
        self.x_label = ''
        self.y_label = ''
        self.z_label = ''
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
        self.labelling_mode = 'Off'
        self.flist = [record_file for record_file in os.listdir(self.dir) if record_file.lower().endswith('.csv') or record_file.lower().endswith('.data')]
        
        self.flist_sorted = np.asarray(self.frame.rdp_plot.sort_alphnum(self.flist))
        
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
        self.title = ''
        self.figure_resize = None
        self.colorbar = None
        self.axis_ticks = None
        self.sel_cbox_selection = ''
                
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
        
        self.labelling_choices = ['Off', 'LateX', 'Standard']
        
        self.figure_azimuth = None
        self.figure_elevation = None
        self.figure_distance = None
        
        self.surface_linewidth = 0
        
        # Labels
        #========================================================================================================================
        self.sel_label = wx.StaticText(self, -1, 'Recording')
        self.mode_label = wx.StaticText(self, -1, 'Plot mode')
        self.proj_label = wx.StaticText(self, -1, 'Projection')
        self.proj_method_label = wx.StaticText(self, -1, 'Projection method \t')
        self.style_label = wx.StaticText(self, -1, 'Plot style')
        
        if self.frame.parent is None:
            self.time_stamp_display = wx.StaticText(self, -1, '-')
            self.time_stamp_label = wx.StaticText(self, -1, 'Time stamp t \t')
            
        self.player_label = wx.StaticText(self, -1, 'Mark time slice')
        #========================================================================================================================
        
        self.line_1 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        self.line_2 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        self.line_4 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(300,10))
        
        # Control widgets
        #========================================================================================================================
        self.sel_cbox = wx.ComboBox(self, size=(200, 27), choices = self.flist_sorted, value = ' ', style = wx.CB_READONLY) 
        self.mode_cbox = wx.ComboBox(self, size = self.sel_cbox.GetSize(), style = wx.CB_READONLY)
        self.proj_cbox = wx.ComboBox(self, size = self.sel_cbox.GetSize(), style = wx.CB_READONLY)
        self.proj_method_cbox = wx.ComboBox(self, size = self.sel_cbox.GetSize(), choices = self.proj_methods, value=' ', style = wx.CB_READONLY)
        self.style_cbox = wx.ComboBox(self, size = self.sel_cbox.GetSize(), choices = self.style_ch, style = wx.CB_READONLY)
        
        if self.frame.parent is None:
            self.pos_slider = wx.Slider(self, value=0, minValue = 0, maxValue = 1, style = wx.SL_LABELS|wx.SL_AUTOTICKS)
            
        #========================================================================================================================

        for i in range(len(self.flist_sorted)):
            
                self.header_list.append(self.frame.rdp_plot.get_header(csv_f=self.dir + '/' + self.flist_sorted[i]))
                self.ndim.append(self.frame.rdp_plot.get_dimension(self.header_list[i]))
        
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
        self.axes_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
        self.axes_grid_sizer.SetFlexibleDirection(wx.BOTH)
        self.cbox_grid_sizer = wx.FlexGridSizer(rows=5, cols=2)
        self.cbox_grid_sizer.SetFlexibleDirection(wx.BOTH)
        self.selection_sizer = wx.BoxSizer(wx.VERTICAL)
        
        if self.frame.parent is None:
            play = self.frame.rdp_image_files.play.GetImage()
            self.play_bitmap = wx.BitmapFromImage(play)
            reverse_play = self.frame.rdp_image_files.reverse_play.GetImage()
            self.reverse_play_bitmap = wx.BitmapFromImage(reverse_play)
            pause = self.frame.rdp_image_files.pause.GetImage()
            self.pause_bitmap = wx.BitmapFromImage(pause)
            reset = self.frame.rdp_image_files.reset.GetImage()
            self.reset_bitmap = wx.BitmapFromImage(reset)
            decrease_single_step = self.frame.rdp_image_files.decrease_single_step.GetImage()
            self.decrease_single_step_bitmap = wx.BitmapFromImage(decrease_single_step)
            increase_single_step = self.frame.rdp_image_files.increase_single_step.GetImage()
            self.increase_single_step_bitmap = wx.BitmapFromImage(increase_single_step)
            
        # Buttons
        #========================================================================================================================
        self.plot_btn = wx.Button(self, label = 'Plot')
        self.load_config_btn = wx.Button(self, label = 'Load configuration')
        
        if self.frame.parent is None:
            self.switch_btn = wx.Button(self, label = 'Switch directory')
            self.add_figure_frame_btn = wx.Button(self, label = 'Additional figure panel')

            # Player buttons
            #========================================================================================================================
            self.play_pause_btn = wx.BitmapButton(self, -1, bitmap=self.play_bitmap)
            self.reverse_play_pause_btn = wx.BitmapButton(self, -1, bitmap=self.reverse_play_bitmap)
            self.reset_btn = wx.BitmapButton(self, -1, bitmap=self.reset_bitmap)
            self.decrease_single_step_btn = wx.BitmapButton(self, -1, bitmap=self.decrease_single_step_bitmap)
            self.increase_single_step_btn = wx.BitmapButton(self, -1, bitmap=self.increase_single_step_bitmap)
            
            # Player button tooltips
            #========================================================================================================================
            #self.self.control_plot_frame.resolution_spn.SetToolTipString('Determines the resolution of the plot. Lower values lead to finer resolutions.')
            self.play_pause_btn.SetToolTipString('Starts and pauses the Plot animation.')
            self.reverse_play_pause_btn.SetToolTipString('Starts and pauses the reverse Plot animation.')
            self.reset_btn.SetToolTipString('Resets the plot to default start parameters.')
            self.increase_single_step_btn.SetToolTipString('Updates the plot by increasing the time slice index by 1.')
            self.decrease_single_step_btn.SetToolTipString('Updates the plot by decreasing the time slice index by 1.')
        
        # Build partial sizers
        #========================================================================================================================
            
        # Slider and player functionalities
        #========================================================================================================================
        self.player_caption_sizer.Add(self.player_label, 1)
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
        
        if self.frame.parent is None:
            self.selection_sizer.Add(self.switch_btn, 1, wx.EXPAND)
            self.selection_sizer.Add(self.add_figure_frame_btn, 1, wx.EXPAND)
            
        self.selection_sizer.Add(self.sel_cbox, 1, wx.EXPAND)
        
        # Selection ComboBoxes
        #========================================================================================================================
        self.cbox_grid_sizer.Add(self.sel_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        self.cbox_grid_sizer.Add(self.selection_sizer, 1, wx.ALIGN_RIGHT)
        self.cbox_grid_sizer.Add(self.mode_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        self.cbox_grid_sizer.Add(self.mode_cbox, 1, wx.ALIGN_RIGHT)
        self.cbox_grid_sizer.Add(self.proj_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        self.cbox_grid_sizer.Add(self.proj_cbox, 1, wx.ALIGN_RIGHT)
        self.cbox_grid_sizer.Add(self.proj_method_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        self.cbox_grid_sizer.Add(self.proj_method_cbox, 1, wx.ALIGN_RIGHT)
        self.cbox_grid_sizer.Add(self.style_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        self.cbox_grid_sizer.Add(self.style_cbox, 1, wx.ALIGN_RIGHT)
        #========================================================================================================================
        
        # Buttons
        #========================================================================================================================
        self.plot_sizer.Add(item=self.plot_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.EXPAND)
        self.plot_sizer.Add(item=self.load_config_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.EXPAND)
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
        self.sel_cbox.Bind(wx.EVT_COMBOBOX, self.evt_sel_cbox)
        self.mode_cbox.Bind(wx.EVT_COMBOBOX, self.evt_mode_cbox)
        self.proj_cbox.Bind(wx.EVT_COMBOBOX, self.evt_proj_cbox)
        self.proj_method_cbox.Bind(wx.EVT_COMBOBOX, self.evt_proj_method_cbox)
        self.style_cbox.Bind(wx.EVT_COMBOBOX, self.evt_style_cbox)
        self.load_config_btn.Bind(wx.EVT_BUTTON, self.evt_load_config_btn)
        
        if self.frame.parent is None:
            
            self.pos_slider.Bind(wx.EVT_COMMAND_SCROLL_THUMBTRACK, self.evt_pos_slider)
            self.pos_slider.Bind(wx.EVT_COMMAND_SCROLL_CHANGED, self.evt_pos_slider)       
            self.play_pause_btn.Bind(wx.EVT_BUTTON, self.evt_play_pause_btn)
            self.reverse_play_pause_btn.Bind(wx.EVT_BUTTON, self.evt_reverse_play_pause_btn)
            self.increase_single_step_btn.Bind(wx.EVT_BUTTON, self.evt_increase_single_step_btn)
            self.decrease_single_step_btn.Bind(wx.EVT_BUTTON, self.evt_decrease_single_step_btn)
            self.reset_btn.Bind(wx.EVT_BUTTON, self.evt_reset_btn)
            self.switch_btn.Bind(wx.EVT_BUTTON, self.evt_switch_btn)
                    
        if self.frame.parent is None:
            self.add_figure_frame_btn.Bind(wx.EVT_BUTTON, self.evt_add_figure_frame_btn)
        #========================================================================================================================
        
        # Layout
        #========================================================================================================================
        self.SetSizerAndFit(self.main_sizer)

        #========================================================================================================================
        
        self.mode_cbox.Disable()
        self.proj_cbox.Disable()
        self.proj_method_cbox.Disable()
        self.style_cbox.Disable()
                  
        return
    
    
    def prepare_plot_configuration(self, directory, x_label, y_label, z_label, marked, step, slider_max, resolution, vmax, vmin, proj, 
                                   proj_method, style, mode, labelling_mode, data, reduced_data, header, ndim, time_stamps, selection, sel_cbox_selection,
                                   proj_choice_time_course, proj_choice_snapshot, line_color, aux_line_color, marker_color, aux_marker_color, title, 
                                   axis_ticks, figure_azimuth, figure_elevation, figure_distance, surface_linewidth, flist_sorted, mode_ch, proj_ch):
                              
        save_object = [directory, x_label, y_label, z_label, marked, step, slider_max, resolution, vmax, vmin, proj, 
                       proj_method, style, mode, labelling_mode, data, reduced_data, header, ndim, time_stamps, selection, sel_cbox_selection,
                       proj_choice_time_course, proj_choice_snapshot, line_color, aux_line_color, marker_color, aux_marker_color, title, 
                       axis_ticks, figure_azimuth, figure_elevation, figure_distance, surface_linewidth, flist_sorted, mode_ch, proj_ch]
                
        return save_object
    
    
    def evt_change_plot_parameters(self, event):
        
        self.figure = plt.figure(str(self.title))
        self.plot = self.figure.gca()
        
        if 'Axes3D' in str(type(self.plot)):
            self.figure_azimuth = int(self.figure.gca(projection='3d').azim)
            self.figure_elevation = int(self.figure.gca(projection='3d').elev)
                    
        self.control_plot_frame.azimuth_spn.SetValue(self.figure_azimuth)
        self.control_plot_frame.elevation_spn.SetValue(self.figure_elevation)
                
    
    def evt_load_config_btn(self, event):       
        self._load_plot_configuration()
        
        
    def _load_plot_configuration(self):
        
        dlg = LoadConfigDialog(self, self.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        file_path = dlg.GetPath()
        loaded_object = pickle.load(open(file_path, 'rb'))
        
        self.dir = loaded_object[0]
        self.x_label = loaded_object[1] 
        self.y_label = loaded_object[2]
        self.z_label = loaded_object[3]  
        self.marked = loaded_object[4]  
        self.step = loaded_object[5]  
        self.slider_max = loaded_object[6]  
        self.resolution = loaded_object[7]  
        self.vmax = loaded_object[8]  
        self.vmin = loaded_object[9]  
        self.proj = loaded_object[10]  
        self.proj_method = loaded_object[11]  
        self.style = loaded_object[12]  
        self.mode = loaded_object[13]  
        self.labelling_mode = loaded_object[14]  
        self.data = loaded_object[15]  
        self.reduced_data = loaded_object[16]  
        self.header = loaded_object[17]  
        self.ndim = loaded_object[18]  
        self.time_stamps = loaded_object[19]  
        self.selection = loaded_object[20] 
        self.sel_cbox_selection = loaded_object[21]
        self.proj_choice_time_course = loaded_object[22]  
        self.proj_choice_snapshot = loaded_object[23]  
        self.line_color = loaded_object[24]  
        self.aux_line_color = loaded_object[25]  
        self.marker_color = loaded_object[26] 
        self.aux_marker_color = loaded_object[27] 
        self.title = loaded_object[28]  
        self.axis_ticks = loaded_object[29]  
        self.figure_azimuth = loaded_object[30]  
        self.figure_elevation = loaded_object[31]  
        self.figure_distance = loaded_object[32]  
        self.surface_linewidth = loaded_object[33]  
        self.flist_sorted = loaded_object[34] 
        self.mode_ch = loaded_object[35]
        self.proj_ch = loaded_object[36]
           
        self.plot_btn.Bind(wx.EVT_BUTTON, self.evt_plot)
        self.sel_cbox.SetItems(self.flist_sorted)
        self.sel_cbox.SetValue(self.sel_cbox_selection)
        
        try:
            self.pos_slider.SetMax(self.slider_max)
            self.pos_slider.SetValue(self.step)
            self.time_stamp_display.SetLabel(str(self.time_stamps[self.step]))
        except AttributeError:
            pass
        
        self.proj_cbox.SetItems(self.proj_ch)
        self.proj_cbox.SetValue(self.proj)
        self.proj_method_cbox.SetValue(self.proj_method)
        self.mode_cbox.SetItems(self.mode_ch)
        self.mode_cbox.SetValue(self.mode)
        self.style_cbox.SetValue(self.style)
        
        self.mode_cbox.Enable()
        self.style_cbox.Enable()
        self.proj_cbox.Enable()
        self.proj_method_cbox.Enable()
        
            
    def evt_save_config_btn(self, event):
        #dlg_name = self.dir + '/' + self.flist_sorted[self.selection].strip('.p') + '-' + str(self.mode) + '-1.p'
        dlg = SaveConfigDialog(self, self.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        save_object = self.prepare_plot_configuration(directory=self.dir,
                                                      x_label=self.x_label,
                                                      y_label=self.y_label, 
                                                      z_label=self.z_label, 
                                                      marked=self.marked, 
                                                      step=self.step, 
                                                      slider_max=self.slider_max, 
                                                      resolution=self.resolution, 
                                                      vmax=self.vmax, 
                                                      vmin=self.vmin, 
                                                      proj=self.proj, 
                                                      proj_method=self.proj_method, 
                                                      style=self.style, 
                                                      mode=self.mode, 
                                                      labelling_mode=self.labelling_mode, 
                                                      data=self.data, 
                                                      reduced_data=self.reduced_data, 
                                                      header=self.header, 
                                                      ndim=self.ndim, 
                                                      time_stamps=self.time_stamps, 
                                                      selection=self.selection,
                                                      sel_cbox_selection=self.sel_cbox_selection,
                                                      proj_choice_time_course=self.proj_choice_time_course, 
                                                      proj_choice_snapshot=self.proj_choice_snapshot, 
                                                      line_color=self.line_color, 
                                                      aux_line_color=self.aux_line_color, 
                                                      marker_color=self.marker_color, 
                                                      aux_marker_color=self.aux_marker_color,
                                                      title=self.title, 
                                                      axis_ticks=self.axis_ticks, 
                                                      figure_azimuth=self.figure_azimuth, 
                                                      figure_elevation=self.figure_elevation, 
                                                      figure_distance=self.figure_distance, 
                                                      surface_linewidth=self.surface_linewidth,
                                                      flist_sorted=self.flist_sorted,
                                                      mode_ch = self.mode_cbox.GetItems(),
                                                      proj_ch=self.proj_cbox.GetItems())
                
        file_path = dlg.GetPath()

        pickle.dump(save_object, open(file_path, 'wb'), 0)
        
        
    def evt_elevation_spn(self, event):
        widget = event.GetEventObject()
        value = widget.GetValue()
        self.figure_elevation = value
        self.figure.gca().elev = self.figure_elevation
           
        wx.CallAfter(self._update_plot)
        
        
    def evt_azimuth_spn(self, event):
        widget = event.GetEventObject()
        value = widget.GetValue()
        self.figure_azimuth = value      
        self.figure.gca().azim = self.figure_azimuth
        
        wx.CallAfter(self._update_plot)
        

    def evt_distance_spn(self, event):
        widget = event.GetEventObject()
        value = widget.GetValue()
        self.figure_distance = value        
                
        wx.CallAfter(self._update_plot)
        
        
    def evt_close_figure(self, event):
        
        self.figure_canvas.mpl_disconnect(self.figure_canvas_connection_id)
        
        self.figure_canvas_connection_id = None
        self.figure.clf()
        self.plot = None
        self.figure = None
        self.colorbar = None
        self.figure_canvas = None
        self.control_plot_frame.Hide()
        self.control_plot_frame = None
        
    
    def evt_add_figure_frame_btn(self, event):
        
        try:
            if self.ndim[self.selection] == 0:
                self.mode_cbox.SetItems(['time course'])
            else:
                self.mode_cbox.SetItems(self.multi_mode_ch)
        
        except TypeError:
            pass
        
        new_frame = RDPMainWindow(parent=self.frame, title='', pos=None, size=None, style=None, name=None)     
        new_frame.rdp_gui.step = self.step   
        new_frame.Bind(wx.EVT_CLOSE, self.evt_close_frame)
        
        new_frame_id = new_frame.GetId()
        self.frame_ids.append(new_frame_id)
        
        wx.FindWindowById(new_frame_id).step = self.step
        
                             
    def evt_close_frame(self, event):
        frame = event.GetEventObject()
        frame_id = frame.GetId()
        
        if frame_id in self.frame_ids:
            self.frame_ids.remove(frame_id)
        
        wx.FindWindowById(frame_id).Hide()
                
                
    def evt_play_pause_btn(self, event):
        self._play_pause_btn(reverse=False)
        

    def evt_reverse_play_pause_btn(self, event):
        self._play_pause_btn(reverse=True)
        
            
    def evt_reset_btn(self, event):
        '''Reset plot to default.'''
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.step = 0
            current_frame.marked = False
            
            current_frame.x_label = ''
            current_frame.y_label = ''
            current_frame.z_label = ''
            current_frame.nstep = 0
            current_frame.step_size = 0
            current_frame.resolution = 5
            current_frame.line_color = '#FF9600'
            current_frame.aux_line_color = '#FF9600'
            current_frame.marker_color = '#FF9600'
            current_frame.aux_marker_color = '#FF9600'
            current_frame.save_mode = False
            current_frame.figure_azimuth = None
            current_frame.figure_elevation = None
            current_frame.figure_distance = None
            current_frame.surface_linewidth = 0
            
            if i == 0:
                current_frame.pos_slider.SetValue(current_frame.step)
                current_frame.time_stamp_display.SetLabel(' ')
                    
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
            event.Skip()
                
        
    def evt_increase_single_step_btn(self, event):
                    
        self._move_single_step(increase=True)
                    
    
    def evt_decrease_single_step_btn(self, event):
                    
        self._move_single_step(increase=False)
        
    
    def evt_axis_label(self, event, x_axis_label, y_axis_label, z_axis_label=None):
        
        self.x_label = x_axis_label.GetValue()
        self.y_label = y_axis_label.GetValue()
        
        if z_axis_label != None:
            try:
                self.z_label = z_axis_label.GetValue()
            except AttributeError:
                pass
                    
        wx.CallAfter(self._update_plot)
        
    
    def evt_add_time_course(self, event):
        '''Add time course to currently displayed plot.'''
        
        self._add_time_course()
        
        
    def evt_resolution_spn(self, event):
        
        self.resolution = int(self.control_plot_frame.resolution_spn.GetValue())
        wx.CallAfter(self._update_plot)
        wx.Yield()
        

    def evt_vmax_spn(self, event):
        
        self.vmax = self.control_plot_frame.vmax_spn.GetValue()
        wx.CallAfter(self._update_plot)

    
    def evt_vmin_spn(self, event):
        
        self.vmin = self.control_plot_frame.vmin_spn.GetValue()
        wx.CallAfter(self._update_plot)
        
    
    def evt_proj_cbox(self, event):
        self.proj = self.proj_cbox.GetValue()
        
        if ',' not in self.proj and self.mode != 'time course':
            self.style_cbox.Disable()
            self.proj_method_cbox.Disable()
            self.style_cbox.SetValue('')
            self.proj_method_cbox.SetValue('')
        else:
            self.style_cbox.Enable()
            self.proj_method_cbox.Enable()
            
        if self.proj == ' ':
            self.proj_method = ' '
            self.style = ' '
            self.proj = ' '
            
            self.proj_method_cbox.Disable()
            self.style_cbox.Disable()
            
            self.proj_method_cbox.SetValue(self.proj_method)
            self.style_cbox.SetValue(self.style)
        
        wx.Yield()
        event.Skip()
        
        
    def evt_proj_method_cbox(self, event):
        
        self.proj_method = self.proj_method_cbox.GetValue()
        
        if self.proj_method == ' ':
            self.style_cbox.Disable()
            self.style = ' '
            self.style_cbox.SetValue(self.style)
        else:
            self.style_cbox.Enable()
                    
        wx.Yield()
        event.Skip()
        
    
    def evt_style_cbox(self, event):

        self.style = self.style_cbox.GetValue()
        self.plot_btn.Bind(wx.EVT_BUTTON, self.evt_plot)
        
        if self.style != 'image':
            self.mode_cbox.Enable()
            self.proj_cbox.Enable()
            self.proj_method_cbox.Enable()
            
        else:
            self.mode = self.mode_cbox.GetValue()
            self.mode_cbox.Disable()
            self.proj_cbox.Disable()
            self.proj_method_cbox.Disable()
    
        wx.Yield()
        event.Skip()

    
    def evt_reset_heatmap_boundaries_btn(self, event):
        
        self.vmin = self.vmax = None
        
        self.control_plot_frame.vmin_spn.SetValue(0.0000)
        self.control_plot_frame.vmax_spn.SetValue(0.0000)
        
        wx.CallAfter(self._update_plot)

    
    def evt_switch_btn(self, event):  
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            
            # Clear memory
            if current_frame.data is not None:
                del current_frame.data
                current_frame.data = None
            
            # Close open plots              
            if current_frame.figure:
                plt.close(current_frame.figure)
                current_frame.figure = None
                        
        frame = self.GetParent()
        browser_panel = RDPBrowserPanel(parent=frame)
        
        # Replace plot generation frame with file browser
        frame.SetSizer(browser_panel.main_sizer)
        
        # Layout
        wx.CallAfter(frame.Sizer.Fit, frame)
        wx.CallAfter(frame.Layout)
        event.Skip()
        
                    
    def evt_pos_slider(self, event):
        
        step = self.pos_slider.GetValue()
                
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.step = step
        
            if i == 0:
                # Set time code
                current_time_stamp = self.time_stamps[current_frame.step]
                current_frame.time_stamp_display.SetLabel(current_time_stamp)
                
            # Update currently displayed plot
            current_frame._update_plot()
        
            
    def evt_mode_cbox(self, event):
        
        self.mode = self.mode_cbox.GetValue()
        
        if self.figure:
            plt.close(self.figure)
                    
        else:
            self.proj_method = ' '
            self.style = ' '
            self.proj = ' '
            
            self.proj_method_cbox.Disable()
            self.style_cbox.Disable()
            
            self.proj_cbox.SetValue(self.proj)
            self.proj_method_cbox.SetValue(self.proj_method)
            self.style_cbox.SetValue(self.style)
        
        # Fill projection combobox with the fitting options
        if self.mode == 'time course':
            self.proj_cbox.SetItems(self.proj_choice_time_course)
            self.proj_cbox.Enable()
        elif self.mode == 'snapshot' or self.mode == 'snapshot sequence':
            self.proj_cbox.SetItems(self.proj_choice_snapshot)
            self.proj_cbox.Enable()
        else:
            self.proj_cbox.SetItems([])
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.plot_btn.Bind(wx.EVT_BUTTON, current_frame.evt_plot)
            
            
    def enforce_labelling_mode(self, plot, labelling_mode, style):
        
        if labelling_mode == 'Off':
            plt.rcdefaults()
            plt.gca().axes.set_xticklabels([])
            plt.gca().axes.set_yticklabels([])
                        
            if 'Axes3D' in str(type(self.plot)):
                plt.gca().axes.set_zticklabels([])
        
        elif labelling_mode == 'Standard':
            plt.rcdefaults()
        
        else:
            plt.rc('text', usetex=True)
            mpl.rcParams['text.latex.unicode']=True
            plt.rc('font', family='serif')
            
        try:
            if self.axis_ticks == False:
                plt.gca().tick_params(axis='both', which='both', bottom='off', top='off', left='off', right='off')
                    
            else:
                plt.gca().tick_params(axis='both', which='both', bottom='on', top='on', left='on', right='on')
        except AttributeError:
            pass
                
        return plot
            
            
    def evt_plot(self, event):
        
        if self.style != 'heatmap' and self.ndim[self.selection] != 0:    
            self._plot()

            
        if self.mode != 'snapshot sequence':
            if self.control_plot_frame is None:
                RDPPlotFrame(parent=self)                
                self.control_plot_frame.Show()
        
        self._update_plot()
        self._update_plot()
        
                
    def evt_linewidth_spn(self, event):
        self.surface_linewidth = float(self.control_plot_frame.linewidth_spn.GetValue() / 20.)
        wx.CallAfter(self._update_plot)
        wx.Yield()
        
                                
    def evt_sel_cbox(self, event):
        
        widget = event.GetEventObject()
        self.selection = widget.GetSelection()
        self._update_selection_data()
        self.sel_cbox.SetValue(widget.GetValue())
        self.sel_cbox_selection = widget.GetValue()
                                
        if 'CV_8U' in self.header_list[self.selection] or 'CV_8UC3' in self.header_list[self.selection]:
            self.style_ch = ['image']
            self.style_cbox.Enable()
            self.style_cbox.SetItems(self.style_ch)
            self.style_cbox.SetSelection(0)
            self.style = 'image'
            self.plot_btn.Bind(wx.EVT_BUTTON, self.evt_plot)
        else:
            self.style_ch = [' ', 'heatmap', 'surface', 'wireframe']
            self.style_cbox.SetItems(self.style_ch)
            self.mode_cbox.Enable()
            
            
    def evt_write_csv_btn(self, event):
                
        dlg_name = self.dir + '/' + self.flist_sorted[self.selection].strip('.csv') + '-' + str(self.mode) + '-1.csv'
        dlg = WriteCSVDialog(self, self.dir, dlg_name)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        file_path = dlg.GetPath()
        np.savetxt(fname=file_path, X=self.reduced_data, delimiter=',')
        

    def evt_save_plot_btn(self, event):
        
        dlg = SaveDialog(parent=self, defaultDir=self.dir)
        
        if dlg.ShowModal() == wx.ID_CANCEL:
            return
        
        file_path = dlg.GetPath()
                        
        if self.mode != 'snapshot sequence':
            self._plot(save=True, file_path=file_path)
        
                                    
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
            wx.CallAfter(current_frame._update_plot)
            
        
    def _update_plot(self):
        if self.figure_canvas:
    
            if self.ndim[self.selection] == 0:
                plt.cla()
            else:
                self.figure.clf()
        
        self._plot()
            
        wx.YieldIfNeeded()

                   
    def _add_time_course(self):
        self._plot()

                
    def evt_resize_control_plot_panel(self, event):
        
        pix = self.control_plot_frame.control_plot_panel.GetClientSize()
        
        self.figure.set_size_inches(pix[0]/self.figure.get_dpi(),
                                    pix[1]/self.figure.get_dpi())
        
        
        x,y = self.control_plot_frame.GetSize()  
        
        self.figure.set_size_inches(x/self.figure.get_dpi(),
                                    y/self.figure.get_dpi())
        
        self.figure_canvas.SetSize((x-1, y-1))
        self.figure_canvas.SetSize((x, y))
                        
        event.Skip()
                
                                   
    def evt_resize_canvas(self, event):
        self.figure_canvas.draw()
        
                                
    def _update_selection_data(self):
        '''Reset the control panel and update it with the newly selected data.'''
        
        self.data = None    
        self.reduced_data = None
        
        if len(self.frame_ids) == 1 and self.frame.parent is None:
            if self.ndim[self.selection] == 0:
                self.mode_cbox.SetItems(['time course'])
            else:
                self.mode_cbox.SetItems(self.mode_ch)
        else:
            if self.ndim[self.selection] == 0:
                self.mode_cbox.SetItems(['time course'])
            else:
                self.mode_cbox.SetItems(self.multi_mode_ch)
                                                
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
            
        self.mode_cbox.Disable()

        # Generate adequate options for the projection combobox, depending on data dimensionality and plot mode
        self.proj_choice_time_course = self.proj_ch[:self.ndim[self.selection]+1]
        self.proj_choice_snapshot = self.frame.rdp_plot.build_proj_ch_step(ndim=self.ndim[self.selection], temp_proj_ch_step=self.proj_ch_step)   
        
        # Get data and data header
        self.header = self.frame.rdp_plot.get_header(csv_f=self.dir + '/' + self.flist_sorted[self.selection])        
        temp_data = self.frame.rdp_plot.get_data(csv_f=self.dir + '/' + self.flist_sorted[self.selection], header=self.header)
        
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

                
    def _plot(self, save=False, file_path=None):
        '''Build plot(s) to either visualize or save as pdf file'''
                
        if save is True and self.mode != 'snapshot sequence':
            self.frame.rdp_plot.save_plot(plot=self.plot, plot_mode=self.mode, file_name=self.flist_sorted[self.selection], file_directory=self.dir, figure=self.figure, file_path=file_path)
            self._update_plot()
            
            
        else:         
                       
            if self.mode != 'snapshot sequence':
                if self.figure is None:
                    self.figure = plt.figure(str(self.title))
                                                                       
            if self.mode == 'snapshot' or self.style == 'image':            
                try:             
                    self.plot, self.reduced_data = self.frame.rdp_plot.plot_snapshot(step = self.step, 
                                                                  style = self.style, 
                                                                  data = self.data, 
                                                                  header = self.header, 
                                                                  vmin = self.vmin,
                                                                  vmax = self.vmax,
                                                                  resolution = self.resolution, 
                                                                  surface_linewidth = self.surface_linewidth,
                                                                  proj = self.proj,
                                                                  proj_method = self.proj_method,
                                                                  color = self.line_color,
                                                                  figure = self.figure,
                                                                  title = self.title)
                    
                    if self.style == 'heatmap' or self.style == 'surface' or self.style == 'wireframe':
                        self.figure.gca().invert_yaxis()
                                
                    if self.mode == 'time course' and self.x_label == '':
                        try:
                            self.plot.set_xlabel('Time')
                        except AttributeError:
                            pass
                    
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
                    self.plot = self.frame.rdp_plot.plot_time_course(data = self.data, 
                                                                     header = self.header, 
                                                                     vmin = self.vmin,
                                                                     vmax = self.vmax,
                                                                     resolution = self.resolution,
                                                                     surface_linewidth = self.surface_linewidth,
                                                                     plot = self.plot,
                                                                     proj = self.proj,
                                                                     proj_method = self.proj_method,
                                                                     color = self.line_color,
                                                                     step = self.step, 
                                                                     marker = self.marked, 
                                                                     style = self.style,
                                                                     marker_color = self.marker_color,
                                                                     figure = self.figure,
                                                                     title = self.title)
                    
                    if self.style == 'heatmap' or self.style == 'surface' or self.style == 'wireframe':
                        self.figure.gca().invert_yaxis()
                                
                    if self.mode == 'time course' and self.x_label == '':
                        try:
                            self.plot.set_xlabel('Time')
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
                self.frame.rdp_plot.label_axis(plot=self.plot, x_label=self.x_label, y_label=self.y_label, z_label=self.z_label)
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
                    
                    try:
                        self.figure_canvas.draw()
                    except UnicodeEncodeError:
                        pass
                    except RuntimeError:
                        #self._update_plot()
                        self.figure_canvas.draw()
                    
                except AttributeError:
                    pass
                    
                wx.YieldIfNeeded()
        
#========================================================================================================================

class RDPPlot():

    def __init__(self):
        return
    
                
    def sort_alphnum(self, unsorted):
        '''Sort given list alphanumerically.'''
        conv = lambda text: int(text) if text.isdigit() else text
        alphnum_key = lambda key: [conv(c) for c in re.split('([0-9]+)', key)]
        
        return sorted(unsorted, key=alphnum_key)
    
    def build_proj_ch_step(self, ndim, temp_proj_ch_step):
        '''Build the various projection choices for snapshot plots.'''
        
        # Empty (default) selection option
        proj_ch_step = [' ']
                
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
    
    
    def get_dimension(self, header):
        '''Return the dimensionality of the data belonging to the given header.'''
        
        ndim = len(header[2:])
                
        for i in range(1, len(header[2:])+1):
            
            if int(header[-i]) == 1:
                ndim -= 1
            elif int(header[-i]) != 1:
                break
           
        return ndim    
    
    
    def _project(self, mode, steps, data, header, proj, proj_method='average'):
        '''Project data onto the given axis.'''
        
        ndim = self.get_dimension(header)
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
    
    
    def _project2D(self, step, data, header, proj, proj_method='average'):
        '''Project data onto 2 given axes.'''
        
        ndim = self.get_dimension(header)
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
    
        
    def _set_marker(self, step, data, plot, style, marker_color='#FF9600'):
        '''Mark the given time slice in a time course plot.'''
                    
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
        #========================================================================================================================
                    
        if style == ' ' or style == 'heatmap':
            # Mark step with line
            plot.axvline(x=step, color=marker_color)
            
        else:
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
    
        
    def get_data(self, csv_f, header):
        '''Gets data and time codes from given csv file.'''
        
        time_stamps = []
        
        data = None
        count = 0
        csv_file = open(csv_f, 'rb')
        
        reader = csv.reader(csv_file)
                
        row_count = len(list(open(csv_f)))-1 
                
        # skip header
        next(reader, None)
        
        # Progress bar
        progress_dlg = Progress(None, -1, 'Loading data', '', row_count+1)
        
        # build time_stamps list and data matrix
        #========================================================================================================================
        for row in reader:
            
            # Update time_stamps list
            time_stamps.append(row[0])
            row.pop(0)
            
            # Matrix generation
            if data is None:
                data = np.zeros((row_count, len(row)))
            
            # Fill current data row and update progress bar
            data[count] = row
            count += 1
            progress_dlg.Update(count)
        #========================================================================================================================
                               
        progress_dlg.Hide()
        wx.CallAfter(progress_dlg.Destroy)
        csv_file.close()
                                                        
        return data, time_stamps


    def get_header(self, csv_f):
        '''Gets header from given csv file.'''
        
        #if csv_f.endswith('.csv'):
        csv_file = open(csv_f, 'rb')        
        reader = csv.reader(csv_file)
        header = reader.next()
        csv_file.close()

        if csv_f.endswith('.data'):
            header = header[:-1]

        return header   
    
            
    def _initialize_3D_plot(self, mode=None, figure=None, title=''):
        
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
    
    
    def _process_image(self, data, header, step):
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


    def plot_snapshot(self, step, data, vmin, vmax, resolution, header, style, surface_linewidth, mode=' ', proj=' ', proj_method='average', color='#FF9600', figure=None, title=None):        
        ndim = self.get_dimension(header)
        steps = data.shape[0]
                        
        if figure is None:
            fig = plt.figure()
        else:
            fig = figure
                    
        if style == 'image':
            try:
                image = self._process_image(data=data, header=header, step=step)                
                plot = plt.imshow(image, origin='lower', rasterized=True)
                
                return plot, data
                
            except TypeError:
                dlg = wx.MessageDialog(parent = None, 
                                       message = 'The plot was not updated.', 
                                       caption = '', 
                                       style = wx.OK|wx.ICON_INFORMATION|wx.CENTER|wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Hide()
                wx.CallAfter(dlg.Destroy)
        
        else:                    
            if ndim == 1:                
                plot = fig.add_subplot(1,1,1)
                plot.plot(data[step], color=color)
            
            elif ndim != 1:
                if proj != ' ':
                                    
                    if ',' in proj:
                        
                        try:
                            X_1, X_2, data = self._project2D(step=step, data=data, header=header, proj=proj, proj_method=proj_method)
                            
                        except IndexError:
                            dlg = wx.MessageDialog(parent = None, 
                                                   message = 'The ' + str(step) + '. snapshot does not exist.', 
                                                   caption = 'An Error has occurred.', 
                                                   style = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                            
                            dlg.ShowModal()
                            dlg.Hide()
                            wx.CallAfter(dlg.Destroy)
                            
                        if style != 'heatmap':
                            plot = self._initialize_3D_plot(mode=mode, figure=figure, title=title)
                            
                            if style == 'surface':
                                plot.plot_surface(X_1, X_2, data,rstride=resolution, cstride=resolution,cmap='coolwarm', alpha=0.5, linewidth=surface_linewidth, rasterized=True)
                            elif style == 'wireframe':
                                plot.plot_wireframe(X_1,X_2, data, rstride=resolution,cstride=resolution, color=color, rasterized=True)
                            
                        elif style == 'heatmap':
                            plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode, figure=figure)
                                                    
                    elif ',' not in proj:
                        
                        try:
                            data = self._project(mode=mode, steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)[2]    
                        except UnboundLocalError:
                            raise UnboundLocalError
                        
                        if mode == 'snapshot sequence':
                            fig = plt.figure()
                        else:
                            fig = figure
                    
                        plot = fig.gca()
                        
                        try:
                            plot.plot(data[step], color=color)
                        except IndexError:
                            raise IndexError
                                            
                elif proj == ' ':
                    
                    if ndim == 2:
                        x_1 = int(header[2])
                        x_2 = int(header[3])                    
                        
                        if style != 'image':
                            data = np.reshape(data[step], (x_2, x_1))
                            X_1, X_2 = np.mgrid[:x_2, :x_1]
                    
                        if style != 'heatmap' and style != 'image':
                            plot = self._initialize_3D_plot(mode=mode, figure=figure, title=title)
            
                            if style == 'surface': 
                                plot.plot_surface(X_1,X_2,data,rstride=resolution, cstride=resolution,cmap='coolwarm', alpha=0.5, linewidth=surface_linewidth, rasterized=False)
                            elif style == 'wireframe':
                                plot.plot_wireframe(X_1,X_2,data, rstride=resolution,cstride=resolution, color=color)
                            
                        elif style == 'heatmap':
                            plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode, figure=figure)
            
            try:
                return plot, data

            except UnboundLocalError:
                pass
            
        
    def plot_snapshot_sequence(self, data, header, vmin, vmax, resolution, surface_linewidth, start, step_size, steps, proj, proj_method, style, 
                               x_label=None, y_label=None, z_label=None, file_name=None, file_directory=None, save_mode=False, color='#FF9600', figure=None, title=None):
        
        plot_mode = 'snapshot sequence'
        
        for i in range(int(steps)):
            plot = self.plot_snapshot(data = data,
                                      header = header, 
                                      vmin = vmin, 
                                      vmax = vmax, 
                                      resolution = resolution, 
                                      step = start + (i*step_size), 
                                      style = style, 
                                      surface_linewidth = surface_linewidth,
                                      mode = plot_mode, 
                                      proj = proj, 
                                      proj_method = proj_method,
                                      color = color,
                                      figure = figure,
                                      title = title)[0]
            
            if style == 'heatmap' or style == 'surface' or style == 'wireframe':
                try:
                    figure.gca().invert_yaxis()
                except AttributeError:
                    plot.invert_yaxis()
                    
            self.label_axis(plot=plot, x_label=x_label, y_label=y_label, z_label=z_label)
            
            if save_mode == True:
                self.save_plot(plot=plot, plot_mode=plot_mode, file_name=file_name, file_directory=file_directory, save_mode='sequence', plot_number=i, figure=figure)
            else:
                plt.draw()
                        
    
    def plot_time_course(self, data, header, vmin, vmax, resolution, surface_linewidth, proj, proj_method, style, color=None, plot=None, marker=False, step=None, 
                         marker_color=None, figure=None, title=None):
        
        ndim = self.get_dimension(header)
        steps = data.shape[0]
        
        if figure is None:
            fig = plt.figure()
        else:
            fig = figure
        
        if plot is None:
            plot = fig.gca()
                    
        if ndim == 0:
            if plot is None:
                if color == None:
                    plot.plot(data)
                else:
                    plot.plot(data, color=color)
            else:
                if color == None:
                    plot.add_line(plt.plot(data)[0])
                else:
                    plot.add_line(plt.plot(data, color=color)[0])
        
        elif ndim != 0:
            if ndim == 1:
                x = int(header[2])
                X_1,X_2 = np.mgrid[:steps, :x]
            
            elif ndim >= 2:
                try:
                    X_1, X_2, data = self._project(mode='time course', steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)
                    
                except UnboundLocalError:
                    raise UnboundLocalError
                
            if style != 'heatmap':
                plot = self._initialize_3D_plot(figure=figure, title=title)
                
                if style == 'surface':   
                    plot.plot_surface(X_1, X_2, data, rstride=resolution, cstride=resolution, cmap='coolwarm', alpha=0.5, linewidth=surface_linewidth, rasterized=True)
                elif style == 'wireframe': 
                    plot.plot_wireframe(X_1, X_2, data, rstride=resolution, cstride=resolution, color=color, rasterized=True)
            
            elif style == 'heatmap':
                plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, figure=figure)
        
        # mark the selected time-slice if true                              
        if marker == True:
            plot = self._set_marker(step=step, data=data, plot=plot, style=style, marker_color=marker_color)
              
        return plot
    
    
    def plot_heatmap(self, X_1, X_2, data, vmin, vmax, mode=None, figure=None):
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
                        
    
    def label_axis(self, plot, x_label, y_label, z_label=None):
        '''Adds axis labels to an existing plot.'''
        
        try:
            colorbar = None
        except UnboundLocalError:
            pass
        
        # Plot is no heatmap
        #if 'matplotlib.image.AxesImage' not in str(type(plot)):
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


    def save_plot(self, plot, plot_mode, file_name, file_directory, save_mode='single', plot_number=0, figure=None, file_path=None):
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
            
#========================================================================================================================

if __name__ == '__main__':
    app = RDPApp()
    app.MainLoop()