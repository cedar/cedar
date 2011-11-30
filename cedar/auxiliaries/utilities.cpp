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

    File:        utilities.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 08 11

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#ifdef GCC
  // for name unmangling/undecorating
  #include <cxxabi.h>
  // for generating stack traces
  #include <execinfo.h>
#endif // GCC

#ifdef MSVC
  #include "cedar/auxiliaries/stringFunctions.h"

  #include <Windows.h>
  #include <Dbghelp.h>
  #pragma comment(lib, "Dbghelp.lib")
#endif // MSVC


std::string cedar::aux::unmangleName(const std::type_info& typeinfo)
{
  return cedar::aux::unmangleName(typeinfo.name());
}

std::string cedar::aux::unmangleName(const char* mangledName)
{
#ifdef GCC
  int status;
  char *realname = abi::__cxa_demangle(mangledName, 0, 0, &status);

  switch (status)
  {
    case 0:
      // result is ok
      break;

    case -1:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "A memory allocation error occurred.");
      break;

    case -2:
    {
      std::string mangled_name = mangledName;
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "The mangled name \"" + mangled_name + "\" is not a valid name under the C++ ABI mangling rules.");
      break;
    }

    case -3:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "One of the arguments to abi::__cxa_demangle is invalid.");
      break;

    default:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "An unknown error occurred.");
      break;
  }

  std::string result(realname);
  free(realname);
  return result;
#else // GCC
  std::string name(mangledName);
  if (name.find("class ") == 0 && name.size() > 6)
  {
    name = name.substr(6);
  }
  return name;
#endif // GCC
}

std::ostream& cedar::aux::operator<< (std::ostream& stream, const cedar::aux::StackTrace& trace)
{
  stream << "Backtrace generated with ";
#ifdef GCC
  stream << "GCC";
#elif defined MSVC
  stream << "MSVC";
#else
  stream << "unsupported compiler";
#endif // GCC
  stream << "." << std::endl;
  stream << "Backtrace contains " << trace.size() << " items:" << std::endl;

  for (size_t i = 0; i < trace.size(); ++i)
  {
    stream << "  [" << i << "] " << trace.at(i) << std::endl;
  }

  return stream;
}

std::ostream& cedar::aux::operator<< (std::ostream& stream, const cedar::aux::StackEntry& entry)
{
  stream << entry.mFile;
  if (!entry.mSymbol.empty())
  {
    stream << " (" << entry.mSymbol;
    if (!entry.mSymbolOffset.empty())
    {
      stream << " + " << entry.mSymbolOffset;
    }
    stream << ")";
  }
  if (!entry.mAddress.empty())
    stream << ": " << entry.mAddress;
  return stream;
}

void cedar::aux::StackEntry::setRawString(const std::string& rawString)
{
#ifdef GCC
  std::string tmp = rawString;
  size_t idx = rawString.find('(');
  if (idx == std::string::npos)
  {
    this->mFile = rawString;
    return;
  }
  this->mFile = tmp.substr(0, idx);
  tmp = tmp.substr(idx + 1);

  idx = tmp.find_first_of("+)");
  if (idx == std::string::npos)
    return;

  this->mSymbol = tmp.substr(0, idx);
  tmp = tmp.substr(idx + 1);
  try
  {
    this->mSymbol = cedar::aux::unmangleName(this->mSymbol.c_str());
  }
  catch (const cedar::aux::UnmanglingFailedException&)
  {
    // nothing to do, just use the mangled name.
  }

  idx = tmp.find(")");
  if (idx != std::string::npos)
  {
    this->mSymbolOffset = tmp.substr(0, idx);
    tmp = tmp.substr(idx + 1);
  }

  idx = tmp.find("[");
  size_t idx2 = tmp.find("]");
  if (idx != std::string::npos && idx2 != std::string::npos)
  {
    this->mAddress = tmp.substr(idx + 1, idx2 - idx - 1);
  }
#else
//#warning Implement for MSVC
#endif // GCC
}

