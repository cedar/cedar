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

    File:        ArithmeticExpression.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 24

    Description: Header file for the class cedar::aux::ArithmeticExpression.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ARITHMETIC_EXPRESSION_H
#define CEDAR_AUX_ARITHMETIC_EXPRESSION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ArithmeticExpression.fwd.h"

// SYSTEM INCLUDES
#include <map>


/*!@brief A simple class for parsing and evaluating arithmetic expressions from strings.
 */
class cedar::aux::ArithmeticExpression
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct Token
  {
    enum Type {Constant, Variable, Operator};

    std::string token;
    Type type;
  };

public:
  typedef std::map<std::string, double> Variables;

  class Value
  {
  public:
    virtual ~Value()
    {
    }

    public:
      virtual void writeTo(std::ostream& stream, size_t indentation = 0) const = 0;
      virtual double evaluate(const Variables& variables) const = 0;
  };

  CEDAR_GENERATE_POINTER_TYPES(Value);

  class ConstantValue : public Value
  {
    public:
      ConstantValue(double value)
      :
      mValue(value)
      {
      }

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& /* variables */) const
      {
        return mValue;
      }

      double mValue;
  };
  CEDAR_GENERATE_POINTER_TYPES(ConstantValue);

  class Variable : public Value
  {
    public:
      Variable(const std::string& variable)
      :
      mVariable(variable)
      {
      }

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables) const;

      std::string mVariable;
  };
  CEDAR_GENERATE_POINTER_TYPES(Variable);


  class SubExpression : public Value
  {
    public:
      SubExpression();

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables) const;

      ArithmeticExpressionPtr mExpression;
  };
  CEDAR_GENERATE_POINTER_TYPES(SubExpression);

  class Factor
  {
    public:
      void parse(const std::vector<Token>& tokens, size_t& current);

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables = (Variables())) const;

      bool mIsDivision;
    private:
      ValuePtr mValue;
  };
  CEDAR_GENERATE_POINTER_TYPES(Factor);

  class Term
  {
    public:
      void parse(const std::vector<Token>& tokens, size_t& current);

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables = (Variables())) const;

      double mSign;

    private:
      std::vector<FactorPtr> mFactors;
  };
  CEDAR_GENERATE_POINTER_TYPES(Term);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ArithmeticExpression();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void parse(const std::string& expression);

  //!@todo Return mat, use mat in variables?
  double evaluate(const Variables& variables = (Variables())) const;

  void writeTo(std::ostream& stream, size_t indentation = 0) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  std::vector<Token> tokenize(const std::string& expression) const;

  void parse(const std::vector<Token>& tokens, size_t& current);

  static bool isValidVariableCharacter(char c);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<TermPtr> mTerms;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ArithmeticExpression

#endif // CEDAR_AUX_ARITHMETIC_EXPRESSION_H

