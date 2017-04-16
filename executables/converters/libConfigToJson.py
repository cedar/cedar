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

    File:        libConfigToJson.py

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 10

    Description: This script converts simple libconfig++-style configuration files to json trees.

    Credits:

========================================================================================================================
"""

from __future__ import print_function
import sys

class CfgParser:
    
    def __init__(self, text):
        self._text = text
        self._index = 0
        self._temporaries = ["", ""]
        self.tree = []
        
    def parse(self):
        self._parse_key(0)
        
    def _parse_key(self, index):
        if index >= len(self._text):
            return
            
        char = self._text[index]
        if char == "/" and self._text[index + 1] == "/":
            self._parse_line_comment(index + 2, self._parse_key)
        elif char == "/" and self._text[index + 1] == "*":
            self._parse_block_comment(index + 2, self._parse_key)
        elif char == "=":
            self._parse_value(index + 1)
        else:
            self._temporaries[0] += char
            self._parse_key(index + 1)
            
    def _parse_value(self, index):
        char = self._text[index]
        if char == "/" and self._text[index + 1] == "/":
            self._parse_line_comment(index + 2, self._parse_value)
        elif char == "/" and self._text[index + 1] == "*":
            self._parse_block_comment(index + 2, self._parse_value)
        elif char == ";" or char == "\n":
            key = self._temporaries[0].strip().rstrip()
            value = self._temporaries[1].strip().rstrip()
            self.tree.append((key, value))
            self._temporaries = ["", ""]
            self._parse_eol(index + 1)
        elif char == "\"":
            self._parse_string(index + 1, 1, self._parse_value)
        else:
            self._temporaries[1] += char
            self._parse_value(index + 1)
            
    def _parse_string(self, index, temp_index, prev):
        while self._text[index] != "\"":
            self._temporaries[temp_index] += self._text[index]
            index += 1
        
        prev(index + 1)
            
                   
    def _parse_eol(self, index):
        if index >= len(self._text):
            return
            
        while self._text[index] != "\n":
            index += 1
            
        self._parse_key(index + 1)
                   
    def _parse_block_comment(self, index, prev):
        char = self._text[index]
        while char != "*" or self._text[index + 1] != "/":
            index += 1
            char = self._text[index]
            
        prev(index + 2)
            
        
    def _parse_line_comment(self, index, prev):
        char = self._text[index]
        if char == "\n":
            prev(index + 1)
        else:
            self._parse_line_comment(index + 1, prev)
    
if __name__ == "__main__":
    print ("\n")
    print("Welcome to the config file conversion script. Please note that comments in the configuration file",
          "will be lost in the translation from libconfig++-style to json.")
    print ("\n")
    
    if len(sys.argv) < 2:
        print ("Error: Missing arguments.")
        print ("Usage:", sys.argv[0], "path/to/file.cfg")
        print ("\n")
        sys.exit(-1)
      
      
    file_in = sys.argv[1]
    index = file_in.find(".cfg")
    if index != -1:
        file_out = file_in[:file_in.find(".cfg")]
    else:
        file_out = file_in
        
    file_out += ".json"
    
    print ("\n", "Converting file", file_in, "to", file_out)
    
    text = ""
    with open(file_in, "r") as h_in:
        line = h_in.readline()
        while line:
            text += line
            line = h_in.readline()
            
    # No special state, just plain text.
    state_plain = 0
    
    # In the key of the key-value pair
    state_key = 1
    
    # In the value of the key-value pair
    state_value = 2
    
    # Found the (first) comment slash
    state_comment_start_slash = 3

    # In a block comment
    state_comment_block = 4
    
    # In a line comment
    state_comment_line = 5
    
    # Found the first indication of the end of the comment
    state_comment_end_asterisk = 6
    
    # In a string value
    state_value_string = 7
    
    
    # Parsing
    #line_count = 0
    #state = state_key
    #prev_state = None
    
    parser = CfgParser(text)
    parser.parse()
    
    pairs = []
    for key, value in parser.tree:
        pairs.append("\"" + key + "\": \"" + value + "\"")
        
    with open(file_out, "w") as h_out:
        h_out.write("{\n  ")
        h_out.write(",\n  ".join(pairs))
        h_out.write("\n}")
