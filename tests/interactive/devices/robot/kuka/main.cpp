/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Maintainer:  Guido Knips
 ----- Email:       guido.knips@ini.rub.de
 ----- Date:        2011 1 11

 ----- Description:

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "devices/robot/kuka/KukaInterface.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>
#include <exception>

using namespace std;
using cedar::dev::robot::kuka::KukaInterface;

int main(int argc, char **argv)
{

    KukaInterface kukain("test_arm.conf", false);
    vector<double> angles = kukain.getJointAngles();
    for(unsigned i=0; i<angles.size(); i++)
    {
      cout << i+1 << "\t" << angles[i] << "\t" << angles[i]/3.14*180 << endl;
    }
    cout << "Current State: " <<kukain.getFriState()<<endl;
    while(!kukain.isPowerOn());
    cout <<"Power is " << (kukain.isPowerOn()?string("ON"):string("OFF")) <<endl;
    return 0;
}
