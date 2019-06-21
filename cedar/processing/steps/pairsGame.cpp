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

    File:        pairsGame.cpp

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/pairsGame.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/typecheck/SameSize.h"
#include "cedar/processing/typecheck/SameTypeCollection.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"


// SYSTEM INCLUDES
#include <opencv2/core/version.hpp>

#if (CV_MAJOR_VERSION > 3)
#define CV_FILLED cv::FILLED
#endif

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::pairsGame>
      (
        "Sources",
        "cedar.processing.steps.pairsGame"
      )
    );
    declaration->setIconPath(":/steps/pairsGame.svg");
    declaration->setDescription
    (
      "Pairs Game. TODO: This needs more description, here."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::pairsGame::pairsGame()
        :
        _mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 120, 80, 800)),
        _mTileSize (new cedar::aux::UIntParameter(this, "tile size", 20,10,200,1)),
        _mPadding (new cedar::aux::UIntParameter(this, "padding", 8,1,80,1)),
        _mNumberOfPairs (new cedar::aux::UIntParameter(this, "number of pairs", 4,0,8,1)),
        _mSingleFeature (new cedar::aux::BoolParameter(this,"single Feature",false))
{
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
  QObject::connect(_mTileSize.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
  QObject::connect(_mPadding.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
  QObject::connect(_mNumberOfPairs.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
  QObject::connect(_mSingleFeature.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));

  auto slot_0 = this->declareInput("start game",false);

    auto slot_1 = this->declareInput("guess 1 x",false);
    auto slot_2 = this->declareInput("guess 1 y",false);
    auto slot_3 = this->declareInput("guess 2 x",false);
    auto slot_4 = this->declareInput("guess 2 y",false);
    auto slot_5 = this->declareInput("hide solved",false);


    cedar::proc::typecheck::Matrix matrix_check;
    matrix_check.addAcceptedDimensionality(0);
    matrix_check.addAcceptedType(CV_32F);
    matrix_check.acceptsEmptyMatrix(true);
    slot_0->setCheck(matrix_check);
    slot_1->setCheck(matrix_check);
    slot_2->setCheck(matrix_check);
    slot_3->setCheck(matrix_check);
    slot_4->setCheck(matrix_check);
    slot_5->setCheck(matrix_check);




  int sizes[2];
  sizes[0] = 120;
  sizes[1] = 120;
  mOutput = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0))));
  mOutputD1 = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0))));
  mOutputD2 = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0))));
  mOutputD3 = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0))));
  this->declareOutput("board", this->mOutput);
  this->declareBuffer("[dbg]org", this->mOutputD1);
  this->declareBuffer("[dbg]rotVis", this->mOutputD2);
  this->declareBuffer("[dbg]rotHid", this->mOutputD3);

    colorsSF.push_back(cv::Scalar(255,0,0));
    colorsSF.push_back(cv::Scalar(0,255,0));
    colorsSF.push_back(cv::Scalar(0,0,255));
    colorsSF.push_back(cv::Scalar(0,255,255));
    colorsSF.push_back(cv::Scalar(255,255,0));
    colorsSF.push_back(cv::Scalar(255,0,255));
    colorsSF.push_back(cv::Scalar(125,0,125));
    colorsSF.push_back(cv::Scalar(125,125,0));

    colors.push_back(cv::Scalar(0,255,0));
    colors.push_back(cv::Scalar(0,0,255));
    colors.push_back(cv::Scalar(0,255,0));
    colors.push_back(cv::Scalar(0,0,255));
    colors.push_back(cv::Scalar(0,255,0));
    colors.push_back(cv::Scalar(0,0,255));
    colors.push_back(cv::Scalar(0,255,0));
    colors.push_back(cv::Scalar(0,0,255));

    orientations.push_back(0.);
    orientations.push_back(0.);
    orientations.push_back(45.);
    orientations.push_back(45.);
    orientations.push_back(90.);
    orientations.push_back(90.);
    orientations.push_back(135.);
    orientations.push_back(135.);

    unsigned int row = 0;
    unsigned int col = 0;
    for (unsigned int k = 0; k < 16; ++k)
    {
        positions.push_back(cv::Rect(8+(col*28),8+(row*28),20,20));
        col++;
        if(col == 4)
        {
            col = 0;
            row++;
        }
    }
    std::random_shuffle ( positions.begin(), positions.end() );
    rot = 1 + (rand() % static_cast<int>(3));

    paintPairs();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::pairsGame::reset()
{
    QObject::disconnect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::disconnect(_mTileSize.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::disconnect(_mPadding.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::disconnect(_mNumberOfPairs.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::disconnect(_mSingleFeature.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    std::random_shuffle ( positions.begin(), positions.end() );
    rot = 1 + (rand() % static_cast<int>(3));
    solved.clear();
    paintPairs();
    QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::connect(_mTileSize.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::connect(_mPadding.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::connect(_mNumberOfPairs.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
    QObject::connect(_mSingleFeature.get(), SIGNAL(valueChanged()), this, SLOT(pairsChanged()));
}

void cedar::proc::steps::pairsGame::pairsChanged()
{
    auto siz = _mSizes->getValue();
    int sizes[2];
    sizes[0] = siz[0];
    sizes[1] = siz[1];

    QWriteLocker l(&this->mOutput->getLock());
    this->mOutput->setData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0)));
    l.unlock();

    QWriteLocker l2(&this->mOutputD1->getLock());
    this->mOutputD1->setData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0)));
    l2.unlock();
    QWriteLocker l3(&this->mOutputD2->getLock());
    this->mOutputD2->setData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0)));
    l3.unlock();
    QWriteLocker l4(&this->mOutputD3->getLock());
    this->mOutputD3->setData(cv::Mat(2, sizes, CV_8UC3, cv::Scalar(0,0,0)));
    l4.unlock();

    positions.clear();
    solved.clear();

    auto tilesize = static_cast<unsigned int>(_mTileSize->getValue());
    auto pad = static_cast<unsigned int>(_mPadding->getValue());

    int anchor_x = (siz[1]/2) - ((4*tilesize + 5*pad) / 2);
    int anchor_y = (siz[0]/2) - ((4*tilesize + 5*pad) / 2);

    unsigned int row = 0;
    unsigned int col = 0;
    for (unsigned int k = 0; k < 16; ++k)
    {
        positions.push_back(cv::Rect(anchor_x + pad + (col*(tilesize+pad)),anchor_y + pad + (row*(tilesize+pad)),tilesize,tilesize));
        col++;
        if(col == 4)
        {
            col = 0;
            row++;
        }
    }
    std::random_shuffle ( positions.begin(), positions.end() );
    rot = 1 + (rand() % static_cast<int>(3));

    paintPairs();
    this->callComputeWithoutTriggering();
    this->emitOutputPropertiesChangedSignal("board");
}

