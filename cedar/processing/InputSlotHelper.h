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

    File:        InputSlotHelper.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 09 03

    Description: Header file for the class cedar::proc::InputSlotHelper.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_INPUT_SLOT_HELPER_H
#define CEDAR_PROC_INPUT_SLOT_HELPER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/MatData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/InputSlotHelper.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
  #include <boost/optional.hpp>
#endif // Q_MOC_RUN
#include <vector>

namespace cedar
{
  namespace proc
  {
    /*!@brief an abstract output relation
     */
    template <typename DataType>
    class OutputRelation
    {
    private:
      CEDAR_GENERATE_POINTER_TYPES(DataType);
    public:
      //! constructor
      OutputRelation(cedar::proc::DataSlotPtr target)
      :
      mTarget(target)
      {
        CEDAR_DEBUG_ASSERT(this->mTarget);
      }

      //! Updates the attached slot.
      void update(ConstDataTypePtr source, cedar::proc::Connectable* pConnectable)
      {
        bool emit_change_signal = this->assign(source, this->mTarget->getData());

        if (emit_change_signal)
        {
          pConnectable->emitOutputPropertiesChangedSignal(this->mTarget->getName());
        }
      }

    protected:
      /*! Applies the relation. Note, that source can be a nullptr.
       *
       * @return True if an output properties changed signal should be emitted, false otherwise.
       */
      virtual bool assign(ConstDataTypePtr source, cedar::aux::DataPtr target) = 0;

    private:
      cedar::proc::DataSlotPtr mTarget;
    };

    /*!@brief An output relation copying the size and type of a source matrix.
     */
    class CopyMatrixProperties : public OutputRelation<cedar::aux::MatData>
    {
    public:
      //! constructor
      CopyMatrixProperties(cedar::proc::DataSlotPtr target)
      :
      OutputRelation<cedar::aux::MatData>(target)
      {
      }

      //! If specified, this type will override the type of the source matrix.
      void setTypeOverride(int cvType)
      {
        this->mTypeOverride = cvType;
      }

    protected:
      //! assigns a matrix to target given source; returns true if size or type changed
      bool assign(cedar::aux::ConstMatDataPtr source, cedar::aux::DataPtr target)
      {
        auto mat_target = boost::dynamic_pointer_cast<cedar::aux::MatData>(target);
        CEDAR_DEBUG_ASSERT(mat_target);

        const cv::Mat& target_mat = mat_target->getData();

        cv::Mat clone;
        if (source)
        {
          const auto& source_mat = source->getData();
          int type = source_mat.type();
          if (this->mTypeOverride)
          {
            type = this->mTypeOverride.get();
          }

          clone = cv::Mat(source_mat.dims, source_mat.size, type, cv::Scalar(0));
        }

        bool changed = false;
        if (target_mat.type() != clone.type() || target_mat.size != clone.size)
        {
          changed = true;
        }

        mat_target->setData(clone);

        return changed;
      }

    private:
      boost::optional<int> mTypeOverride;
    };

    CEDAR_GENERATE_POINTER_TYPES(CopyMatrixProperties);
  }
}

/*!@brief automatizes caching of input data in matrices. can do more!
 *
 * @remarks This functionality is currently in the prototype stage.
 */
template <typename DataType>
class cedar::proc::InputSlotHelper
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  CEDAR_GENERATE_POINTER_TYPES(DataType);

  typedef cedar::proc::InputSlotHelper<DataType> SelfType;

  typedef cedar::proc::OutputRelation<DataType> OutputRelationType;
  CEDAR_GENERATE_POINTER_TYPES(OutputRelationType);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  InputSlotHelper(cedar::proc::Connectable* pConnectable, const std::string& name, bool mandatory = true)
  :
  mpConnectable(pConnectable)
  {
    CEDAR_ASSERT(this->mpConnectable != nullptr);
    this->mSlot = this->mpConnectable->declareInput(name, mandatory);

    this->mChangedConnection =
        this->mpConnectable->connectToInputConnectionChangedSignal(boost::bind(&SelfType::dataChanged, this));
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! is set?
  bool isSet() const
  {
    return static_cast<bool>(this->mCachedData);
  }

  //! get cached data type
  ConstDataTypePtr get() const
  {
    return this->mCachedData;
  }

  //! get cached data
  const typename DataType::DataType& getData() const
  {
    return this->mCachedData->getData();
  }

  //! get slot
  cedar::proc::DataSlotPtr getSlot() const
  {
    return this->mSlot;
  }

  //! adds output relation
  void addOutputRelation(OutputRelationTypePtr relation)
  {
    mOutputRelations.push_back(relation);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void dataChanged()
  {
    this->mCachedData = boost::dynamic_pointer_cast<ConstDataType>(this->mSlot->getData());

    for (auto relation : this->mOutputRelations)
    {
      relation->update(this->mCachedData, this->mpConnectable);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::Connectable* mpConnectable;

  cedar::proc::DataSlotPtr mSlot;

  ConstDataTypePtr mCachedData;

  boost::signals2::scoped_connection mChangedConnection;

  std::vector<OutputRelationTypePtr> mOutputRelations;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::InputSlotHelper

#endif // CEDAR_PROC_INPUT_SLOT_HELPER_H

