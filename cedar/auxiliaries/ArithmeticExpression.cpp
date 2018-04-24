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

    File:        ArithmeticExpression.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 24

    Description: Source file for the class cedar::aux::ArithmeticExpression.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ArithmeticExpression.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <iostream>
#include <string>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ArithmeticExpression::ArithmeticExpression()
{
}

cedar::aux::ArithmeticExpression::ArithmeticExpression(const std::string& expression)
{
  this->parse(expression);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::ArithmeticExpression::Expression::equalsVariable(const std::string& variable) const
{
  if (this->mTerms.size() != 1)
  {
    return false;
  }

  return this->mTerms.at(0)->equalsVariable(variable);
}

bool cedar::aux::ArithmeticExpression::isSolvedFor(const std::string& variable) const
{
  // To solve equations, we need a left and right side
  if (!this->mLeft || !this->mRight)
  {
    return false;
  }

  bool variable_on_left = this->mLeft->contains(variable);
  bool variable_on_right = this->mRight->contains(variable);

  // If the variable is on both sides, the equation is not solved for it
  if (variable_on_left && variable_on_right)
  {
    return false;
  }

  if (variable_on_right)
  {
    return this->mRight->equalsVariable(variable);
  }
  else
  {
    return this->mLeft->equalsVariable(variable);
  }
}

cedar::aux::ArithmeticExpressionPtr cedar::aux::ArithmeticExpression::solveFor(const std::string& variable) const
{
  // To solve equations, we need a left and right side
  if (!this->mLeft && !this->mRight)
  {
    CEDAR_THROW(cedar::aux::ArithmeticExpressionException, "Cannot solve equation: need both a left- and righ hand side.");
  }

  auto left = boost::dynamic_pointer_cast<Expression>(this->mLeft->clone());
  auto right = boost::dynamic_pointer_cast<Expression>(this->mRight->clone());

  left->flatten();
  right->flatten();

  ArithmeticExpressionPtr result(new ArithmeticExpression());

  result->mLeft = ExpressionPtr(new Expression());
  result->mRight = ExpressionPtr(new Expression());

  ExpressionPtr expressions [] = {left, right};

  // sort terms: put terms containing the variable to be solved for on the left, others on the right
  for (size_t i = 0; i < 2; ++i)
  {
    ExpressionPtr expression = expressions[i];
    for (auto term : expression->mTerms)
    {
      auto clone = term->clone();
      if (term->contains(variable))
      {
        result->mLeft->mTerms.push_back(clone);

        if (expression == right)
        {
          clone->mSign = -1.0;
        }
      }
      else
      {
        result->mRight->mTerms.push_back(clone);

        if (expression == left)
        {
          clone->mSign = -1.0;
        }
      }
    }
  }

  // if the left side has more than one term, see if we can factorise it
  if (result->mLeft->mTerms.size() > 1)
  {
    ExpressionPtr factorized = result->mLeft->factorize(variable);

    result->mLeft = factorized;
  }

  // if the only term on the left is one containing the variable ...
  if (result->mLeft->mTerms.size() == 1)
  {
    auto term = result->mLeft->mTerms.at(0);
    // ... check if there are some factors in front of it
    if (term->mFactors.size() > 1)
    {
      for (auto factor_iter = term->mFactors.begin(); factor_iter != term->mFactors.end(); )
      {
        auto factor = *factor_iter;
        if (!factor->contains(variable))
        {
          result->mRight->divideBy(factor);
          factor_iter = term->mFactors.erase(factor_iter);
        }
        else
        {
          ++factor_iter;
        }
      }
    }

    // ... check if the sign needs to be flipped
    if (term->mSign < 0)
    {
      term->mSign = +1.0;

      for (const auto& term : result->mRight->mTerms)
      {
        term->mSign *= -1.0;
      }
    }
  }

  // Finally, make the equations a bit more simple
  result->mLeft->simplify();
  result->mRight->simplify();

  return result;
}

std::string cedar::aux::ArithmeticExpression::toString() const
{
  std::string result;
  if (this->mLeft)
  {
    result += this->mLeft->toString();
  }
  if (this->mRight)
  {
    result += " = ";
    result += this->mRight->toString();
  }

  return result;
}

void cedar::aux::ArithmeticExpression::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');
  stream << indent << "[ ArithmeticExpression" << std::endl;

  if (this->mLeft)
  {
    stream << indent << "[ Left" << std::endl;
    this->mLeft->writeTo(stream, indentation + 1);
    stream << indent << "]" << std::endl;
  }

  if (this->mRight)
  {
    stream << indent << "[ Right" << std::endl;
    this->mRight->writeTo(stream, indentation + 1);
    stream << indent << "]" << std::endl;
  }

  stream << indent << "]" << std::endl;
}