void cedar::proc::steps::pairsGame::paintPairs()
{
    cv::Mat& output = this->mOutput->getData();
    auto nr = static_cast<unsigned int>(_mNumberOfPairs->getValue());
    bool singleFeature = static_cast<bool>(_mSingleFeature->getValue());
    bool isStarted = false;
    auto siz = _mSizes->getValue();
    auto tilesize = static_cast<int>(_mTileSize->getValue());
    cv::rectangle(output, cv::Rect(0,0,siz[1],siz[0]), cv::Scalar(0,0,0),CV_FILLED);
    int g1_x = 0;
    int g2_x = 0;
    int g1_y = 0;
    int g2_y = 0;
    auto startPtr = this->getInput("start game");
    if(auto mat_data_start = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(startPtr))
    {
        const cv::Mat& start = mat_data_start->getData();
        if(start.at<float>(0,0) > 0.99)
        {
            isStarted = true;
        }
    }
    bool isSaveMode = false;
    auto saveModePtr = this->getInput("hide solved");
    if(auto mat_data_save = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(saveModePtr))
    {
        const cv::Mat& save = mat_data_save->getData();
        if(save.at<float>(0,0) > 0.99)
        {
            isSaveMode = true;
        }
    }
    auto g1_xPtr = this->getInput("guess 1 x");
    if(auto mat_data_g1_x = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(g1_xPtr))
    {
        g1_x = static_cast<int>(mat_data_g1_x->getData().at<float>(0,0));
    }
    auto g1_yPtr = this->getInput("guess 1 y");
    if(auto mat_data_g1_y = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(g1_yPtr))
    {
        g1_y = static_cast<int>(mat_data_g1_y->getData().at<float>(0,0));
    }
    auto g2_xPtr = this->getInput("guess 2 x");
    if(auto mat_data_g2_x = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(g2_xPtr))
    {
        g2_x = static_cast<int>(mat_data_g2_x->getData().at<float>(0,0));
    }
    auto g2_yPtr = this->getInput("guess 2 y");
    if(auto mat_data_g2_y = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(g2_yPtr))
    {
        g2_y = static_cast<int>(mat_data_g2_y->getData().at<float>(0,0));
    }

    int vis1 = -1;
    int vis2 = -1;

    for (unsigned int j = 0; j < nr; ++j)
    {
        if(isStarted)
        {
            if(!(std::find(solved.begin(), solved.end(), j) != solved.end()))
            {
                cv::Rect pos1 = positions[j];
                cv::Rect pos2 = positions[j+8];
                float angle = 0;
                if(rot == 1)
                {
                    angle = 90;
                }
                else if(rot == 2)
                {
                    angle = 180;
                }
                else if(rot == 3)
                {
                    angle = 270;
                }

                int cx = (siz[1]/2) - (tilesize/2); //(siz[1]/2) - ((4*tilesize + 5*pad) / 2)
                int cy = (siz[0]/2) - (tilesize/2);

                float s = sin(angle * cedar::aux::math::pi / 180.0);
                float c = cos(angle * cedar::aux::math::pi / 180.0);

                pos1.x -= cx;
                pos1.y -= cy;

                float xnew1 = pos1.x * c - pos1.y * s;
                float ynew1 = pos1.x * s + pos1.y * c;

                pos1.x = xnew1 + cx;
                pos1.y = ynew1 + cy;

                pos2.x -= cx;
                pos2.y -= cy;

                float xnew2 = pos2.x * c - pos2.y * s;
                float ynew2 = pos2.x * s + pos2.y * c;

                pos2.x = xnew2 + cx;
                pos2.y = ynew2 + cy;

                cv::rectangle(output, pos1, cv::Scalar(255,255,255),CV_FILLED);
                cv::rectangle(output, pos2, cv::Scalar(255,255,255),CV_FILLED);

                cv::rectangle(output, cv::Rect(g1_x,g1_y,2,2), cv::Scalar(99,99,99),CV_FILLED);
                cv::rectangle(output, cv::Rect(g2_x,g2_y,2,2), cv::Scalar(99,99,99),CV_FILLED);

                int ht = tilesize/2;

                if( ((pos1.x <= g1_x && (pos1.x+tilesize) >= g1_x ) && (pos1.y <= g1_y && (pos1.y+tilesize) >= g1_y )) || ((pos1.x <= g2_x && (pos1.x+tilesize) >= g2_x ) && (pos1.y <= g2_y && (pos1.y+tilesize) >= g2_y )) )
                {
                    cv::rectangle(output, pos1, cv::Scalar(255,255,255),CV_FILLED);
                    cv::Scalar color = colors[j];
                    float rotationDegrees = orientations[j];
                    if(singleFeature)
                    {
                        color = colorsSF[j];
                        rotationDegrees = 0.;
                    }
                    cv::RotatedRect rotatedRectangle1(cv::Point(pos1.x+ht,pos1.y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
                    cv::Point2f vertices2f1[4];
                    rotatedRectangle1.points(vertices2f1);
                    cv::Point vertices1[4];
                    for(int i = 0; i < 4; ++i){
                        vertices1[i] = vertices2f1[i];
                    }
                    cv::fillConvexPoly(output,vertices1,4,color);
                    vis1 = j;
                }
                else
                {
                    vis1 = -1;
                }

                if( ( (pos2.x <= g1_x && (pos2.x+tilesize) >= g1_x ) && (pos2.y <= g1_y && (pos2.y+tilesize) >= g1_y ) ) || ( (pos2.x <= g2_x && (pos2.x+tilesize) >= g2_x ) && (pos2.y <= g2_y && (pos2.y+tilesize) >= g2_y ) ))
                {
                    cv::rectangle(output, pos2, cv::Scalar(255,255,255),CV_FILLED);
                    cv::Scalar color = colors[j];
                    float rotationDegrees = orientations[j];
                    if(singleFeature)
                    {
                        color = colorsSF[j];
                        rotationDegrees = 0.;
                    }
                    cv::RotatedRect rotatedRectangle1(cv::Point(pos2.x+ht,pos2.y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
                    cv::Point2f vertices2f1[4];
                    rotatedRectangle1.points(vertices2f1);
                    cv::Point vertices1[4];
                    for(int i = 0; i < 4; ++i){
                        vertices1[i] = vertices2f1[i];
                    }
                    cv::fillConvexPoly(output,vertices1,4,color);
                    vis2 = j;
                }
                else
                {
                    vis2 = -1;
                }
                if(vis1 > -1 && vis2 > -1)
                {
                    if(vis1 == vis2)
                    {
                        if(isSaveMode)
                        {
                            solved.push_back(vis1);
                        }
                    }
                }
            }
        }
        else
        {
            cv::rectangle(output, positions[j], cv::Scalar(255,255,255),CV_FILLED);
            cv::rectangle(output, positions[j+8], cv::Scalar(255,255,255),CV_FILLED);
            cv::Scalar color = colors[j];
            float rotationDegrees = orientations[j];
            if(singleFeature)
            {
                color = colorsSF[j];
                rotationDegrees = 0.;
            }
            int ht = tilesize/2;
            cv::RotatedRect rotatedRectangle1(cv::Point(positions[j].x+ht,positions[j].y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
            cv::RotatedRect rotatedRectangle2(cv::Point(positions[j+8].x+ht,positions[j+8].y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
            cv::Point2f vertices2f1[4];
            rotatedRectangle1.points(vertices2f1);
            cv::Point2f vertices2f2[4];
            rotatedRectangle2.points(vertices2f2);
            cv::Point vertices1[4];
            cv::Point vertices2[4];
            for(int i = 0; i < 4; ++i){
                vertices1[i] = vertices2f1[i];
                vertices2[i] = vertices2f2[i];
            }
            cv::fillConvexPoly(output,vertices1,4,color);
            cv::fillConvexPoly(output,vertices2,4,color);
        }
    }
    paintDebug1();
    paintDebug2();
    paintDebug3();
}

void cedar::proc::steps::pairsGame::paintDebug1()
{
    cv::Mat& output = this->mOutputD1->getData();
    auto nr = static_cast<unsigned int>(_mNumberOfPairs->getValue());
    bool singleFeature = static_cast<bool>(_mSingleFeature->getValue());
    auto siz = _mSizes->getValue();
    auto tilesize = static_cast<int>(_mTileSize->getValue());
    cv::rectangle(output, cv::Rect(0,0,siz[1],siz[0]), cv::Scalar(0,0,0),CV_FILLED);
    for (unsigned int j = 0; j < nr; ++j)
    {
        cv::rectangle(output, positions[j], cv::Scalar(255,255,255),CV_FILLED);
        cv::rectangle(output, positions[j+8], cv::Scalar(255,255,255),CV_FILLED);
        cv::Scalar color = colors[j];
        float rotationDegrees = orientations[j];
        if(singleFeature)
        {
            color = colorsSF[j];
            rotationDegrees = 0.;
        }
        int ht = tilesize/2;
        cv::RotatedRect rotatedRectangle1(cv::Point(positions[j].x+ht,positions[j].y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
        cv::RotatedRect rotatedRectangle2(cv::Point(positions[j+8].x+ht,positions[j+8].y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
        cv::Point2f vertices2f1[4];
        rotatedRectangle1.points(vertices2f1);
        cv::Point2f vertices2f2[4];
        rotatedRectangle2.points(vertices2f2);
        cv::Point vertices1[4];
        cv::Point vertices2[4];
        for(int i = 0; i < 4; ++i){
            vertices1[i] = vertices2f1[i];
            vertices2[i] = vertices2f2[i];
        }
        cv::fillConvexPoly(output,vertices1,4,color);
        cv::fillConvexPoly(output,vertices2,4,color);
    }
}

void cedar::proc::steps::pairsGame::paintDebug2()
{
    cv::Mat& output = this->mOutputD2->getData();
    auto nr = static_cast<unsigned int>(_mNumberOfPairs->getValue());
    bool singleFeature = static_cast<bool>(_mSingleFeature->getValue());
    auto siz = _mSizes->getValue();
    auto tilesize = static_cast<int>(_mTileSize->getValue());
    cv::rectangle(output, cv::Rect(0,0,siz[1],siz[0]), cv::Scalar(0,0,0),CV_FILLED);

    for (unsigned int j = 0; j < nr; ++j)
    {
        cv::Rect pos1 = positions[j];
        cv::Rect pos2 = positions[j+8];
        float angle = 0;
        if(rot == 1)
        {
            angle = 90;
        }
        else if(rot == 2)
        {
            angle = 180;
        }
        else if(rot == 3)
        {
            angle = 270;
        }

        int cx = (siz[1]/2) - (tilesize/2); //(siz[1]/2) - ((4*tilesize + 5*pad) / 2)
        int cy = (siz[0]/2) - (tilesize/2);

        float s = sin(angle * cedar::aux::math::pi / 180.0);
        float c = cos(angle * cedar::aux::math::pi / 180.0);

        pos1.x -= cx;
        pos1.y -= cy;

        float xnew1 = pos1.x * c - pos1.y * s;
        float ynew1 = pos1.x * s + pos1.y * c;

        pos1.x = xnew1 + cx;
        pos1.y = ynew1 + cy;

        pos2.x -= cx;
        pos2.y -= cy;

        float xnew2 = pos2.x * c - pos2.y * s;
        float ynew2 = pos2.x * s + pos2.y * c;

        pos2.x = xnew2 + cx;
        pos2.y = ynew2 + cy;

        cv::rectangle(output, pos1, cv::Scalar(255,255,255),CV_FILLED);
        cv::rectangle(output, pos2, cv::Scalar(255,255,255),CV_FILLED);

        cv::Scalar color = colors[j];
        float rotationDegrees = orientations[j];
        if(singleFeature)
        {
            color = colorsSF[j];
            rotationDegrees = 0.;
        }
        int ht = tilesize/2;
        cv::RotatedRect rotatedRectangle1(cv::Point(pos1.x+ht,pos1.y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
        cv::RotatedRect rotatedRectangle2(cv::Point(pos2.x+ht,pos2.y+ht), cv::Size(tilesize/4,tilesize*0.8), rotationDegrees);
        cv::Point2f vertices2f1[4];
        rotatedRectangle1.points(vertices2f1);
        cv::Point2f vertices2f2[4];
        rotatedRectangle2.points(vertices2f2);
        cv::Point vertices1[4];
        cv::Point vertices2[4];
        for(int i = 0; i < 4; ++i){
            vertices1[i] = vertices2f1[i];
            vertices2[i] = vertices2f2[i];
        }
        cv::fillConvexPoly(output,vertices1,4,color);
        cv::fillConvexPoly(output,vertices2,4,color);

    }
}

void cedar::proc::steps::pairsGame::paintDebug3()
{
    cv::Mat& output = this->mOutputD3->getData();
    auto nr = static_cast<unsigned int>(_mNumberOfPairs->getValue());
    auto siz = _mSizes->getValue();
    auto tilesize = static_cast<int>(_mTileSize->getValue());
    cv::rectangle(output, cv::Rect(0,0,siz[1],siz[0]), cv::Scalar(0,0,0),CV_FILLED);

    for (unsigned int j = 0; j < nr; ++j)
    {
        cv::Rect pos1 = positions[j];
        cv::Rect pos2 = positions[j+8];
        float angle = 0;
        if(rot == 1)
        {
            angle = 90;
        }
        else if(rot == 2)
        {
            angle = 180;
        }
        else if(rot == 3)
        {
            angle = 270;
        }

        int cx = (siz[1]/2) - (tilesize/2); //(siz[1]/2) - ((4*tilesize + 5*pad) / 2)
        int cy = (siz[0]/2) - (tilesize/2);

        float s = sin(angle * cedar::aux::math::pi / 180.0);
        float c = cos(angle * cedar::aux::math::pi / 180.0);

        pos1.x -= cx;
        pos1.y -= cy;

        float xnew1 = pos1.x * c - pos1.y * s;
        float ynew1 = pos1.x * s + pos1.y * c;

        pos1.x = xnew1 + cx;
        pos1.y = ynew1 + cy;

        pos2.x -= cx;
        pos2.y -= cy;

        float xnew2 = pos2.x * c - pos2.y * s;
        float ynew2 = pos2.x * s + pos2.y * c;

        pos2.x = xnew2 + cx;
        pos2.y = ynew2 + cy;

        cv::rectangle(output, pos1, cv::Scalar(255,255,255),CV_FILLED);
        cv::rectangle(output, pos2, cv::Scalar(255,255,255),CV_FILLED);
    }
}


void cedar::proc::steps::pairsGame::compute(const cedar::proc::Arguments&)
{
  if (!this->allInputsValid())
  {
    return;
  }
  paintPairs();
    this->callComputeWithoutTriggering();
    this->emitOutputPropertiesChangedSignal("board");
}

void cedar::proc::steps::pairsGame::inputConnectionChanged(const std::string& inputName)
{
  if (auto data = this->getInput(inputName))
  {
    if(auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      if (!this->allInputsValid())
      {
        return;
      }
      this->callComputeWithoutTriggering();
      this->emitOutputPropertiesChangedSignal("board");
    }
  }
}
