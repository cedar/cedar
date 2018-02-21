#!/usr/bin/python
"""
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

    File:        update_copyright.py

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2015 01 14

    Description: This script replaces the copyright information in all files to a new range of years.

    Credits:

========================================================================================================================
"""

import os, fnmatch, re
from fileinput import FileInput

#  Script configuration

# putting 2013 here would produce a copyright notice covering the years 2011, 2012, and 2013
last_year_of_copyright = 2017
# the default value assumes that you execute the script in the tools directory of the cedar repository
top_path = "../.."
# file extensions for the files scanned
file_extensions = ['cpp', 'h', 'in', 'md', 'py', 'txt', 'conf.example']

copyright_prefix = "Copyright"
copyright_institute = "Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany"


#  Code

def find_replace(topPath, find, replace, filePattern):
  # recursively search through the given path
  for path, dirs, files in os.walk(topPath, topdown=True):
    # exclude mercurial directory
    dirs[:] = [d for d in dirs if d != '.hg']

    for filename in files:
      # only look at files with the given file extension
      match = filePattern.search(filename)
      if match:
        filepath = os.path.join(path, filename)

        s = ""
        with open(filepath) as f:
          s = f.read()
        s = re.sub(find, replace, s)
        with open(filepath, "w") as f:
          f.write(s)

if __name__ == "__main__":
  cedar_home = os.path.realpath(__file__ + "/" + top_path)
  extensions = "|".join(file_extensions)
  file_pattern = re.compile(r'.*\.(' + extensions + r')$')
  years = range(2011, last_year_of_copyright + 1)
  copyright_years = ", ".join(map(str, years))

  find = copyright_prefix + r'.*' + copyright_institute
  replace = copyright_prefix + ' ' + copyright_years + ' ' + copyright_institute

  find_replace(cedar_home, find, replace, file_pattern)
