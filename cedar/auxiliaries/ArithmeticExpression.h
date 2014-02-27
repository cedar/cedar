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
 *
 *        The Grammar parsed by this class is:
 *
 *        arithmetic expression = expression ('=' expression)?
 *        expression = term ('+' term | '-' term)*
 *        term = factor ('*' factor | '/' factor)*
 *        factor = double | variable | '(' expression ')'
 *
 *        where 'c' means a literal c, * means zero or more repetitions and (A|B) means A or B.
 *
 * @todo This would be solved much nicer with boost spirit, but I could not get that to work.
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
  class Term;
  CEDAR_GENERATE_POINTER_TYPES(Term);
  class Value;
  CEDAR_GENERATE_POINTER_TYPES(Value);
  class Variable;
  CEDAR_GENERATE_POINTER_TYPES(Variable);
  class Factor;
  CEDAR_GENERATE_POINTER_TYPES(Factor);
  class ConstantValue;
  CEDAR_GENERATE_POINTER_TYPES(ConstantValue);

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
      virtual ValuePtr clone() const = 0;
      virtual std::string toString() const = 0;
      virtual bool contains(const std::string& variable) const = 0;
  };

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

      bool contains(const std::string& /* variable */) const
      {
        return false;
      }

      ValuePtr clone() const;

      std::string toString() const;

      double mValue;
  };

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

      ValuePtr clone() const;

      std::string toString() const;

      bool contains(const std::string& variable) const
      {
        return variable == this->mVariable;
      }

      std::string mVariable;
  };


  class Factor
  {
    public:
      void parse(const std::vector<Token>& tokens, size_t& current);

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables = (Variables())) const;

      bool contains(const std::string& variable) const;

      FactorPtr clone() const;

      std::string toString() const;

      bool mIsDivision;
    private:
      ValuePtr mValue;
  };

  class Term
  {
    public:
      void parse(const std::vector<Token>& tokens, size_t& current);

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables = (Variables())) const;

      bool contains(const std::string& variable) const;

      void divideBy(FactorPtr factor);

      TermPtr clone() const;

      std::string toString() const;

      double mSign;

      std::vector<FactorPtr> mFactors;
  };

  class Expression : public Value
  {
    public:
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables) const;

      void parse(const std::vector<Token>& tokens, size_t& current);

      bool contains(const std::string& variable) const;

      void divideBy(FactorPtr factor);

      ValuePtr clone() const;

      std::string toString() const;

      std::vector<TermPtr> mTerms;
  };
  CEDAR_GENERATE_POINTER_TYPES(Expression);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ArithmeticExpression();

  //! Constructor that takes an expression and parses it immediately.
  ArithmeticExpression(const std::string& expression);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Parses the given expression.
  void parse(const std::string& expression);

  //! Removes the contents of the equation.
  void clear();

  //!@todo Return mat, use mat in variables?
  double evaluate(const Variables& variableValues = (Variables())) const;

  //! Solves the equation for the given variable
  ArithmeticExpressionPtr solveFor(const std::string& variable) const;

  //! Writes a detailed version of the equation to the given stream.
  void writeTo(std::ostream& stream, size_t indentation = 0) const;

  //! Converts the equation to a string that can be parsed again.
  std::string toString() const;

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
  //! Left-hand side of the equation
  ExpressionPtr mLeft;

  //! Right-hand side of the equation. May be empty.
  ExpressionPtr mRight;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ArithmeticExpression

#endif // CEDAR_AUX_ARITHMETIC_EXPRESSION_H

