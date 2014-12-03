#!/usr/bin/python
"""
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
template_base_path = "../templates/"

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
parser = OptionParser()
parser.add_option("-c", "--header-only", dest="header_only",
                  help="When specified, no cpp file will be generated",
                  default=False, action="store_true")
parser.add_option("-f", "--fwd-only", dest="fwd_only",
                  help="When specified, only the forward-declaration header will be generated.",
                  default=False, action="store_true")
parser.add_option("-p", "--output-path", dest="output_path",
                  help="When specified, the folder in which the new files are put.",
                  default=None, action="store")
                  
parser.add_option("-t", "--template", dest="template",
                  help="When specified, selects a specific template to be used. Currently, the only option is processing_step",
                  default=None, action="store")
                 

(options, args) = parser.parse_args()

# Deal with the arguments
header_only = options.header_only
fwd_only = options.fwd_only
if not options.template is None:
    if not os.path.exists(template_base_path + options.template):
        print "Could not find template", options.template
        sys.exit(-1)
    template_base_path += options.template + os.sep

if len(args) > 1:
    print "Too many arguments (", args, "). Stopping."
    sys.exit(-1)
elif len(args) == 0:
    print "Not enough arguments."
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
  
# derive some names & values
class_path = class_name_full

namespaces = class_name_full.split("::")
namespaces = namespaces[:-1]
if namespaces[0] == "cedar":
  top_level_namespace = namespaces[1]
  is_cedar_class = True
else:
  top_level_namespace = namespaces[0]
  is_cedar_class = False

for namespace, alias in namespace_aliases.items():
  class_path = class_path.replace("::" + namespace, "::" + alias)
  
class_path = class_path.replace("::", os.sep)
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
replacements["<class header path>"] = class_path + ".h"
replacements["class cedar::xxx::xxx"] = "class " + class_name_full
replacements["CEDAR_XXX_XXX_H"] = class_id_all_cap + "_H"
replacements["CEDAR_XXX_XXX_FWD_H"] = class_id_all_cap + "_FWD_H"
replacements["<first name> <last name>"] = users_name
replacements["<email address>"] = users_mail
replacements["<class name>"] = class_name
replacements["<full class name>"] = class_name_full
replacements["<namespace path>"] = namespace_path
replacements["<copyright years>"] = "2011"
for year in range(2012, today.year + 1):
  replacements["<copyright years>"] += ", " + str(year)

if is_cedar_class:
  replacements["<base namespace path>"] = "cedar/" + namespace_aliases[top_level_namespace]
else:
  replacements["<base namespace path>"] = top_level_namespace
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

replacements["<begin namespaces>"] = namespace_begin
replacements["<end namespaces>"] = namespace_end
replacements["<namespaces indent>"] = indent
  
# Get user confirmation

print "Creating class:", class_name_full, "aka", class_name
if not options.template is None:
    print "Using template", options.template
print "at:", class_path + ".{" + ", ".join(extensions) + "}"
# print "replacements:", replacements
# print "namespaces:", namespaces
print "top-level namespace:", top_level_namespace
choice = None
while not choice in ('y', 'n'):
    choice = raw_input("Do you want to proceed? (y/n): ")

if choice == 'n':
    print "Okay, bye!"
    sys.exit()

templates = {"h": "classHeader.h", "fwd.h": "classHeader.fwd.h", "cpp": "classImplementation.cpp"}

base_directory = cedar_home

if options.output_path is not None:
    base_directory = options.output_path

for extension in extensions:
    print "Copying template", templates[extension]
    replacements["<filename>"] = class_name + "." + extension
    with open(template_base_path + templates[extension], "r") as header:
        contents = header.read()
    
    for search, replace in replacements.items():
      contents = contents.replace(search, replace)
      
    destination = base_directory + os.sep + class_path + "." + extension
    print "destination:", destination
    
    if os.path.exists(destination):
      print "File already exists. Skipping."
      continue
      
    with open(destination, "w") as out:
      out.write(contents)
    
print "Done. Please remember to rerun cmake!"

