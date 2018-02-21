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

    File:        dlg.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 29

    Description: 

    Credits:

========================================================================================================================
'''

import wx

def load_config_dlg(parent, defaultDir):
    
    wildcard = 'Pickle files (*.p)|*.p|'  \
               'All files (*.*)|*.*'
    
    dlg = wx.FileDialog(parent=parent, message='Load Plot configuration', wildcard=wildcard, style=wx.FD_OPEN|wx.FD_PREVIEW, defaultDir=defaultDir)
    
    return dlg


def browser_dlg(parent, defaultPath):
    
    '''
    wildcard = 'Pickle files (*.p)|*.p|'  \
               'All files (*.*)|*.*'
    '''
    
    dlg = wx.DirDialog(parent=parent, message='Select directory', defaultPath=defaultPath)
    
    return dlg


def progress_dlg(parent, _, title, message, maximum):
    dlg = wx.ProgressDialog(title, message=message, maximum=maximum, parent=parent, style=wx.PD_ELAPSED_TIME|wx.PD_REMAINING_TIME|wx.STAY_ON_TOP)
    
    return dlg


def save_dlg(parent, defaultDir):
    
    wildcard = 'Scalable Vector Graphic (*.svg)|*.svg|'  \
               'Portable Document Format (*.pdf)|*.pdf|' \
               'Encapsulated PostScript (*.eps)|*.eps|'  \
               'All files (*.*)|*.*'
    
    dlg = wx.FileDialog(parent=parent, message='Save plot', wildcard=wildcard, style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_PREVIEW, defaultDir=defaultDir)
    
    return dlg


def save_config_dlg(parent, defaultDir):
    
    wildcard = 'Pickle (*.p)|*.p|'  \
               'All files (*.*)|*.*'
    
    dlg = wx.FileDialog(parent=parent, message='Save Plot configuration', wildcard=wildcard, style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_PREVIEW, defaultDir=defaultDir)
    
    return dlg


def save_csv_dlg(parent, defaultDir, defaultFile):
    
    wildcard = 'Comma-separated Values (*.csv)|*.csv|'  \
               'All files (*.*)|*.*'
    
    dlg = wx.FileDialog( parent=parent, message='Write .csv file', wildcard=wildcard, style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_PREVIEW, defaultDir=defaultDir)
    
    return dlg
