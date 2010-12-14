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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        cedarAux.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 26

    Description: Header file that includes all headers of the auxiliaries library.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_CEDAR_AUX_H
#define CEDAR_CEDAR_AUX_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "auxiliaries/Base.h"
#include "auxiliaries/ConfigurationInterface.h"
#include "auxiliaries/LogFile.h"
#include "auxiliaries/namespace.h"
#include "auxiliaries/LoopedThread.h"
#include "auxiliaries/UserData.h"

#include "auxiliaries/exceptions/ExceptionBase.h"
#include "auxiliaries/exceptions/IndexOutOfRangeException.h"
#include "auxiliaries/exceptions/namespace.h"
#include "auxiliaries/exceptions/NullPointerException.h"

#include "auxiliaries/math/algebraTools.h"
#include "auxiliaries/math/coordinateTransformations.h"
#include "auxiliaries/math/Limits.h"
#include "auxiliaries/math/namespace.h"
#include "auxiliaries/math/screwCalculus.h"
#include "auxiliaries/math/sigmoids.h"
#include "auxiliaries/math/tools.h"

#include "auxiliaries/gui/namespace.h"
#include "auxiliaries/gui/BaseWidget.h"
// SYSTEM INCLUDES

#endif // CEDAR_CEDAR_AUX_H
