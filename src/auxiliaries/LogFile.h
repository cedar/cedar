/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        LogFile.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 27

 ----- Description: Header for the \em cedar::aux::LogFile class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_LOG_FILE_H
#define CEDAR_AUX_LOG_FILE_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES
#include "src/Namespace.h"

// SYSTEM INCLUDES
#include <fstream>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::LogFile : public std::ofstream
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  LogFile(const std::string& logFileName, const std::string& logFilePath = CEDAR_LOG_DIR);
  LogFile(const char* pLogFileName, const char* pLogFilePath = CEDAR_LOG_DIR);

  //!@brief Destructor
  virtual ~LogFile(void);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void init(const std::string& logFilePath, const std::string& logFileName);
  void addSeparatorLine(void);
  void addTimeStamp(void);

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
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::LogFile

#endif // CEDAR_AUX_LOG_FILE_H

