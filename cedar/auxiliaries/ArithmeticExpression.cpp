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

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::ArithmeticExpression::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');
  stream << indent << "[ Expression" << std::endl;

  for (const auto& term : this->mTerms)
  {
    term->writeTo(stream, indentation + 1);
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

double cedar::aux::ArithmeticExpression::SubExpression::evaluate(const Variables& variables) const
{
  return this->mExpression->evaluate(variables);
}

void cedar::aux::ArithmeticExpression::SubExpression::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ SubExpression" << std::endl;

  this->mExpression->writeTo(stream, indentation + 1);

  stream << indent << "]" << std::endl;
}

void cedar::aux::ArithmeticExpression::ConstantValue::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ ";
  stream << "Constant: " << this->mValue;
  stream << " ]" << std::endl;
}

double cedar::aux::ArithmeticExpression::Variable::evaluate(const Variables& variables) const
{
  auto iter = variables.find(this->mVariable);
  CEDAR_ASSERT(iter != variables.end());
  return iter->second;
}

void cedar::aux::ArithmeticExpression::Variable::writeTo(std::ostream& stream, size_t indentation) const
{
  std::string indent(2*indentation, ' ');

  stream << indent << "[ ";
  stream << "Variable: " << this->mVariable;
  stream << " ]" << std::endl;
}

double cedar::aux::ArithmeticExpression::evaluate(const Variables& variables) const
{
  double result = 0.0;

  for (auto term : this->mTerms)
  {
    result += term->mSign * term->evaluate(variables);
  }

  return result;
}

void cedar::aux::ArithmeticExpression::parse
(
  const std::string& expression
)
{

  // tokenize
  std::vector<Token> tokens = this->tokenize(expression);

  // process tokens into tree
  size_t current = 0;
  this->parse(tokens, current);
}

void cedar::aux::ArithmeticExpression::parse(const std::vector<Token>& tokens, size_t& current)
{
  /* The Grammar we're following here:
   *
   * expression = term ('+' term | '-' term)*
   * term = factor ('*' factor | '/' factor)*
   * factor = number | variable | '(' expression ')'
   */
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
        else if (token.token == ")")
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

void cedar::aux::ArithmeticExpression::Term::parse(const std::vector<Token>& tokens, size_t& current)
{
  /* The Grammar we're following here:
   *
   * expression = term ('+' term | '-' term)*
   * term = factor ('*' factor | '/' factor)*
   * factor = number | variable | '(' expression ')'
   */
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

double cedar::aux::ArithmeticExpression::Factor::evaluate(const Variables& variables) const
{
  return this->mValue->evaluate(variables);
}

void cedar::aux::ArithmeticExpression::Factor::parse(const std::vector<Token>& tokens, size_t& current)
{
  /* The Grammar we're following here:
   *
   * expression = term ('+' term | '-' term)*
   * term = factor ('*' factor | '/' factor)*
   * factor = number | variable | '(' expression ')'
   */
  const auto& token = tokens.at(current);

  switch (token.type)
  {
    case Token::Operator:
    {
      if (token.token == "(")
      {
        ++current;
        SubExpressionPtr expression(new SubExpression());
        this->mValue  = expression;
        expression->mExpression->parse(tokens, current);
        CEDAR_ASSERT(current < tokens.size())
        CEDAR_ASSERT(tokens.at(current).type == Token::Operator);
        CEDAR_ASSERT(tokens.at(current).token == ")");
        ++current;
        break;
      }
      else
      {
        // TODO
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

cedar::aux::ArithmeticExpression::SubExpression::SubExpression()
:
mExpression(new cedar::aux::ArithmeticExpression())
{
}

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
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            break;

          case '+':
          case '-':
          case '(':
          case ')':
          case '*':
          case '/':
          {
            Token op;
            op.token = c;
            op.type = Token::Operator;
            tokens.push_back(op);
            current_token = Token();
            break;
          }

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
          case '.':
            current_token.token += c;
            break;

          case ' ':
          case '\r':
          case '\n':
          case '\t':
            // constant over, go to nothing state and push current token
            tokens.push_back(current_token);
            current_token = Token();
            break;


          case '+':
          case '-':
          case '(':
          case ')':
          case '*':
          case '/':
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
              //!@todo Proper exception: cannot handle anything but numbers in constants
              CEDAR_ASSERT(false); // tokenizer error
            }
            else
            {
              CEDAR_ASSERT(state == InVariable);

              //!@todo Proper exception
              CEDAR_ASSERT(isValidVariableCharacter(c));

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
  if (c > 'a' && c < 'z')
  {
    return true;
  }
  // test for upper-case letter
  if (c > 'A' && c < 'Z')
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
