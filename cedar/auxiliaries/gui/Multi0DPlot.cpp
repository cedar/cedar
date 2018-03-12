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

    File:        Multi0DPlot.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 02 02

    Description: Source file for the class cedar::aux::gui::Multi0DPlot.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_PORTABLE

// CLASS HEADER
#include "cedar/auxiliaries/gui/Multi0DPlot.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/DiscreteMetric.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/LockerBase.h"
#include "cedar/auxiliaries/threadingUtilities.h"

// SYSTEM INCLUDES
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    using cedar::aux::MatData;
    using cedar::aux::gui::Multi0DPlot;

    typedef cedar::aux::gui::PlotDeclarationTemplate<MatData, Multi0DPlot> DeclarationType;

    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();
    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Multi0DPlot::Multi0DPlot(QWidget* pParent)
    :
    cedar::aux::gui::QCLinePlot(pParent)
{
  this->DiscreteMetricLabels = true;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::gui::Multi0DPlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    return mat_data->getDimensionality() == 0;
  }
  else
  {
    return false;
  }
}

bool cedar::aux::gui::Multi0DPlot::canDetach(cedar::aux::ConstDataPtr /* data */) const
{
  //!@todo Implement detaching
  return false;
}

void cedar::aux::gui::Multi0DPlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& titleStr)
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  CEDAR_ASSERT(mat_data);
  this->mData[titleStr] = mat_data;

  std::string title, subtitle;
  this->splitTitleString(titleStr, title, subtitle);



  // get/create title index
  auto title_iter = this->mTitleIndex.find(title);
  unsigned int title_index;
  if (title_iter == this->mTitleIndex.end())
  {
    title_index = this->mTitleIndex.size();
    this->mTitleIndex[title] = title_index;
    //this->getPlot()->setAxisScale(QwtPlot::xBottom, -0.5, static_cast<double>(title_index) + 0.5, 1.0);
    textTicker->addTick(this->DiscreteMetricLabelCounter, QString::fromStdString( title ));
    this->DiscreteMetricLabelCounter++;
  }
  else
  {
    title_index = title_iter->second;
  }

  auto data_iter = this->mSubtitleData.find(subtitle);
  auto new_mat = cv::Mat::zeros(title_index + 1, 1, CV_32F);
  if (data_iter == this->mSubtitleData.end())
  {
    // create a new data object
    cedar::aux::MatDataPtr subtitle_data(new cedar::aux::MatData(new_mat));
    this->mSubtitleData[subtitle] = subtitle_data;

    // add an annotation that tells the plot that the data is discrete
    cedar::aux::annotation::DiscreteMetricPtr annotation(new cedar::aux::annotation::DiscreteMetric());
    subtitle_data->setAnnotation(annotation);

    // append the data
    this->Super::doAppend(subtitle_data, ( (!subtitle.empty()) ? subtitle : title ));
  }
  else
  {
    auto subtitle_data = data_iter->second;
    subtitle_data->setData(new_mat);
  }
}

void cedar::aux::gui::Multi0DPlot::doDetach(cedar::aux::ConstDataPtr /* data */)
{
  //!@todo Implement detaching
}

void cedar::aux::gui::Multi0DPlot::timerEvent(QTimerEvent *pEvent)
{
  // write data into appropriate matrices
  for (const auto& path_data_pair : this->mData)
  {
    const auto& path = path_data_pair.first;
    const auto& data = path_data_pair.second;
    std::string title, subtitle;
    this->splitTitleString(path, title, subtitle);
    CEDAR_DEBUG_ASSERT(this->mSubtitleData.find(subtitle) != this->mSubtitleData.end());
    CEDAR_DEBUG_ASSERT(this->mTitleIndex.find(title) != this->mTitleIndex.end());

    unsigned int index = this->mTitleIndex[title];
    cedar::aux::MatDataPtr subdata = this->mSubtitleData[subtitle];
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, &data->getLock(), cedar::aux::LOCK_TYPE_READ);
    cedar::aux::append(locks, &subdata->getLock(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::LockerBase locker(boost::bind(&cedar::aux::lock, boost::ref(locks)), boost::bind(&cedar::aux::unlock, boost::ref(locks)));
    if (!data->isEmpty() && data->getDimensionality() == 0)
    {
      subdata->getData().at<float>(static_cast<int>(index), 0) = cedar::aux::math::getMatrixEntry<float>(data->getData(), 0);
    }
    else
    {
      subdata->getData().at<float>(static_cast<int>(index), 0) = 0.0;
    }
  }

  this->Super::timerEvent(pEvent);
}

void cedar::aux::gui::Multi0DPlot::splitTitleString(const std::string& title, std::string& titlePart, std::string& subtitlePart) const
{
  cedar::aux::splitFirst(title, ".", titlePart, subtitlePart);
}
#else
#ifdef CEDAR_USE_QWT

// CLASS HEADER
#include "cedar/auxiliaries/gui/Multi0DPlot.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/DiscreteMetric.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/LockerBase.h"
#include "cedar/auxiliaries/threadingUtilities.h"

// SYSTEM INCLUDES
#include <qwt_scale_draw.h>
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    using cedar::aux::MatData;
    using cedar::aux::gui::Multi0DPlot;

    typedef cedar::aux::gui::PlotDeclarationTemplate<MatData, Multi0DPlot> DeclarationType;

    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();
    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// custom scale div
//----------------------------------------------------------------------------------------------------------------------

//!@cond SKIPPED_DOCUMENTATION
/* Implementation of a Qwt scale draw that displays labels instead of numbers.
 */
class cedar::aux::gui::Multi0DPlot::LabeledScaleDraw : public QwtScaleDraw
{
public:
  LabeledScaleDraw(cedar::aux::gui::Multi0DPlot* pPlot)
  :
  mpPlot(pPlot)
  {
    this->setLabelRotation(90);
    this->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
  }

  virtual QwtText label(double value) const
  {
    double r = std::fmod(value, 1);

    if (r <= std::numeric_limits<double>::min())
    {
      std::string label = "";
      unsigned int label_id = static_cast<unsigned int>(std::round(value));

      // find the label
      for (const auto& pair : this->mpPlot->mTitleIndex)
      {
        if (pair.second == label_id)
        {
          label = pair.first;
          break;
        }
      }

      return QString::fromStdString(label);
    }
    else
    {
      return QString("");
    }
  }

private:
  cedar::aux::gui::Multi0DPlot* mpPlot;
};
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Multi0DPlot::Multi0DPlot(QWidget* pParent)
:
cedar::aux::gui::QwtLinePlot(pParent)
{
  this->setAutoDetermineXLimits(false);
  this->getPlot()->setAxisScaleDraw(QwtPlot::xBottom, new LabeledScaleDraw(this));
  this->getPlot()->setAxisMaxMinor(QwtPlot::xBottom, 0);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::gui::Multi0DPlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    return mat_data->getDimensionality() == 0;
  }
  else
  {
    return false;
  }
}

