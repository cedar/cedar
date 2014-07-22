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

    File:        Component.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Abstract component of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_COMPONENT_H
#define CEDAR_DEV_COMPONENT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/auxiliaries/LockableMember.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/devices/Channel.h"
#include "cedar/devices/namespace.h"

// FORWARD DECLARATDeviceNS
#include "cedar/auxiliaries/Data.fwd.h"

// SYSTEM INCLUDES
#include <opencv/cv.h>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <QReadWriteLock>
#include <QObject>

#include <vector>
#include <map>

/*!@brief Base class for components of robots.
 */
class cedar::dev::Component : public QObject,
                              public cedar::aux::NamedConfigurable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef boost::function< void (cv::Mat) > CommandFunctionType;
  typedef boost::function< cv::Mat () >     MeasurementFunctionType;
  typedef boost::function< cv::Mat (cv::Mat) > TransformationFunctionType;
  typedef unsigned int                      ComponentDataType;
  typedef std::map< ComponentDataType, cedar::aux::MatDataPtr > BufferDataType;
private:
  typedef std::map< ComponentDataType, TransformationFunctionType > InnerTransformationHookContainerType;
  typedef std::map< ComponentDataType, InnerTransformationHookContainerType > TransformationHookContainerType;

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::dev::ComponentSlot;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:

private:
  class DataCollection;
  CEDAR_GENERATE_POINTER_TYPES(DataCollection);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  Component();

  //!@brief Constructor taking an externally created channel
  Component(cedar::dev::ChannelPtr channel);

  //!@brief Destructor
  ~Component();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  CEDAR_DECLARE_DEPRECATED(void start());
  CEDAR_DECLARE_DEPRECATED(void stop());
  CEDAR_DECLARE_DEPRECATED(bool isRunning());
  CEDAR_DECLARE_DEPRECATED(void startTimer(double d));
  CEDAR_DECLARE_DEPRECATED(void stopTimer());

  //!@ check if these functions have to be exposed at all (may at least be changed to protected visibility)
  void setStepSize(const cedar::unit::Time& time);
  void setIdleTime(const cedar::unit::Time& time);
  void setSimulatedTime(const cedar::unit::Time& time);
  bool isRunningNolocking();

  cedar::unit::Time getDeviceStepSize();

  //!@brief Returns the channel associated with the component.
  inline cedar::dev::ChannelPtr getChannel() const
  {
    return mChannel;
  }

  inline void setChannel(cedar::dev::ChannelPtr channel)
  {
    this->mChannel = channel;
  }

  // utility Transformations
  cv::Mat integrateDevice(cv::Mat data, ComponentDataType type);
  cv::Mat integrateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2);
  cv::Mat differentiateDevice(cv::Mat data, ComponentDataType type);
  cv::Mat differentiateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2);

  void processStart();

  void startDevice();
  void stopDevice();

  //! Returns a list of all installed measurement types.
  std::vector<ComponentDataType> getInstalledMeasurementTypes() const;

  //! Returns a list of all installed command types.
  std::vector<ComponentDataType> getInstalledCommandTypes() const;

  //! Returns the data that contains the current measurements.
  cedar::aux::DataPtr getDeviceMeasurementData(const ComponentDataType &type);

  //! Returns the data that contains the commands that will be sent to the device.
  cedar::aux::DataPtr getDeviceCommandData(const ComponentDataType &type);

