/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <boost/shared_ptr.hpp>
#include <typeinfo>


namespace cedar
{
  namespace aux
  {
    /*!@brief   If you think a dynamic cast can never fail, use this cast instead.
     *
     *          In debug builds, it automatically checks whether you are right. In release mode, it statically casts to
     *          the desired type.
     *          The advantage of using this function is a speed gain: dynamic_casts are slow compared to static_casts.
     *
     * @remarks The syntax for using this cast is DerivedType* p2 = cedar::aux::asserted_cast<DerivedType*>(p);
     *          The second template argument does not have to be specified; it is determined automatically by the
     *          compiler!
     */
    template <typename TOut, typename TIn>
    TOut asserted_cast(TIn pIn)
    {
      CEDAR_DEBUG_ASSERT(dynamic_cast<TOut>(pIn) != 0);
      return static_cast<TOut>(pIn);
    }

    /*!@brief   If you think a dynamic cast can never fail, use this cast instead.
     *
     * @see     cedar::aux::asserted_cast.
     */
    template <typename TOut, typename TIn>
    boost::shared_ptr<TOut> shared_asserted_cast(boost::shared_ptr<TIn> pIn)
    {
      CEDAR_DEBUG_ASSERT(boost::shared_dynamic_cast<TOut>(pIn));
      return boost::static_pointer_cast<TOut>(pIn);
    }

    /*!@brief Unmangles a c++ name.
     */
    CEDAR_AUX_LIB_EXPORT std::string unmangleName(const char* mangledName);

    /*!@brief Unmangles the c++ name of a given typeinfo object.
     */
    CEDAR_AUX_LIB_EXPORT std::string unmangleName(const std::type_info& typeinfo);


    /*!@brief Structure holding information about an entry in the stack trace.
     */
    struct CEDAR_AUX_LIB_EXPORT StackEntry
    {
      public:
        inline const std::string& symbol() const
        {
          return this->mSymbol;
        }

        inline const std::string& file() const
        {
          return this->mFile;
        }

        inline void setSymbol(const std::string& symbol)
        {
          this->mSymbol = symbol;
        }

        inline void setFile(const std::string& file)
        {
          this->mFile = file;
        }

        inline void setAddress(const std::string& address)
        {
          this->mAddress = address;
        }

        inline void setSymbolOffset(const std::string& offset)
        {
          this->mSymbolOffset = offset;
        }

        void setRawString(const std::string& rawString);

        friend CEDAR_AUX_LIB_EXPORT std::ostream& operator<< (std::ostream& stream, const StackEntry& trace);

      private:
        std::string mFile;
        std::string mSymbol;
        std::string mAddress;
        std::string mSymbolOffset;
    };

    /*!@brief Structure holding information about an entire stack trace.
     *
     *        The stack trace is always generated at the moment the object is created.
     */
    struct CEDAR_AUX_LIB_EXPORT StackTrace
    {
      public:
        StackTrace();

        inline size_t size() const
        {
          return this->mStackTrace.size();
        }

        inline const cedar::aux::StackEntry& at(size_t index) const
        {
          return this->mStackTrace.at(index);
        }

        friend CEDAR_AUX_LIB_EXPORT std::ostream& operator<< (std::ostream& stream, const StackTrace& trace);

      private:
        std::vector<cedar::aux::StackEntry> mStackTrace;
    };
    
  }
}

#endif // CEDAR_AUX_UTILITIES_H