cedar::aux::StackTrace::StackTrace()
{
#ifdef GCC
  // array for the backtraces
  void *array[10];
  size_t size;
  // array for the backtrace symbols
  char **strings;
  size_t i;

  // get the backtrace
  size = backtrace(array, 10);

  // transform the backtrace into symbols
  strings = backtrace_symbols(array, size);

  for (i = 0; i < size; i++)
  {
    cedar::aux::StackEntry entry;
    entry.setRawString(strings[i]);
    this->mStackTrace.push_back(entry);
  }

  free (strings);
#else
  SYSTEM_INFO sys_info; // we need this to determine the processor architecture.
  GetSystemInfo(&sys_info);
  DWORD machine_type;
  switch (sys_info.wProcessorArchitecture)
  {
  case PROCESSOR_ARCHITECTURE_AMD64:
    machine_type = IMAGE_FILE_MACHINE_AMD64;
    break;
  case PROCESSOR_ARCHITECTURE_IA64:
    machine_type = IMAGE_FILE_MACHINE_IA64;
    break;
  case PROCESSOR_ARCHITECTURE_INTEL:
    machine_type = IMAGE_FILE_MACHINE_I386;
    break;
  default:
    //!@todo Throw an exception.
    return;
  }
  STACKFRAME stack_frame;
  memset(&stack_frame, 0, sizeof(STACKFRAME));
  CONTEXT context_record;
  RtlCaptureContext(&context_record);
  stack_frame.AddrPC.Offset = context_record.Eip;
  stack_frame.AddrPC.Mode = AddrModeFlat;
  stack_frame.AddrFrame.Offset = context_record.Ebp;
  stack_frame.AddrFrame.Mode = AddrModeFlat;
  stack_frame.AddrStack.Offset = context_record.Esp;
  stack_frame.AddrStack.Mode = AddrModeFlat;

  // initialize  the stack_frame variable
  SymInitialize(GetCurrentProcess(), NULL, TRUE);
  while (StackWalk(machine_type, GetCurrentProcess(), GetCurrentThread(), &stack_frame, &context_record, NULL, NULL, NULL, NULL))
  {
    // Warning: ugly windows code ahead. Read at your own peril!
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
    memset(symbol, 0, sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR));
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;
    DWORD64 offset = 0;
    if (SymFromAddr(GetCurrentProcess(), stack_frame.AddrPC.Offset, &offset, symbol))
    {
      cedar::aux::StackEntry entry;
      entry.setSymbol(symbol->Name);
      //entry.setFile(stack_frame.
      //!@todo read out more stack information.
      const size_t line_buf_size = sizeof(IMAGEHLP_LINE) + MAX_SYM_NAME * sizeof(TCHAR);
      char line_buf[line_buf_size];
      memset(line_buf, 0, line_buf_size);
      PIMAGEHLP_LINE line = reinterpret_cast<PIMAGEHLP_LINE>(line_buf);
      line->SizeOfStruct = sizeof(IMAGEHLP_LINE);
      DWORD displacement = 0;
      if(SymGetLineFromAddr(GetCurrentProcess(), stack_frame.AddrPC.Offset, &displacement, line))
      {
        entry.setFile(std::string(line->FileName) + std::string(": Line ") + cedar::aux::toString(line->LineNumber));
      }
      //!@todo Proper error handling

      this->mStackTrace.push_back(entry);
    }
    else
    {
      DWORD last_error = GetLastError();
      LPVOID lpMsgBuf;
      LPVOID lpDisplayBuf;
      DWORD dw = GetLastError(); 

      FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | 
          FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          dw,
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
          (LPTSTR) &lpMsgBuf,
          0, NULL );
      //LPTSTR msg = (LPTSTR) lpMsgBuf;
      //!@todo: process error message
    }
  }
#endif // GCC
}