signals:
  void updatedUserMeasurementSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  inline cedar::dev::ComponentSlotWeakPtr getSlot()
  {
    return this->mSlot;
  }

  inline void setSlot(cedar::dev::ComponentSlotWeakPtr slot)
  {
    this->mSlot = slot;
  }

  void installCommandType(ComponentDataType type);
  void installMeasurementType(ComponentDataType type);
  void installCommandAndMeasurementType(ComponentDataType type);

  void setCommandDimensionality(ComponentDataType type, unsigned int dim);
  void setMeasurementDimensionality(ComponentDataType type, unsigned int dim);
  void setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim);

  void registerDeviceCommandHook(ComponentDataType type, CommandFunctionType fun);
  void registerDeviceMeasurementHook(ComponentDataType type, MeasurementFunctionType fun);

  void registerUserCommandTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun);
  void registerDeviceMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun);

  void setUserCommandBuffer(ComponentDataType type, cv::Mat);
  void setUserCommandBufferIndex(ComponentDataType type, int index, double value);
  void setInitialUserCommandBuffer(ComponentDataType type, cv::Mat);

  cv::Mat getUserMeasurementBuffer(ComponentDataType type) const;
  double  getUserMeasurementBufferIndex(ComponentDataType type, int index) const;

  cv::Mat getPreviousDeviceMeasurementBuffer(ComponentDataType type) const;
  double  getPreviousDeviceMeasurementBufferIndex(ComponentDataType type, int index) const;

  void applyDeviceCommandsAs(ComponentDataType type);


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  void resetComponent();

  void stepDevice(cedar::unit::Time); //!todo: make friend to LoopFunctionInThread
  void stepDeviceCommands(cedar::unit::Time);
  void stepDeviceMeasurements(cedar::unit::Time);

  void updateUserMeasurements();

  void lazyInitializeMember(cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr> >& lockableMember, boost::function<void (ComponentDataType)> initFun, ComponentDataType type);

  void resetUserCommandBufferUnlocked(ComponentDataType type);
  void lazyInitializeUserCommandBufferUnlocked(ComponentDataType type);
  void setUserCommandBufferUnlocked(ComponentDataType &type, cv::Mat);
  void setUserCommandIndexUnlocked(ComponentDataType &type, int index, double value);
  void setInitialUserCommandBufferUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getUserCommandBufferUnlocked(ComponentDataType &type) const;

  void resetUserMeasurementBufferUnlocked(ComponentDataType type);
  void resetPreviousDeviceMeasurementBufferUnlocked(ComponentDataType type);
  void lazyInitializeUserMeasurementBufferUnlocked(ComponentDataType type);
  void lazyInitializePreviousDeviceMeasurementBufferUnlocked(ComponentDataType type);
  void setUserMeasurementBufferUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getUserMeasurementBufferUnlocked(ComponentDataType &type) const;
  double  getUserMeasurementIndexUnlocked(ComponentDataType &type, int index) const;
  cv::Mat getPreviousDeviceMeasurementBufferUnlocked(ComponentDataType &type) const;
  double  getPreviousDeviceMeasurementIndexUnlocked(ComponentDataType &type, int index) const;

  void resetDeviceCommandBufferUnlocked(ComponentDataType type);
  void lazyInitializeDeviceCommandBufferUnlocked(ComponentDataType type);
  void setDeviceCommandBufferUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getDeviceCommandBufferUnlocked(ComponentDataType &type) const;

  void resetDeviceMeasurementBufferUnlocked(ComponentDataType type);
  void lazyInitializeDeviceMeasurementBufferUnlocked(ComponentDataType type);
  void setDeviceMeasurementBufferUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getDeviceMeasurementBufferUnlocked(ComponentDataType &type) const;


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::ChannelPtr mChannel;
  cedar::dev::ComponentSlotWeakPtr mSlot;

  DataCollectionPtr mMeasurementData;
  DataCollectionPtr mCommandData;

  //! the Device-thread's wrapper
  std::unique_ptr< cedar::aux::LoopFunctionInThread > mDeviceThread;

  std::map< ComponentDataType, CommandFunctionType > mSubmitCommandHooks;
  std::map< ComponentDataType, MeasurementFunctionType > mRetrieveMeasurementHooks;
  TransformationHookContainerType mCommandTransformationHooks;
  TransformationHookContainerType mMeasurementTransformationHooks;

  boost::optional<ComponentDataType> mDeviceCommandSelection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet
}; // class cedar::dev::Component


// Code for the component factory manager ==============================================================================

#include "cedar/auxiliaries/FactoryManager.h"


namespace cedar
{
  namespace dev
  {
    //!@brief The manager of all sigmoind instances
    typedef cedar::aux::FactoryManager<cedar::dev::ComponentPtr> ComponentManager;

    //!@brief The singleton object of the TransferFunctionFactory.
//    typedef cedar::aux::Singleton<cedar::dev::ComponentManager> ComponentManagerSingleton;
  }
}
CEDAR_DEV_SINGLETON(ComponentManager);

#endif // CEDAR_DEV_COMPONENT_H
