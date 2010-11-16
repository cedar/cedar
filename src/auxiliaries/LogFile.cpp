/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        LogFile.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 27

 ----- Description: Header for the @em cedar::aux::LogFile class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "LogFile.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace cedar::aux;
using namespace boost::posix_time;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
LogFile::LogFile(const std::string& logFileName, const std::string& logFilePath)
{
  init(logFilePath, logFileName);
}

LogFile::LogFile(const char* pLogFileName, const char* pLogFilePath)
{
  init(std::string(pLogFilePath), std::string(pLogFileName));
}

//! destructor
LogFile::~LogFile()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void LogFile::init(const std::string& logFilePath, const std::string& logFileName)
{
  const std::string log_path_and_file_name = logFilePath + "/" + logFileName;

  open(log_path_and_file_name.c_str(), ios_base::in | ios_base::app);
}

void LogFile::addSeparatorLine(void)
{
  *this << std::string(120, '-') << "\n";
}

void LogFile::addTimeStamp(void)
{
  ptime time_stamp = second_clock::local_time();

  if (!time_stamp.is_not_a_date_time())
  {
    *this << to_simple_string(time_stamp) << " ";
  }
}
