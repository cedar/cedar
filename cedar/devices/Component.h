/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/DeclarationManagerTemplate.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/auxiliaries/LockableMember.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/devices/Channel.h"

// FORWARD DECLARATIONS
#include "cedar/devices/Component.fwd.h"
#include "cedar/devices/ComponentSlot.fwd.h"
#include "cedar/auxiliaries/Data.fwd.h"

// SYSTEM INCLUDES
#include <opencv/cv.h>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
  #include <boost/function.hpp>
#endif
#include <QReadWriteLock>
#include <QMutex>
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
  typedef boost::function< void () > NoCommandFunctionType;
  typedef boost::function< cv::Mat (cedar::unit::Time, cv::Mat) > TransformationFunctionType;
  typedef unsigned int                      ComponentDataType;
  typedef std::map< ComponentDataType, cedar::aux::MatDataPtr > BufferDataType;
  typedef boost::function< cv::Mat() >      ControllerCallback;

private:
  typedef std::map< ComponentDataType, TransformationFunctionType > InnerTransformationHookContainerType;
  typedef std::map< ComponentDataType, InnerTransformationHookContainerType > TransformationHookContainerType;

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::dev::ComponentSlot;

  //--------------------------------------------------------------------------------------------------------------------
  // exceptions
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Exception that is thrown when a buffer is initialized more than once.
  class AlreadyInitializedException : public cedar::aux::ExceptionBase {};

  //! Exception that is thrown when a function that requires the component to be stopped is called during running.
  class AlreadyCommunicatingException : public cedar::aux::ExceptionBase {};

  //! Exception that is thrown when a type that is not installed is requested.
  class TypeNotFoundException : public cedar::aux::NotFoundException {};

  //! Exception that is thrown when a hook is set that already exists.
  class DuplicateTypeException : public cedar::aux::DuplicateIdException {};

  //! Exception that is thrown when a hook is set that already exists.
  class DuplicateHookException : public cedar::aux::DuplicateIdException {};

  //! Exception that is thrown when a transformation hook is set that already exists.
  class DuplicateTransformationHookException : public DuplicateHookException {};

  //! Exception that is thrown when a dimensionality is not provided.
  class DimensionalityNotSetException : public cedar::aux::ExceptionBase {};

  //! Exception that is thrown when a hook is not found.
  class HookNotFoundException : public cedar::aux::NotFoundException {};

  //! Exception that is thrown when the device type could not be guessed.
  class CouldNotGuessDeviceTypeException : public cedar::aux::ExceptionBase {};

  //! Exception that is thrown when the command type could not be guessed.
  class CouldNotGuessCommandTypeException : public cedar::aux::ExceptionBase {};

  //! Thrown when a group name already exists.
  class DuplicateGroupNameException : public cedar::aux::DuplicateNameException {};

  //! Thrown when a group name cannot be found.
  class GroupNameNotFoundException : public cedar::aux::NotFoundException {};

  //! Exception that is thrown when there are no submit hooks even though they are needed.
  class NoSubmitHooksException : public cedar::aux::ExceptionBase {};

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class DataCollection;
  CEDAR_GENERATE_POINTER_TYPES(DataCollection);

  class CommandDataCollection;
  CEDAR_GENERATE_POINTER_TYPES(CommandDataCollection);

  class MeasurementDataCollection;
  CEDAR_GENERATE_POINTER_TYPES(MeasurementDataCollection);

  struct ControllerCollection {
    cedar::dev::Component::ComponentDataType  mBufferType;
    cedar::dev::Component::ControllerCallback mCallback;
  };
  CEDAR_GENERATE_POINTER_TYPES(ControllerCollection);

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
  static void handleCrash(); // called from the IDE
  static void brakeNowAllComponents(); 
  static void startBrakingAllComponents(); 
  static bool anyComponentsRunning();

  // replaced by startCommunication()
  CEDAR_DECLARE_DEPRECATED(void start());

  // replaced by stopCommunication()
  CEDAR_DECLARE_DEPRECATED(void stop());
  CEDAR_DECLARE_DEPRECATED(bool isRunning());
  bool isCommunicating() const;

  CEDAR_DECLARE_DEPRECATED(void startTimer(double d));
  CEDAR_DECLARE_DEPRECATED(void stopTimer());

  //!@ check if these functions have to be exposed at all (may at least be changed to protected visibility)
  void setStepSize(const cedar::unit::Time& time);
  void setIdleTime(const cedar::unit::Time& time);
  void setSimulatedTime(const cedar::unit::Time& time);
  bool isRunningNolocking(); //@todo: rename to isCommunicatingNolocking() ?

  //! will we move
  virtual bool isReadyForCommands() const;
  virtual bool isReadyForMeasurements() const;
  

  cedar::unit::Time getCommunicationStepSize();

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
  cv::Mat integrateDevice(cedar::unit::Time dt, cv::Mat data, ComponentDataType type);
  cv::Mat integrateDeviceTwice(cedar::unit::Time dt, cv::Mat data, ComponentDataType type1, ComponentDataType type2);
  cv::Mat differentiateDevice(cedar::unit::Time dt, cv::Mat data, ComponentDataType type);
  cv::Mat differentiateDeviceTwice(cedar::unit::Time dt, cv::Mat data, ComponentDataType type1, ComponentDataType type2);

  void processStart();

  void startCommunication(bool suppressUserSideInteraction = false);
  void stopCommunication();
  void destroyCommunication();

  void waitUntilCommunicated() const;

  void startBraking(); // non-blocking. will set a Controller that smoothly brakes
  void brakeNow();     // blocking. will try to instantly reduce velocity, high inertias possible
  void crashbrake();   // last-resort braking, may disconnect device or even break the robot

  //! Returns a list of all installed measurement types.
  std::set<ComponentDataType> getInstalledMeasurementTypes() const;

  //! Returns a list of all installed command types.
  std::set<ComponentDataType> getInstalledCommandTypes() const;

  //! Returns the data that contains the current measurements.
  cedar::aux::DataPtr getMeasurementData(const ComponentDataType &type);

  //! Returns the data that contains the current measurements.
  cedar::aux::ConstDataPtr getMeasurementData(const ComponentDataType &type) const;

  //! Returns the data that contains the commands that will be sent to the device.
  cedar::aux::DataPtr getDeviceCommandData(const ComponentDataType &type);

  //! Returns the data that contains the commands that will be sent to the device.
  cedar::aux::ConstDataPtr getDeviceCommandData(const ComponentDataType &type) const;

  cedar::aux::DataPtr getUserCommandData(const ComponentDataType &type);

  cedar::aux::ConstDataPtr getUserCommandData(const ComponentDataType &type) const;

  //! Returns the name for the given command.
  std::string getNameForCommandType(ComponentDataType type) const;

  //! Returns the command type associated with the given name.
  ComponentDataType getCommandTypeForName(const std::string& name) const;

  //! Returns the name for the given measurement.
  std::string getNameForMeasurementType(ComponentDataType type) const;

  //! Returns the measurement type associated with the given name.
  ComponentDataType getMeasurementTypeForName(const std::string& name) const;

  void applyDeviceCommandsAs(ComponentDataType type);

  //! Returns the dimensionality (size) of the given command type.
  unsigned int getCommandDimensionality(ComponentDataType type) const;

  void setUserSideCommandBuffer(ComponentDataType type, cv::Mat);

  //!@brief this function resets the internally used user command and allows to subsequently use a different type
  void clearUserCommand();


  //! Defines a new command group.
  void defineCommandGroup(const std::string& groupName);

  //! Lists all available command groups.
  std::vector<std::string> listCommandGroups() const;

  //! Adds a given command to the specified group.
  void addCommandTypeToGroup(const std::string& groupName, const ComponentDataType& commandType);

  //! Checks whether any command groups are defined for this component.
  bool hasCommandGroups() const;

  //! Returns the command types that are in the given command group.
  std::vector<ComponentDataType> getCommandsInGroup(const std::string& groupName) const;

  void clearController();
  void setController( ComponentDataType buffer, cedar::dev::Component::ControllerCallback fun );

  //! Checks if a new step measurements duration is available.
  bool hasLastStepMeasurementsDuration() const;

  //! Removes and returns the duration of the last step measurements call.
  cedar::unit::Time retrieveLastStepMeasurementsDuration();

  //! Checks if a new step measurements duration is available.
  bool hasLastStepCommandsDuration() const;

  //! Removes and returns the duration of the last step commands call.
  cedar::unit::Time retrieveLastStepCommandsDuration();

  //! Returns the error rate (number of communications failed / number of communications sent) for commands and measurements.
  void getCommunicationErrorRates(double& commands, double& measurements) const;

  //! Returns the last communication errors.
  std::vector<std::string> getLastCommandCommunicationErrors() const;

  //! Returns the last communication errors.
  std::vector<std::string> getLastMeasurementCommunicationErrors() const;

  //! even if the communication with the hardware is running in the background  you may suppress interaction with the user-side (if parts of your architecture is not ready, for example)
  void setSuppressUserInteraction(bool what);

  //! ist user side communication allowed? @setSuppressUserInteraction
  bool getSuppressUserInteraction() const;
  
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

  void installCommandType(ComponentDataType type, const std::string& name);
  void installMeasurementType(ComponentDataType type, const std::string& name);
  void installCommandAndMeasurementType(ComponentDataType type, const std::string& name);

  void setCommandDimensionality(ComponentDataType type, unsigned int dim);
  void setMeasurementDimensionality(ComponentDataType type, unsigned int dim);
  void setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim);

  void registerCommandHook(ComponentDataType type, CommandFunctionType fun);
  void registerMeasurementHook(ComponentDataType type, MeasurementFunctionType fun);
  void registerNoCommandHook(NoCommandFunctionType fun);
  void registerNotReadyForCommandHook(NoCommandFunctionType fun);
  void registerAfterCommandBeforeMeasurementHook(NoCommandFunctionType fun);

  void registerCommandTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun);
  void registerMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun);

  boost::signals2::connection registerStartCommunicationHook(boost::function<void ()> slot);

  void setUserSideCommandBufferIndex(ComponentDataType type, int index, double value);
  void setInitialUserSideCommandBuffer(ComponentDataType type, cv::Mat);

  cv::Mat getUserSideMeasurementBuffer(ComponentDataType type) const;
  double  getUserSideMeasurementBufferIndex(ComponentDataType type, int index) const;

  cv::Mat getPreviousDeviceSideMeasurementBuffer(ComponentDataType type) const;
  double  getPreviousDeviceSideMeasurementBufferIndex(ComponentDataType type, int index) const;

  void prepareComponentDestructAbsolutelyRequired();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  void resetComponent();

  void stepCommunication(cedar::unit::Time); 
  void stepCommandCommunication(cedar::unit::Time);
  void stepMeasurementCommunication(cedar::unit::Time);
  void stepAfterCommandBeforeMeasurementCommunication();

  void updateUserSideMeasurements();

  //!@brief checks whether a given command type conflicts with already set commands and throws an exception if this happens
  void checkExclusivenessOfCommand(ComponentDataType type);

  virtual bool applyBrakeController() = 0; // returning FALSE will allow re-try
  virtual bool applyBrakeNow() = 0;
  virtual bool applyCrashbrake();

  static void stepStaticWatchDog(cedar::unit::Time);

  void handleStopCommunicationNonBlocking();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::ChannelPtr mChannel;
  cedar::dev::ComponentSlotWeakPtr mSlot;

  MeasurementDataCollectionPtr mMeasurementData;
  CommandDataCollectionPtr mCommandData;

  //! the Device-thread's wrapper
  std::unique_ptr<cedar::aux::LoopFunctionInThread> mCommunicationThread;

  cedar::aux::LockableMember<std::map<ComponentDataType, CommandFunctionType> > mSubmitCommandHooks;
  cedar::aux::LockableMember<std::map<ComponentDataType, MeasurementFunctionType> > mRetrieveMeasurementHooks;

  cedar::aux::LockableMember< NoCommandFunctionType > mNoCommandHook;
  cedar::aux::LockableMember< NoCommandFunctionType > mNotReadyForCommandHook;
  cedar::aux::LockableMember< NoCommandFunctionType > mAfterCommandBeforeMeasurementHook;
  boost::signals2::signal<void ()> mStartCommunicationHook;

  boost::optional<ComponentDataType> mDeviceCommandSelection;

  cedar::aux::LockableMember<std::set<ComponentDataType>> mUserCommandUsed;

  cedar::aux::LockableMember<ControllerCollectionPtr> mController;

  cedar::aux::LockableMember<boost::optional<cedar::unit::Time> > mLastStepMeasurementsTime;
  cedar::aux::LockableMember<boost::optional<cedar::unit::Time> > mLastStepCommandsTime;

  mutable QMutex mGeneralAccessLock;

  bool mDestructWasPrepared; // helper bool

  //! Integration time that is lost due to skipping stepCommunication calls.
  cedar::unit::Time mLostTime;

  static std::map< cedar::dev::Component*, boost::posix_time::ptime > mRunningComponentInstancesAliveTime;
  static std::map< cedar::dev::Component*, boost::posix_time::ptime > mRunningComponentInstancesStartTime;
  static std::unique_ptr<cedar::aux::LoopFunctionInThread> mWatchDogThread;

  unsigned int mTooSlowCounter;
  unsigned int mNotReadyForCommandsCounter;

  bool mSuppressUserInteraction;

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

    //!@brief The singleton object of the ComponentManager.
//    typedef cedar::aux::Singleton<cedar::dev::ComponentManager> ComponentManagerSingleton;

    // this will be used for managing declarations; we still have to define a declaration class (see below)
   typedef cedar::aux::DeclarationManagerTemplate<cedar::dev::ComponentPtr> ComponentDeclarationManager;

   typedef cedar::aux::Singleton<cedar::dev::ComponentDeclarationManager> ComponentDeclarationManagerSingleton;

   typedef cedar::aux::FactoryManager<cedar::dev::ComponentPtr> ComponentFactoryManager;
  }
}
CEDAR_DEV_SINGLETON(ComponentManager);
// this makes sure the singletons work properly on windows (across DLLs)
CEDAR_DEV_SINGLETON(ComponentDeclarationManager);
CEDAR_DEV_SINGLETON(ComponentFactoryManager);

#endif // CEDAR_DEV_COMPONENT_H
