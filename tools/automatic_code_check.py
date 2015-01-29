#!/usr/bin/python
"""
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
import argparse
import datetime


cedar_home = os.path.realpath(__file__ + "/../..")
base_directory = cedar_home

issue_type_compatibility = 'C'
issue_type_miscellaneous = 'M'

class Issue:
  def __init__(self, issue_type, issue_no, description, line = None, column = None):
    self._type = issue_type
    self._number = issue_no
    self._description = description

  def __str__(self):
    return self._type + ('%04i:' % self._number) + " " + self._description
    
class IssueC0001 (Issue):
  def __init__(self, stl_class, stl_header):
    Issue.__init__(self, 
                   issue_type_compatibility, 1,
                   "The stl class/function std::" + stl_class + " is used without including \"" + stl_header + "\"."
                   )

class IssueC0002 (Issue):
  def __init__(self, line, column, boost_call):
    replacements = {"shared_dynamic_cast": "dynamic_pointer_cast"}
    Issue.__init__(self, 
                   issue_type_compatibility, 2,
                   "In line " + str(line) + ", col. " + str(column) + ": Replace the function boost::" + boost_call + " by " + replacements[boost_call] + "."
                   )

class IssueM0001 (Issue):
  def __init__(self, line, column):
    Issue.__init__(self, 
                   issue_type_miscellaneous, 1,
                   "In line " + str(line) + ", col. " + str(column) + ": copyright year is outdated."
                   )

class IssueM0002 (Issue):
  def __init__(self):
    Issue.__init__(self, 
                   issue_type_miscellaneous, 2,
                   "The copyright information does not conform to the established standards."
                   )

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
      f.write("For detailed documentation of the issues, see IssueDocumentation.md.\n\n")
    
      for file in sorted(self._issues.keys()):
        issues = self._issues[file]
        if len(issues) > 0:
          f.write(file + "\n\n")
          for issue in issues:
            self._write_issue(f, issue)
            issue_count += 1
          f.write("\n\n")
          
      if issue_count == 0:
        f.write("No issues found. Hooray!")  
      
    print issue_count, "issue(s) written to \"" + issue_file + "\"."
    

  def _write_issue(self, issues_out, issue):
    issues_out.write("[ ] " + str(issue) + "\n")

issues = IssueList()


class REBasedCheck:
  def __init__(self, re, use_preprocessed = True):
    self._use_preprocessed = use_preprocessed
    self._re = re
    
  def check(self, issues, filename, file_contents, preprocessed_contents):
    if self._use_preprocessed:
      contents = preprocessed_contents
    else:
      contents = file_contents
      
    current_pos = 0
    any_found = False
    while True:
      m = self._re.search(file_contents, current_pos)
      if m is None:
        break
        
      any_found = True
        
      current_pos = m.end()
      lines = contents[:m.start()].split("\n")
      line_no = len(lines)
      col_no = len(lines[-1]) + 1
      
      self._found(issues, filename, line_no, col_no, m)
      
    if not any_found and "_no_matches" in dir(self):
      self._no_matches(issues, filename)

    
class CheckC0001 (REBasedCheck):
  def __init__(self):
    REBasedCheck.__init__(self,
                          re = re.compile(r'boost\:\:(shared_dynamic_cast)'),
                          use_preprocessed = True
                          )

  def _found(self, issues, filename, line, column, match):
    issues.add_issue(filename, IssueC0002(line, column, match.group(1)))
    
    
class CheckM0001 (REBasedCheck):
  def __init__(self):
    self.years = "2011"
    today = datetime.date.today()
    for year in range(2012, today.year + 1):
      self.years += ", " + str(year)
      
    REBasedCheck.__init__(self,
                          re = re.compile(r'Copyright (\d{4},? )+'),
                          use_preprocessed = False
                          )

  def _found(self, issues, filename, line, column, match):
    if match.group(0) != "Copyright " + self.years + " ":
      issues.add_issue(filename, IssueM0001(line, column))
    
  def _no_matches(self, issues, filename):
      issues.add_issue(filename, IssueM0002())
    

  
#
# returns the header corresponding to a cpp file
#
def get_header(filename):
  return ".".join(filename.split('.')[:-1]) + ".h"
  
#
# preprocesses c++ file contents (replaces all comments and block-comments by spaces)
# 
def preprocess(source):
  plain, comment_begin, in_line_comment, in_block_comment = (0, 1, 2, 3)
  state = plain
  no_comments = list(source)
  for i in range(len(source)):
    if state == plain:
      if source[i] == '/':
        state = comment_begin
        
    elif state == comment_begin:
      if source[i] == '/':
        state = in_line_comment
      elif source[i] == '*':
        state = in_block_comment
      else:
        state = plain

      if state != plain:
        no_comments[i - 1] = ' ';
        no_comments[i] = ' ';
        
    elif state in (in_line_comment, in_block_comment):
      if state == in_line_comment and source[i] == '\n':
        state = plain
      if state == in_block_comment and source[i] == '*' and len(source) > i + 1 and source[i + 1] == '/':
        no_comments[i] = ' '
        no_comments[i + 1] = ' '
        state = plain
      elif source[i] != '\n':
        no_comments[i] = ' '
  
  assert len(no_comments) == len(source)
  no_comments = "".join(no_comments)
  return no_comments

#
# Tests for C0001: checks if all the std headers are included where they should be.
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
def check_std_headers(filename, file_contents, preprocessed_contents, ):
  matches = set(std_use_regex.findall(preprocessed_contents))
  filetype = filename.split('.')[-1]
  for classname in matches:
    # determine the header for the class
    if classname in std_class_headers:
      header = std_class_headers[classname]
    else:
      header = classname
    
    # see if the header is being included
    include_re = re.compile(r'\s*#\s*include\s*[\<\"]' + header + r'[\>\"]')
    include = include_re.search(preprocessed_contents)
    
    header_found = True
    if include is None:
      header_found = False
      
    if not header_found and filetype == "cpp":
      file_header = get_header(filename)
      #print "also checking header", file_header
      try:
        with open(file_header, "r") as f_header: 
          contents = preprocess(f_header.read())
          include = include_re.search(contents)
          if not include is None:
            header_found = True
      except IOError:
        pass # ok: if the header file doesn't exist, we don't care about it.
    
    if header_found == False:
      issues.add_issue(filename, IssueC0001(classname, header))

#
# Tests for C0002: Checks if boost::shared_dynamic_cast is used
#
def check_deprecated_boost_casts(filename, file_contents, preprocessed_contents):
  check = CheckC0001()
  check.check(issues, filename, file_contents, preprocessed_contents)
  
def check_copyright_year(filename, file_contents, preprocessed_contents):
  check = CheckM0001()
  check.check(issues, filename, file_contents, preprocessed_contents)
  

#
#
#
file_regex = re.compile(r"\.(h|cpp)$")
def check_file(filename):
  match = file_regex.search(filename)
  if match is None:
    return

  print "Checking file", filename.split(base_directory)[-1]
  
  with open(filename, "r") as f:
    file_contents = f.read()
    preprocessed_contents = preprocess(file_contents)
    check_std_headers(filename, file_contents, preprocessed_contents)
    check_deprecated_boost_casts(filename, file_contents, preprocessed_contents)
    check_copyright_year(filename, file_contents, preprocessed_contents)
    
    
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
def check_all(directory):
  check_directory(directory)
  
#
#
#
if __name__ == "__main__":
  try:
    parser = argparse.ArgumentParser(description='Checks code for style and other issues.')
    parser.add_argument('--input-directory', dest='input_directory', action='store',
                       default=cedar_home + os.sep + "cedar",
                       help='Specify the directory in which files should be checked.')
                       
    parser.add_argument('--output', dest='output', action='store',
                       default="issues.txt",
                       help='Specify the file to which the issues are to be written.')

    args = parser.parse_args()
    check_all(args.input_directory)
    issues.write_issues(args.output)
    print "Done."
  except KeyboardInterrupt:
    print "Cancelled by user. Bye."
