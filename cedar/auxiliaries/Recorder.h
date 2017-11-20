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

    File:        Recorder.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2013 07 01

    Description: Singleton class for recording and saving the data generated in a network.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_RECORDER_H
#define CEDAR_AUX_RECORDER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/DataSpectator.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATION
#include "cedar/auxiliaries/Recorder.fwd.h"

// SYSTEM INCLUDES
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <QReadWriteLock>
#include <QTime>
#include <fstream>


/*!@brief Singleton class for recording and saving the data registered to disk.
 */
class cedar::aux::Recorder : public cedar::aux::LoopedThread
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  // uses singleton template.
  friend class cedar::aux::Singleton<Recorder>;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The private constructor for singleton usage.
  Recorder();
  
public:
  //!@brief The Destructor.
  ~Recorder();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Can be called to register a DataPtr to be recorded.
   *
   *             It will create a new thread which will record the data in the specified interval.
   *             Recording will only start when cedar::aux::Recorder::start() is called. If name is already
   *             exist in the Recorder, the DataPtr will not be registered.
   *
   */
  void registerData(cedar::aux::ConstDataPtr toSpectate, cedar::unit::Time recordInterval, const std::string& name);

  //!@brief Used to unregister a DataPtr to stop him from being recorded.
  void unregisterData(const std::string& name);

  //!@brief Used to unregister a DataPtr to stop him from being recorded.
  void unregisterData(cedar::aux::ConstDataPtr);

  //!@brief Unregister all DataPtr.
  void clear();

  //!@brief Saves the records in a special folder just for one session
  void setSubfolder(const std::string& subfolderName);

  //!brief Takes a snapshot of all registered DataPtr
  void takeSnapshot();

  /*!@brief Sets the directory the recorded data will be written to.
   *            If @param name is a full path the function will extract the stem of the filename.
   */
  void setRecordedProjectName(const std::string& name);

  //!@brief Returns the directory the recorded data will be written to.
  const std::string& getRecorderProjectName();

  //!@brief Gets the OutputDirectory
  const std::string& getOutputDirectory() const;

  /*!@brief Change the record interval of 'name'
   *          If 'name' is not a registered it will throw an UnknownNameExeption.
   */
  void setRecordIntervalTime(const std::string& name, cedar::unit::Time recordInterval);

  /*!@brief Returns the specified record interval of the DataPtr 'name'.
   */
  cedar::unit::Time getRecordIntervalTime(const std::string& name) const;

  /*!@brief Returns the specified record interval of the DataPtr.
   */
  cedar::unit::Time getRecordIntervalTime(cedar::aux::ConstDataPtr data) const;

  //!@brief Checks if a DataPtr with a certain name is registered.
  bool isRegistered(const std::string& name) const;

  //!@brief Checks if a DataPtr with a certain DataPtr is registered.
  bool isRegistered(cedar::aux::ConstDataPtr data) const;

  //!@brief Changes the name of the DataPtr.
  void renameRegisteredData(cedar::aux::ConstDataPtr data, const std::string& newName);

  //!@brief Returns the current time formattedin a string
  std::string getTimeStamp();

  //!@brief Returns all registered DataPtr by name and their record interval
  std::map<std::string, cedar::unit::Time> getRegisteredData() const;

  //!@brief Starts all threads.
  void startAllRecordings();

  //!@brief Stops all threads.
  void stopAllRecordings();

  //!@brief Removes all threads.
  void removeAllRecordings();

  //! Returns true if any data is set to be recorded.
  bool hasDataToRecord() const;

  //! Returns the chosen serialization mode for writing data.
  cedar::aux::SerializationFormat::Id getSerializationMode() const;

  //! Sets the serialization mode for writing data.
  void setSerializationMode(cedar::aux::SerializationFormat::Id mode);

signals:
  //! Emitted whenver data is added or removed.
  void recordedDataChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Calls write in the specified interval.
  void step(cedar::unit::Time time);

  //!@brief Creates a new Output directory
  void createOutputDirectory();

  /*!@brief Starts the recorder thread.
  *
  *              By calling start, every registered observer thread will automatically be started.
  *              Additionally all files will be created and filled with headers.
  */
  void prepareStart();

  /*!@brief Stops the recorder thread.
   *
   *              By calling stop, the calling thread waits until all recorded data has been written to disk.
   */
  void processQuit();

//--------------------------------------------------------------------------------------------------------------------
// members
//--------------------------------------------------------------------------------------------------------------------

protected:
  // none yet

private:
  //!@brief The registered DataSpectators.
  std::map<std::string, cedar::aux::DataSpectatorPtr> mDataSpectators;
  QReadWriteLock* mpListLock;

  //!@brief The output directory.
  std::string mOutputDirectory;

  //!@brief The name of the project that will be recorded
  std::string mProjectName;

  std::string mSubFolder;
};



#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace aux
  {
    CEDAR_INSTANTIATE_AUX_TEMPLATE(cedar::aux::Singleton<cedar::aux::Recorder>);
    //! a singleton for the recorder
    typedef cedar::aux::Singleton<cedar::aux::Recorder> RecorderSingleton;
  }
}

#endif // CEDAR_AUX_RECORDER_H
