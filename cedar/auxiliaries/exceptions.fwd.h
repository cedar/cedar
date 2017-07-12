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

    File:        exceptions.fwd.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 11 12

    Description: Forward declaration file for exceptions in the cedar::aux namespace.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_EXCEPTIONS_FWD_H
#define CEDAR_AUX_EXCEPTIONS_FWD_H

//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace aux
  {
    /* exceptions */
    CEDAR_DECLARE_AUX_CLASS(AnnotationNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(ArithmeticExpressionException);
    CEDAR_DECLARE_AUX_CLASS(BadConnectionException);
    CEDAR_DECLARE_AUX_CLASS(ConversionFailedException);
    CEDAR_DECLARE_AUX_CLASS(DeadReferenceException);
    CEDAR_DECLARE_AUX_CLASS(DimensionalityMismatchException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateIdException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateNameException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateChannelNameException);
    CEDAR_DECLARE_AUX_CLASS(FileNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(FailedAssertionException);
    CEDAR_DECLARE_AUX_CLASS(IndexOutOfRangeException);
    CEDAR_DECLARE_AUX_CLASS(InitializationException);
    CEDAR_DECLARE_AUX_CLASS(InvalidNameException);
    CEDAR_DECLARE_AUX_CLASS(InvalidPathException);
    CEDAR_DECLARE_AUX_CLASS(InvalidValueException);
    CEDAR_DECLARE_AUX_CLASS(MalformedConfigurationTreeException);
    CEDAR_DECLARE_AUX_CLASS(MatrixMismatchException);
    CEDAR_DECLARE_AUX_CLASS(NoDefaultException);
    CEDAR_DECLARE_AUX_CLASS(NotFoundException);
    CEDAR_DECLARE_AUX_CLASS(NotImplementedException);
    CEDAR_DECLARE_AUX_CLASS(NullPointerException);
    CEDAR_DECLARE_AUX_CLASS(ParameterNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(ParseException);
    CEDAR_DECLARE_AUX_CLASS(PluginException);
    CEDAR_DECLARE_AUX_CLASS(PluginNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(RangeException);
    CEDAR_DECLARE_AUX_CLASS(ResourceNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(TypeMismatchException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledValueException);
    CEDAR_DECLARE_AUX_CLASS(UnknownNameException);
    CEDAR_DECLARE_AUX_CLASS(UnknownPluginException);
    CEDAR_DECLARE_AUX_CLASS(UnknownTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnknownUnitSuffixException);
    CEDAR_DECLARE_AUX_CLASS(UnmanglingFailedException);
    CEDAR_DECLARE_AUX_CLASS(ValidationFailedException);
    CEDAR_DECLARE_AUX_CLASS(ThreadingErrorException);
    CEDAR_DECLARE_AUX_CLASS(NotImplementedException);
    CEDAR_DECLARE_AUX_CLASS(RecorderException);
    CEDAR_DECLARE_AUX_CLASS(ParameterLinkingException);
  }
}
//!@endcond

#endif // CEDAR_AUX_EXCEPTIONS_FWD_H
