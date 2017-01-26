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

    File:        Parameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 01

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PARAMETER_H
#define CEDAR_AUX_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/IntrusivePtrBase.h"
#include "cedar/auxiliaries/LockType.h"
#include "cedar/auxiliaries/LockableMember.h"
#include "cedar/auxiliaries/LockerBase.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/Parameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QReadWriteLock>
#include <set>
#include <string>


/*!@brief Base class for all parameters.
 *
 *        This is the base class for all parameters that can be registered in the cedar::aux::Configurable class.
 *
 * @see   @ref ParametersConcept for a description of the parameters concept.
 */
class cedar::aux::Parameter : public QObject, public cedar::aux::IntrusivePtrBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Configurable;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! An RAII-based read-locker for parameters.
   *
   * @see cedar::aux::LockerBase
   */
  class ReadLocker : public cedar::aux::LockerBase
  {
  public:
    //! Constructor that takes a raw pointer.
    ReadLocker(cedar::aux::Parameter* parameter)
    :
    cedar::aux::LockerBase
    (
      boost::bind(&cedar::aux::Parameter::lockForRead, parameter),
      boost::bind(&cedar::aux::Parameter::unlock, parameter)
    )
    {
    }

    //! Constructor that takes a smart pointer.
    ReadLocker(cedar::aux::ParameterPtr parameter)
    :
    cedar::aux::LockerBase
    (
      boost::bind(&cedar::aux::Parameter::lockForRead, parameter),
      boost::bind(&cedar::aux::Parameter::unlock, parameter)
    )
    {
    }
  };
  CEDAR_GENERATE_POINTER_TYPES(ReadLocker);

  /*! An RAII-based write-locker for parameters.
   *
   * @see cedar::aux::LockerBase
   */
  class WriteLocker : public cedar::aux::LockerBase
  {
  public:
    //! Constructor that takes a raw pointer.
    WriteLocker(cedar::aux::Parameter* parameter)
    :
    cedar::aux::LockerBase
    (
      boost::bind(&cedar::aux::Parameter::lockForWrite, parameter),
      boost::bind(&cedar::aux::Parameter::unlock, parameter)
    )
    {
    }

    //! Constructor that takes a smart pointer.
    WriteLocker(cedar::aux::ParameterPtr parameter)
    :
    cedar::aux::LockerBase
    (
      boost::bind(&cedar::aux::Parameter::lockForWrite, parameter),
      boost::bind(&cedar::aux::Parameter::unlock, parameter)
    )
    {
    }
  };
  CEDAR_GENERATE_POINTER_TYPES(WriteLocker);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Parameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault = true);

  //!@brief Destructor
  virtual ~Parameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Set the owner of the parameter. Can only be called if the parameter doesn't have an owner yet.
  void setOwner(cedar::aux::Configurable *pOwner);

  //! Resets the owner of the parameter.
  void unsetOwner();

  //!@brief get the flag "read automatically"
  bool getReadAutomatically() const;
  //!@brief set the flag "read automatically"
  void setReadAutomatically(bool value);

  //!@brief return if this parameter has a default value
  bool getHasDefault() const;
  //!@brief set if this parameter has a default value
  void setHasDefault(bool value);

  //!@brief return if this parameter is constant
  bool isConstant() const;
  //!@brief set if this parameter is constant
  void setConstant(bool value);

  //!@brief set this parameter to a value, read from a configuration node
  virtual void readFromNode(const cedar::aux::ConfigurationNode& node) = 0;
  //!@brief write value to a configuration node
  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const = 0;
  //!@brief set parameter to default
  virtual void makeDefault() = 0;

  //!@brief return flag if this parameter is hidden (for GUI)
  bool isHidden() const;
  //!@brief set flag if this parameter is hidden (for GUI)
  void setHidden(bool hide);

  //! Whether the parameter is linked to other parameters.
  bool isLinked() const;

  //! Sets whether the parameter is linked to other parameters. This shouldn't be called by users.
  void setLinked(bool linked);

  /*!@brief   Marks this parameter as advanced.
   *
   *          Advanced parameters are those that usually don't need to be changed. This is mainly used by user interfaces
   *          to avoid cluttering lists with parameters that are rarely needed, or may need specific knowledge to be
   *          set to proper values.
   *
   * @remarks Currently, this should not be changed at runtime; call this once, right after you add the parameter to a
   *          configurable.
   */
  void markAdvanced(bool advanced = true)
  {
    this->mAdvanced = advanced;
  }

  //!@brief Returns whether this parameter is marked as advanced.
  bool isAdvanced() const
  {
    return this->mAdvanced;
  }

  //!@brief emit the value changed signal
  void emitChangedSignal();
  //!@brief emit the property changed signal
  void emitPropertyChangedSignal();

  //!@brief Returns the value of the @em mChanged flag.
  bool isChanged() const
  {
    return this->mChanged;
  }

  //!@brief Returns the name of the object.
  std::string getName() const;

  //!@brief Sets the name of the object to the given name.
  //!@param name New name of the object.
  void setName(const std::string& name);

  //!@brief Returns the lock associated with this parameter.
  inline QReadWriteLock* getLock()
  {
    return this->mpLock;
  }

  //!@brief Locks the parameter for writing.
  void lockForWrite() const;

  //!@brief Locks the parameter for writing.
  void lockForRead() const;

  //!@brief Unlocks the parameter.
  void unlock() const;

  //!@brief Appends all the locks needed to lock this parameter properly to the set.
  virtual void appendLocks(std::set<QReadWriteLock*>& locks) const;

  //!@brief Removes all the locks needed to lock this parameter properly from the set.
  virtual void removeLocks(std::set<QReadWriteLock*>& locks) const;

  //! Returns the owner of the parameter.
  cedar::aux::Configurable* getOwner() const
  {
    return this->mpOwner;
  }

  /*! @brief Adds a deprecated name to the parameter.
   *
   *         Values for this parameter can then be read under the deprecated name(s), but these names are no longer used
   *         to write the value. Also, a warning about this will be printed.
   *
   * @param  deprecatedName
   */
  void addDeprecatedName(const std::string& deprecatedName);

  /*! @brief If possible, copies the value from another parameter into this parameter.
   *
   * The default implementation always throws an exception.
   */
  virtual void copyValueFrom(cedar::aux::ConstParameterPtr other);

  /*!@brief Checks if this parameter can copy its value from the given one.
   */
  virtual bool canCopyFrom(cedar::aux::ConstParameterPtr other) const;

  //! This specifies whether or not the parameter can have configurable children.
  virtual bool canHaveConfigurableChildren() const;

  /*! Specifies whether the parameter has a single configurable child.
   *
   * Parameters that return true for this always have exactly one configurable child that is returned by
   * getConfigurableChild().
   */
  virtual bool hasSingleConfigurableChild() const;

  //! Returns the number of configurable children in the parameter.
  virtual size_t getNumberOfConfigurableChildren() const;

  //! Returns the configurable child with the given index.
  cedar::aux::ConfigurablePtr getConfigurableChild(size_t i);

  //! Returns the configurable child with the given index.
  cedar::aux::ConstConfigurablePtr getConfigurableChild(size_t i) const;

  /*! Returns the path component for configurable child i. This is an index used later to identify the child.
   *  The default implementation returns the number i, converted to a string.
   */
  virtual std::string childIndexToString(size_t i) const;

  /*! Returns the position of the child indexed by string str.
   */
  virtual size_t childStringToIndex(std::string str) const;

  /*! Returns the only configurable child of the parameter.
   */
  virtual cedar::aux::ConstConfigurablePtr getSingleConfigurableChild() const;

  /*! Returns the only configurable child of the parameter.
   */
  cedar::aux::ConfigurablePtr getSingleConfigurableChild();

