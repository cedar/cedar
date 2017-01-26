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

  //! A mapping from a variable name to a value for that variable.
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
      /*! Writes the value to a given stream. This is not meant to parsed again, but exposes the structure of the value
       *  to the human user.
       */
      virtual void writeTo(std::ostream& stream, size_t indentation = 0) const = 0;

      /*! Evaluates the value. The result is a double value.
       */
      virtual double evaluate(const Variables& variables) const = 0;

      /*! Returns a deep copy of the value.
       */
      virtual ValuePtr clone() const = 0;

      /*! Returns a string-representation of the value. The result can be parsed again.
       */
      virtual std::string toString() const = 0;

      /*! Checks, if the value contains the given variable.
       */
      virtual bool contains(const std::string& variable) const = 0;

      /*! Checks if the value is exactly equal to the given variable.
       */
      virtual bool equalsVariable(const std::string& variable) const = 0;

      /*! Simplifies the value, removing any redundant components (e.g., 2/2).
       */
      virtual void simplify() = 0;

      /*! Checks, if the value can be evaluated.
       */
      virtual bool canEvaluate() const = 0;
  };

  /*! Represents a constant value in an arithmetic expression tree.
   */
  class ConstantValue : public Value
  {
    public:
      //! Constructs a ConstantValue object that represents the given value.
      ConstantValue(double value)
      :
      mValue(value)
      {
      }

      //! Writes the constant value to the stream.
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      //! Evaluates the constant value (returns the value).
      double evaluate(const Variables& /* variables */) const
      {
        return mValue;
      }

      //! Always returns false (constant values cannot contain variables).
      bool contains(const std::string& /* variable */) const
      {
        return false;
      }

      //! Always returns false (constant values cannot contain variables).
      bool equalsVariable(const std::string& /* variable */) const
      {
        return false;
      }

      //! Does nothing, constant values are as simple as it gets.
      void simplify()
      {
        // nothing to do -- it doesn't get simpler than a constant value
      }

      //! Always returns true, constant values can always be evaluated.
      bool canEvaluate() const
      {
        // constant values can always be evaluated
        return true;
      }

      //! Copies the value.
      ValuePtr clone() const;

      //! Turns the value into a string.
      std::string toString() const;

      //! The actual value stored in this object.
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

      //! Writes the variable to a stream.
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      double evaluate(const Variables& variables) const;

      ValuePtr clone() const;

      std::string toString() const;

      //! Checks, if the given variable is the one represented by this object.
      bool contains(const std::string& variable) const
      {
        return variable == this->mVariable;
      }

      //! Checks, if the given variable is the one represented by this object.
      bool equalsVariable(const std::string& variable) const
      {
        return variable == this->mVariable;
      }

      //! Plain variables cannot be simplified, thus, this function does nothing.
      void simplify()
      {
        // nothing to do -- it doesn't get simpler than a single variable
      }

      //! Variables can themselves not be evaluated to a double, thus, always returns false.
      bool canEvaluate() const
      {
        // variables can never be evaluated
        return false;
      }

      //! Identifier of the variable.
      std::string mVariable;
  };

  //! Factor node in an arithmetic expression tree.
  class Factor
  {
    public:
      //! Parses a factor from a string.
      void parse(const std::vector<Token>& tokens, size_t& current);

      //! Writes all components of the factor to the stream.
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      //! Evaluates the factor.
      double evaluate(const Variables& variables = (Variables())) const;

      //! Checks if one of the components of the factor contains the variable.
      bool contains(const std::string& variable) const;

      //! Creates a deep copy of the factor.
      FactorPtr clone() const;

      //! Turns the factor into a string.
      std::string toString() const;

      //! Checks, if this factor is equal to the given variable.
      bool equalsVariable(const std::string& variable) const;

      //! Simplifies the expression.
      void simplify();

      //! Checks if the factor can be evaluated.
      bool canEvaluate() const;

      //! Checks if the factor consists of multiple values.
      bool containsExpression() const;

      //! Inverts the factor, making it a division.
      bool mIsDivision;

      //! Value node containing all factors.
      ValuePtr mValue;
  };

  //! Term node in an arithmetic expression tree.
  class Term
  {
    public:
      //! Parses the term node from a string.
      void parse(const std::vector<Token>& tokens, size_t& current);

      //! Writes the terms to the stream.
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      //! Evaluates the term node.
      double evaluate(const Variables& variables = (Variables())) const;

      //! Checks, if one of the terms contains the given variable.
      bool contains(const std::string& variable) const;

      //! Divide the term by the given factor.
      void divideBy(FactorPtr factor);

      //! Multiply the term by the given factor.
      void multiplyBy(FactorPtr factor);

      //! Creates a deep copy of the term.
      TermPtr clone() const;

      //! Creates a string representation of the term.
      std::string toString() const;

      //! Simplifies the term.
      void simplify();

      //! Checks, if the term can be evaluated.
      bool canEvaluate() const;

      //! Checks, if the term contains only one element that is equal to the given variable.
      bool equalsVariable(const std::string& variable) const;

      //! Cancels out the given variable from all terms.
      void cancelDivisive(const std::string& variable);

      //! True, if the term contains no subexpressions
      bool isFlat() const;

      //! Contracts all factors in the term into a single expression
      ExpressionPtr flattenFactors();

      //! Sign of the term.
      double mSign;

      //! Factors contained in this term.
      std::vector<FactorPtr> mFactors;
  };

  //! Expression node in an arithmetic expression tree. Contains an ordered list of terms.
  class Expression : public Value
  {
    public:
      //! Writes the expression to the tree.
      void writeTo(std::ostream& stream, size_t indentation = 0) const;

      //! Evaluates the expression, giving the result as a double.
      double evaluate(const Variables& variables) const;

      //! Parses the expression and all its subnodes from the given string, starting at position @em current.
      void parse(const std::vector<Token>& tokens, size_t& current);

      //! Checks if the expression contains the given variable.
      bool contains(const std::string& variable) const;

      //! Divides the expression by the given factor.
      void divideBy(FactorPtr factor);

      //! Multiplies the expression by the given factor.
      void multiplyBy(FactorPtr factor);

      //! Checks, if the expression is exactly equal to the variable.
      bool equalsVariable(const std::string& variable) const;

      //! Flattens the expression so that it does not contain any sub-expressions
      void flatten();

      //! Simplifies the expression.
      void simplify();

      //! Checks, if the expression can be evaluated.
      bool canEvaluate() const;

      //! Returns a deep copy of the expression.
      ValuePtr clone() const;

      //! Attempt to factorize the given variable.
      ExpressionPtr factorize(const std::string& variable) const;

      //! Converts the expression to a string.
      std::string toString() const;

      //! The terms contained in the expression.
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

  //! Evaluates the expression given the values for all variables
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

  //! Checks if either the left or right side contains only the given variable (without any factors).
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

