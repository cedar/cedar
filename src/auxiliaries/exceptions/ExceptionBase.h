/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ExceptionBase.h

 ----- Author:      Oliver Lomp
 ----- Email:       oliver.lomp@ini.rub.de
 ----- Date:        2010 01 20

 ----- Description: Header for the \em cedar::aux::exc::ExceptionBase class.

 ----- Credits:     Christine Goehrke, Tim Dierkes, Marc Hoener, Tobias Kirchhof
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_EXC_EXCEPTION_BASE_H
#define CEDAR_AUX_EXC_EXCEPTION_BASE_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <exception>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::exc::ExceptionBase : public std::exception
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

   /*!@def     CEDAR_THROW(Exception_type, message)
   *
   *  @brief   Throws an exception of the given \em type with the specified \em message.
   *
   *           This macro assigns the correct filename, line and message to a newly generated exception.
   *           An example use would be:
   *           RG_THROW(cedar::aux::exc::NullPointerException, "A pointer was null.");
   *
   *  @remarks The type passed as \em Exception_type should inherit from \em cedar::aux::exc::ExceptionBase.
   *           Do not line-break the following macro(s), or the __LINE__ specification will be wrong!
   */
  #define CEDAR_THROW(Exception_type, message) Exception_type exception; exception.setMessage(message); exception.setLine(__LINE__); exception.setFile(__FILE__); throw exception;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ExceptionBase(void);

  /*!@brief The destructor.
   *
   * @remarks The destructor may not throw any exception.
   */
  virtual ~ExceptionBase(void) throw ();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Compiles the exception info string.
  virtual std::string exceptionInfo(void) const;

  //!@brief Sets the message for the exception.
  void setMessage(const std::string& message);

  //!@brief Sets the name of the source file where the exception occurred.
  void setFile(const std::string& fileName);

  //!@brief Sets the line in the source file where the exception was thrown.
  void setLine(int lineNumber);

  /*!@brief   Prints the string returned by \em cedar::aux::exc::ExceptionBase::exceptionInfo()
   *
   * @remarks The information is printed to \em std::cerr.
   */
  void printInfo() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief STL-required function returning the exception info.
  const char* what(void) const throw();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  //! The type-name of the exception.
  std::string mType;
private:
  //! The message
  std::string mMessage;

  //! Number of line in the source-file where the exception was thrown.
  int mLineNumber;

  //! Source-file that threw the exception.
  std::string mFileName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::exc::ExeptionBase

#endif // CEDAR_AUX_EXC_EXCEPTION_BASE_H
