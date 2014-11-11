/*======================================================================================================================

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

    File:        PathTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 07 25

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PATH_TEMPLATE_H
#define CEDAR_AUX_PATH_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <vector>


namespace cedar
{
  namespace aux
  {
    //! Policy for paths that determines the separator for path entries.
    template <char Separator, typename StorageT = std::string>
    class CharSeparator
    {
      public:
        //! Returns the separator used for the path.
        static StorageT separator()
        {
          StorageT separator;
          separator += Separator;
          return separator;
        }
    };

    template <typename SeparatorT, typename StorageT = std::string> class PathTemplate;
  }
}


/*!@brief A generic class for paths of any kind.
 */
template <typename SeparatorT, typename StorageT>
class cedar::aux::PathTemplate
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The string type used for storing components of the path.
  typedef StorageT StringType;

  //! Type of separator used.
  typedef SeparatorT SeparatorType;

private:
  //! The type of this class (for meta-programming reasons...)
  typedef PathTemplate<SeparatorType, StringType> SelfType;

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  //! Writes the path to a stream.
  friend std::ostream& operator<< (std::ostream& stream, const SelfType& path)
  {
    stream << path.toString();
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Constructs an empty path.
  PathTemplate()
  {
  }

  //! Constructs a path from a string.
  PathTemplate(const StringType& path)
  {
    // append the vector above to the path
    this->fromString(path);
  }

  //! Constructs a path from a const char string.
  PathTemplate(const char* path)
  {
    // append the vector above to the path
    this->fromString(StringType(path));
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Clears the path.
  void clear()
  {
    this->mComponents.clear();
  }

  //! Appends a component to the path.
  void append(const std::vector<StringType>& path)
  {
    // append the vector above to the path
    this->mComponents.insert(this->mComponents.end(), path.begin(), path.end());
  }

  //! Appends a string component to the path
  void append(const StringType& string)
  {
    // this makes sure the string is properly split into its components if it contains the separator
    SelfType subpath(string);
    this->append(subpath);
  }

  //! Appends another path to this path.
  void append(const SelfType& other)
  {
    this->append(other.mComponents);
  }

  //! Converts the path to a string.
  StringType toString() const
  {
    return cedar::aux::join(this->mComponents, SeparatorType::separator());
  }

  //! Returns the number of components in the path.
  size_t getElementCount() const
  {
    return this->mComponents.size();
  }

  //! Checks whether the path is empty
  bool empty() const
  {
    return this->mComponents.empty();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public operators
  //--------------------------------------------------------------------------------------------------------------------
  //! Concatenates two paths.
  SelfType operator+(const SelfType& other) const
  {
    SelfType copy(*this);
    copy.append(other);
    return copy;
  }

  //! Appends a string to the path.
  SelfType& operator+=(const StringType& string)
  {
    this->append(string);
    return *this;
  }

  //! Concatenates two paths.
  friend StringType operator+(const StringType& string, const SelfType& path)
  {
    return string + path.toString();
  }

  //! Automatic string conversion operator for the path.
  operator StringType() const
  {
    return this->toString();
  }

  //! Returns the element of the path that has the given index.
  StringType operator[](size_t index) const
  {
    CEDAR_ASSERT(index < this->getElementCount());
    return this->mComponents.at(index);
  }

  //! Returns the path, starting from the given index.
  SelfType operator()(size_t start) const
  {
    return this->operator()(start, this->getElementCount());
  }

  //! Returns the section of the path between start and end.
  SelfType operator()(size_t start, size_t end) const
  {
    if (start >= this->getElementCount())
    {
      CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Invalid start index " + cedar::aux::toString(start) + " in path with " + cedar::aux::toString(this->getElementCount()) + " element(s).");
    }

    if (end >= this->getElementCount())
    {
      CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Invalid end index " + cedar::aux::toString(end) + " in path with " + cedar::aux::toString(this->getElementCount()) + " element(s).");
    }

    SelfType slice;
    slice.mComponents.insert(slice.mComponents.end(), this->mComponents.begin() + start, this->mComponents.begin() + end);
    return slice;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void fromString(const StringType& string)
  {
    this->clear();

    std::vector<StringType> components;
    cedar::aux::split(string, SeparatorType::separator(), components);
    this->append(components);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The elements of the path, split apart.
  std::vector<StringType> mComponents;

}; // class cedar::aux::PathTemplate

#endif // CEDAR_AUX_PATH_TEMPLATE_H

