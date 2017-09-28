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

    File:        DataSpectator.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2013 07 01

    Description: The Recorder uses this class to observe the registered DataPtr.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_DATA_SPECTATOR_H
#define CEDAR_AUX_DATA_SPECTATOR_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/DataSpectator.fwd.h"
#include "cedar/auxiliaries/Recorder.fwd.h"

// SYSTEM INCLUDES
#include <QTime>
#include <string>
#include <fstream>
#include <list>

/*!@brief The Recorder uses this class to observe the registered DataPtr.
 *        This class copies the observed DataPtr in each time step and stores the copy in a queue together with time
 *        stamp. The recorder can access this queue and write the elements to disk.
 */
class cedar::aux::DataSpectator : public cedar::aux::LoopedThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  /* Recorder should be the only class that could create a DataSpectator. Thus the constructor is private,
   * Recorder must be a friend class.
   */
  friend class cedar::aux::Recorder;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief A data structure to store all the DataPtr with time stamp (in ms) in a list.
  struct RecordData
  {
    cedar::unit::Time mRecordTime;
    cedar::aux::DataPtr mData;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  ~DataSpectator();
private:
  /*!@brief The private Constructor. Can only called by friend classes such as cedar::aux::Recorder. name is a unique
   * name for this DataPtr, so a file with this name can be created in the output dictionary.
   */
  DataSpectator(cedar::aux::ConstDataPtr toSpectate, cedar::unit::Time recordInterval, const std::string& name);


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Gets the DataPtr.
  cedar::aux::ConstDataPtr getData() const;

  //!@brief Gets the unique Name.
  const std::string& getName() const;

  //!@brief Sets the unique Name.
  void setName(const std::string& name);

  //!@brief Returns the record interval for this DataPtr.
  cedar::unit::Time getRecordIntervalTime() const;

  //!@brief Makes a snapshot of the data.
  void makeSnapshot();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Calls record() each time step. Inherited from Looped Thread.
  void step(cedar::unit::Time time);

  //!@brief Writes the header for the DataPtr to the output file.
  void writeHeader(cedar::aux::SerializationFormat::Id mode);

  //!@brief Writes the first element of the RecordData queue to the output file.
  void writeFirstRecordData(cedar::aux::SerializationFormat::Id mode);

  //!@brief Writes the whole RecordData queue to the output file.
  void writeAllRecordData(cedar::aux::SerializationFormat::Id mode);

  //!@brief Copies the DataPtr and stores it as new RecordData in the queue.
  void record();

  //!@brief Starts the DataSpectator: Before starting the output file will be opened and the header be written.
  void prepareStart();

  //!@brief Stops the DataSpactator. Before stopping all RecordDatas in the queue will be written to disk.
  void processQuit();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

protected:
  // none yet

private:
  //!@brief The pointer to the Data.
  cedar::aux::ConstDataPtr mData;

  //!@brief The path for storing the recorded Data. This is the full output path including the file name.
  std::string mOutputPath;

  //!@brief The output stream to mOutputPath.
  std::ofstream mOutputStream;

  //!@brief The Lock for mOutputStream.
  QReadWriteLock* mpOfstreamLock;

  //!@brief This queue will be write to disk every time step is called.
  std::list<RecordData> mDataQueue;

  //!@brief Locks mDataQueue.
  QReadWriteLock* mpQueueLock;

  //!@brief Unique name of the DataPtr.
  std::string mName;
};

#endif // CEDAR_AUX_DATASPECTATOR_H_
