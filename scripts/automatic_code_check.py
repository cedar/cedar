#!/usr/bin/python
"""
========================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        create_cedar_class.py

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 12 10

    Description: This script automatically checks the quality of the code in cedar.

    Credits:

========================================================================================================================
"""

import os
import re


cedar_home = os.path.realpath(__file__ + "/../..")

class IssueList:
  def __init__(self):
    self._issues = dict()
    
  def add_issue(self, filename, issue):
    if not filename in self._issues:
      self._issues[filename] = list()
    
    self._issues[filename].append(issue)
    
  def write_issues(self, issue_file):
    issue_count = 0
    with open(issue_file, "w") as f:
      for file in sorted(self._issues.keys()):
        issues = self._issues[file]
        if len(issues) > 0:
          f.write(file + "\n\n")
          for issue in issues:
            self._write_issue(f, issue + "\n")
            issue_count += 1
          f.write("\n\n")
          
      if issue_count == 0:
        f.write("No issues found. Hooray!")       
      
    print issue_count, "issues written to \"" + issue_file + "\"."
    

  def _write_issue(self, issues_out, issue_description):
    issues_out.write("[ ] " + str(issue_description))

issues = IssueList()

  
#
# returns the header corresponding to a cpp file
#
def get_header(filename):
  return ".".join(filename.split('.')[:-1]) + ".h"
  
#
# preprocesses c++ file contents (removes all comments and block-comments
# 
block_comment = re.compile(r'(\/\*.*?\*\/)', re.DOTALL)
inline_comment = re.compile(r'(\/\/.*?)$', re.MULTILINE)

def preprocess(source):
  no_blocks = block_comment.sub("", source)
  no_comments = inline_comment.sub("", no_blocks)
  return no_comments

#
# checks if all the std headers are included where they should be.
#
std_use_regex = re.compile(r"std\:\:([a-zA-Z]+)[ \<]")
std_class_headers = {
                      "cout": "iostream",
                      "endl": "iostream",
                      "cerr": "iostream",
                      "stringstream": "sstream",
                      "function": "functional",
                      "ifstream": "fstream",
                      "ofstream": "fstream",
                      "multiset": "set",
                      "pair": "utility",
                      "min": "algorithm",
                      "max": "algorithm",
                      "istringstream": "sstream",
                      "ostringstream": "sstream",
                      "get": "tuple"
                    }
def check_std_headers(filename, file_contents):
  matches = set(std_use_regex.findall(file_contents))
  filetype = filename.split('.')[-1]
  for classname in matches:
    # determine the header for the class
    if classname in std_class_headers:
      header = std_class_headers[classname]
    else:
      header = classname
    
    # see if the header is being included
    include_re = re.compile(r'\s*#\s*include\s*[\<\"]' + header + r'[\>\"]')
    include = include_re.search(file_contents)
    
    header_found = True
    if include is None:
      header_found = False
      
    if not header_found and filetype == "cpp":
      file_header = get_header(filename)
      #print "also checking header", file_header
      with open(file_header, "r") as f_header: 
        contents = preprocess(f_header.read())
        include = include_re.search(contents)
        if not include is None:
          header_found = True
    
    if header_found == False:
      issues.add_issue(filename, "The stl class/function std::" + classname + " is used without including \"" + header + "\".")

#
#
#
file_regex = re.compile(r"\.(h|cpp)$")
def check_file(filename):
  match = file_regex.search(filename)
  if match is None:
    return

  print "Checking file", filename.split("cedar")[-1]
  
  with open(filename, "r") as f:
    file_contents = preprocess(f.read())
    check_std_headers(filename, file_contents)
    
def check_directory(directory):
  files = os.listdir(directory)
  
  for file in files:
    path = directory + os.sep + file
    if os.path.isdir(path) == True:
      check_directory(path)
    else:
      check_file(path)

#
#
#
def check_all():
  check_directory(cedar_home + os.sep + "cedar")
  
#
#
#
if __name__ == "__main__":
  # todo: read from command line which 
  check_all()
  issues.write_issues("issues.txt")
  print "Done."
