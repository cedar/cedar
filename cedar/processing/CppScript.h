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

    File:        CppScript.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 20

    Description: Header file for the class cedar::proc::CppScript.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_CPP_SCRIPT_H
#define CEDAR_PROC_CPP_SCRIPT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/DeclarationManagerTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/processing/CppScript.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/auxiliaries/CallFunctionInThread.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif
#include <string>


/*!@brief Derive this class to create scripts that can be executed on architectures from the cedar gui.
 */
class cedar::proc::CppScript : public QObject,
                               public cedar::aux::NamedConfigurable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CppScript();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Prepares the script and then calls the run() function.
   */
  void callRun();

  //! Returns whether requestStop() was called.
  bool stopRequested() const;

  //! Returns true if the script is running.
  bool isRunning() const;

  //! Returns the type of the script (i.e., what must be used with the factory manager to instantiate it).
  std::string getType() const;

  //! Sets the group that the script is run on.
  void setGroup(cedar::proc::GroupPtr group);

  /*! Returns the group on which the script operates. Scripts should not store this as a strong reference, as this would
   *  lead to cyclic referencing (the script points to the group, the group to the script) and thus a memory leak.
   */
  cedar::proc::GroupPtr getGroup() const;

  //! Returns the current status of the script.
  std::string getStatus() const;

public slots:
  /*! Ask the script nicely to stop what it is doing.
   *
   * Scripts that run for a long time should periodically check with
   */
  void requestStop();

signals:
  //! signal that is emitted if a script is started
  void scriptStarted();

  //! signal that is emitted if a script is stopped
  void scriptStopped();

  //! The status message of the script changed.
  void statusChanged(QString);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void setStatus(const std::string& status);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Method in which child classes can provide their functionality.
   *
   * @remarks This method is called in the GUI context. Thus, it should not perform any long operations directly.
   *          Typically, the code in this method should start a thread or open qt widgets.
   */
  virtual void run() = 0;

  void setStopRequested(bool requested);

  void emitScriptStartedSignal();

private slots:
  void threadStopped();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::GroupWeakPtr mGroup;

  cedar::aux::LockableMember<bool> mStopRequested;

  cedar::aux::LockableMember<std::string> mStatus;

  cedar::aux::CallFunctionInThreadPtr mRunCaller;

  boost::signals2::scoped_connection mConnectionThreadStarted;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::CppScript


namespace cedar
{
  namespace proc
  {
    //! A manager for c++ script declarations
    typedef
      cedar::aux::DeclarationManagerTemplate<cedar::proc::CppScriptPtr>
      CppScriptDeclarationManager;

    //! Sinleton of the c++ script declaration manager.
    typedef
      cedar::aux::Singleton<cedar::proc::CppScriptDeclarationManager>
      CppScriptDeclarationManagerSingleton;

    //! A manager for element factories
    typedef
      cedar::aux::FactoryManager<cedar::proc::CppScriptPtr>
      CppScriptFactoryManager;
  }
}

//!@cond SKIPPED_DOCUMENTATION
CEDAR_PROC_SINGLETON(CppScriptDeclarationManager);
CEDAR_PROC_SINGLETON(CppScriptFactoryManager);
//!@endcond

#endif // CEDAR_PROC_CPP_SCRIPT_H