public:
  //! Signal that is emitted whenever the name of the parameter changes. The first string is the old name, the second the new one.
  CEDAR_DECLARE_SIGNAL(NameChanged, void(const std::string&, const std::string&));

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief set the changed flag to a given state
  virtual void setChangedFlag(bool changed);

  /*! Should return the configurable child with the index i.
   *
   *  Parameters that can have configurable children should override this function along with
   *  getNumberOfConfigurableChildren and canHaveConfigurableChildren (always return true).
   *
   *  This method should always return the children in a fixed order that depends on the index i. How the children are
   *  indexed internally, however, is left to the paramter itself.
   */
  virtual cedar::aux::ConstConfigurablePtr retrieveConfigurableChild(size_t index) const;

signals:
  //!@brief a signal that is emitted each time the value of a parameter changes
  void valueChanged();

  //!@brief a signal that is emitted each time a characteristic of this parameter changes (for example vector size)
  void propertyChanged();

  //!@brief Signal that is emitted when mChanged is changed.
  void changedFlagChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The owner of this parameter, i.e., the object using it.
  cedar::aux::Configurable *mpOwner;

  //! The name of the parameter.
  cedar::aux::LockableMember<std::string> mName;

  //! Whether the parameter should be read automatically. If not, the user has to read it by hand.
  bool mAutoRead;

  //! Whether a default value should be set
  bool mHasDefault;

  //! Whether this parameter can be changed during runtime.
  bool mConstant;

  //! Whether this parameter is hidden. This is relevant, e.g., for the gui.
  bool mIsHidden;

  //! Flag that indicates whether this parameter was changed since the last file reading.
  bool mChanged;

  //! Flag that indicates whether this parameter is advanced.
  bool mAdvanced;

  bool mIsLinked;

  //! Type of the last lock.
  mutable cedar::aux::LOCK_TYPE mLastLockType;

  //! Lock for the parameter.
  mutable QReadWriteLock* mpLock;

}; // class cedar::aux::Parameter


#endif // CEDAR_AUX_PARAMETER_H