void cedar::aux::ArithmeticExpression::Term::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ Term: " << (this->mSign > 0? "+" : "-") << std::endl;

  for (const auto& factor : this->mFactors)
  {
    factor->writeTo(stream, indentation + 1);
  }

  stream << indent << "]" << std::endl;
}

void cedar::aux::ArithmeticExpression::Factor::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ Factor " << (this->mIsDivision? "/" : "*")<< std::endl;

  this->mValue->writeTo(stream, indentation + 1);

  stream << indent << "]" << std::endl;
}

void cedar::aux::ArithmeticExpression::clear()
{
  this->mLeft.reset();
  this->mRight.reset();
}

double cedar::aux::ArithmeticExpression::evaluate(const Variables& variables) const
{
  // cannot evaluate if the equation has two sides because more information is needed
  if (!this->mLeft)
  {
    CEDAR_THROW(cedar::aux::ArithmeticExpressionException, "Cannot evaluate: need a left-hand side.");
  }
  if (this->mRight)
  {
    CEDAR_THROW(cedar::aux::ArithmeticExpressionException, "Cannot evaluate if a right-hand side is present.");
  }
  return this->mLeft->evaluate(variables);
}

void cedar::aux::ArithmeticExpression::Expression::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ Expression" << std::endl;

  for (auto term : this->mTerms)
  {
    term->writeTo(stream, indentation + 1);
  }

  stream << indent << "]" << std::endl;
}

void cedar::aux::ArithmeticExpression::ConstantValue::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ ";
  stream << "Constant: " << this->mValue;
  stream << " ]" << std::endl;
}

std::string cedar::aux::ArithmeticExpression::ConstantValue::toString() const
{
  return cedar::aux::toString(this->mValue);
}

cedar::aux::ArithmeticExpression::ValuePtr cedar::aux::ArithmeticExpression::ConstantValue::clone() const
{
  ConstantValuePtr clone(new ConstantValue(this->mValue));
  return clone;
}

cedar::aux::ArithmeticExpression::ValuePtr cedar::aux::ArithmeticExpression::Variable::clone() const
{
  VariablePtr clone(new Variable(this->mVariable));
  return clone;
}

std::string cedar::aux::ArithmeticExpression::Variable::toString() const
{
  return this->mVariable;
}

double cedar::aux::ArithmeticExpression::Variable::evaluate(const Variables& variables) const
{
  auto iter = variables.find(this->mVariable);
  if (iter == variables.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Cannot evaluate variable \"" + this->mVariable + "\". No value specified for it.");
  }
  return iter->second;
}

void cedar::aux::ArithmeticExpression::Variable::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ ";
  stream << "Variable: " << this->mVariable;
  stream << " ]" << std::endl;
}

cedar::aux::ArithmeticExpression::ExpressionPtr
  cedar::aux::ArithmeticExpression::Expression::factorize(const std::string& variable) const
{
  ExpressionPtr factorized(new Expression());

  TermPtr factorized_term(new Term());
  factorized->mTerms.push_back(factorized_term);

  FactorPtr variable_factor(new Factor());
  variable_factor->mValue = VariablePtr(new Variable(variable));
  factorized_term->mFactors.push_back(variable_factor);

  ExpressionPtr sub_terms(new Expression());
  FactorPtr sub_terms_factor(new Factor());
  sub_terms_factor->mValue = sub_terms;
  factorized_term->mFactors.push_back(sub_terms_factor);
  for (auto term : this->mTerms)
  {
    auto sub_term = term->clone();
    sub_term->cancelDivisive(variable);
    sub_terms->mTerms.push_back(sub_term);
  }

  return factorized;
}

