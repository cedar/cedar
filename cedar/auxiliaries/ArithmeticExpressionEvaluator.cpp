/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ArithmeticExpressionEvaluator.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 24

    Description: Source file for the class cedar::aux::ArithmeticExpressionEvaluator.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ArithmeticExpressionEvaluator.h"

// SYSTEM INCLUDES
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_utree.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <iostream>
#include <string>

#if BOOST_PHOENIX_VERSION == 0x2000
namespace boost { namespace phoenix
{
    // There's a bug in the Phoenix V2 type deduction mechanism that prevents
    // correct return type deduction for for the math operations below. Newer
    // versions of Phoenix will be switching to BOOST_TYPEOF. In the meantime,
    // we will use the specializations helping with return type deduction
    // below:
    template <>
    struct result_of_plus<spirit::utree&, spirit::utree&>
    {
        typedef spirit::utree type;
    };

    template <>
    struct result_of_minus<spirit::utree&, spirit::utree&>
    {
        typedef spirit::utree type;
    };

    template <>
    struct result_of_multiplies<spirit::utree&, spirit::utree&>
    {
        typedef spirit::utree type;
    };

    template <>
    struct result_of_divides<spirit::utree&, spirit::utree&>
    {
        typedef spirit::utree type;
    };

    template <>
    struct result_of_negate<spirit::utree&>
    {
        typedef spirit::utree type;
    };
}}
#endif

namespace client
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace spirit = boost::spirit;

    template <typename Iterator>
    struct calculator : qi::grammar<Iterator, ascii::space_type, spirit::utree()>
    {
        calculator(const std::map<std::string, double>& variables) : calculator::base_type(expression)
        {
            using qi::uint_;
            using qi::lit;
            using qi::_val;
            using qi::_1;

            expression =
                term                            [_val = _1]
                >> *(   ('+' >> term            [_val = _val + _1])
                    |   ('-' >> term            [_val = _val - _1])
                    )
                ;

            term =
                factor                          [_val = _1]
                >> *(   ('*' >> factor          [_val = _val * _1])
                    |   ('/' >> factor          [_val = _val / _1])
                    )
                ;

            factor =
                uint_                           [_val = _1]
                | variable                      [_val = _1]
                |   '(' >> expression           [_val = _1] >> ')'
                |   ('-' >> factor              [_val = -_1])
                |   ('+' >> factor              [_val = _1])
                ;

            for (const auto& variable_value_pair : variables)
            {
              variable %= lit(variable_value_pair.first) [_val = variable_value_pair.second];
            }


            BOOST_SPIRIT_DEBUG_NODE(expression);
            BOOST_SPIRIT_DEBUG_NODE(term);
            BOOST_SPIRIT_DEBUG_NODE(factor);
        }

        qi::rule<Iterator, ascii::space_type, spirit::utree()> expression, term, factor, variable;
    };
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ArithmeticExpressionEvaluator::ArithmeticExpressionEvaluator()
//:
//mGrammar(new cedar::aux::ArithmeticExpressionEvaluator::CalculatorGrammar())
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::aux::ArithmeticExpressionEvaluator::evaluate
(
  const std::string& expression,
  const std::map<std::string, double>& variables
) const
{
  using boost::spirit::ascii::space;
  using boost::spirit::utree;
  typedef std::string::const_iterator iterator_type;
  typedef client::calculator<iterator_type> calculator;

  calculator calc(variables); // Our grammar
  std::string::const_iterator iter = expression.begin();

  std::string::const_iterator end = expression.end();
  utree ut;
  bool r = phrase_parse(iter, end, calc, space, ut);


  if (r && iter == end)
  {
      std::cout << "-------------------------\n";
      std::cout << "Parsing succeeded: " << ut << "\n";
      std::cout << "-------------------------\n";
      return ut.get<double>();
  }
  else
  {
      std::string rest(iter, end);
      std::cout << "-------------------------\n";
      std::cout << "Parsing failed\n";
      std::cout << "stopped at: \": " << rest << "\"\n";
      std::cout << "-------------------------\n";
  }

  return 0.0;
}
