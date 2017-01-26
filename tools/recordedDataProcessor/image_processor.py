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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2015 03 12

    Description: 

    Credits: Most of the code in this script was copied from recorded_data_processor.py.

========================================================================================================================
'''

from scipy import ndimage

import csv
import math
import numpy as np
import os
import re
import sys
from PIL import Image


#========================================================================================================================

if __name__ == '__main__':
        # read input filename and check for help
        csv_filename = sys.argv[1]
        if csv_filename in ('-h','--help') or len(sys.argv) < 2:
            print 'usage: ./image_processor.py source_filename.csv output_filename.jpg'
            print '\nsource file must be either a 3 or 1 channel unsigned int snapshot'
            sys.exit()
        # read output filename
        output_filename = sys.argv[2]
        data = None
        count = 0
        # open file
        csv_file = open(csv_filename, 'rb')
        # determine data rows
        row_count = sum(1 for line in open(csv_filename))-1        
        time_codes = []
        # create csv reader
        reader = csv.reader(csv_file)
        
        # pop first line, this is the header
        header = reader.next()
        # read all remaining lines
        for row in reader:
            time_codes.append(row[0])
            row.pop(0)
            
            # Matrix generation
            if data is None:
                data = np.zeros((row_count, len(row)))
            
            data[count] = row
            count += 1
        v = 1
        # there was more than one image, ask user for index
        if count > 1:
            message = 'Detected ' + str(count) + ' images. Please input desired image number [1-' + str(count) +']: '
            v = int(input(message))
        # check index
        if v < 1 or v > count:
            print 'Chosen index is out of range, defaulting to 1...'
            v = 1
        # pick data
        img_data = data[v - 1]
        # read sizes in header
        x_1 = int(header[2])
        x_2 = int(header[3])
        
        # decide whether to use 3 or 1 channel(s)
        if (img_data.shape[0]/x_1)/x_2 == 3:
            img_array = np.zeros((x_2, x_1, 3), 'uint8')
            img_array_red = np.reshape(img_data[2::3], (x_2, x_1))
            img_array_green = np.reshape(img_data[1::3], (x_2, x_1))
            img_array_blue = np.reshape(img_data[0::3], (x_2, x_1))
            
            img_array[..., 0] = img_array_red
            img_array[..., 1] = img_array_green
            img_array[..., 2] = img_array_blue
            
            img_array = ndimage.rotate(img_array, 90)

        elif (img_data.shape[0]/x_1)/x_2 == 1:
            img_array = np.reshape(img_data, (x_2, x_1))
            img_array = ndimage.rotate(img_array, 90)
        else:
            print 'Image does not seem to be either 3 or 1 channel, qutting...'
            sys.exit()
        # write image (flip first)
        j = Image.fromarray(img_array[::-1])
        j.save(output_filename)
        print 'Done!'
