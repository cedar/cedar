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

    File:        Qt5SurfacePlot.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"


#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/processing/gui/PlotDockWidget.h"
#include <QHBoxLayout>



// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::PlotDockWidget::PlotDockWidget(const QString & title, QWidget * parent , Qt::WindowFlags flags )
        :
        QDockWidget(title,parent,flags)
{
  mTitleLabel = new QLabel(title);
  QFont f( "Arial", 10, QFont::Bold);
  mTitleLabel->setFont(f);
  mTitleLabel->setAlignment(Qt::AlignVCenter);
  mTitleLabel->setContentsMargins(4,0,0,3);



  //Construct titlebar
  QWidget* titleBarWidget = new QWidget();
  auto layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  int buttonSize =15;
  mJumpButton = new QPushButton(QIcon(":/cedar/auxiliaries/gui/jump.svg"),"");
  mJumpButton->setFixedSize(buttonSize, buttonSize);
  mJumpButton->setIconSize(QSize(buttonSize,buttonSize));
  mJumpButton->setToolTip("jump to the connected step");

  mDeleteButton = new QPushButton();
  mDeleteButton->setFixedSize(buttonSize, buttonSize);
  mDeleteButton->setIcon(QIcon(":/cedar/auxiliaries/gui/clear.svg"));
  mDeleteButton->setIconSize(QSize(buttonSize,buttonSize));
  mDeleteButton->setToolTip("close this plot");



  layout->addWidget(mTitleLabel);
  layout->addWidget(mJumpButton);
  layout->addWidget(mDeleteButton);
  titleBarWidget->setLayout(layout);

  this->setTitleBarWidget(titleBarWidget);

}

void cedar::proc::gui::PlotDockWidget::setTitleLabelText(const QString & title)
{
  mTitleLabel->setText(title);
}

QLabel* cedar::proc::gui::PlotDockWidget::getTitleLabel()
{
  return mTitleLabel;
}

QPushButton* cedar::proc::gui::PlotDockWidget::getJumpButton()
{
  return mJumpButton;
}

QPushButton* cedar::proc::gui::PlotDockWidget::getDeleteButton()
{
  return mDeleteButton;
}


void cedar::proc::gui::PlotDockWidget::paintEvent(QPaintEvent *event)
{
  QDockWidget::paintEvent(event);

  // Somehow setting a titlebar deletes all look features, thus we draw it ourselves ;)
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  const int linepadding = 2;
  const int penSize = 3;
//  painter.drawRoundedRect(QRect(distance, distance, width() - 2 * distance, height() - 2 * distance), 2, 2);

  //white
  painter.setPen(QPen(QColor("#000000"), penSize));
  painter.drawLine(width(),0,width(),height());
  painter.drawLine(0,height(),width(),height());

  //grey
  painter.setPen(QPen(QColor("#a5a5a5"), penSize-1));
  painter.drawLine(width()-linepadding,0,width()-linepadding,height()-linepadding);
  painter.drawLine(0,height()-linepadding,width()-linepadding,height()-linepadding);

  //black
  painter.setPen(QPen(QColor("#FFFFFF"), penSize));
  painter.drawLine(linepadding,0,width(),0);
  painter.drawLine(linepadding,0,linepadding,height()-(linepadding*2));
}




