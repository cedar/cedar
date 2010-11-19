/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        sigmoids.cpp

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: sigmoid functions

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// includes
#include "sigmoids.h"

using namespace std;

double cedar::aux::math::sigmoid(const double x, const double beta, const double threshold)
{
  return 1 / (1 + exp(-beta * (x - threshold)));
}

double cedar::aux::math::sigmoidAbs(const double x, const double beta, const double threshold)
{
  return 0.5 * (1. + beta * (x - threshold) / (1. + beta * fabs(x - threshold)));
}

std::vector<double> cedar::aux::math::sigmoid(const std::vector<double>& x, const double beta, const double threshold)
{
  std::vector<double> buffer;
  buffer.resize(x.size());
  for (unsigned int i = 0; i < x.size(); i++)
  {
//    buffer[i] = 1 / (1 + exp(-beta * (x[i] - threshold)));
    buffer[i] = sigmoid(x[i], beta, threshold);
  }
  return buffer;
}

double cedar::aux::math::sigmoidInterval(const double value, const double t1, const double t2, const bool decreasing)
{
  if (decreasing)
  {
    return 1 - sigmoidInterval(value, t1, t2);
  }
  if (value < t1)
  {
    return 0;
  }
  if (value > t2)
  {
    return 1;
  }
  else
  {
    return 0.5 * (-cos((value - t1) / (t2 - t1) * M_PI) + 1);
  }
}
