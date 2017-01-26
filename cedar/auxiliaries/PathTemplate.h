/*======================================================================================================================

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
#include <string>


namespace cedar
{
  namespace aux
  {
    //! Policy for paths that determines the separator for path entries.
    template <char Separator, typename StorageT = std::string>
    class CharSeparator
    {
      public:
        //! Splits the string based on the separator specified via template argument.
        static std::vector<StorageT> separate(const StorageT& string)
        {
          // we have to use += because std::string doesn't have a constructor that takes a single character.
          std::string separator;
          separator += Separator;

          std::vector<StorageT> components;
          cedar::aux::split(string, separator, components);
          return components;
        }

        static StorageT joinComponents(const std::vector<StorageT>& components)
        {
          // we have to use += because std::string doesn't have a constructor that takes a single character.
          std::string separator;
          separator += Separator;
          return cedar::aux::join(components, separator);
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

  //! Appends another path to this path.
  void append(const SelfType& other)
  {
    this->append(other.mComponents);
  }

  //! Sets the path from a given string.
  virtual void fromString(const StringType& string)
  {
    this->clear();
    this->append(SeparatorType::separate(string));
  }

  //! Converts the path to a string.
  virtual StringType toString() const
  {
    return SeparatorType::joinComponents(this->mComponents);
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

  //! Returns the first element in the path
  const StorageT& getFirst() const
  {
    if (this->mComponents.empty())
    {
      CEDAR_THROW(cedar::aux::InvalidValueException, "Cannot return first part of path: path is empty.");
    }

    return this->mComponents.front();
  }

  //! Returns the last element in the path
  const StorageT& getLast() const
  {
    if (this->mComponents.empty())
    {
      CEDAR_THROW(cedar::aux::InvalidValueException, "Cannot return last part of path: path is empty.");
    }

    return this->mComponents.back();
  }

  //! Returns the component with the given index.
  const StorageT& getComponent(size_t i) const
  {
    return this->mComponents.at(i);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public operators
  //--------------------------------------------------------------------------------------------------------------------
  //! Returns the component with the given index.
  const StorageT& operator[](size_t index) const
  {
    return this->getComponent(index);
  }

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

    if (end > this->getElementCount())
    {
      CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Invalid end index " + cedar::aux::toString(end) + " in path with " + cedar::aux::toString(this->getElementCount()) + " element(s).");
    }

    SelfType slice;
    slice.mComponents.insert(slice.mComponents.end(), this->mComponents.begin() + start, this->mComponents.begin() + end);
    return slice;
  }

  //! Compares two paths.
  virtual bool operator< (const SelfType& other) const
  {
    if (other.mComponents.size() < this->mComponents.size())
    {
      return false;
    }
    if (other.mComponents.size() > this->mComponents.size())
    {
      return true;
    }

    auto this_iter = this->mComponents.begin();
    auto other_iter = other.mComponents.begin();
    for (; this_iter != this->mComponents.end() && other_iter != other.mComponents.end(); ++this_iter, ++other_iter)
    {
      const auto& this_component = *this_iter;
      const auto& other_component = *other_iter;

      if (this_component < other_component)
      {
        return true;
      }
      else if (this_component > other_component)
      {
        return false;
      }
    }
    return false;
  }

  //! Compares two paths.
  virtual bool operator== (const SelfType& other) const
  {
    if (other.mComponents.size() != this->mComponents.size())
    {
      return false;
    }

    auto this_iter = this->mComponents.begin();
    auto other_iter = other.mComponents.begin();
    for (; this_iter != this->mComponents.end() && other_iter != other.mComponents.end(); ++this_iter, ++other_iter)
    {
      const auto& this_component = *this_iter;
      const auto& other_component = *other_iter;

      if (this_component != other_component)
      {
        return false;
      }
    }

    return true;
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! The elements of the path, split apart.
  std::vector<StringType> mComponents;
private:
  // none yet

}; // class cedar::aux::PathTemplate

#endif // CEDAR_AUX_PATH_TEMPLATE_H

