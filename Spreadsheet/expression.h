// expression.h
#pragma once

#include <vector>
#include <string>
#include <functional>

#define NUMBER double

const int kMaxOperand = 2;

enum class TokenType
{
  kError,
  kValue,
  kVariable,
  kBinaryOp,
  kUnaryOp,
  kFunction,
  kParenthesesOpen,
  kParenthesesClose
};

enum class ExprType
{
  kNone,
  kError,
  kValue,
  kVariable,
  kNeg,
  kAdd,
  kSub,
  kMul,
  kDiv,
  kSin,
  kCos
};

enum class ParsingErrorType
{
  kNone,
  kBadNumber,
  kExpectTokenFoundClosingParentheses,
  kExpectClosingParenthesesFoundNothing,
  kUnexpectedClosingParentheses,
  kUnexpectedFunctionArgumenType,
  kUnexpectedUnaryNegArgumenType,
  kUnexpectedBinaryOpArgumenType,
  kExpectOperatorFoundNothing,
  kVariableNotDefined,
  kVariableHasNoValue,
  kNoVariableSourceCollection,
  kRecursion
};

struct Token {
  TokenType type; 
  ExprType exprtype;
  std::wstring value;
};

typedef std::vector<Token> Tokens;

struct Variable {
  std::wstring name;
};

struct ParsingError {
  ParsingErrorType type;
};

struct Expression {
  ExprType type;
  Variable variable;
  union {
    NUMBER value;
    int operand[kMaxOperand];
    ParsingError parsingerror;
  };
};

struct ExpressionResult {
  bool iscalculated;
  NUMBER value;
  ParsingError parsingerror;
};

typedef std::vector<Expression> Expressions;
//TODO(prog.dmitry@gmail.com) Optional result
typedef std::function<ExpressionResult(const std::wstring &varname)> GetVarByNameFunction;

Tokens Tokenizer(const std::wstring &expr_string);

Expressions ParseTokens(const Tokens &tokens);
Expressions Parse(const std::wstring &expr_string);

ExpressionResult Calculate(const Expressions &expressions, GetVarByNameFunction vargetter);
ExpressionResult Calculate(const Expressions &expressions);