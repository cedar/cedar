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
    Date:        2015 05 13

    Description: 

    Credits:

========================================================================================================================
'''

from matplotlib.collections import PolyCollection
from mpl_toolkits.mplot3d import Axes3D
from scipy import ndimage

import csv
import matplotlib as mpl
mpl.use('WXAgg')
import matplotlib.pyplot as plt
mpl.interactive(True)
import math
import numpy as np
import os
import re
import sys
import wx

from wx.lib.embeddedimage import PyEmbeddedImage
from functools import partial

# Prevent Eclipse warning from Axes3D import
Axes3D


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
    

class Progress(wx.ProgressDialog):
    '''
    Simple progress bar
    '''
    def __init__(self, parent, _, title, message, maximum):
        wx.ProgressDialog.__init__(self, title, message=message, maximum=maximum, parent=parent, style=wx.PD_ELAPSED_TIME|wx.PD_REMAINING_TIME|wx.STAY_ON_TOP)
        
        return
        
#========================================================================================================================

class SnapshotSequenceDialog(wx.Dialog):
    def __init__(self, parent, _, title):
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
        
        x_axis_label.Bind(wx.EVT_TEXT, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
        y_axis_label.Bind(wx.EVT_TEXT, partial(parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
        
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
            
            z_axis_label.Bind(wx.EVT_TEXT, partial(self.parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            x_axis_label.Bind(wx.EVT_TEXT, partial(self.parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            y_axis_label.Bind(wx.EVT_TEXT, partial(self.parent.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
        
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

        self.parent.plot = self.parent.frame.rdp_plot.plot_snapshot_sequence(start = self.parent.step, 
                                                                             step_size = self.parent.step_size, 
                                                                             steps = self.parent.nstep, 
                                                                             style = self.parent.style, 
                                                                             data = self.parent.data, 
                                                                             header = self.parent.header, 
                                                                             vmin = self.parent.vmin,
                                                                             vmax = self.parent.vmax,
                                                                             resolution = self.parent.resolution,
                                                                             proj = self.parent.proj,
                                                                             proj_method = self.parent.proj_method,
                                                                             x_label = self.parent.x_label,
                                                                             y_label = self.parent.y_label,
                                                                             z_label = self.parent.z_label,
                                                                             file_name = self.parent.flist_sorted[self.parent.selection],
                                                                             file_directory = self.parent.dir,
                                                                             save_mode = self.parent.save_mode,
                                                                             color = self.parent.line_color,
                                                                             figure = self.parent.figure)
                
        self.Hide()
        wx.CallAfter(self.Destroy)

        
    def evt_cancel_button(self, event):
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
        
        self.rdp_plot = RDPPlot()
        self.rdp_image_files = RDPImageFiles()
        self.frame = self
        self.parent = parent
                
        if self.parent is None:
            wx.Frame.__init__(self, parent=self.parent, title=title, style=wx.MINIMIZE_BOX|wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN)
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
                
                if 'cedarRecordings' in self.dir:
                    break
                
        else:
            wx.Frame.__init__(self, parent=self.parent, title=' ', style=wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN|wx.FRAME_NO_TASKBAR)
            self.dir = self.parent.dir
        
        # Frame layout    
        if self.parent is None:
            self.rdp_browser = RDPBrowserPanel(parent=self)
            self.SetSizer(self.rdp_browser.main_sizer)
            
        if self.parent is not None:
            self.rdp_gui = RDPGUI(parent=self)
            self.SetSizer(self.rdp_gui.main_sizer)
            
        self.Sizer.Fit(self)            
        self.Show()
            
        # Event Handling
        self.Bind(wx.EVT_CLOSE, self.evt_close)
                    
        return
    
    
    def evt_close(self, event):
            
        if self.parent is None:
            sys.exit()
        else:
            self.Hide()
            wx.CallAfter(self.Close)
            wx.Yield()
    
#========================================================================================================================

class RDPBrowserPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.frame = parent
        
        self.SetAutoLayout(True)
        
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.select_directory = wx.StaticText(self, -1, 'Select directory:')
        
        # File browser
        self.browser = wx.GenericDirCtrl(parent=self, filter=('*.csv'), dir=self.frame.dir, size=(325, 450))            
        self.sel_btn = wx.Button(parent=self, label = 'Select')
        self.sel_btn.Bind(wx.EVT_BUTTON, self.evt_sel_btn)
        
        # Main sizer
        self.main_sizer.Add(item=self.select_directory, proportion=0, flag=wx.TOP|wx.LEFT|wx.BOTTOM, border=10)
        self.main_sizer.Add(item=self.browser, proportion=0, flag=wx.LEFT|wx.BOTTOM|wx.RIGHT|wx.EXPAND, border=10)
        self.main_sizer.Add(item=self.sel_btn, proportion=0, flag=wx.LEFT|wx.BOTTOM, border=10)
        
        # Layout
        self.SetSizer(self.main_sizer)
        self.Sizer.Fit(self)
        self.Fit()
        
        return
    
        
    def evt_sel_btn(self, event):
        '''Switch browser panel with control panel for plot generation/manipulation'''
                      
        frame = self.GetParent()
        frame.dir = self.browser.GetPath()
                
        # Initialize plot generation GUI      
        rdp_gui = RDPGUI(parent=frame)
        frame.SetSizer(rdp_gui.main_sizer)
        frame.Sizer.Fit(frame)
        self.Hide()
        wx.CallAfter(self.Close)
        
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
        self.ext = '.csv'
        self.flist = [record_file for record_file in os.listdir(self.dir) if record_file.lower().endswith(self.ext)]
        self.flist_sorted = self.frame.rdp_plot._sort_alphnum(self.flist)
                
        self.data = None
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
        self.timer = wx.Timer(self)
        self.frame_ids = []
        self.frame_ids.append(self.frame.GetId())
        self.figure = None
        self.title = ''
        
        # Plot modes
        self.mode_ch = [' ', 'snapshot', 'snapshot sequence', 'time course']
        self.multi_mode_ch = [' ', 'snapshot', 'time course']
        
        # Projection choices for time course plot mode
        self.proj_ch = [' ', 'x_1', 'x_2', 'x_3', 'x_4', 'x_5']
        
        # Projection choices for snapshot/snapshot sequence plot modes
        self.proj_ch_step = [' ', 'x_1', 'x_2', 'x_3', 'x_4', 'x_5', 
                             'x_1, x_2', 'x_1, x_3', 'x_1, x_4', 'x_1, x_5', 'x_2, x_3', 
                             'x_2, x_4', 'x_2, x_5', 'x_3, x_4', 'x_3, x_5', 'x_4, x_5']
        self.style_ch = [' ', 'heatmap', 'image', 'surface', 'wireframe']
        self.proj_methods = [' ', 'average', 'maximum', 'sum']
        
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
        self.sel_cbox = wx.ComboBox(self, choices = self.flist_sorted, value = ' ', style = wx.CB_READONLY) 
        self.mode_cbox = wx.ComboBox(self, style = wx.CB_READONLY)          
        self.proj_cbox = wx.ComboBox(self, style = wx.CB_READONLY)
        self.proj_method_cbox = wx.ComboBox(self, choices = self.proj_methods, value=' ', style = wx.CB_READONLY)
        self.style_cbox = wx.ComboBox(self, choices = self.style_ch, style = wx.CB_READONLY)
        
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
            
        self.marked_check_box = wx.CheckBox(self)
        
        # Buttons
        #========================================================================================================================
        self.plot_btn = wx.Button(self, label = 'Plot')
        self.save_btn = wx.Button(self, wx.ID_SAVE, label = 'Save')
        
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
        self.player_caption_sizer.Add(self.marked_check_box, 0)
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
            self.selection_sizer.Add(self.switch_btn, 0, wx.EXPAND)
            self.selection_sizer.Add(self.add_figure_frame_btn, 0, wx.EXPAND)
            
        self.selection_sizer.Add(self.sel_cbox, 0)
        
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
        self.cbox_grid_sizer.Add(self.style_cbox, 2, wx.ALIGN_RIGHT)
        #========================================================================================================================
        
        # Buttons
        #========================================================================================================================
        self.plot_sizer.Add(item=self.plot_btn, proportion=1, flag=wx.ALIGN_LEFT|wx.EXPAND)
        self.plot_sizer.Add(item=self.save_btn, proportion=1, flag=wx.ALIGN_LEFT|wx.EXPAND)
        self.btn_sizer.Add(item=self.plot_sizer, proportion=1, flag=wx.ALIGN_LEFT|wx.EXPAND)
        #========================================================================================================================
        
        # Dividing lines
        #========================================================================================================================
        self.line_sizer_1.Add(item=self.line_1, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        self.line_sizer_2.Add(item=self.line_2, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        self.line_sizer_4.Add(item=self.line_4, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        #========================================================================
                
        # Build main sizer
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
        
        if self.frame.parent is None:
            self.pos_slider.Bind(wx.EVT_SLIDER, self.evt_pos_slider)
            self.pos_slider.Bind(wx.EVT_SCROLL, self.evt_pos_slider)
            self.play_pause_btn.Bind(wx.EVT_BUTTON, self.evt_play_pause_btn)
            self.reverse_play_pause_btn.Bind(wx.EVT_BUTTON, self.evt_reverse_play_pause_btn)
            self.increase_single_step_btn.Bind(wx.EVT_BUTTON, self.evt_increase_single_step_btn)
            self.decrease_single_step_btn.Bind(wx.EVT_BUTTON, self.evt_decrease_single_step_btn)
            self.reset_btn.Bind(wx.EVT_BUTTON, self.evt_reset_btn)
            self.switch_btn.Bind(wx.EVT_BUTTON, self.evt_switch_btn)
            
        self.marked_check_box.Bind(wx.EVT_CHECKBOX, self.evt_marked_check_box)
        
        if self.frame.parent is None:
            self.add_figure_frame_btn.Bind(wx.EVT_BUTTON, self.evt_add_figure_frame_btn)
        #========================================================================================================================
        
        # Layout
        #========================================================================================================================
        self.SetSizer(self.main_sizer)
        self.Fit()
        #========================================================================================================================
        
        self.mode_cbox.Disable()
        self.proj_cbox.Disable()
        self.proj_method_cbox.Disable()
        self.style_cbox.Disable()
        self.marked_check_box.Disable()
          
        return
    
    
    def evt_add_figure_frame_btn(self, event):
        new_frame = RDPMainWindow(parent=self.frame, title='', pos=self.frame.GetPosition(), size=None, style=None, name=None) 
        new_frame.rdp_gui.step = self.step  
            
        new_frame.Show()
        new_frame_id = new_frame.GetId()        
        self.frame_ids.append(new_frame_id)
                
         
    def evt_marked_check_box(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            
            current_frame.marked = current_frame.marked_check_box.GetValue()
            wx.CallAfter(current_frame._update_plot)
            wx.Yield()
    
        
    def evt_close_plot_control_frame(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame.timer.Stop()
            current_frame.control_plot_frame.Hide()
        
        
    def evt_close_figure(self, event):
        self.figure = None
        
        
    def evt_check_figures(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
        
            if current_frame.figure is None:
                if current_frame.control_plot_frame:
                    current_frame.control_plot_frame.Close()
                current_frame.plot_btn.SetLabel('Plot')
            else:
                current_frame.plot_btn.SetLabel('Update')
                
                
    def evt_play_pause_btn(self, event):
        self._play_pause_btn(reverse=False)
        
                
    def evt_reverse_play_pause_btn(self, event):
        self._play_pause_btn(reverse=True)
        
            
    def evt_reset_btn(self, event):
        '''Reset plot to default.'''
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
        
            current_frame.step = 0
            current_frame.marked = False
            current_frame.marked_check_box.SetValue(False)
            
            if i == 0:
                current_frame.pos_slider.SetValue(current_frame.step)
                current_frame.time_stamp_display.SetLabel(' ')
    
            wx.CallAfter(current_frame._update_plot)
        
        
    def evt_increase_single_step_btn(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame._move_single_step(increase=True)
        
    
    def evt_decrease_single_step_btn(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]        
            current_frame._move_single_step(increase=False)
        
    
    def evt_axis_label(self, event, x_axis_label, y_axis_label, z_axis_label=None):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
        
            current_frame.x_label = x_axis_label.GetValue()
            current_frame.y_label = y_axis_label.GetValue()
            
            if current_frame.z_label is not None:
                current_frame.z_label = z_axis_label.GetValue()
            
            wx.CallAfter(current_frame._update_plot)
        
    
    def evt_add_time_course(self, event):
        '''Add time course to currently displayed plot.'''
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame._add_time_course()

        
    def evt_resolution_spn(self, event):
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame.resolution = current_frame.control_plot_frame.resolution_spn.GetValue()
            wx.CallAfter(current_frame._update_plot)
        
        wx.Yield()

    
    def evt_vmax_spn(self, event):
        
        widget = event.GetEventObject()
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame.vmax = float(widget.GetValue())
            wx.CallAfter(current_frame._update_plot)
        
        wx.Yield()

    
    def evt_vmin_spn(self, event):
        
        widget = event.GetEventObject() 
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]       
            current_frame.vmin = float(widget.GetValue())
            wx.CallAfter(current_frame._update_plot)
            
        wx.Yield()
        
    
    def evt_proj_cbox(self, event):
        widget = event.GetEventObject()
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]          
            current_frame.proj = widget.GetValue()
            
            if ',' not in current_frame.proj and current_frame.mode != 'time course':
                current_frame.style_cbox.Disable()
                current_frame.proj_method_cbox.Disable()
                current_frame.style_cbox.SetValue('')
                current_frame.proj_method_cbox.SetValue('')
            else:
                current_frame.style_cbox.Enable()
                current_frame.proj_method_cbox.Enable()
                
            if self.proj == ' ':
                current_frame.proj_method = ' '
                current_frame.style = ' '
                current_frame.proj = ' '
                
                current_frame.proj_method_cbox.Disable()
                current_frame.style_cbox.Disable()
                
                current_frame.proj_method_cbox.SetValue(self.proj_method)
                current_frame.style_cbox.SetValue(self.style)
                
        wx.Yield()
        
        
    def evt_proj_method_cbox(self, event):
        widget = event.GetEventObject()
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]   
            current_frame.proj_method = widget.GetValue()
            
            if current_frame.proj_method == ' ':
                current_frame.style_cbox.Disable()
                current_frame.style = ' '
                current_frame.style_cbox.SetValue(current_frame.style)
            else:
                current_frame.style_cbox.Enable()
                        
        wx.Yield()

    
    def evt_style_cbox(self, event):
        widget = event.GetEventObject()
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i] 
            current_frame.style = widget.GetValue()
            current_frame.plot_btn.Bind(wx.EVT_BUTTON, current_frame.evt_plot)
                    
            if current_frame.style != 'image':
                current_frame.mode_cbox.Enable()
                current_frame.proj_cbox.Enable()
                current_frame.proj_method_cbox.Enable()
            
            else:
                current_frame.mode = 'snapshot'
                current_frame.mode_cbox.Disable()
                current_frame.proj_cbox.Disable()
                current_frame.proj_method_cbox.Disable()
    
        wx.Yield()

    
    def evt_reset_heatmap_boundaries_btn(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame.vmin = current_frame.vmax = None
            
            wx.CallAfter(current_frame._update_plot)
            
        wx.Yield()

    
    def evt_switch_btn(self, event):  
        
        # Clear memory
        if self.data is not None:
            del self.data
            self.data = None
        
        # Close open plots              
        if self.figure:
            plt.close(self.figure)
            self.figure = None
                
        frame = self.GetParent()
        browser_panel = RDPBrowserPanel(parent=frame)
        
        # Replace plot generation frame with file browser
        frame.SetSizer(browser_panel.main_sizer)
        
        # Layout
        frame.Sizer.Fit(frame)
        frame.Layout()
        wx.CallAfter(self.Hide)
        wx.CallAfter(self.Close)
                
    
    def evt_pos_slider(self, event):
        
        widget = event.GetEventObject()
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
            current_frame.step = widget.GetValue()
        
            if i == 0:
                # Set time code
                current_time_stamp = self.time_stamps[current_frame.step]
                current_frame.time_stamp_display.SetLabel(current_time_stamp)
            else:
                current_frame = wx.FindWindowById(self.frame_ids[i]).rdp_gui
                
            # Update currently displayed plot
            if current_frame.figure:
                wx.CallAfter(current_frame._update_plot)
            
            wx.Yield()
            
        event.Skip()
            

    def evt_mode_cbox(self, event):
        
        for i in range(len(self.frame_ids)):
            current_frame = self._set_current_frame(i)
            current_frame.title = self.frame_ids[i]
        
            current_frame.mode = current_frame.mode_cbox.GetValue()   
            
            if current_frame.figure:
                plt.close(current_frame.figure)
            
            if current_frame.mode == 'time course':
                current_frame.marked_check_box.Enable()
            else:
                current_frame.marked_check_box.Disable()
            
            if current_frame.mode != ' ':
                current_frame.proj_cbox.Enable()
            else:
                current_frame.proj_cbox.Disable()
                current_frame.proj_method = ' '
                current_frame.style = ' '
                current_frame.proj = ' '
                
                current_frame.proj_method_cbox.Disable()
                current_frame.style_cbox.Disable()
                
                current_frame.proj_cbox.SetValue(current_frame.proj)
                current_frame.proj_method_cbox.SetValue(current_frame.proj_method)
                current_frame.style_cbox.SetValue(current_frame.style)
            
            # Fill projection combobox with the fitting options
            if current_frame.mode == 'time course':
                current_frame.proj_cbox.SetItems(current_frame.proj_choice_time_course)
            elif current_frame.mode == 'snapshot' or current_frame.mode == 'snapshot sequence':
                current_frame.proj_cbox.SetItems(current_frame.proj_choice_snapshot)
            else:
                current_frame.proj_cbox.SetItems([])
            
            # Update plot
            if current_frame.mode != 'snapshot sequence':
                if current_frame.figure:
                    try:
                        wx.CallAfter(current_frame._update_plot)
                    except TypeError:
                        pass
            
                
            current_frame.plot_btn.Bind(wx.EVT_BUTTON, current_frame.evt_plot)
            current_frame.save_btn.Bind(wx.EVT_BUTTON, current_frame.evt_save_plot)
            
            
    def evt_plot(self, event):
        self.plot = None        
        self._plot()
                
        if self.mode != 'snapshot sequence':
            self.create_control_plot_frame(parent=self)
            self.control_plot_frame.Show()
            
            
    def evt_line_color_ctrl(self, event):
        widget = event.GetEventObject()
        self.aux_line_color = widget.GetColour()
        widget.SetColour(self.aux_line_color)
        
        # Color conversion
        red = self.aux_line_color.Red()/255.
        green = self.aux_line_color.Green()/255.
        blue = self.aux_line_color.Blue()/255.
        
        self.line_color = [red, green, blue]
        
        # Update currently displayed plot
        if self.figure:
            wx.CallAfter(self._update_plot)
            
        wx.Yield()
        
        
    def evt_marker_color_ctrl(self, event):
        widget = event.GetEventObject()
        self.aux_marker_color = widget.GetColour()
        widget.SetColour(self.aux_marker_color)
        
        # Color conversion
        red = self.aux_marker_color.Red()/255.
        green = self.aux_marker_color.Green()/255.
        blue = self.aux_marker_color.Blue()/255.
        
        self.marker_color = [red, green, blue]
        
        # Update currently displayed plot
        if self.figure:
            wx.CallAfter(self._update_plot)
            
        wx.Yield()
        
                            
    def evt_sel_cbox(self, event):
        widget = event.GetEventObject()
        
        self.selection = widget.GetSelection()
        self._update_selection_data()
        self.sel_cbox.SetValue(widget.GetValue())
        
        self.mode_cbox.Enable()
        
        # Mirror current selection on the control plot frame, if present
        if self.control_plot_frame:
            if hasattr(self.control_plot_frame, 'sel_cbox'):
                self.control_plot_frame.sel_cbox.SetValue(widget.GetValue())
        
                    
    def evt_save_plot(self, event):
        self._plot(save=True)
        
                
    def create_control_plot_frame(self, parent):
        
        self.timer.Start(5)
        
        # Initialize control plot frame if none is present
        if self.control_plot_frame is None:
            self.control_plot_frame = wx.Frame(parent=parent, id=-1, title='Plot controls ' + str(self.title), style=wx.CLIP_CHILDREN|wx.FRAME_NO_TASKBAR|wx.CAPTION|wx.CLOSE_BOX)

            # Bind close event
            self.control_plot_frame.Bind(wx.EVT_CLOSE, self.evt_close_plot_control_frame)
                
        control_plot_panel = wx.Panel(parent=self.control_plot_frame, id=-1)
        self.Bind(wx.EVT_TIMER, self.evt_check_figures)
        
        # Sizers
        #========================================================================================================================
        top_sizer = wx.BoxSizer(wx.VERTICAL)
        
        if 'Axes3DSubplot' in str(type(self.plot)) or self.style == 'heatmap':
            axes_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
        else:
            axes_grid_sizer = wx.FlexGridSizer(rows=2, cols=2)
        
        axes_grid_sizer.SetFlexibleDirection(wx.BOTH)
        axes_label_sizer = wx.BoxSizer(wx.VERTICAL)
        min_max_sizer = wx.FlexGridSizer(rows=2, cols=2)         
        min_max_sizer.SetFlexibleDirection(wx.BOTH)
        #========================================================================================================================
        
        # Labels
        #========================================================================================================================
        label_axes_txt = wx.StaticText(control_plot_panel, -1, 'Axis labels')
        x_axis_txt = wx.StaticText(control_plot_panel, -1, 'X axis \t')
        y_axis_txt = wx.StaticText(control_plot_panel, -1, 'Y axis \t')
        
        if self.style != 'heatmap' and 'Axes3DSubplot' in str(type(self.plot)):
            z_axis_txt = wx.StaticText(control_plot_panel, -1, 'Z axis \t')
        elif self.style == 'heatmap':
            z_axis_txt = wx.StaticText(control_plot_panel, -1, 'color bar \t')
        #========================================================================================================================
            
        x_axis_label = wx.TextCtrl(control_plot_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        y_axis_label = wx.TextCtrl(control_plot_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        
        x_axis_label.SetValue(self.x_label)
        y_axis_label.SetValue(self.y_label)
        
        # Build sizers
        #========================================================================================================================
        axes_grid_sizer.Add(x_axis_txt, 0, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        axes_grid_sizer.Add(x_axis_label, 1, wx.ALIGN_CENTER|wx.EXPAND)
        axes_grid_sizer.Add(y_axis_txt, 0, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        axes_grid_sizer.Add(y_axis_label, 1, wx.ALIGN_CENTER|wx.EXPAND)
        
        # Plot is either 3-dimensional or a heatmap
        if 'Axes3DSubplot' in str(type(self.plot)) or self.style == 'heatmap':
            z_axis_label = wx.TextCtrl(control_plot_panel, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
            z_axis_label.SetValue(self.z_label)
            z_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            axes_grid_sizer.Add(z_axis_txt, 0, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
            axes_grid_sizer.Add(z_axis_label, 1, wx.ALIGN_CENTER|wx.EXPAND)
            x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
            y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=z_axis_label))
        else:
            x_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
            y_axis_label.Bind(wx.EVT_TEXT_ENTER, partial(self.evt_axis_label, x_axis_label=x_axis_label, y_axis_label=y_axis_label, z_axis_label=None))
                
        axes_label_sizer.Add(item=label_axes_txt, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.BOTTOM, border=10)
        axes_label_sizer.Add(axes_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)
        
        top_sizer.Add(item=axes_label_sizer, proportion=0, flag=wx.EXPAND|wx.ALIGN_CENTER|wx.RIGHT|wx.LEFT, border=10)
        
        if self.style == 'heatmap':
            line1 = wx.StaticLine(control_plot_panel, -1, style=wx.LI_HORIZONTAL)
            top_sizer.Add(line1, 0, wx.ALIGN_CENTER|wx.EXPAND|wx.RIGHT|wx.LEFT|wx.BOTTOM, border=5)
            heatmap_boundaries_txt = wx.StaticText(control_plot_panel, -1, 'Heatmap boundaries')
            
            # Controls for narrowing of depicted value range
            vmin_label = wx.StaticText(control_plot_panel, -1, 'Minimum \t')
            vmax_label = wx.StaticText(control_plot_panel, -1, 'Maximum \t')
            vmin_spn = wx.TextCtrl(control_plot_panel, style=wx.TE_PROCESS_ENTER)
            vmax_spn = wx.TextCtrl(control_plot_panel, style=wx.TE_PROCESS_ENTER)
            
            # Set widgets to default values
            if self.vmin is not None:
                vmin_spn.SetValue(str(self.vmin))
            else:
                vmin_spn.SetValue(str(0.0000))
                
            if self.vmax is not None:
                vmax_spn.SetValue(str(self.vmax))
            else:
                vmax_spn.SetValue(str(0.0000))
            
            reset_heatmap_boundaries_btn = wx.Button(control_plot_panel, label = 'Reset')
            reset_heatmap_boundaries_btn.Bind(wx.EVT_BUTTON, self.evt_reset_heatmap_boundaries_btn)
        
            min_max_sizer.Add(vmin_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=10)
            min_max_sizer.Add(vmin_spn, 2, wx.ALIGN_RIGHT|wx.EXPAND|wx.RIGHT, border=10)
            min_max_sizer.Add(vmax_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=10)
            min_max_sizer.Add(vmax_spn, 2, wx.ALIGN_RIGHT|wx.EXPAND|wx.RIGHT, border=10)
            
            top_sizer.Add(heatmap_boundaries_txt, 0, flag=wx.ALIGN_LEFT|wx.BOTTOM|wx.LEFT, border=10)
            top_sizer.Add(min_max_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)
            top_sizer.Add(reset_heatmap_boundaries_btn, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=10)
            
            vmin_spn.Bind(wx.EVT_TEXT_ENTER, self.evt_vmin_spn)
            vmax_spn.Bind(wx.EVT_TEXT_ENTER, self.evt_vmax_spn)
        
        elif self.style == 'wireframe' or self.style == 'surface':
            
            visual_controls_txt = wx.StaticText(control_plot_panel, -1, 'Visual controls')
            plot_control_sizer = wx.BoxSizer(wx.VERTICAL)
            
            if self.style == 'wireframe':
                if self.mode == 'time course':
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
                else:
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=3, cols=2)
                    
            elif self.style == 'surface':
                if self.mode == 'time course':
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=2, cols=2)
                else:
                    plot_control_grid_sizer = wx.FlexGridSizer(rows=1, cols=2)
                
            plot_control_grid_sizer.SetFlexibleDirection(wx.BOTH)
                        
            if self.style == 'wireframe':
                line_color_label = wx.StaticText(control_plot_panel, -1, label='Line color ')  
                self.control_plot_frame.line_color_ctrl = wx.ColourPickerCtrl(control_plot_panel, -1, col=self.aux_line_color)
                self.control_plot_frame.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_line_color_ctrl)
                
                plot_control_grid_sizer.Add(line_color_label, 1, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
                plot_control_grid_sizer.Add(self.control_plot_frame.line_color_ctrl, 1, wx.ALIGN_CENTER|wx.EXPAND)
                
            if self.mode == 'time course':
                marker_color_label = wx.StaticText(control_plot_panel, -1, label='Marker color ') 
                self.control_plot_frame.marker_color_ctrl = wx.ColourPickerCtrl(control_plot_panel, -1, col=self.aux_marker_color)
                self.control_plot_frame.marker_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_marker_color_ctrl)
                           
                plot_control_grid_sizer.Add(marker_color_label, 0, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
                plot_control_grid_sizer.Add(self.control_plot_frame.marker_color_ctrl, 1, wx.ALIGN_CENTER|wx.EXPAND)
                
            resolution_label = wx.StaticText(control_plot_panel, -1, 'Resolution ')
            self.control_plot_frame.resolution_spn = wx.SpinCtrl(control_plot_panel, min=1, max=100, value=str(self.resolution))
            self.control_plot_frame.resolution_spn.Bind(wx.EVT_SPINCTRL, self.evt_resolution_spn)
                        
            plot_control_grid_sizer.Add(resolution_label, 0, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
            plot_control_grid_sizer.Add(self.control_plot_frame.resolution_spn, 1, wx.ALIGN_CENTER|wx.EXPAND)
            
            plot_control_sizer.Add(item=visual_controls_txt, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.BOTTOM|wx.TOP, border=10)
            plot_control_sizer.Add(item=plot_control_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.LEFT, border=10)    
            top_sizer.Add(item=plot_control_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT|wx.EXPAND, border=10)
            
        if self.ndim[self.selection] == 0 and self.mode == 'time course':
            self.control_plot_frame.sel_cbox = wx.ComboBox(control_plot_panel, choices=self.flist_sorted, value=self.sel_cbox.GetValue(), style=wx.CB_READONLY)
            self.control_plot_frame.sel_cbox.Bind(wx.EVT_COMBOBOX, self.evt_sel_cbox)
            line2 = wx.StaticLine(control_plot_panel, -1, style=wx.LI_HORIZONTAL)
            top_sizer.Add(line2, 0, wx.ALIGN_CENTER|wx.EXPAND|wx.RIGHT|wx.LEFT|wx.BOTTOM, border=5)
            multi_plot_btn = wx.Button(control_plot_panel, label = 'Add time course', size=(100,30))
            multi_plot_btn.Bind(wx.EVT_BUTTON, self.evt_add_time_course)
            line_color_label = wx.StaticText(control_plot_panel, -1, label='Line color \t')
            
            self.control_plot_frame.line_color_ctrl = wx.ColourPickerCtrl(control_plot_panel, -1, col=self.aux_line_color)
            self.control_plot_frame.line_color_ctrl.Bind(wx.EVT_COLOURPICKER_CHANGED, self.evt_line_color_ctrl)
            
            line_sizer = wx.BoxSizer(wx.HORIZONTAL)
            line_sizer.Add(line_color_label, 1, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
            line_sizer.Add(self.control_plot_frame.line_color_ctrl, 1, wx.ALIGN_RIGHT|wx.EXPAND)
            
            top_sizer.Add(item=self.control_plot_frame.sel_cbox, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND, border=10)
            top_sizer.Add(item=line_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT|wx.EXPAND, border=10)
            top_sizer.Add(item=multi_plot_btn, proportion=0, flag=wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, border=10)
        
        #========================================================================================================================
        
        # Layout
        #========================================================================================================================
        self.control_plot_frame.SetSizerAndFit(top_sizer)
        control_plot_panel.SetSizerAndFit(top_sizer)
        self.control_plot_frame.Fit()
        
                
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
            loop_range = range(self.step, -2, -1)
        
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
                            wx.CallAfter(current_frame._update_plot)
                            
                        inactive_btn.Disable()
                        self.increase_single_step_btn.Disable()
                        self.decrease_single_step_btn.Disable()
                        self.reset_btn.Disable()
                        self.pos_slider.SetValue(min(self.slider_max, self.step))
                        self.time_stamp_display.SetLabel(str(self.time_stamps[self.pos_slider.GetValue()]))
                        
                        wx.Yield()
        
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
                self.pos_slider.SetValue(max(-1, self.step))
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
            current_frame.title = self.frame_ids[i]
            wx.CallAfter(current_frame._update_plot)
        
        
    def _update_plot(self):
                    
        if self.figure:
            if self.ndim[self.selection] == 0:
                plt.cla()                
            else:
                wx.CallAfter(self.figure.clf)
            
        wx.CallAfter(self._plot)
        
        if self.figure:
            if self.control_plot_frame is None:
                self.create_control_plot_frame(self)
                wx.CallAfter(self.control_plot_frame.Show)
            
        wx.Yield()
        
                
    def _add_time_course(self):
        wx.CallAfter(self._plot)
        wx.Yield()
    

    def _update_selection_data(self):
        '''Reset the control panel and update it with the newly selected data.'''
        
        # Clear memory
        if self.data is not None:
            del self.data
            self.data = None
        
        if len(self.frame_ids) == 1:
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
        self.proj_choice_snapshot = self.frame.rdp_plot._build_proj_ch_step(ndim=self.ndim[self.selection], temp_proj_ch_step=self.proj_ch_step)   
        
        # Get data and data header
        self.header = self.frame.rdp_plot.get_header(csv_f=self.dir + '/' + self.flist_sorted[self.selection])
        temp_data = self.frame.rdp_plot.get_data(csv_f=self.dir + '/' + self.flist_sorted[self.selection])
        
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
    
    
    def _plot(self, save=False):
        '''Build plot(s) to either visualize or save as pdf file'''
        
        if self.figure is None:
            self.figure = plt.figure(str(self.title))
            self.figure.canvas.mpl_connect('close_event', self.evt_close_figure)
         
        if self.mode == 'snapshot':                    
            try:
                self.plot = self.frame.rdp_plot.plot_snapshot(step = self.step, 
                                                              style = self.style, 
                                                              data = self.data, 
                                                              header = self.header, 
                                                              vmin = self.vmin,
                                                              vmax = self.vmax,
                                                              resolution = self.resolution, 
                                                              proj = self.proj,
                                                              proj_method = self.proj_method,
                                                              color = self.line_color,
                                                              figure = self.figure,
                                                              title = self.title)
                
            except IndexError:
                dlg = wx.MessageDialog(parent  = None, 
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
            except UnboundLocalError:
                dlg = wx.MessageDialog(parent = None, 
                                       message = 'It is not possible to build a time course out of 2-dimensional time slices.', 
                                       caption = 'The attempted operation is not possible.', 
                                       style = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Hide()
                wx.CallAfter(dlg.Destroy)
                
        self.frame.rdp_plot.label_axis(plot=self.plot, x_label=self.x_label, y_label=self.y_label, z_label=self.z_label)
        
        if save is False and self.mode != 'snapshot sequence':
            self.figure.canvas.draw()
        
        elif save is True and self.mode != 'snapshot sequence':
            self.frame.rdp_plot.save_plot(plot=self.plot, plot_mode=self.mode, file_name=self.flist_sorted[self.selection], file_directory=self.dir, figure=self.figure)  
            self.figure = None 
                
#========================================================================================================================

class RDPPlot(object):

    def __init__(self):
        return
    
                
    def _sort_alphnum(self, unsorted):
        '''Sort given list alphanumerically.'''
        conv = lambda text: int(text) if text.isdigit() else text
        alphnum_key = lambda key: [conv(c) for c in re.split('([0-9]+)', key)]
        
        return sorted(unsorted, key=alphnum_key)
    
    def _build_proj_ch_step(self, ndim, temp_proj_ch_step):
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
                    
        xs = np.arange(0, data.shape[1], 0.1)
        ys = np.zeros(xs.shape[0])
        ys.fill(min_data)
        ys[0], ys[-1] = max_data, max_data
            
        v = []
        v.append(list(zip(xs, ys)))
        
        if style == ' ' or style == 'heatmap':
            # Mark step with line
            plt.axvline(x=step, color=marker_color)
            
        else:
            if style == 'wireframe':
                cross = PolyCollection(v, facecolors=marker_color, closed=False)
            elif style == 'surface':
                cross = PolyCollection(v, facecolors=marker_color, closed=False)
            
            cross.set_alpha(0.25)
            
            # Mark x with either blue (wireframe) or red (surface) plane
            plot.add_collection3d(cross, zs=step, zdir='x')
                    
        return plot
    
        
    def get_data(self, csv_f):
        '''Gets data and time codes from given csv file.'''
    
        data = None
        count = 0
        csv_file = open(csv_f, 'rb')
        reader = csv.reader(csv_file)
        row_count = len(list(open(csv_f)))-1 
        time_stamps = []
                
        # skip header
        next(reader, None)
        
        # Progress bar
        progress_dlg = Progress(None, -1, 'Load data recording', 'loading...', row_count+1)
        
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
        
        csv_file = open(csv_f, 'rb')        
        reader = csv.reader(csv_file)
        header = reader.next()
        csv_file.close()
                
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


    def plot_snapshot(self, step, data, vmin, vmax, resolution, header, style, mode=' ', proj=' ', proj_method='average', color='#FF9600', figure=None, title=None):        
        ndim = self.get_dimension(header)
        steps = data.shape[0]
                    
        if style == 'image':
            
            try:
                image = self._process_image(data=data, header=header, step=step)
                plot = plt.imshow(image, origin='lower')
                
                return plot
            
            except TypeError:
                dlg = wx.MessageDialog(parent  = None, 
                message = 'The plot was not updated.', 
                caption = '', 
                style = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Hide()
                wx.CallAfter(dlg.Destroy)
        
        else:                    
            if ndim == 1:
                if figure is None:
                    if mode == 'snapshot sequence':
                        fig = plt.figure()
                    else:
                        if figure is None:
                            fig = plt.figure()
                        else:
                            fig = figure
                    
                    fig.canvas.set_window_title('Test')

                else:
                    fig = figure
                
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
                                plot.plot_surface(X_1,X_2,data,rstride=resolution, cstride=resolution,cmap='coolwarm', alpha=0.5)
                            elif style == 'wireframe':
                                plot.plot_wireframe(X_1,X_2, data, rstride=resolution,cstride=resolution, color=color)
                            
                        elif style == 'heatmap':
                            plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode)
                                                    
                    elif ',' not in proj:
                        
                        try:
                            data = self._project(mode=mode, steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)[2]    
                        except UnboundLocalError:
                            raise UnboundLocalError
                        
                        if figure is None:
                            if mode == 'snapshot sequence':
                                fig = plt.figure()
                            else:
                                if figure is None:
                                    fig = plt.figure()
                                else:    
                                    fig = figure
                            
                            fig.canvas.set_window_title(' ')             
                        else:
                            fig = figure
                            
                        plot = fig.add_subplot(1,1,1)
                        
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
                                plot.plot_surface(X_1,X_2,data,rstride=resolution, cstride=resolution,cmap='coolwarm', alpha=0.5)
                            elif style == 'wireframe':
                                plot.plot_wireframe(X_1,X_2,data, rstride=resolution,cstride=resolution, color=color)
                            
                        elif style == 'heatmap':
                            plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode)
                            
            return plot
        
        
    def plot_snapshot_sequence(self, data, header, vmin, vmax, resolution, start, step_size, steps, proj, proj_method, style, 
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
                                      mode = plot_mode, 
                                      proj = proj, 
                                      proj_method = proj_method,
                                      color = color,
                                      figure = figure,
                                      title = title)
            #if i == 0:
            self.label_axis(plot = plot, x_label=x_label, y_label=y_label, z_label=z_label)
            
            if save_mode == True:
                self.save_plot(plot=plot, plot_mode=plot_mode, file_name=file_name, file_directory=file_directory, save_mode='sequence', plot_number=i, figure=figure)
                        
    
    def plot_time_course(self, data, header, vmin, vmax, resolution, proj, proj_method, style, color=None, plot=None, marker=False, step=None, marker_color=None, figure=None, title=None):
        ndim = self.get_dimension(header)
        steps = data.shape[0]
        
        if plot is None and ndim == 0:
            
            if figure is None:
                fig = plt.figure()
            else:
                fig = figure
                
            plot = fig.add_subplot(1,1,1)
                    
        if ndim == 0:
            if color == None:
                plot.plot(data)
            else:
                plot.plot(data, color=color)
        
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
                    plot.plot_surface(X_1, X_2, data, rstride=resolution, cstride=resolution, cmap='coolwarm', alpha=0.5)
                elif style == 'wireframe': 
                    plot.plot_wireframe(X_1, X_2, data, rstride=resolution, cstride=resolution, color=color)
            
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
        
        # Either plot each plot in a separate figure (snapshot sequence) or override existing figure with new plot (otherwise)
        if mode != 'snapshot sequence':
            if figure is None:
                fig = plt.figure()
            else:
                fig = figure
        
        else:
            fig = figure
        
        fig.canvas.set_window_title('Test')  
        plot = fig.add_subplot(1,1,1)
        plot.add_collection(plt.pcolor(X_1, X_2, data, cmap='RdYlBu_r', vmin=vmin, vmax=vmax))
        plt.axis([X_1.min(), X_1.max(), X_2.min(), X_2.max()])
                
        return plot
    
    
    def label_axis(self, plot, x_label, y_label, z_label=None):
        '''Adds axis labels to an existing plot.'''
        
        if plot is None:
            return 
        
        else:
            
            # Plot is no heatmap
            if 'matplotlib.image.AxesImage' not in str(type(plot)):
                plot.set_xlabel(x_label)
                plot.set_ylabel(y_label)
            
            # Plot is in 3D
            if 'Axes3DSubplot' in str(type(plot)):
                plot.set_zlabel(z_label)
            
            # Plot is a heatmap
            elif 'matplotlib.axes.AxesSubplot' in str(type(plot)):
                try:
                    colorbar = plt.colorbar()
                    colorbar.set_label(z_label)
                    
                except RuntimeError:
                    pass
            
            return plot


    def save_plot(self, plot, plot_mode, file_name, file_directory, save_mode='single', plot_number=0, figure=None):
        '''Saves either a plot or a sequence of plots to one/several pdf files.'''
        
        plot_count = 1
        
        file_path_partial = None
        
        if plot_mode == 'snapshot sequence': 
            plot_mode = 'snapshot_sequence'
        if plot_mode == 'time course':
            plot_mode = 'time_course'
                    
        # Snapshot or time course
        if save_mode == 'single':
            file_path = file_directory + '/' + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(plot_count) + '.pdf'
                    
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
                                       
            file_path = file_path_partial + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(plot_count) + '.pdf'
                                
            while os.path.exists(file_path):
                if file_path_partial is not None:
                    plot_count += 1
                    file_path = file_path_partial + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(plot_count) + '.pdf'
        
        plt.savefig(file_path, transparent=True)
        
        try:
            plt.close(figure)
            
        except TypeError:
            pass
        
        
#========================================================================================================================

if __name__ == '__main__':
    app = RDPApp(False)    
    app.MainLoop()
