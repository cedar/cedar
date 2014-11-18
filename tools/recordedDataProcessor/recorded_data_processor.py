#!/usr/bin/python
'''
========================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
    Date:        2014 11 13

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
plt.interactive(True)
import math
import numpy as np
import os
import re
import wx
import wx.lib.agw.floatspin as FS
from wx.lib.embeddedimage import PyEmbeddedImage

    
class ImageFiles():
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
    def __init__(self, parent, id, title, message, maximum):
        wx.ProgressDialog.__init__(self, title, message=message, maximum=maximum, parent=parent, style=wx.PD_ELAPSED_TIME|wx.PD_REMAINING_TIME|wx.STAY_ON_TOP)
        
        return
        
#========================================================================================================================

class SnapshotSequenceDialog(wx.Dialog):
    def __init__(self, parent, id, title):
        wx.Dialog.__init__(self, parent, id, title)
        
        top_sizer = wx.BoxSizer(wx.VERTICAL)        
        btn_sizer = wx.BoxSizer(wx.HORIZONTAL)
        ok_btn = wx.Button(self, wx.ID_OK, 'OK')
        cancel_btn = wx.Button(self, wx.ID_CANCEL, 'Cancel')
        
        btn_sizer.Add(ok_btn, 1, wx.ALL)
        btn_sizer.Add(cancel_btn, 1, wx.ALL)
        
        step_num = wx.StaticText(self, -1, 'Step number')
        step_num_entry = wx.TextCtrl(self, -1)
        step_num_sizer = wx.BoxSizer(wx.HORIZONTAL)
        step_size = wx.StaticText(self, -1, 'Step size')
        step_size_entry = wx.TextCtrl(self, -1)
        step_size_sizer = wx.BoxSizer(wx.HORIZONTAL)
         
        step_num_sizer.Add(step_num, 3, wx.ALL)
        step_num_sizer.Add(step_num_entry, 2, wx.ALL)
        step_size_sizer.Add(step_size, 3, wx.ALL)
        step_size_sizer.Add(step_size_entry, 2, wx.ALL)
        
        top_sizer.Add(step_num_sizer, 0, wx.ALL)
        top_sizer.Add(step_size_sizer, 0, wx.ALL)
        top_sizer.Add(btn_sizer, 0, wx.ALL|wx.CENTER)
                
        ok_btn.Bind(wx.EVT_BUTTON, lambda evt, step_num_entry=step_num_entry, step_size_entry=step_size_entry: 
                    self.evt_ok_btn(step_num_entry, step_size_entry, evt))
        cancel_btn.Bind(wx.EVT_BUTTON, self.evt_cancel_button)
        
        self.SetSizer(top_sizer)
        top_sizer.Fit(self)
        self.Center(wx.CENTER_ON_SCREEN)
    
    def evt_ok_btn(self, step_num_entry, step_size_entry, event):
        self.GetParent().nstep = step_num_entry.GetValue()
        self.GetParent().step_size = step_size_entry.GetValue()
        self.Destroy()
        
    def evt_cancel_button(self, event):
        self.Destroy()

#========================================================================================================================

class RecordedDataProcessorApp(wx.App):
    def OnInit(self):
        frame = MainWindow(None, 'Recorded Data Processor')
        frame.Show()
        
        return True

#========================================================================================================================

class MainWindow(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent=None, title=title, style=wx.MINIMIZE_BOX|wx.SYSTEM_MENU|wx.CAPTION|wx.CLOSE_BOX|wx.CLIP_CHILDREN)
                        
        self.dir = None 
        BrowserPanel(self)
                        
        return

#========================================================================================================================

class BrowserPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
                                  
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        select_directory = wx.StaticText(self, -1, 'Select directory :')
        
        self.browser = wx.GenericDirCtrl(self, 
                                         filter = ("*.csv"),
                                         style = wx.DIRCTRL_3D_INTERNAL)
        
        self.sel_btn = wx.Button(self, label = 'Select')
        
        self.sel_btn.Bind(wx.EVT_BUTTON, self.evt_sel_button)
        self.main_sizer.Add(select_directory,0, wx.ALL|wx.EXPAND)
        self.main_sizer.Add(self.browser, 5, wx.ALL|wx.EXPAND)
        self.main_sizer.Add(self.sel_btn, 0, wx.ALL)
        
        self.SetSizer(self.main_sizer)
        self.main_sizer.Fit(self)
        self.Show()
        
    def evt_sel_button(self, event):
        frame = self.GetParent()
        frame.dir = self.browser.GetPath()
        RecordedDataProcessorPanel(frame)
        frame.Fit()
        self.Destroy()
        
#========================================================================================================================

class RecordedDataProcessorPanel(wx.Panel):
    
    def __init__(self, parent):
        self.panel = wx.Panel.__init__(self, parent)
        wx.ToolTip.SetDelay(10)
        self.frame = parent
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.dir = self.GetParent().dir
        
        self.x_label = ' '
        self.y_label = ' '
        self.z_label = ' '
        self.marked = False
        self.nstep = 0
        self.step = 0
        self.step_size = 0
        self.slider_max = 0
        self.resolution = 1
        self.vmin = None
        self.vmax = None
        self.proj = None
        self.proj_method = 'addition'
        self.style = ' '
        self.mode = ' '
        self.ext = '.csv'
        self.flist = [file for file in os.listdir(self.dir) if file.lower().endswith(self.ext)]
        self.flist_sorted = FieldPlot()._sort_alphnum(self.flist)
        self.data = None
        self.header = None
        self.header_list = []
        self.ndim = []
        self.time_codes = []
        self.selection = None
        self.proj_choice = None
        self.proj_choice_step = None
        self.plot = None
        
        self.mode_ch = [' ', 'snapshot', 'snapshot sequence', 'timeline']
        self.proj_ch = [' ', 'x_1', 'x_2', 'x_3', 'x_4', 'x_5']
        self.proj_ch_step = [' ', 'x_1', 'x_2', 'x_3', 'x_4', 'x_5', 
                             'x_1, x_2', 'x_1, x_3', 'x_1, x_4', 'x_1, x_5', 'x_2, x_3', 
                             'x_2, x_4', 'x_2, x_5', 'x_3, x_4', 'x_3, x_5', 'x_4, x_5']
        self.style_ch = [' ', 'heatmap', 'image', 'surface', 'wireframe']
        self.proj_methods = [' ', 'addition', 'maximum']
        
        self.vmin_spn = FS.FloatSpin(self, digits=4)
        self.vmax_spn = FS.FloatSpin(self, digits=4)
        self.reset_heatmap_boundaries_btn = wx.Button(self, label = 'Reset')
        self.vmin_spn.Disable()
        self.vmax_spn.Disable()
        self.reset_heatmap_boundaries_btn.Disable()
        
        self.sel_label = wx.StaticText(self, -1, 'Recording')
        self.mode_label = wx.StaticText(self, -1, 'Plot mode')
        self.proj_label = wx.StaticText(self, -1, 'Projection')
        self.proj_method_label = wx.StaticText(self, -1, 'Projection method')
        self.style_label = wx.StaticText(self, -1, 'Plot style')
        self.time_code_display = wx.StaticText(self, -1, '-')
        self.time_code_label = wx.StaticText(self, -1, 'Time code t \t')
        self.vmin_label = wx.StaticText(self, -1, 'Minimum')
        self.vmax_label = wx.StaticText(self, -1, 'Maximum')
        
        self.x_axis_label = wx.StaticText(self, -1, 'X axis')
        self.y_axis_label = wx.StaticText(self, -1, 'Y axis')
        self.z_axis_label = wx.StaticText(self, -1, 'Z axis')
        
        self.heatmap_boundaries_txt = wx.StaticText(self, -1, 'Heatmap boundaries')
        self.label_axes_txt = wx.StaticText(self, -1, 'Label plot axes')
        self.resolution_label = wx.StaticText(self, -1, 'Plot resolution')
        
        self.line_1 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(400,10))
        self.line_2 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(400,10))
        self.line_3 = wx.StaticLine(self, -1, style=wx.LI_VERTICAL)
        self.line_4 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(400,10))
        self.line_5 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL, size=(400,10))
        
        self.sel_cbox = wx.ComboBox(self, choices = self.flist_sorted, value = ' ', style = wx.CB_READONLY)
        self.mode_cbox = wx.ComboBox(self, choices = self.mode_ch, style = wx.CB_READONLY)
        self.proj_cbox = wx.ComboBox(self, value=' ', style = wx.CB_READONLY)
        self.proj_method_cbox = wx.ComboBox(self, choices = self.proj_methods, value=' ', style = wx.CB_READONLY)
        self.style_cbox = wx.ComboBox(self, choices = self.style_ch, style = wx.CB_READONLY)
        self.pos_slider = wx.Slider(self, value=-1, minValue = -1, maxValue = 0, style = wx.SL_LABELS|wx.SL_AUTOTICKS)
        self.pos_slider.Disable()
        self.resolution_spn = wx.SpinCtrl(self, min=1, max=100)
        self.x_axis_text = wx.TextCtrl(self, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        self.y_axis_text = wx.TextCtrl(self, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        self.z_axis_text = wx.TextCtrl(self, -1, size=(100, 25), style=wx.TE_PROCESS_ENTER)
        
        for i in range(len(self.flist_sorted)): 
            self.header_list.append(FieldPlot().get_header(csv_f=self.dir + '/' + self.flist_sorted[i]))
            self.ndim.append(FieldPlot().get_dimension(self.header_list[i]))
            
        # Sizers
        #========================================================================================================================
        pos_slider_sizer = wx.BoxSizer(wx.VERTICAL)
        time_code_sizer = wx.BoxSizer(wx.HORIZONTAL)        
        plot_sizer = wx.BoxSizer(wx.HORIZONTAL)
        line_sizer_1 = wx.BoxSizer(wx.HORIZONTAL)
        line_sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        line_sizer_3 = wx.BoxSizer(wx.HORIZONTAL)
        line_sizer_4 = wx.BoxSizer(wx.HORIZONTAL)
        line_sizer_5 = wx.BoxSizer(wx.HORIZONTAL)
        player_sizer = wx.BoxSizer(wx.HORIZONTAL)
        player_time_code_sizer = wx.BoxSizer(wx.VERTICAL)
        heatmap_sizer = wx.BoxSizer(wx.VERTICAL)
        axes_label_sizer = wx.BoxSizer(wx.VERTICAL)
        btn_sizer = wx.BoxSizer(wx.VERTICAL)
        v_grid_sizer = wx.GridSizer(rows=2, cols=2) 
        axes_grid_sizer = wx.GridSizer(rows=3, cols=2)
        cbox_grid_sizer = wx.GridSizer(rows = 6, cols = 2)
        organization_sizer = wx.BoxSizer(wx.HORIZONTAL)
        
        play = ImageFiles().play.GetImage()
        self.play_bitmap = wx.BitmapFromImage(play)
        reverse_play = ImageFiles().reverse_play.GetImage()
        self.reverse_play_bitmap = wx.BitmapFromImage(reverse_play)
        pause = ImageFiles().pause.GetImage()
        self.pause_bitmap = wx.BitmapFromImage(pause)
        reset = ImageFiles().reset.GetImage()
        self.reset_bitmap = wx.BitmapFromImage(reset)
        decrease_single_step = ImageFiles().decrease_single_step.GetImage()
        self.decrease_single_step_bitmap = wx.BitmapFromImage(decrease_single_step)
        increase_single_step = ImageFiles().increase_single_step.GetImage()
        self.increase_single_step_bitmap = wx.BitmapFromImage(increase_single_step)
        
        # Buttons
        #========================================================================================================================
        self.plot_btn = wx.Button(self, label = 'Plot')
        self.save_btn = wx.Button(self, wx.ID_SAVE, label = 'Save')
        self.switch_btn = wx.Button(self, label = 'Switch directory')
        self.multiple_plot_btn = wx.Button(self, label = 'Add timeline')    
        self.axes_label_ok_btn = wx.Button(self, label='OK')
        
        # Player buttons
        self.play_pause_btn = wx.BitmapButton(self, -1, bitmap=self.play_bitmap)
        self.reverse_play_pause_btn = wx.BitmapButton(self, -1, bitmap=self.reverse_play_bitmap)
        self.reset_btn = wx.BitmapButton(self, -1, bitmap=self.reset_bitmap)
        self.decrease_single_step_btn = wx.BitmapButton(self, -1, bitmap=self.decrease_single_step_bitmap)
        self.increase_single_step_btn = wx.BitmapButton(self, -1, bitmap=self.increase_single_step_bitmap)
        
        self.play_pause_btn.Disable()
        self.reverse_play_pause_btn.Disable()
        self.reset_btn.Disable()
        self.decrease_single_step_btn.Disable()
        self.increase_single_step_btn.Disable()
        self.multiple_plot_btn.Disable()
        
        
        # Controls
        #========================================================================================================================

        self.resolution_spn.SetToolTipString('Determines the resolution of the plot. Lower values lead to finer resolutions.')
        self.vmin_spn.SetToolTipString('Caps the colormap of heatmaps at the given minimum and maximum.')
        self.vmax_spn.SetToolTipString('Caps the colormap of heatmaps at the given minimum and maximum.')
        self.play_pause_btn.SetToolTipString('Starts and pauses the Plot animation.')
        self.reverse_play_pause_btn.SetToolTipString('Starts and pauses the reverse Plot animation.')
        self.reset_btn.SetToolTipString('Resets the plot to default start parameters.')
        self.increase_single_step_btn.SetToolTipString('Updates the plot by increasing the time slice index by 1.')
        self.decrease_single_step_btn.SetToolTipString('Updates the plot by decreasing the time slice index by 1.')
        
        # Other widgets
        #========================================================================================================================   
        
        # Build partial sizers
        #========================================================================================================================
                
        # Slider and player functionalities
        #========================================================================
        pos_slider_sizer.Add(self.pos_slider, 1, wx.EXPAND|wx.ALIGN_CENTER)
        
        time_code_sizer.Add(self.time_code_label, 1, wx.ALIGN_LEFT)
        time_code_sizer.Add(self.time_code_display, 1, wx.ALIGN_RIGHT)
        
        player_sizer.Add(self.decrease_single_step_btn, 1, wx.ALIGN_CENTER)
        player_sizer.Add(self.reverse_play_pause_btn, 1, wx.ALIGN_CENTER)
        player_sizer.Add(self.play_pause_btn, 1, wx.ALIGN_CENTER)
        player_sizer.Add(self.increase_single_step_btn, 1, wx.ALIGN_CENTER)
        player_sizer.Add(self.reset_btn, 1, wx.ALIGN_CENTER)
        
        player_time_code_sizer.Add(pos_slider_sizer, 2, wx.EXPAND|wx.ALIGN_CENTER)
        player_time_code_sizer.Add(player_sizer, 2, wx.ALIGN_CENTER)
        player_time_code_sizer.AddSpacer(10)
        player_time_code_sizer.Add(time_code_sizer, 1, wx.ALIGN_LEFT)
        #========================================================================
        
        # Axes label functionalities
        #========================================================================
        axes_grid_sizer.Add(self.x_axis_label, 2, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        axes_grid_sizer.Add(self.x_axis_text, 3, wx.ALIGN_CENTER)
        axes_grid_sizer.Add(self.y_axis_label, 2, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        axes_grid_sizer.Add(self.y_axis_text, 3, wx.ALIGN_CENTER)
        axes_grid_sizer.Add(self.z_axis_label, 2, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        axes_grid_sizer.Add(self.z_axis_text, 3, wx.ALIGN_CENTER)
        
        axes_label_sizer.Add(item=self.label_axes_txt, proportion=1, flag=wx.ALIGN_LEFT|wx.RIGHT|wx.BOTTOM, border=10)
        axes_label_sizer.Add(axes_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT, border=10)
        axes_label_sizer.Add(self.axes_label_ok_btn,proportion=0, flag=wx.ALIGN_CENTER|wx.RIGHT|wx.TOP|wx.EXPAND, border=10)
        #========================================================================
        
        # Selection ComboBoxes
        #========================================================================
        cbox_grid_sizer.Add(self.sel_label, 1, wx.ALIGN_LEFT)
        cbox_grid_sizer.Add(self.sel_cbox, 2, wx.ALIGN_RIGHT)
        cbox_grid_sizer.Add(self.mode_label, 1, wx.ALIGN_LEFT)
        cbox_grid_sizer.Add(self.mode_cbox, 2, wx.ALIGN_RIGHT)
        cbox_grid_sizer.Add(self.proj_label, 1, wx.ALIGN_LEFT)
        cbox_grid_sizer.Add(self.proj_cbox, 2, wx.ALIGN_RIGHT)
        cbox_grid_sizer.Add(self.proj_method_label, 1, wx.ALIGN_LEFT)
        cbox_grid_sizer.Add(self.proj_method_cbox, 2, wx.ALIGN_RIGHT)
        cbox_grid_sizer.Add(self.style_label, 1, wx.ALIGN_LEFT)
        cbox_grid_sizer.Add(self.style_cbox, 2, wx.ALIGN_RIGHT)
        cbox_grid_sizer.Add(self.resolution_label, 1, wx.ALIGN_LEFT)
        cbox_grid_sizer.Add(self.resolution_spn, 2, wx.ALIGN_RIGHT)
        #========================================================================
        
        # Heatmap functionalities
        #========================================================================
        v_grid_sizer.Add(self.vmin_label, 1, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        v_grid_sizer.Add(self.vmin_spn, 1, wx.ALIGN_CENTER)
        v_grid_sizer.Add(self.vmax_label, 1, wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL)
        v_grid_sizer.Add(self.vmax_spn, 1, wx.ALIGN_CENTER)
        
        heatmap_sizer.Add(self.heatmap_boundaries_txt, 1, flag=wx.ALIGN_LEFT|wx.BOTTOM|wx.LEFT, border=10)
        heatmap_sizer.Add(v_grid_sizer, proportion=0, flag=wx.ALIGN_LEFT|wx.RIGHT, border=10)
        heatmap_sizer.AddSpacer(25)
        heatmap_sizer.Add(self.reset_heatmap_boundaries_btn, 0, wx.ALIGN_CENTER|wx.LEFT|wx.TOP|wx.EXPAND, border=10)
        #========================================================================
                
        # Buttons
        #========================================================================
        plot_sizer.Add(self.plot_btn, 1)
        plot_sizer.Add(self.save_btn, 1)
        btn_sizer.Add(plot_sizer, 1, wx.ALIGN_LEFT|wx.EXPAND)
        btn_sizer.Add(self.multiple_plot_btn, 1, wx.ALIGN_LEFT|wx.EXPAND)
        btn_sizer.Add(self.switch_btn, 1, wx.ALIGN_LEFT|wx.EXPAND)
        #========================================================================
        
        # Dividing lines
        #========================================================================
        line_sizer_1.Add(item=self.line_1, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        line_sizer_2.Add(item=self.line_2, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        line_sizer_3.Add(item=self.line_3, proportion=1, flag=wx.EXPAND)
        line_sizer_4.Add(item=self.line_4, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        line_sizer_5.Add(item=self.line_5, proportion=1, flag=wx.LEFT|wx.RIGHT, border=10)
        #========================================================================
        
        organization_sizer.Add(item=axes_label_sizer)
        organization_sizer.Add(item=line_sizer_3, flag=wx.EXPAND)
        organization_sizer.Add(item=heatmap_sizer)
        
        # Build main sizer
        #========================================================================================================================
        self.main_sizer.Add(line_sizer_1, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND)
        self.main_sizer.Add(item=cbox_grid_sizer, proportion=0, flag=wx.EXPAND|wx.ALIGN_CENTER|wx.LEFT|wx.RIGHT|wx.TOP, border=10)
        self.main_sizer.Add(line_sizer_2, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND)
        self.main_sizer.Add(item=player_time_code_sizer, proportion=0, flag=wx.EXPAND|wx.ALIGN_CENTER|wx.LEFT|wx.RIGHT, border=10)
        self.main_sizer.Add(line_sizer_4, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND)
        self.main_sizer.Add(item=organization_sizer, proportion=0, flag=wx.ALIGN_CENTER|wx.RIGHT|wx.LEFT, border=10)
        self.main_sizer.Add(line_sizer_5, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND)
        self.main_sizer.Add(btn_sizer, 0, wx.ALIGN_LEFT|wx.LEFT|wx.BOTTOM|wx.TOP, border=10)
        
        self.SetSizer(self.main_sizer)
        self.main_sizer.Fit(self)
        
        # Event handling
        #========================================================================================================================
        self.sel_cbox.Bind(wx.EVT_COMBOBOX, self.evt_sel_cbox)
        self.mode_cbox.Bind(wx.EVT_COMBOBOX, self.evt_mode_cbox)
        self.reset_heatmap_boundaries_btn.Bind(wx.EVT_BUTTON, self.evt_reset_heatmap_boundaries_btn)
        self.proj_cbox.Bind(wx.EVT_COMBOBOX, self.evt_proj_cbox)
        self.proj_method_cbox.Bind(wx.EVT_COMBOBOX, self.evt_proj_method_cbox)
        self.style_cbox.Bind(wx.EVT_COMBOBOX, self.evt_style_cbox)
        self.pos_slider.Bind(wx.EVT_SCROLL, self.evt_slider)
        self.play_pause_btn.Bind(wx.EVT_BUTTON, self.evt_play_pause_btn)
        self.reverse_play_pause_btn.Bind(wx.EVT_BUTTON, self.evt_reverse_play_pause_btn)
        self.increase_single_step_btn.Bind(wx.EVT_BUTTON, self.evt_increase_single_step_btn)
        self.decrease_single_step_btn.Bind(wx.EVT_BUTTON, self.evt_decrease_single_step_btn)
        self.reset_btn.Bind(wx.EVT_BUTTON, self.evt_reset_btn)
        self.switch_btn.Bind(wx.EVT_BUTTON, self.evt_switch_button)
        self.vmin_spn.Bind(FS.EVT_FLOATSPIN, self.evt_vmin_spn)
        self.vmax_spn.Bind(FS.EVT_FLOATSPIN, self.evt_vmax_spn)
        self.resolution_spn.Bind(wx.EVT_SPINCTRL, self.evt_resolution_spn)
        self.multiple_plot_btn.Bind(wx.EVT_BUTTON, self.evt_add_timeline)
        self.x_axis_text.Bind(wx.EVT_TEXT_ENTER, self.evt_axis_label)
        self.y_axis_text.Bind(wx.EVT_TEXT_ENTER, self.evt_axis_label)
        self.z_axis_text.Bind(wx.EVT_TEXT_ENTER, self.evt_axis_label)
        self.axes_label_ok_btn.Bind(wx.EVT_BUTTON, self.evt_axis_label)
        #========================================================================================================================
                
        self.Show()
        
                    
    def evt_play_pause_btn(self, event):
        
        if self.play_pause_btn.GetBitmapLabel() == self.play_bitmap:
            self.play_pause_btn.SetBitmapLabel(self.pause_bitmap)
            
            if plt.get_fignums():
                for i in range(self.step, self.slider_max+1):
                    
                    if self.play_pause_btn.GetBitmapLabel() == self.play_bitmap:
                        break
                    else:
                        self.reverse_play_pause_btn.Disable()
                        self.increase_single_step_btn.Disable()
                        self.decrease_single_step_btn.Disable()
                        self.reset_btn.Disable()
                        self.step = i
                        self.marked = True
                        self.pos_slider.SetValue(min(self.slider_max, self.step))
                        self.time_code_display.SetLabel(str(self.time_codes[self.pos_slider.GetValue()]))
                        self._update_plot()
                        wx.Yield()
        
            self.play_pause_btn.SetBitmapLabel(self.play_bitmap)
            
        else:
            self.play_pause_btn.SetBitmapLabel(self.play_bitmap)
            
        self.reverse_play_pause_btn.Enable()
        self.increase_single_step_btn.Enable()
        self.decrease_single_step_btn.Enable()
        self.reset_btn.Enable()
            
    
    def evt_reverse_play_pause_btn(self, event):
        
        if self.reverse_play_pause_btn.GetBitmapLabel() == self.reverse_play_bitmap:
            self.reverse_play_pause_btn.SetBitmapLabel(self.pause_bitmap)
            
            if plt.get_fignums():
                for i in range(self.step, -2, -1):
                    
                    if self.reverse_play_pause_btn.GetBitmapLabel() == self.reverse_play_bitmap:
                        break
                    else:
                        self.play_pause_btn.Disable()
                        self.increase_single_step_btn.Disable()
                        self.decrease_single_step_btn.Disable()
                        self.reset_btn.Disable()
                        self.step = i
                        self.pos_slider.SetValue(self.step)
                        
                        if self.step == -1:
                            self.time_code_display.SetLabel('-')
                            self.marked = False
                        else:
                            self.marked = True
                            self.time_code_display.SetLabel(str(self.time_codes[self.pos_slider.GetValue()]))
                            
                        self._update_plot()
                        wx.Yield()
        
            self.reverse_play_pause_btn.SetBitmapLabel(self.reverse_play_bitmap)
            
        else:
            self.reverse_play_pause_btn.SetBitmapLabel(self.reverse_play_bitmap)
            
        self.play_pause_btn.Enable()
        self.increase_single_step_btn.Enable()
        self.decrease_single_step_btn.Enable()
        self.reset_btn.Enable()
        
            
    def evt_reset_btn(self, evt):

        self.step = -1
        self.marked = False
        self.pos_slider.SetValue(self.step)
        self.time_code_display.SetLabel('-')
        self._update_plot()
        
    
    def evt_increase_single_step_btn(self, evt):
        self.step += 1
        self.pos_slider.SetValue(min(self.slider_max, self.step))
        self.time_code_display.SetLabel(str(self.time_codes[self.pos_slider.GetValue()]))
        if self.step != -1:
            self.marked = True
        else:
            self.marked = False
            
        self._update_plot()
        
    
    def evt_decrease_single_step_btn(self, evt):
        self.step -= 1
        self.pos_slider.SetValue(max(-1, self.step))
        if self.step != -1:
            self.time_code_display.SetLabel(str(self.time_codes[self.pos_slider.GetValue()]))
            self.marked = True
        else:
            self.time_code_display.SetLabel('-')
            self.marked = False
            
        self._update_plot()
        
        
           
    def evt_axis_label(self, event):
        self.x_label = self.x_axis_text.GetValue()
        self.y_label = self.y_axis_text.GetValue()
        self.z_label = self.z_axis_text.GetValue()
        
        if plt.get_fignums():
            self._update_plot()
        
        return
    
    def evt_add_timeline(self, event):
        '''
        adds timeline to currently displayed plot.
        '''
        self._add_timeline()
        
        return
        
    def evt_resolution_spn(self, event):
        widget = event.GetEventObject()
        self.resolution = int(widget.GetValue())
        
        if plt.get_fignums():
            self._update_plot()
        
        return
    
    def evt_vmax_spn(self, event):
        widget = event.GetEventObject()
        self.vmax = float(widget.GetValue())
        
        if plt.get_fignums():
            self._update_plot()
        
        return
    
    def evt_vmin_spn(self, event):
        widget = event.GetEventObject()
        self.vmin = float(widget.GetValue())
        
        if plt.get_fignums():
            self._update_plot()
        
        return
    
    def evt_proj_cbox(self, event):
        widget = event.GetEventObject()
        self.proj = widget.GetValue()
                        
        if plt.get_fignums():
            self._update_plot()
            
        return
    
    def _update_plot(self):
        plt.clf()
        self._plot()
        
    def _add_timeline(self):
        self._plot()
    
    def evt_proj_method_cbox(self, event):
        widget = event.GetEventObject()
        self.proj_method = widget.GetValue()
        
        if plt.get_fignums() and self.proj_method != '':
            self._update_plot()
        
        return
    
    def evt_style_cbox(self, event):
        widget     = event.GetEventObject()
        self.style = widget.GetValue()
        
        self.plot_btn.Bind(wx.EVT_BUTTON, self.evt_plot)
        
        if self.style == 'image':
            self.mode = 'snapshot'
            self.reset_heatmap_boundaries_btn.Disable()
            self.mode_cbox.Disable()
            self.x_axis_text.Disable()
            self.axes_label_ok_btn.Disable()
            self.y_axis_text.Disable()
            self.z_axis_text.Disable()
            self.proj_cbox.Disable()
            self.proj_method_cbox.Disable()
            
        else:
            self.reset_heatmap_boundaries_btn.Enable()
            self.mode_cbox.Enable()
            self.x_axis_text.Enable()
            self.axes_label_ok_btn.Enable()
            self.y_axis_text.Enable()
            self.z_axis_text.Enable()
            self.proj_cbox.Enable()
            self.proj_method_cbox.Enable()
        
        if self.style == 'heatmap':
            self.z_axis_label.SetLabel('Legend')
            self.resolution_spn.Disable()
            self.vmin_spn.Enable()
            self.vmax_spn.Enable()
            self.reset_heatmap_boundaries_btn.Enable()
        else:
            self.z_axis_label.SetLabel('Z axis')
            self.resolution_spn.Enable()
            self.vmin_spn.Disable()
            self.vmax_spn.Disable()
            self.reset_heatmap_boundaries_btn.Disable()
                    
        if plt.get_fignums():
            try:
                self._update_plot()
            except TypeError:
                raise TypeError
        
        return
    
    def evt_reset_heatmap_boundaries_btn(self, event):
        self.vmin_spn.SetValue(0.0)
        self.vmax_spn.SetValue(0.0)
        self.vmin = self.vmax = None
        
        if plt.get_fignums():
            self._update_plot()

        return
    
    def evt_switch_button(self, event):
        frame = MainWindow(None, 'Recorded Data Processor')
        app.SetTopWindow(frame)
        frame.Show()
        self.frame.Destroy()
        self.Destroy()
        
        return
    
    def evt_slider(self, event):
        widget    = event.GetEventObject()
        self.step = widget.GetValue()
            
        if self.step == -1:
            self.time_code_display.SetLabel('-')
            self.marked = False
        else:
            self.time_code_display.SetLabel(str(self.time_codes[self.pos_slider.GetValue()]))
            self.marked = True
            
        if plt.get_fignums():
            self._update_plot()
            
        return
    

    def evt_mode_cbox(self, event):
        
        self.mode  = self.mode_cbox.GetValue()         
        empty_list = []        
                
        if self.mode == ' ':
            self.plot_btn.SetLabel('Plot')
        else:
            self.plot_btn.Bind(wx.EVT_BUTTON, self.evt_plot)
            
        # snapshot option selected
        #======================================================================================================================== 
        if self.mode == 'snapshot':
            self.proj_cbox.SetItems(self.proj_choice_step)
            self.save_btn.Bind(wx.EVT_BUTTON, self.evt_save_plot)
            
            if plt.get_fignums():
                self._update_plot() 

        # snapshot sequence option selected
        #======================================================================================================================== 
        elif self.mode == 'snapshot sequence':
            self.proj_cbox.SetItems(self.proj_choice_step)
            self.save_btn.Bind(wx.EVT_BUTTON, self.evt_save_plot)
                        
        # timeline option selected
        #======================================================================================================================== 
        elif self.mode == 'timeline':                
            self.proj_cbox.SetItems(self.proj_choice)
            
            self.save_btn.Bind(wx.EVT_BUTTON, self.evt_save_plot) 
            
            if plt.get_fignums():
                self._update_plot()  
        
        else:
            self.proj_cbox.SetItems(empty_list) 
        
    
    def _update_selection_data(self):
        
        # clear memory
        del self.data
        self.slider_max = 0
        self.step = -1
        self.pos_slider.SetValue(self.step)
        self.time_code_display.SetLabel('-')
        self.pos_slider.Disable()
        self.play_pause_btn.Disable()
        self.reverse_play_pause_btn.Disable()
        self.reset_btn.Disable()
        self.decrease_single_step_btn.Disable()
        self.increase_single_step_btn.Disable()
        
        # valid checkbox selected
        self.proj_choice = self.proj_ch[:self.ndim[self.selection]+1]
        self.proj_choice_step = FieldPlot().build_proj_ch_step(ndim=self.ndim[self.selection], temp_proj_ch_step=self.proj_ch_step)   
        
        self.header = FieldPlot().get_header(csv_f=self.dir + '/' + self.flist_sorted[self.selection])
        temp_data = FieldPlot().get_data(csv_f=self.dir + '/' + self.flist_sorted[self.selection])
        
        self.pos_slider.Enable()
        self.play_pause_btn.Enable()
        self.reverse_play_pause_btn.Enable()
        self.reset_btn.Enable()
        self.decrease_single_step_btn.Enable()
        self.increase_single_step_btn.Enable()
        
        self.data = temp_data[0]
        self.time_codes = temp_data[1]
        self.slider_max = len(self.time_codes)-1
        self.pos_slider.SetMax(self.slider_max)
                
    
    def evt_sel_cbox(self, event):
        
        
        #plt.close()
        self.selection = self.sel_cbox.GetSelection()
        self._update_selection_data()
        
        if self.ndim[self.selection] == 0:
            self.multiple_plot_btn.Enable()
        else:
            self.multiple_plot_btn.Disable()
        
        return
                
    def evt_save_plot(self, event):
        
        if self.mode == 'snapshot':
            if self.step < 0:
                step = 0
            else:
                step = self.step
            
            try:
                self.plot = FieldPlot().plot_snapshot(step = step, 
                                                      style = self.style, 
                                                      data = self.data, 
                                                      header = self.header, 
                                                      vmin = self.vmin,
                                                      vmax = self.vmax,
                                                      resolution = self.resolution,
                                                      proj = self.proj,
                                                      proj_method = self.proj_method)
                    
                FieldPlot().label_axis(plot=self.plot, x_label=self.x_label, y_label=self.y_label, z_label=self.z_label)
                FieldPlot().save_plot(plot=self.plot, plot_mode=self.mode, file_name=self.flist_sorted[self.selection], file_directory=self.dir)
            except IndexError:
                dlg = wx.MessageDialog(parent  = None, 
                message = 'The specified timeslice does not seem to exist.', 
                caption = 'An Error has occurred.', 
                style   = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Destroy()
        
        elif self.mode == 'snapshot sequence':
            dlg = SnapshotSequenceDialog(self, -1, 'Configure snapshot sequence')
            dlg.ShowModal()
            dlg.Destroy()
            
            self.plot = FieldPlot().plot_snapshot_sequence(start = self.step, 
                                                           step_size = self.step_size, 
                                                           steps = self.nstep, 
                                                           style = self.style, 
                                                           data = self.data, 
                                                           header = self.header, 
                                                           vmin = self.vmin,
                                                           vmax = self.vmax,
                                                           resolution = self.resolution,
                                                           proj = self.proj,
                                                           proj_method = self.proj_method,
                                                           x_label = self.x_label,
                                                           y_label = self.y_label,
                                                           z_label = self.z_label,
                                                           file_name = self.flist_sorted[self.selection],
                                                           file_directory = self.dir,
                                                           save_mode = True)
                                                
        elif self.mode == 'timeline':
            try:
                self.plot = FieldPlot().plot_timeline(data = self.data, 
                                                      header = self.header,
                                                      vmin = self.vmin,
                                                      vmax = self.vmax,
                                                      resolution = self.resolution,
                                                      plot = self.plot,
                                                      proj = self.proj, 
                                                      proj_method = self.proj_method,
                                                      step = self.step, 
                                                      marker = self.marked, 
                                                      style = self.style)
            
                FieldPlot().label_axis(plot=self.plot, x_label=self.x_label, y_label=self.y_label, z_label=self.z_label)
                FieldPlot().save_plot(plot=self.plot, plot_mode=self.mode, file_name=self.flist_sorted[self.selection], file_directory=self.dir)
                
            except UnboundLocalError:
                dlg = wx.MessageDialog(parent  = None, 
                message = 'It is not possible to build a timeline out of 2-dimensional timeslices.', 
                caption = 'The attempted operation is not possible.', 
                style   = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Destroy()
                        
        return
    
    def _plot(self):
        if self.mode == 'snapshot':
            if self.step < 0:
                step = 0
            else:
                step = self.step
            
            try:
                self.plot = FieldPlot().plot_snapshot(step = step, 
                                                      style = self.style, 
                                                      data = self.data, 
                                                      header = self.header, 
                                                      vmin = self.vmin,
                                                      vmax = self.vmax,
                                                      resolution = self.resolution, 
                                                      proj = self.proj,
                                                      proj_method = self.proj_method)
                        
                FieldPlot().label_axis(plot=self.plot, x_label=self.x_label, y_label=self.y_label, z_label=self.z_label)
                
                manager = plt.get_current_fig_manager()
                manager.window.SetPosition((500, 250))
            
                plt.draw()
                
            except IndexError:
                dlg = wx.MessageDialog(parent  = None, 
                                       message = 'The specified timeslice does not exist.', 
                                       caption = 'An Error has occurred.', 
                                       style = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Destroy()
        
        elif self.mode == 'snapshot sequence':
            dlg = SnapshotSequenceDialog(self, -1, 'Configure snapshot sequence')
            dlg.ShowModal()
            dlg.Destroy()
            
            self.plot = FieldPlot().plot_snapshot_sequence(start = self.step, 
                                                           step_size = self.step_size, 
                                                           steps = self.nstep, 
                                                           style = self.style, 
                                                           data = self.data, 
                                                           header = self.header, 
                                                           vmin = self.vmin,
                                                           vmax = self.vmax,
                                                           resolution = self.resolution,
                                                           proj = self.proj,
                                                           proj_method = self.proj_method,
                                                           x_label = self.x_label,
                                                           y_label = self.y_label,
                                                           z_label = self.z_label)
                                
        elif self.mode == 'timeline':
            try:
                self.plot = FieldPlot().plot_timeline(data = self.data, 
                                                      header = self.header, 
                                                      vmin = self.vmin,
                                                      vmax = self.vmax,
                                                      resolution = self.resolution,
                                                      plot = self.plot,
                                                      proj = self.proj, 
                                                      proj_method = self.proj_method,
                                                      step = self.step, 
                                                      marker = self.marked, 
                                                      style = self.style)
                
                FieldPlot().label_axis(plot=self.plot, 
                                       x_label=self.x_label, 
                                       y_label=self.y_label, 
                                       z_label=self.z_label)
            
                manager = plt.get_current_fig_manager()
                manager.window.SetPosition((500, 250))
        
                plt.draw()
                
            except UnboundLocalError:
                plt.close()
                dlg = wx.MessageDialog(parent = None, 
                                       message = 'It is not possible to build a timeline out of 2-dimensional timeslices.', 
                                       caption = 'The attempted operation is not possible.', 
                                       style = wx.OK | wx.ICON_INFORMATION | wx.CENTER | wx.STAY_ON_TOP)
                dlg.ShowModal()
                dlg.Destroy()

        return
        
    
    def evt_plot(self, event):
        
        self._plot()            
        plt.show()
                
        return
       
#========================================================================================================================

class FieldPlot(object):

    def __init__(self):
        return
    
                
    def _sort_alphnum(self, unsorted):
        conv = lambda text: int(text) if text.isdigit() else text
        alphnum_key = lambda key: [conv(c) for c in re.split('([0-9]+)', key)]
        
        return sorted(unsorted, key=alphnum_key)
    
    def build_proj_ch_step(self, ndim, temp_proj_ch_step):
        '''
        Builds the various projection choices for snapshot plots
        '''
        
        proj_ch_step = [' ']
                
        for j in range(ndim+1):
            for k in range(len(temp_proj_ch_step)):
                if 'x_'+ str(j) == temp_proj_ch_step[k]:
                    proj_ch_step.append(temp_proj_ch_step[k])
                elif 'x_'+ str(j) in temp_proj_ch_step[k]:
                    count = 0
                    for l in range(len(re.findall('\d', temp_proj_ch_step[k]))):
                        if int(re.findall('\d', temp_proj_ch_step[k])[l]) <= ndim :
                            count += 1
                            if count == 2 and temp_proj_ch_step[k] not in proj_ch_step:
                                proj_ch_step.append(temp_proj_ch_step[k])
        
        # sort list by length of entries
        proj_ch_step.sort(key=lambda t: len(t))
        
        return proj_ch_step
    
    def get_dimension(self, header):
        '''
        Returns the dimensionality of the data belonging to the given header.
        '''
        
        ndim = len(header[2:])
        
        for i in range(1, len(header[2:])+1):
            
            if int(header[-i]) == 1:
                ndim -= 1
            elif int(header[-i]) != 1:
                break
                        
        return ndim    
    
    
    def _project(self, mode, steps, data, header, proj, proj_method='addition'):
        '''
        Projects data onto the given dimension.
        '''
        
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
                            
                        elif proj_method == 'addition':
                            div = aux_X.shape[j]
                            aux_X = np.add.reduce(array=aux_X, axis=j)                        
                            aux_X = np.true_divide(aux_X, div)
                        
                        if j < aux_col:
                            aux_col -= 1
    
                    else:
                        j += 1
                        continue
                            
                if new_X is None:
                    new_X = np.zeros((steps, aux_X.shape[0]))
                
                if proj_method == 'maximum':
                    new_X[i] = aux_X
                elif proj_method == 'addition':
                    new_X[i] = aux_X
    
                X_1,X_2 = np.mgrid[:steps, :X[col]]
                Z = new_X
                
            except UnboundLocalError:
                raise UnboundLocalError()
        
        return X_1, X_2, Z
    
    def _project2D(self, step, data, header, proj, proj_method='addition'):
        '''
        Projects data onto 2 given dimensions.
        '''
        
        ndim = self.get_dimension(header)
        X = np.zeros(ndim)
        col = np.zeros(2)
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
                    
                elif proj_method == 'addition':
                    div = aux_X.shape[j]
                    aux_X = np.add.reduce(array=aux_X, axis=j)
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
        
    def _set_marker(self, step, data, plot, style):
        '''
        Marks the given time slice in a timeline plot.
        '''
                    
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
            plt.axvline(x=step, color='red')
        
        else:
            if style == 'wireframe':
                cross = PolyCollection(v, facecolors='b', closed=False)
            elif style == 'surface':
                cross = PolyCollection(v, facecolors='r', closed=False)
            
            cross.set_alpha(0.25)
            plot.add_collection3d(cross, zs=step, zdir='x')
                    
        return plot
        
    def get_data(self, csv_f):
        '''
        Gets data and time codes from given csv file.
        '''
    
        data = None
        count = 0
        csv_file = open(csv_f, 'rb')
        row_count = sum(1 for line in open(csv_f))-1        
        time_codes = []
        reader = csv.reader(csv_file)
        
        # skip header
        next(reader, None)
        
        # Progress bar
        progress_dlg = Progress(None, -1, 'Load data recording', 'loading...', row_count+1)
        
        # build time_codes list and data matrix
        #========================================================================================================================
        for row in reader:
            time_codes.append(row[0])
            row.pop(0)
            
            # Matrix generation
            if data is None:
                data = np.zeros((row_count, len(row)))
            
            data[count] = row
            count += 1
            progress_dlg.Update(count)
        #========================================================================================================================
                               
        progress_dlg.Destroy()
        csv_file.close()
                    
        return data, time_codes

    def get_header(self, csv_f):
        '''
        Gets header from given csv file.
        '''
        
        csv_file = open(csv_f, 'rb')        
        reader   = csv.reader(csv_file)
        header   = reader.next()
        csv_file.close()
                
        return header      
    
    def _initialize_3D_plot(self, mode=None):
        
        if mode == 'snapshot sequence':
            fig = plt.figure()
        else:
            if not plt.get_fignums():
                fig = plt.figure()
            else:
                fig = plt.figure(1)
            
        plot = fig.gca(projection='3d')
        
        # make grid background transparent
        plot.w_xaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
        plot.w_yaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
        plot.w_zaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
        
        return plot
    
    def _process_image(self, data, header, step):
        
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
        #========================================================================================================================

    def plot_snapshot(self, step, data, vmin, vmax, resolution, header, style, mode=' ', proj=' ', proj_method='addition'):        
        ndim = self.get_dimension(header)
        steps = data.shape[0]
        
        if step == -1: 
            step = 0
            
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
                dlg.Destroy()
        
        else:                    
            if ndim == 0:
                if mode == 'snapshot sequence':
                    fig = plt.figure()
                else:
                    if not plt.get_fignums():
                        fig = plt.figure()
                    else:
                        fig = plt.figure(1)
                    
                plot = fig.add_subplot(1,1,1)
                plot.plot(data[step], '+')
            
            elif ndim == 1:
                if mode == 'snapshot sequence':
                    fig = plt.figure()
                else:
                    if not plt.get_fignums():
                        fig = plt.figure()
                    else:
                        fig = plt.figure(1)
                    
                plot = fig.add_subplot(1,1,1)
                plot.plot(data[step])
            
            elif ndim != 1:
                if proj != ' ':
                                    
                    if ',' in proj:
                        
                        try:
                            X_1, X_2, data = self._project2D(step=step, data=data, header=header, proj=proj, proj_method=proj_method)
                        except IndexError:
                            dlg = wx.MessageDialog(parent = None, 
                                                   message = 'The ' + str(step) + '. snapshot does not seem to exist.', 
                                                   caption = 'An Error has occurred.', 
                                                   style = wx.OK | wx.ICON_ERROR | wx.CENTER | wx.STAY_ON_TOP)
                            dlg.ShowModal()
                            dlg.Destroy()
                            
                        if style != 'heatmap':
                            plot = self._initialize_3D_plot(mode=mode)
                            
                            if style == 'surface':   
                                plot.plot_surface(X_1,X_2,data,rstride=resolution, cstride=resolution,cmap='coolwarm', alpha=0.5)
                            elif style == 'wireframe':
                                plot.plot_wireframe(X_1,X_2, data, rstride=resolution,cstride=resolution)
                            
                        elif style == 'heatmap':
                            plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode)
                                                    
                    elif ',' not in proj:
                        
                        try:
                            data = self._project(mode=mode, steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)[2]    
                        except UnboundLocalError:
                            raise UnboundLocalError
                            
                        if not plt.get_fignums():
                            fig = plt.figure()
                        else:    
                            fig = plt.figure(1)
                                            
                        plot = fig.add_subplot(1,1,1)
                        
                        try:
                            plot.plot(data[step])
                        except IndexError:
                            raise IndexError
                                            
                elif proj == ' ':
                    
                    if ndim == 2:
                        x_1 = int(header[2])
                        x_2 = int(header[3])                    
                        
                        if style != 'image':
                            data     = np.reshape(data[step], (x_2, x_1))
                            X_1, X_2 = np.mgrid[:x_2, :x_1]
                    
                        if style != 'heatmap' and style != 'image':
                            plot = self._initialize_3D_plot(mode=mode)
            
                            if style == 'surface': 
                                plot.plot_surface(X_1,X_2,data,rstride=resolution, cstride=resolution,cmap='coolwarm', alpha=0.5)
                            elif style == 'wireframe':
                                plot.plot_wireframe(X_1,X_2,data, rstride=resolution,cstride=resolution)
                            
                        elif style == 'heatmap':
                            plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax, mode=mode)
                                                         
            return plot
        
    def plot_snapshot_sequence(self, data, header, vmin, vmax, resolution, start, step_size, steps, proj, proj_method, style, 
                               x_label=None, y_label=None, z_label=None, file_name=None, file_directory=None, save_mode=False):
        
        plot_mode = 'snapshot sequence'
        
        for i in range(int(steps)):
            plot = self.plot_snapshot(data = data, 
                                      header = header, 
                                      vmin = vmin, 
                                      vmax = vmax, 
                                      resolution = resolution, 
                                      step = int(start) + i*int(step_size), 
                                      style = style, 
                                      mode = plot_mode, 
                                      proj = proj, 
                                      proj_method = proj_method)
            
            self.label_axis(plot = plot, x_label=x_label, y_label=y_label, z_label=z_label)
            
            if save_mode == True:
                self.save_plot(plot=plot, plot_mode=plot_mode, file_name=file_name, file_directory=file_directory, save_mode='sequence', plot_number=i)
                        
        return
    
    def plot_timeline(self, data, header, vmin, vmax, resolution, proj, proj_method, style, plot=None, marker=False, step=None):
        ndim  = self.get_dimension(header)
        steps = data.shape[0]
        
        if plot is None and ndim == 0:
            
            if not plt.get_fignums():
                fig = plt.figure()
            else:
                fig = plt.figure(1)
            
            plot = fig.add_subplot(1,1,1)
        
        if ndim == 0:
            plot.plot(data)
        
        if ndim != 0:
            if ndim == 1:
                x = int(header[2])
                X_1,X_2 = np.mgrid[:steps, :x]
            
            elif ndim >= 2:
                try:
                    X_1, X_2, data = self._project(mode='timeline', steps=steps, data=data, header=header, proj=proj, proj_method=proj_method)
                    
                except UnboundLocalError:
                    raise UnboundLocalError
                
            if style != 'heatmap':
                plot = self._initialize_3D_plot()
                
                if style == 'surface':   
                    plot.plot_surface(X_1, X_2, data, rstride=resolution, cstride=resolution, cmap='coolwarm', alpha=0.5)
                elif style == 'wireframe': 
                    plot.plot_wireframe(X_1, X_2, data, rstride=resolution, cstride=resolution)
            
            elif style == 'heatmap':
                plot = self.plot_heatmap(X_1=X_1, X_2=X_2, data=data, vmin=vmin, vmax=vmax)
        
        # mark the selected time-slice if true                              
        if marker == True:
            plot = self._set_marker(step=step, data=data, plot=plot, style=style)
        
        return plot
    
    def plot_heatmap(self, X_1, X_2, data, vmin, vmax, mode=None):
        
        if vmax is None:
            vmax = data.max()
        if vmin is None:
            vmin = data.min()
        
        if mode != 'snapshot sequence':
            if not plt.get_fignums():
                fig = plt.figure()
            else:
                fig = plt.figure(1)
        else:
            fig = plt.figure()
        
        plot = fig.add_subplot(1,1,1)
            
        plot.add_collection(plt.pcolor(X_1, X_2, data, cmap='RdYlBu_r', vmin=vmin, vmax=vmax))
        plt.axis([X_1.min(), X_1.max(), X_2.min(), X_2.max()])
        
        return plot
    
    
    def label_axis(self, plot, x_label, y_label, z_label=None):
        if plot is None:
            return 
        
        else:
            if 'matplotlib.image.AxesImage' not in str(type(plot)):
                plot.set_xlabel(x_label)
                plot.set_ylabel(y_label)
            
            if 'Axes3DSubplot' in str(type(plot)):
                plot.set_zlabel(z_label)
                
            elif 'matplotlib.axes.AxesSubplot' in str(type(plot)):
                try:
                    colorbar = plt.colorbar()
                    colorbar.set_label(z_label)
                    
                except RuntimeError:
                    pass
            
            return plot

    def save_plot(self, plot, plot_mode, file_name, file_directory, save_mode='single', plot_number=0):
        
        plot_count = 1
        
        if plot_mode == 'snapshot sequence':
            plot_mode = 'snapshot_sequence'
                    
        # Snapshot or timeline
        if save_mode == 'single':
            file_path = file_directory + '/' + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(plot_count) + '.pdf'
            
            while os.path.exists(file_path):
                plot_count += 1
                file_path = file_directory + '/' + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(plot_count) + '.pdf'
                
            plt.savefig(file_path, transparent=True)
            plt.close()
        
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
                plot_count += 1
                file_path = file_path_partial + file_name.strip('.csv') + '-' + str(plot_mode) + '-' + str(plot_count) + '.pdf'
            
            plt.savefig(file_path, transparent=True)
            plt.close()
        
        return

#========================================================================================================================

if __name__ == '__main__':
    app = RecordedDataProcessorApp(False)
    app.MainLoop()