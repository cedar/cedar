/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include <vector>
#include <string>


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
  class Expression;
  CEDAR_GENERATE_POINTER_TYPES(Expression);

  typedef std::map<std::string, double> Variables;

  /*! Represents a node in an arithmetic expression tree.
   */
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
      virtual bool equalsVariable(const std::string& variable) const = 0;
      virtual void simplify() = 0;
      virtual bool canEvaluate() const = 0;
  };

  /*! Represents a constant value in an arithmetic expression tree.
   */
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

      bool equalsVariable(const std::string& /* variable */) const
      {
        return false;
      }

      void simplify()
      {
        // nothing to do -- it doesn't get simpler than a constant value
      }

      bool canEvaluate() const
      {
        // constant values can always be evaluated
        return true;
      }

      ValuePtr clone() const;

      std::string toString() const;

      double mValue;
  };

  /*! Represents a variable in an arithmetic expression tree.
   */
  class Variable : public Value
  {
    public:
      //! Constructor that takes the identifier of the variable.
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

      bool equalsVariable(const std::string& variable) const
      {
        return variable == this->mVariable;
      }

      void simplify()
      {
        // nothing to do -- it doesn't get simpler than a single variable
      }

      bool canEvaluate() const
      {
        // variables can never be evaluated
        return false;
      }

      std::string mVariable;
  };

  //! Factor node in an arithmetic expression tree.
  class Factor
  {
    public:
      void parse(const std::vector<Token>& tokens, size_t& current);

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables = (Variables())) const;

      bool contains(const std::string& variable) const;

      FactorPtr clone() const;

      std::string toString() const;

      bool equalsVariable(const std::string& variable) const;

      void simplify();

      bool canEvaluate() const;

      bool containsExpression() const;

      bool mIsDivision;

      ValuePtr mValue;
  };

  //! Term node in an arithmetic expression tree.
  class Term
  {
    public:
      void parse(const std::vector<Token>& tokens, size_t& current);

      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables = (Variables())) const;

      bool contains(const std::string& variable) const;

      void divideBy(FactorPtr factor);

      void multiplyBy(FactorPtr factor);

      TermPtr clone() const;

      std::string toString() const;

      void simplify();

      bool canEvaluate() const;

      bool equalsVariable(const std::string& variable) const;

      void cancelDivisive(const std::string& variable);

      //! True, if the term contains no subexpressions
      bool isFlat() const;

      //! Contracts all factors in the term into a single expression
      ExpressionPtr flattenFactors();

      double mSign;

      std::vector<FactorPtr> mFactors;
  };

  //! Expression node in an arithmetic expression tree.
  class Expression : public Value
  {
    public:
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables) const;

      void parse(const std::vector<Token>& tokens, size_t& current);

      bool contains(const std::string& variable) const;

      void divideBy(FactorPtr factor);

      void multiplyBy(FactorPtr factor);

      bool equalsVariable(const std::string& variable) const;

      //! Flattens the expression so that it does not contain any sub-expressions
      void flatten();

      void simplify();

      bool canEvaluate() const;

      ValuePtr clone() const;

      //! Attempt to factorize the given variable.
      ExpressionPtr factorize(const std::string& variable) const;

      std::string toString() const;

      std::vector<TermPtr> mTerms;
  };

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

  //! Returns the left-hand side of the expression.
  ExpressionPtr getLeft() const
  {
    return this->mLeft;
  }

  //! Returns the right-hand side of the expression.
  ExpressionPtr getRight() const
  {
    return this->mRight;
  }

  //! Solves the equation for the given variable
  ArithmeticExpressionPtr solveFor(const std::string& variable) const;

  //! Writes a detailed version of the equation to the given stream.
  void writeTo(std::ostream& stream, size_t indentation = 0) const;

  //! Converts the equation to a string that can be parsed again.
  std::string toString() const;

  bool isSolvedFor(const std::string& variable) const;

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