void cedar::aux::ArithmeticExpression::Term::cancelDivisive(const std::string& variable)
{
  // try to find a factor that is the variable
  for (auto factor_it = this->mFactors.begin(); factor_it != this->mFactors.end(); ++factor_it)
  {
    auto factor = *factor_it;
    if (factor->equalsVariable(variable))
    {
      // if the factor contains the variable, take it out
      this->mFactors.erase(factor_it);
      return;
    }
  }

  // if none of the factors contains the variable, we cannot cancel it; add it as a divisive factor
  FactorPtr div_factor(new Factor());
  div_factor->mIsDivision = true;
  div_factor->mValue = VariablePtr(new Variable(variable));
  this->mFactors.push_back(div_factor);
}

void cedar::aux::ArithmeticExpression::Expression::flatten()
{
  std::vector<ExpressionPtr> new_expressions;
  std::vector<double> new_signs;
  for (auto term_it = this->mTerms.begin(); term_it != this->mTerms.end(); )
  {
    auto term = *term_it;
    if (!term->isFlat())
    {
      // flatten the term, take it out and queue it to be re-added later
      ExpressionPtr flattened = term->flattenFactors();
      new_expressions.push_back(flattened);
      new_signs.push_back(term->mSign);
      term_it = this->mTerms.erase(term_it);
    }
    else
    {
      ++term_it;
    }
  }

  // re-add terms that were taken out
  CEDAR_DEBUG_ASSERT(new_expressions.size() == new_signs.size());
  for (size_t i = 0; i < new_expressions.size(); ++i)
  {
    ExpressionPtr expr = new_expressions.at(i);
    double sign = new_signs.at(i);
    for (auto term : expr->mTerms)
    {
      term->mSign *= sign;
      this->mTerms.push_back(term);
    }
  }
}

cedar::aux::ArithmeticExpression::ExpressionPtr cedar::aux::ArithmeticExpression::Term::flattenFactors()
{
  size_t first_expression_index;
  for (first_expression_index = 0; first_expression_index < this->mFactors.size(); ++first_expression_index)
  {
    if (this->mFactors.at(first_expression_index)->containsExpression())
    {
      break;
    }
  }

  auto first_expression = boost::dynamic_pointer_cast<Expression>(this->mFactors.at(first_expression_index)->mValue->clone());

  for (size_t i = 0; i < this->mFactors.size(); ++i)
  {
    if (i != first_expression_index)
    {
      first_expression->multiplyBy(this->mFactors.at(i));
    }
  }
  return first_expression;
}


bool cedar::aux::ArithmeticExpression::Term::isFlat() const
{
  for (auto factor : this->mFactors)
  {
    if (factor->containsExpression())
    {
      return false;
    }
  }
  return true;
}

bool cedar::aux::ArithmeticExpression::Factor::containsExpression() const
{
  return static_cast<bool>(boost::dynamic_pointer_cast<Expression>(this->mValue));
}


cedar::aux::ArithmeticExpression::ValuePtr cedar::aux::ArithmeticExpression::Expression::clone() const
{
  ExpressionPtr clone(new Expression());

  for (auto term : this->mTerms)
  {
    clone->mTerms.push_back(term->clone());
  }

  return clone;
}