bool cedar::aux::gui::Multi0DPlot::canDetach(cedar::aux::ConstDataPtr /* data */) const
{
  //!@todo Implement detaching
  return false;
}

void cedar::aux::gui::Multi0DPlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& titleStr)
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  CEDAR_ASSERT(mat_data);
  this->mData[titleStr] = mat_data;

  std::string title, subtitle;
  this->splitTitleString(titleStr, title, subtitle);

  // get/create title index
  auto title_iter = this->mTitleIndex.find(title);
  unsigned int title_index;
  if (title_iter == this->mTitleIndex.end())
  {
    title_index = this->mTitleIndex.size();
    this->mTitleIndex[title] = title_index;
    this->getPlot()->setAxisScale(QwtPlot::xBottom, -0.5, static_cast<double>(title_index) + 0.5, 1.0);
  }
  else
  {
    title_index = title_iter->second;
  }

  auto data_iter = this->mSubtitleData.find(subtitle);
  auto new_mat = cv::Mat::zeros(title_index + 1, 1, CV_32F);
  if (data_iter == this->mSubtitleData.end())
  {
    // create a new data object
    cedar::aux::MatDataPtr subtitle_data(new cedar::aux::MatData(new_mat));
    this->mSubtitleData[subtitle] = subtitle_data;

    // add an annotation that tells the plot that the data is discrete
    cedar::aux::annotation::DiscreteMetricPtr annotation(new cedar::aux::annotation::DiscreteMetric());
    subtitle_data->setAnnotation(annotation);

    // append the data
    this->Super::doAppend(subtitle_data, subtitle);
  }
  else
  {
    auto subtitle_data = data_iter->second;
    subtitle_data->setData(new_mat);
  }
}

void cedar::aux::gui::Multi0DPlot::doDetach(cedar::aux::ConstDataPtr /* data */)
{
  //!@todo Implement detaching
}

void cedar::aux::gui::Multi0DPlot::timerEvent(QTimerEvent *pEvent)
{
  // write data into appropriate matrices
  for (const auto& path_data_pair : this->mData)
  {
    const auto& path = path_data_pair.first;
    const auto& data = path_data_pair.second;
    std::string title, subtitle;
    this->splitTitleString(path, title, subtitle);
    CEDAR_DEBUG_ASSERT(this->mSubtitleData.find(subtitle) != this->mSubtitleData.end());
    CEDAR_DEBUG_ASSERT(this->mTitleIndex.find(title) != this->mTitleIndex.end());

    unsigned int index = this->mTitleIndex[title];
    cedar::aux::MatDataPtr subdata = this->mSubtitleData[subtitle];
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, &data->getLock(), cedar::aux::LOCK_TYPE_READ);
    cedar::aux::append(locks, &subdata->getLock(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::LockerBase locker(boost::bind(&cedar::aux::lock, boost::ref(locks)), boost::bind(&cedar::aux::unlock, boost::ref(locks)));
    if (!data->isEmpty() && data->getDimensionality() == 0)
    {
      subdata->getData().at<float>(static_cast<int>(index), 0) = cedar::aux::math::getMatrixEntry<float>(data->getData(), 0);
    }
    else
    {
      subdata->getData().at<float>(static_cast<int>(index), 0) = 0.0;
    }
  }

  this->Super::timerEvent(pEvent);
}

void cedar::aux::gui::Multi0DPlot::splitTitleString(const std::string& title, std::string& titlePart, std::string& subtitlePart) const
{
  cedar::aux::splitFirst(title, ".", titlePart, subtitlePart);
}

#endif // CEDAR_USE_QWT
#endif