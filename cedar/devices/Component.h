/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/namespace.h"

// FORWARD DECLARATIONS
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

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  Component();

  //!@brief Destructor
  ~Component();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  CEDAR_DECLARE_DEPRECATED(void start());
  CEDAR_DECLARE_DEPRECATED(void stop());
  CEDAR_DECLARE_DEPRECATED(void setStepSize(double));
  CEDAR_DECLARE_DEPRECATED(void setIdleTime(double));
  CEDAR_DECLARE_DEPRECATED(void setSimulatedTime(double));
  CEDAR_DECLARE_DEPRECATED(bool isRunning());
  CEDAR_DECLARE_DEPRECATED(void startTimer(double d));
  CEDAR_DECLARE_DEPRECATED(void stopTimer());
  unsigned int getIOStepSize();

  // utility Transformations
  cv::Mat integrateIO(cv::Mat data, ComponentDataType type);
  cv::Mat integrateIOTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2);
  cv::Mat differentiateIO(cv::Mat data, ComponentDataType type);
  cv::Mat differentiateIOTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2);

  void processStart();

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

  void startIO(); // todo: public?
  void stopIO(); // todo: public?

  void installCommandType(ComponentDataType type);
  void installMeasurementType(ComponentDataType type);
  void installCommandAndMeasurementType(ComponentDataType type);

  void setCommandDimensionality(ComponentDataType type, unsigned int dim);
  void setMeasurementDimensionality(ComponentDataType type, unsigned int dim);
  void setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim);

  void registerIOCommandHook(ComponentDataType type, CommandFunctionType fun);
  void registerIOMeasurementHook(ComponentDataType type, MeasurementFunctionType fun);

  void registerUserCommandTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun);
  void registerIOMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun);

  void submitUserCommand(ComponentDataType type, cv::Mat);
  void submitUserCommandIndex(ComponentDataType type, int index, double value);
  void submitInitialUserCommand(ComponentDataType type, cv::Mat);

  cv::Mat fetchUserMeasurement(ComponentDataType type) const;
  double  fetchUserMeasurementIndex(ComponentDataType type, int index) const;

  cv::Mat fetchLastIOMeasurement(ComponentDataType type) const;
  double  fetchLastIOMeasurementIndex(ComponentDataType type, int index) const;

  void restrictUserCommandsTo(ComponentDataType type);
  void applyIOCommandsAs(ComponentDataType type);


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void resetComponent();

  void loopIO(double); //!todo: make private and friend to LoopFunctionInThread
  void loopIOCommands(double);
  void loopIOMeasurements(double);

  void updateUserMeasurements();

  void lazyInitializeMember(cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr> >& lockableMember, boost::function<void (ComponentDataType)> initFun, ComponentDataType type);

  void resetUserCommandUnlocked(ComponentDataType type);
  void lazyInitializeUserCommandUnlocked(ComponentDataType type);
  void setUserCommandUnlocked(ComponentDataType &type, cv::Mat);
  void setUserCommandIndexUnlocked(ComponentDataType &type, int index, double value);
  void setInitialUserCommandUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getUserCommandUnlocked(ComponentDataType &type) const;

  void resetUserMeasurementUnlocked(ComponentDataType type);
  void resetLastIOMeasurementUnlocked(ComponentDataType type);
  void lazyInitializeUserMeasurementUnlocked(ComponentDataType type);
  void lazyInitializeLastIOMeasurementUnlocked(ComponentDataType type);
  void setUserMeasurementUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getUserMeasurementUnlocked(ComponentDataType &type) const;
  double  getUserMeasurementIndexUnlocked(ComponentDataType &type, int index) const;
  cv::Mat getLastIOMeasurementUnlocked(ComponentDataType &type) const;
  double  getLastIOMeasurementIndexUnlocked(ComponentDataType &type, int index) const;

  void resetIOCommandUnlocked(ComponentDataType type);
  void lazyInitializeIOCommandUnlocked(ComponentDataType type);
  void setIOCommandUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getIOCommandUnlocked(ComponentDataType &type) const;

  void resetIOMeasurementUnlocked(ComponentDataType type);
  void lazyInitializeIOMeasurementUnlocked(ComponentDataType type);
  void setIOMeasurementUnlocked(ComponentDataType &type, cv::Mat);
  cv::Mat getIOMeasurementUnlocked(ComponentDataType &type) const;


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::ComponentSlotWeakPtr mSlot;

  //! the IO-thread's wrapper
  std::unique_ptr< cedar::aux::LoopFunctionInThread > mIOThread;

  std::vector< ComponentDataType > mInstalledCommandTypes;
  std::vector< ComponentDataType > mInstalledMeasurementTypes;

  std::map< ComponentDataType, unsigned int > mInstalledCommandDimensions;
  std::map< ComponentDataType, unsigned int > mInstalledMeasurementDimensions;

  std::map< ComponentDataType, CommandFunctionType > mSubmitCommandHooks;
  std::map< ComponentDataType, MeasurementFunctionType > mRetrieveMeasurementHooks;
  TransformationHookContainerType mCommandTransformationHooks;
  TransformationHookContainerType mMeasurementTransformationHooks;

  // Cache for the user-interface
  cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr> > mUserSubmittedCommands;
  cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr > > mUserRetrievedMeasurements;
  cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr > > mLastIORetrievedMeasurements;

  decltype(mUserSubmittedCommands) mInitialUserSubmittedCommands;

  // Cache for the IO-interface
  cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr > > mIOSubmittedCommands;
  cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr > > mIORetrievedMeasurements;

  boost::optional<ComponentDataType> mIOCommandRestriction;
  boost::optional<ComponentDataType> mUserCommandRestriction;

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
