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

    File:        datatools.py

    Maintainer:  Sascha T. Begovic
    Email:       sascha.begovic@ini.ruhr-uni-bochum.de
    Date:        2015 09 23

    Description: 

    Credits:

========================================================================================================================
'''

import csv
import numpy as np
import re
import wx

from dlg import progress_dlg

def get_header(csv_f):
    '''Gets header from given csv file.'''
    
    csv_file = open(csv_f, 'rb')        
    reader = csv.reader(csv_file)
    header = reader.next()
    csv_file.close()

    if csv_f.endswith('.data'):
        header = header[:-1]

    return header  


def get_data(csv_f, header):
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