void cedar::aux::ArithmeticExpression::Expression::simplify()
{
  for (auto term : this->mTerms)
  {
    term->simplify();
  }

  ConstantValuePtr constants(new ConstantValue(0.0));

  for (auto iter = this->mTerms.begin(); iter != this->mTerms.end(); )
  {
    auto term = *iter;
    if (term->canEvaluate())
    {
      double value = term->mSign * term->evaluate();
      constants->mValue += value;
      iter = this->mTerms.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
  if (constants->mValue != 0.0)
  {
    FactorPtr new_factor(new Factor());
    new_factor->mValue = constants;
    TermPtr new_term(new Term());
    new_term->mSign = +1.0;
    new_term->mFactors.push_back(new_factor);
    this->mTerms.push_back(new_term);
  }
}

void cedar::aux::ArithmeticExpression::Factor::simplify()
{
  this->mValue->simplify();
}

void cedar::aux::ArithmeticExpression::Term::simplify()
{
  // first, simplify all factors
  for (auto factor : this->mFactors)
  {
    factor->simplify();
  }

  // then, merge together all constant factors
  ConstantValuePtr constants(new ConstantValue(1.0));
  for (auto iter = this->mFactors.begin(); iter != this->mFactors.end(); )
  {
    auto factor = *iter;
    if (factor->canEvaluate())
    {
      if (factor->mIsDivision)
      {
        constants->mValue /= factor->evaluate();
      }
      else
      {
        constants->mValue *= factor->evaluate();
      }
      iter = this->mFactors.erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  if (constants->mValue != 1.0 || this->mFactors.empty())
  {
    FactorPtr new_factor(new Factor());
    new_factor->mValue = constants;
    this->mFactors.push_back(new_factor);
  }
}

bool cedar::aux::ArithmeticExpression::Factor::canEvaluate() const
{
  return this->mValue->canEvaluate();
}

bool cedar::aux::ArithmeticExpression::Expression::canEvaluate() const
{
  for (auto term : this->mTerms)
  {
    if (!term->canEvaluate())
    {
      return false;
    }
  }

  return true;
}


bool cedar::aux::ArithmeticExpression::Term::canEvaluate() const
{
  for (auto factor : this->mFactors)
  {
    if (!factor->canEvaluate())
    {
      return false;
    }
  }

  return true;
}

void cedar::aux::ArithmeticExpression::Expression::divideBy(FactorPtr factor)
{
  for (auto term : this->mTerms)
  {
    term->divideBy(factor);
  }
}

void cedar::aux::ArithmeticExpression::Expression::multiplyBy(FactorPtr factor)
{
  for (auto term : this->mTerms)
  {
    term->multiplyBy(factor);
  }
}

void cedar::aux::ArithmeticExpression::Term::divideBy(FactorPtr factor)
{
  FactorPtr clone = factor->clone();

  // Flip the division state of the term (because we are dividing by it)
  clone->mIsDivision = !clone->mIsDivision;

  this->mFactors.push_back(clone);
}

void cedar::aux::ArithmeticExpression::Term::multiplyBy(FactorPtr factor)
{
  FactorPtr clone = factor->clone();
  this->mFactors.push_back(clone);
}

double cedar::aux::ArithmeticExpression::Expression::evaluate(const Variables& variableValues) const
{
  double result = 0.0;

  for (auto term : this->mTerms)
  {
    result += term->mSign * term->evaluate(variableValues);
  }

  return result;
}

bool cedar::aux::ArithmeticExpression::Expression::contains(const std::string& variable) const
{
  for (auto term : this->mTerms)
  {
    if (term->contains(variable))
    {
      return true;
    }
  }

  return false;
}

void cedar::aux::ArithmeticExpression::parse(const std::string& expression)
{
  // remove previous expression content
  this->clear();

  // tokenize
  std::vector<Token> tokens = this->tokenize(expression);

  // process tokens into tree
  size_t current = 0;
  this->parse(tokens, current);
}

void cedar::aux::ArithmeticExpression::parse(const std::vector<Token>& tokens, size_t& current)
{
  bool in_rhs = false;
  while (current < tokens.size())
  {
    if (tokens.at(current).type == Token::Operator && tokens.at(current).token == "=")
    {
      in_rhs = true;
      ++current;
    }
    else if (!in_rhs)
    {
      if (!this->mLeft)
      {
        this->mLeft = ExpressionPtr(new Expression());
      }
      this->mLeft->parse(tokens, current);
    }
    else
    {
      if (!this->mRight)
      {
        this->mRight = ExpressionPtr(new Expression());
      }
      this->mRight->parse(tokens, current);
    }
  }
}

std::string cedar::aux::ArithmeticExpression::Expression::toString() const
{
  std::string result;
  bool first = true;
  if (this->mTerms.empty())
  {
    return "0";
  }
  else
  {
    for (const auto& term : this->mTerms)
    {
      if (first)
      {
        first = false;

        if (term->mSign < 0)
        {
          result += "-";
        }
      }
      else
      {
        if (term->mSign > 0)
        {
          result += " + ";
        }
        else
        {
          result += " - ";
        }
      }
      result += term->toString();
    }
  }

  return result;
}

void cedar::aux::ArithmeticExpression::Expression::parse(const std::vector<Token>& tokens, size_t& current)
{
  double next_sign = +1.0;
  while (current < tokens.size())
  {
    const auto& token = tokens.at(current);

    switch (token.type)
    {
      case Token::Operator:
      {
        if (token.token == "+")
        {
          next_sign = +1;
          ++current;
          break;
        }
        else if (token.token == "-")
        {
          next_sign = -1;
          ++current;
          break;
        }
        else if (token.token == ")" || token.token == "=")
        {
          return;
        }
        // there is no break here on purpose
      }

      default:
      {
        TermPtr term(new Term());
        this->mTerms.push_back(term);
        term->mSign = next_sign;
        term->parse(tokens, current);
      }
    }
  }
}

cedar::aux::ArithmeticExpression::TermPtr cedar::aux::ArithmeticExpression::Term::clone() const
{
  TermPtr clone(new Term());
  clone->mSign = this->mSign;

  for (auto factor : this->mFactors)
  {
    clone->mFactors.push_back(factor->clone());
  }
  return clone;
}

double cedar::aux::ArithmeticExpression::Term::evaluate(const Variables& variables) const
{
  double result = 1.0;

  for (auto factor : this->mFactors)
  {
    if (factor->mIsDivision)
    {
      result /= factor->evaluate(variables);
    }
    else
    {
      result *= factor->evaluate(variables);
    }
  }

  return result;
}

bool cedar::aux::ArithmeticExpression::Term::contains(const std::string& variable) const
{
  for (auto factor : this->mFactors)
  {
    if (factor->contains(variable))
    {
      return true;
    }
  }

  return false;
}

bool cedar::aux::ArithmeticExpression::Term::equalsVariable(const std::string& variable) const
{
  if (this->mFactors.size() != 1)
  {
    return false;
  }

  return this->mFactors.at(0)->equalsVariable(variable);
}

std::string cedar::aux::ArithmeticExpression::Term::toString() const
{
  std::string result;

  bool first = true;
  for (const auto& factor : this->mFactors)
  {
    if (first)
    {
      first = false;
    }
    else
    {
      if (factor->mIsDivision)
      {
        result += " / ";
      }
      else
      {
        result += " * ";
      }
    }

    result += factor->toString();
  }

  return result;
}

void cedar::aux::ArithmeticExpression::Term::parse(const std::vector<Token>& tokens, size_t& current)
{
  bool next_is_division = false;
  while (current < tokens.size())
  {
    const auto& token = tokens.at(current);

    switch (token.type)
    {
      case Token::Operator:
      {
        if (token.token == "*")
        {
          next_is_division = false;
          ++current;
          break;
        }
        else if (token.token == "/")
        {
          next_is_division = true;
          ++current;
          break;
        }
        else if (token.token != "(")
        {
          return;
        }

        // no break on purpose
      }

      default:
      {
        FactorPtr factor(new Factor());
        factor->mIsDivision = next_is_division;
        this->mFactors.push_back(factor);
        factor->parse(tokens, current);
      }
    }
  }
}

cedar::aux::ArithmeticExpression::FactorPtr cedar::aux::ArithmeticExpression::Factor::clone() const
{
  FactorPtr clone(new Factor());
  clone->mIsDivision = this->mIsDivision;
  clone->mValue = this->mValue->clone();
  return clone;
}

double cedar::aux::ArithmeticExpression::Factor::evaluate(const Variables& variables) const
{
  return this->mValue->evaluate(variables);
}

bool cedar::aux::ArithmeticExpression::Factor::contains(const std::string& variable) const
{
  return this->mValue->contains(variable);
}

bool cedar::aux::ArithmeticExpression::Factor::equalsVariable(const std::string& variable) const
{
  return this->mValue->equalsVariable(variable);
}

std::string cedar::aux::ArithmeticExpression::Factor::toString() const
{
  std::string result;
  if (boost::dynamic_pointer_cast<Expression>(this->mValue))
  {
    result += "(";
    result += this->mValue->toString();
    result += ")";
  }
  else
  {
    result += this->mValue->toString();
  }

  return result;
}

void cedar::aux::ArithmeticExpression::Factor::parse(const std::vector<Token>& tokens, size_t& current)
{
  const auto& token = tokens.at(current);

  switch (token.type)
  {
    case Token::Operator:
    {
      if (token.token == "(")
      {
        ++current;
        ExpressionPtr expression(new Expression());
        this->mValue  = expression;
        expression->parse(tokens, current);
        CEDAR_ASSERT(current < tokens.size())
        CEDAR_ASSERT(tokens.at(current).type == Token::Operator);
        CEDAR_ASSERT(tokens.at(current).token == ")");
        ++current;
        break;
      }
    }

    case Token::Constant:
    {
      ValuePtr value(new ConstantValue(cedar::aux::fromString<double>(token.token)));
      this->mValue = value;
      ++current;
      break;
    }

    case Token::Variable:
    {
      ValuePtr value(new Variable(token.token));
      this->mValue = value;
      ++current;
      break;
    }

    default:
      CEDAR_ASSERT(false);
  }
}

#define CASE_WHITE_SPACE \
          case ' ': \
          case '\t': \
          case '\r': \
          case '\n'

#define CASE_OPERATOR \
          case '+': \
          case '-': \
          case '(': \
          case ')': \
          case '*': \
          case '/': \
          case '='

#define CASE_NUMBER \
          case '0': \
          case '1': \
          case '2': \
          case '3': \
          case '4': \
          case '5': \
          case '6': \
          case '7': \
          case '8': \
          case '9'

std::vector<cedar::aux::ArithmeticExpression::Token>
  cedar::aux::ArithmeticExpression::tokenize(const std::string& expression) const
{
  enum TokenizerState
  {
    Nothing,
    InConstant,
    InVariable
  };

  std::string::const_iterator iter = expression.begin();

  std::string::const_iterator end = expression.end();

  std::vector<Token> tokens;

  TokenizerState state = Nothing;

  Token current_token;

  while (iter != end)
  {
    char c = *iter;
    ++iter;

    switch (state)
    {
      case Nothing:
      {
        switch (c)
        {
          // whitespace is ignored in this state
          CASE_WHITE_SPACE:
            break;

          CASE_OPERATOR:
          {
            Token op;
            op.token = c;
            op.type = Token::Operator;
            tokens.push_back(op);
            current_token = Token();
            break;
          }

          CASE_NUMBER:
            state = InConstant;
            current_token.token += c;
            current_token.type = Token::Constant;
            break;

          default:
            CEDAR_ASSERT(isValidVariableCharacter(c));
            state = InVariable;
            current_token.token += c;
            current_token.type = Token::Variable;
        } // switch (c)
        break;
      } // In_Expression

      case InConstant:
      case InVariable:
      {
        switch (c)
        {
          CASE_NUMBER:
          case '.':
            current_token.token += c;
            break;

          CASE_WHITE_SPACE:
            // constant over, go to nothing state and push current token
            tokens.push_back(current_token);
            current_token = Token();
            break;


          CASE_OPERATOR:
          {
            // push back the current token
            if (!current_token.token.empty())
            {
              tokens.push_back(current_token);
            }

            // add a token for the operator
            Token op;
            op.token = c;
            op.type = Token::Operator;
            tokens.push_back(op);

            // return to the Nothing state
            current_token = Token();
            state = Nothing;
            break;
          }

          default:
            if (state == InConstant)
            {
              CEDAR_THROW(cedar::aux::ArithmeticExpressionException, "Cannot evaluate if a right-hand side is present.");
            }
            else
            {
              CEDAR_ASSERT(state == InVariable);

              if (!isValidVariableCharacter(c))
              {
                std::string c_str;
                c_str += c;
                CEDAR_THROW(cedar::aux::InvalidValueException, "Error parsing equation: '" + c_str + "' is not a valid character for variables.");
              }

              current_token.token += c;
            }
            break;
        } // switch (c)
      } // InConstant
    }
  } // while

  if (!current_token.token.empty())
  {
    tokens.push_back(current_token);
  }

  return tokens;
}


bool cedar::aux::ArithmeticExpression::isValidVariableCharacter(char c)
{
  // test for lower-case letter
  if (c >= 'a' && c <= 'z')
  {
    return true;
  }
  // test for upper-case letter
  if (c >= 'A' && c <= 'Z')
  {
    return true;
  }

  switch (c)
  {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '_':
      return true;
  }

  return false;
}
