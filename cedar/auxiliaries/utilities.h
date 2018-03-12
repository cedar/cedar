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

    File:        utilities.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 08 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UTILITIES_H
#define CEDAR_AUX_UTILITIES_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/shared_ptr.hpp>
#endif
#include <typeinfo>
#include <vector>
#include <string>

#ifdef CEDAR_COMPILER_MSVC
  #define NOMINMAX // don't add min and max macros!
  #include <Windows.h>
#endif

namespace cedar
{
  namespace aux
  {
    //! writes the matrix into the shell properly organized by columns and rows
    CEDAR_AUX_LIB_EXPORT void write(cv::Mat matrix);

    /*!@brief Unmangles a c++ name.
     */
    CEDAR_AUX_LIB_EXPORT std::string unmangleName(const char* mangledName);

    /*!@brief Unmangles the c++ name of a given typeinfo object.
     */
    CEDAR_AUX_LIB_EXPORT std::string unmangleName(const std::type_info& typeinfo);

    /*!@brief Generates a type name from the type of the template argument.
     */
    template <typename T>
    std::string typeToString()
    {
      return unmangleName(typeid(T));
    }

    /*!@brief Returns the type of the object pointed to by the given pointer.
     */
    template <typename T>
    std::string objectTypeToString(boost::shared_ptr<T> object)
    {
      return unmangleName(typeid(*object));
    }

    /*!@brief Returns the type of the object pointed to by the given pointer.
     */
    template <typename T>
    std::string objectTypeToString(boost::intrusive_ptr<T> object)
    {
      return unmangleName(typeid(*object));
    }

    /*!@brief Returns the type of the object pointed to by the given pointer.
     */
    template <typename T>
    std::string objectTypeToString(T* object)
    {
      return unmangleName(typeid(*object));
    }

    /*!@brief Structure holding information about an frame in a stack trace.
     */
    struct CEDAR_AUX_LIB_EXPORT StackEntry
    {
      public:
        /*!@brief Returns the symbol associated with this stack entry.
         */
        inline const std::string& symbol() const
        {
          return this->mSymbol;
        }

        /*!@brief Returns the source file associated with this stack entry.
         */
        inline const std::string& file() const
        {
          return this->mFile;
        }

        /*!@brief Sets the symbol of this stack entry.
         */
        inline void setSymbol(const std::string& symbol)
        {
          this->mSymbol = symbol;
        }

        /*!@brief Sets the source file of this stack entry.
         */
        inline void setFile(const std::string& file)
        {
          this->mFile = file;
        }

        /*!@brief Sets the address of the stack frame.
         */
        inline void setAddress(const std::string& address)
        {
          this->mAddress = address;
        }

        /*!@brief Sets the instruction offset from the symbol.
         */
        inline void setSymbolOffset(const std::string& offset)
        {
          this->mSymbolOffset = offset;
        }

#ifdef CEDAR_COMPILER_GCC
        /*!@brief Sets the raw string.
         *
         *        This function parses the raw string from gcc's backtrace capabilities into the relevant information.
         */
        void setRawString(const std::string& rawString);
#endif // CEDAR_COMPILER_GCC

        /*!@brief Operator that writes the stack entry to the given stream in a proper format.
         */
        friend CEDAR_AUX_LIB_EXPORT std::ostream& operator<< (std::ostream& stream, const StackEntry& trace);

      private:
        //! Source file of the current instruction in the stack frame.
        std::string mFile;

        //! Symbol (normally, the current function) of the stack frame.
        std::string mSymbol;

        //! Current instruction address.
        std::string mAddress;

        //! Offset from the current symbol.
        std::string mSymbolOffset;
    };

    /*!@brief Structure holding information about an entire stack trace.
     *
     *        The stack trace is always generated at the moment the object is created.
     */
    struct CEDAR_AUX_LIB_EXPORT StackTrace
    {
      public:
        /*!@brief The default constructor.
         *
         *        This constructor reads the current stack trace from the system and parses it into a list of
         *        StackEntries.
         */
        StackTrace();
#ifdef CEDAR_COMPILER_MSVC
        /*!@brief Generates a stack trace for a specific context.
         */
        StackTrace(CONTEXT* context);
#endif // CEDAR_COMPILER_MSVC

        /*!@brief Returns the number of entries in the stack trace.
         */
        inline size_t size() const
        {
          return this->mStackTrace.size();
        }

        /*!@brief Returns the stack entry at the given index.
         */
        inline const cedar::aux::StackEntry& at(size_t index) const
        {
          return this->mStackTrace.at(index);
        }

        friend CEDAR_AUX_LIB_EXPORT std::ostream& operator<< (std::ostream& stream, const StackTrace& trace);

      private:
        /*!@brief   List of stack entries.
         *
         * @remarks This list is stored in the same order as the stack.
         */
        std::vector<cedar::aux::StackEntry> mStackTrace;

#ifdef CEDAR_COMPILER_MSVC
        /*!@brief Initializes the stack trace from the given context.
         */
        void init(CONTEXT* context);
#endif // CEDAR_COMPILER_MSVC
    };

    /*!@brief Returns the stack entry at the given index.
     */
    CEDAR_AUX_LIB_EXPORT std::ostream& operator<< (std::ostream& stream, const StackEntry& trace);

    /*!@brief Operator that writes the stack trace to the stream.
     */
    CEDAR_AUX_LIB_EXPORT std::ostream& operator<< (std::ostream& stream, const StackTrace& trace);
    
  }
}

#endif // CEDAR_AUX_UTILITIES_H
