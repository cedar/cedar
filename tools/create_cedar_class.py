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

    File:        create_cedar_class.py

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 08 22

    Description: This script creates new files for a class in cedar based on the code templates.

    Credits:

========================================================================================================================
"""

from optparse import OptionParser
import datetime
import sys
import os
import re

# Script configuration:
namespace_aliases = {}
namespace_aliases["unit"] = "units"
namespace_aliases["aux"] = "auxiliaries"
namespace_aliases["proc"] = "processing"
namespace_aliases["dyn"] = "dynamics"
namespace_aliases["dev"] = "devices"
namespace_aliases["conv"] = "convolution"
namespace_aliases["test"] = "testingUtilities"

# determine user's home directory
home = os.getenv('USERPROFILE') or os.getenv('HOME')
users_name = None
users_mail = None
users_name_cfg = "user's name"
users_mail_cfg = "user's mail"

# determine cedar home
cedar_home = os.path.realpath(__file__ + "/../..")
print "cedar home detected as:", cedar_home
template_base_path = cedar_home + os.sep + "templates" + os.sep

# Configuration reading
config_file_path = home + os.sep + ".cedar" + os.sep + "template_script.cfg"
try:
    with open(config_file_path, "r") as config:
        print "Reading user information from", config_file_path
        for line in config.readlines():
            line = line.rstrip()
            vals = line.split("=")
            if len(vals) == 2:
                arg = vals[0].strip().rstrip()
                val = vals[1].strip().rstrip()
                if arg == users_name_cfg:
                    users_name = val
                elif arg == users_mail_cfg:
                    users_mail = val
                else:
                    print "unknown config entry:", arg
            else:
                print "Could not interpret config line:", line
except IOError:
  pass


# Set up option parser
parser = OptionParser(usage = "Usage: %prog [options] name::spaces::ClassName")
parser.add_option("-c", "--header-only", dest="header_only",
                  help="When specified, no cpp file will be generated",
                  default=False, action="store_true")
parser.add_option("-f", "--fwd-only", dest="fwd_only",
                  help="When specified, only the forward-declaration header will be generated.",
                  default=False, action="store_true")
parser.add_option("-s", "--no-subdirectories", dest="no_subdirectories",
                  help="When specified, generates the class exactly in the current directory, i.e., no subdirectories will be created.",
                  default=False, action="store_true")
parser.add_option("-p", "--output-path", dest="output_path",
                  help="When specified, the folder in which the new files are put.",
                  default=None, action="store")
parser.add_option("-o", "--overwrite", dest="overwrite",
                  help="Overwrite existing files.",
                  default=None, action="store_true")
                  
parser.add_option("-t", "--template", dest="template",
                  help="When specified, selects a specific template to be used.  Currently, the only option is 'step'. USE THIS TO CREATE NEW STEPS!", default=None, action="store")
                 

(options, args) = parser.parse_args()

# Deal with the arguments
header_only = options.header_only
fwd_only = options.fwd_only
overwrite = options.overwrite
if not options.template is None:
    if (options.template == "step"):
        options.template= "processing_step"
    if not os.path.exists(template_base_path + options.template):
        print "Could not find template", options.template
        sys.exit(-1)
    template_base_path += options.template + os.sep

if len(args) > 1:
    print "Please specify a single class name (you specified: ", args, ")."
    sys.exit(-1)
elif len(args) == 0:
    print "Please specify a class name."
    parser.print_usage()
    sys.exit(-1)

class_name_full = args[0]

extensions = []
extensions.append("fwd.h")
if not fwd_only:
    extensions.append("h")
if not header_only and not fwd_only:
    extensions.append("cpp")
  
# Get user information if it is missing
if users_name is None:
    users_name = raw_input("Please input your <first name> <last name>: ")
  
if users_mail is None:
    users_mail = raw_input("Please input your <email>: ")

# Store username and email
with open(config_file_path, "w") as config:
    config.write(users_name_cfg + " = " + users_name + "\n")
    config.write(users_mail_cfg + " = " + users_mail + "\n")
  

# split namespaces by separating according to ::
namespaces = class_name_full.split("::")

if len(namespaces) > 1:
  # splice away the last entry, as it is the class name
  namespaces = namespaces[:-1]
  if namespaces[0] == "cedar":
    top_level_namespace = namespaces[1]
    is_cedar_class = True
    has_namespace = True
  else:
    top_level_namespace = namespaces[0]
    is_cedar_class = False
    has_namespace = True
else:
    top_level_namespace = None
    is_cedar_class = False
    has_namespace = False

# derive some names & values
class_path = class_name_full

for namespace, alias in namespace_aliases.items():
  class_path = class_path.replace("::" + namespace, "::" + alias)
  
class_path = class_path.replace("::", os.sep)
if has_namespace:
  namespace_path = class_path[:class_path.rfind(os.sep)]

standard_separator = "/"
if os.sep != standard_separator:
  namespace_path = namespace_path.replace(os.sep, standard_separator)
  class_path = class_path.replace(os.sep, standard_separator)

class_name = class_name_full.split("::")[-1]
class_id_all_cap = class_name_full.replace("::", "_")
class_id_all_cap = re.sub(r"([a-z])([A-Z])", r"\1_\2", class_id_all_cap).upper()
 
# Set up replacements

replacements = {}
replacements["<filename>"] = class_name + ".[extension]"
today = datetime.date.today()
replacements["<creation date YYYY MM DD>"] = str(today.year) + " " + ("%02i") % (today.month) + " " + ("%02i") % (today.day)
if options.no_subdirectories:
  replacements["<class header path>"] = class_name + ".h"
  replacements["<class fwd header path>"] = class_name +".fwd.h"
else:
  replacements["<class header path>"] = class_path + ".h"
  replacements["<class fwd header path>"] = class_path+".fwd.h"
replacements["class cedar::xxx::xxx"] = "class " + class_name_full
replacements["CEDAR_XXX_XXX_H"] = class_id_all_cap + "_H"
replacements["CEDAR_XXX_XXX_FWD_H"] = class_id_all_cap + "_FWD_H"
replacements["<first name> <last name>"] = users_name
replacements["<email address>"] = users_mail
replacements["<class name>"] = class_name
replacements["<full class name>"] = class_name_full
if has_namespace:
  replacements["<namespace path>"] = namespace_path
replacements["<copyright years>"] = "2011"
for year in range(2012, today.year + 1):
  replacements["<copyright years>"] += ", " + str(year)

if is_cedar_class:
  replacements["<base namespace path>"] = "cedar/" + namespace_aliases[top_level_namespace]
elif not top_level_namespace is None:
  replacements["<base namespace path>"] = top_level_namespace

if not top_level_namespace is None:
  replacements["<CAP_SHORT_MAIN_NAMESPACE>"] = top_level_namespace.upper()

# build namespace replacement
namespace_begin = ""
namespace_end = ""
indent = ""

for namespace in namespaces:
    namespace_begin += indent + "namespace " + namespace + "\n" + indent + "{\n"
    namespace_end = indent + "}\n" + namespace_end
    
    indent += "  "

# remove final (superfluous) newline
namespace_begin = namespace_begin[:-1]

if has_namespace:
  replacements["<begin namespaces>"] = namespace_begin
  replacements["<end namespaces>"] = namespace_end
  replacements["<namespaces indent>"] = indent
else:
  replacements["<begin namespaces>"] = ""
  replacements["<end namespaces>"] = ""
  replacements["<namespaces indent>"] = ""
  
# determine where to put the new files
base_directory = cedar_home

is_in_cedar = options.output_path is None and is_cedar_class
if not is_in_cedar:
  if not options.output_path is None:
    base_directory = options.output_path
  else:
    base_directory = os.getcwd()
    
if not options.no_subdirectories:
  destination_base = base_directory + os.sep + class_path
else:
  destination_base = base_directory + os.sep + class_name
  
# Get user confirmation

print "Creating class:", class_name_full, "aka", class_name
if not options.template is None:
    print "Using template", options.template
print "at:", destination_base + ".{" + ", ".join(extensions) + "}"
# print "replacements:", replacements
# print "namespaces:", namespaces
print "top-level namespace:", top_level_namespace
choice = None
while not choice in ('y', 'n'):
    choice = raw_input("Do you want to proceed? (y/n): ")

if choice == 'n':
    print "Okay, bye!"
    sys.exit()

# make sure the path for the namespace exists
if has_namespace and not options.no_subdirectories and not os.path.exists(namespace_path):
  os.makedirs(namespace_path)

templates = {"h": "classHeader.h", "fwd.h": "classHeader.fwd.h", "cpp": "classImplementation.cpp"}
    
if_re = re.compile(r'\<if\s*:\s*(\w+)\s*\>\n(.*?)\<\s*endif\s*\>\n', re.DOTALL)
else_re = re.compile(r'(.*?)\s*\<\s*else\s*\>\n(.*)', re.DOTALL)

for extension in extensions:
    print "Copying template", templates[extension]
    replacements["<filename>"] = class_name + "." + extension
    with open(template_base_path + templates[extension], "r") as header:
        contents = header.read()
    
    # apply if-conditions
    m = if_re.search(contents)
    while not m is None:
        condition = m.group(1)
        block = m.group(2)
        
        # see if there is an else in the block
        m_else = else_re.search(block)
        
        if m_else is None:
            if_true = block
            if_false = ""
        else:
            if_true = m_else.group(1) + "\n"
            if_false = m_else.group(2)
        
        is_true = False
        if condition == "in_cedar":
            is_true = is_in_cedar
        
        if is_true:
            replacement = if_true
        else:
            replacement = if_false
        
        contents = contents[:m.start(0)] + replacement + contents[m.end(0):]
        
        # match next instance
        m = if_re.search(contents)
      
    # apply simple search-and-replace things
    for search, replace in replacements.items():
        contents = contents.replace(search, replace)
        
    destination = destination_base + "." + extension
      
    print "destination:", destination
    
    if not overwrite and os.path.exists(destination):
        print "File already exists. Skipping."
        continue

    try:    
        with open(destination, "w") as out:
            out.write(contents)
    except IOError as e:
        print "Could not create file \"" + str(destination) + "\". Error:", str(e)
        sys.exit(-1)
    
print "Done. Please remember to rerun cmake!"

