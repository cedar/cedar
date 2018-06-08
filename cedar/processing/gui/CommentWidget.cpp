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

    File:        Recorder.cpp

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 04 09

    Description: Implementation of the @em cedar::aux::gui::CommentWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/CommentWidget.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/Data.h"

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <vector>
#include <QTextEdit>

cedar::proc::gui::CommentWidget::CommentWidget()
{
  mMainLayout = new QVBoxLayout();
  this->setLayout(this->mMainLayout);
}

cedar::proc::gui::CommentWidget::CommentWidget(QWidget* pParent)
:
QWidget(pParent)
{
  mMainLayout = new QVBoxLayout();
  this->setLayout(this->mMainLayout);
}

cedar::proc::gui::CommentWidget::~CommentWidget()
{
  delete mMainLayout;
}

void cedar::proc::gui::CommentWidget::setConnectable(cedar::proc::ConnectablePtr connectable)
{
  this->mConnectable = connectable;
  refreshWidget();
}

void cedar::proc::gui::CommentWidget::refreshWidget()
{
  // reset the widget
  clearLayout();

  // Check if step not null
  if (!this->mConnectable)
  {
    return;
  }

  // draw Headers
  this->createHeader(this->mConnectable->getName());

  std::string commentText = this->mConnectable->getCommentString();

  this->fillInCommentSection(commentText);


  mMainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void cedar::proc::gui::CommentWidget::clear()
{
  this->mConnectable.reset();
  this->clearLayout();
}

void cedar::proc::gui::CommentWidget::clearLayout()
{
//  std::cout <<"Clearing the Layout"<<std::endl;

  int counter = 0;
  int counterWidget = 0;

  QLayoutItem *item;
  while ((item = mMainLayout->takeAt(0)) != nullptr)
  {
    counter= counter +1;
//    std::cout<<"Found Layout! Counter: " << counter << " and counterWidget: " << counterWidget << std::endl;
    QLayoutItem* widget;
    while((widget = item->layout()->takeAt(0)) != nullptr)
    {
      counterWidget = counterWidget+1;
//      std::cout<<"Found Widget in Layout! Counter: " << counter << " and counterWidget: " << counterWidget << std::endl;
      delete widget->widget();
    }
    counterWidget = 0;
    delete item->layout();
  }

//  std::cout<<"Finished Cleaning the Layout with counter: " << counter << " and counterWidget: " << counterWidget << std::endl;
}




void cedar::proc::gui::CommentWidget::createHeader(const std::string& name)
{  
  QHBoxLayout* step_name_layout = new QHBoxLayout();
  //Create step name.
  QLabel* step_name = new QLabel(QString(name.c_str()));
  QFont step_name_font= step_name->font();

  step_name_font.setPointSize(step_name_font.pointSize() + 2);
  step_name_font.setKerning(true);
  step_name_font.setWeight( QFont::Light );

  step_name->setFont(step_name_font);


  // imitate the format of aux::gui::Configurable
  step_name_layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  step_name_layout->addWidget(step_name);

  mMainLayout->addLayout(step_name_layout);

}

void cedar::proc::gui::CommentWidget::fillInCommentSection(std::string& comment)
{
  QHBoxLayout* comment_text_layout = new QHBoxLayout();
  mCommentTextField = new QTextEdit();
  mCommentTextField->setText(QString::fromStdString(comment));

  QObject::connect(mCommentTextField, SIGNAL(textChanged()), this, SLOT(updateCommentString()));

  comment_text_layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  comment_text_layout->addWidget(mCommentTextField);

  mMainLayout->addLayout(comment_text_layout);

//  // Create role name.
//  QLabel* role_name = new QLabel(QString::fromStdString(name));
//  QFont role_font = role_name->font();
//  role_font.setPointSize(role_font.pointSize() + 2);
//  role_font.setBold(true);
//  role_name->setFont(role_font);
//  role_font_layout->addWidget(role_name);
//
//  mMainLayout->addLayout(role_font_layout);
}

void cedar::proc::gui::CommentWidget::updateCommentString()
{
  if(mCommentTextField != nullptr && this->mConnectable)
  {
    this->mConnectable->setCommentString(mCommentTextField->toPlainText().toStdString());
  }
}

