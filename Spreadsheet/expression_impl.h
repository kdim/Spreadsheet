// expression_impl.h
#pragma once

struct FunctionDescription {
  std::wstring name;
  ExprType exprtype;
};

const FunctionDescription kFunctionList[] = {
  { L"sin", ExprType::kSin },
  { L"cos", ExprType::kCos }
};

const int kFunctionMaxLength = 10;

struct OperatorDescription {
  std::wstring name;
  ExprType exprtype;
};

const OperatorDescription kOperatorList[] = {
  { L"*", ExprType::kMul },
  { L"/", ExprType::kDiv },
  { L"+", ExprType::kAdd },
  { L"-", ExprType::kSub }
};

const int kOperatorMaxLength = 3;

// Should be bigger or equal then maximum of kFunctionMaxLength & kOperatorMaxLength
const int kBufferMaxSize = 10;

struct TokenWithData {
  Token token;
  std::ptrdiff_t expridx;
  std::ptrdiff_t othersideoffset;
};

typedef std::vector<TokenWithData> TokenWithDataVector;

bool TryFunction(std::wstring::const_iterator *iterator,
  const std::wstring::const_iterator &end,
  const std::wstring &buffer,
  std::vector<Token> *tokens);
bool TryVariable(std::wstring::const_iterator *iterator,
  const std::wstring::const_iterator &end,
  const std::wstring &buffer,
  std::vector<Token> *tokens);
bool TryNumber(std::wstring::const_iterator *iterator,
  const std::wstring::const_iterator &end,
  const std::wstring &buffer,
  std::vector<Token> *tokens);
bool TryOperator(std::wstring::const_iterator *iterator,
  const std::wstring::const_iterator &end,
  const std::wstring &buffer,
  std::vector<Token> *tokens);
bool TryPunctuation(std::wstring::const_iterator *iterator,
  const std::wstring::const_iterator &end,
  const std::wstring &buffer,
  std::vector<Token> *tokens);

bool AbsorbRightArgument(TokenWithDataVector *tokenwithdatavector,
  const TokenWithDataVector::iterator &token,
  Expression *expr,
  std::ptrdiff_t exprindex);

bool AbsorbBothSideArguments(TokenWithDataVector *tokenwithdatavector,
  const TokenWithDataVector::iterator &token,
  Expression *expr,
  std::ptrdiff_t exprindex);

TokenWithDataVector TokenToTokenWithDataVector(const Tokens &tokens);

void ParseValueVariable(TokenWithDataVector *tokenwithdatavector,
  Expressions *expressions);
