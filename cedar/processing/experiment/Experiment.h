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

    File:        Experiment.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_proc_EXPERIMENT_EXPERIMENT_H
#define CEDAR_proc_EXPERIMENT_EXPERIMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/Experiment.fwd.h"
#include "cedar/processing/experiment/ExperimentController.fwd.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@brief
 */
class cedar::proc::experiment::Experiment : public QObject, public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Enumeration class for the different types of reset.
   */
  class ResetType
  {
    public:
      //! Type id of the enumeration.
      typedef cedar::aux::EnumId Id;
      //! Pointer type of the base class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(None, "None"));
        mType.type()->def(cedar::aux::Enum(Wait, "Wait"));
        mType.type()->def(cedar::aux::Enum(Reset, "Reset"));
        mType.type()->def(cedar::aux::Enum(Reload, "Reload"));
      }

      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      static const Id None = 0;

      static const Id Wait = 1;

      static const Id Reset = 2;

      static const Id Reload = 3;

    private:
      static cedar::aux::EnumType<ResetType> mType;
  };

  /*! Enumeration class for the different compare methods.
   */
public:
  class CompareMethod
  {
    public:
      //! Type id of the enumeration.
      typedef cedar::aux::EnumId Id;
      //! Pointer type of the base class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(Greater, ">"));
        mType.type()->def(cedar::aux::Enum(Lower, "<"));
        mType.type()->def(cedar::aux::Enum(Equal, "="));
      }
      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }
      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mType.type();
      }
    public:
     static const Id Greater = 0;
     static const Id Lower = 1;
     static const Id Equal = 2;

    private:
      static cedar::aux::EnumType<CompareMethod> mType;
  };

  //!@brief a parameter for action sequence objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::proc::experiment::ActionSequence> ActionSequencelListParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ActionSequencelListParameter);
  //!@endcond
private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  Experiment(cedar::proc::NetworkPtr network);
  //!@brief Destructor
  ~Experiment();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  const std::string& getFileName() const;
  void setFileName(const std::string& filename);

  unsigned int getRepetitions() const;
  void setRepetitions(unsigned int repetitions);
  void run();
  void cancel();
  void addActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence);
  void removeActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence);
  std::vector<cedar::proc::experiment::ActionSequencePtr> getActionSequences();
  void startNetwork();
  void stopNetwork(ResetType::Id reset = ResetType::Reset);
  void executeAcionSequences(bool initial = false);
  bool isOnInit();
  bool hasStopped();
  void onInit(bool status);

  std::vector<std::string> getAllSteps();
  std::vector<std::string> getStepParameters(std::string step, const std::vector<std::string>& allowedTypes = std::vector<std::string>());
  std::vector<std::string> getStepValues(std::string step, cedar::proc::DataRole::Id role = cedar::proc::DataRole::OUTPUT);
  cedar::aux::ParameterPtr getStepParameter(std::string step, std::string parameter);
  cedar::aux::DataPtr getStepValue(std::string step, std::string value, cedar::proc::DataRole::Id role = cedar::proc::DataRole::OUTPUT);

  unsigned int getTrialNumber();
  //override
  void writeConfiguration(cedar::aux::ConfigurationNode& root);


signals:
  void experimentStopped(bool stopped);
  void trialNumberChanged(int trialNumber);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mFileName;
  cedar::aux::UIntParameterPtr _mRepetitions;
  cedar::proc::NetworkPtr mNetwork;

  //! Used for starting all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStartThreadsCaller;

  //! Used for stopping all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStopThreadsCaller;


  ActionSequencelListParameterPtr _mActionSequences;
  unsigned int mRepetitionCounter;
  bool mInit;
  bool mStopped;

}; // class cedar::proc::experiment::Experiment

#endif // CEDAR_proc_EXPERIMENT_EXPERIMENT_H

