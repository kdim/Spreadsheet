#include "stdafx.h"
#include "expression.h"
#include "expression_impl.h"
#include <string>
#include <functional>
#include <cwctype>
#include <algorithm>
#include <iterator>
#include <cmath>

using std::wstring;

bool TryFunction(std::wstring::const_iterator *iterator,
                const std::wstring::const_iterator &end,
                const std::wstring &buffer,
                std::vector<Token> *tokens) {
  if (!iswalpha(**iterator)) 
    return false;

  for (auto &match_string_func : kFunctionList)
    if (buffer.compare(0, match_string_func.name.length(), match_string_func.name) == 0) {
      tokens->push_back(Token{ TokenType::kFunction, match_string_func.exprtype, match_string_func.name });
      //TODO: arguments

      *iterator += match_string_func.name.length();
      return true;
    }
  
  return false;
}

bool TryVariable(std::wstring::const_iterator *iterator,
                const std::wstring::const_iterator &end,
                const std::wstring &buffer,
                std::vector<Token> *tokens) {
  if (!(iswalpha(**iterator) || **iterator == L'$'))
    return false;

  wstring varname;
  do {
    varname += **iterator;
    ++*iterator;
  } while (*iterator != end && (iswalpha(**iterator) || iswdigit(**iterator) || **iterator == L'$'));

  tokens->push_back(Token{ TokenType::kVariable, ExprType::kVariable, varname });
  return true;
}

bool TryNumber(std::wstring::const_iterator *iterator,
              const std::wstring::const_iterator &end,
              const std::wstring &buffer,
              std::vector<Token> *tokens) {
  if (!(iswdigit(**iterator) || **iterator == L'.'))
    return false;

  wstring number;
  do {
    number += **iterator;
    ++*iterator;
  } while (*iterator != end && (iswdigit(**iterator) || **iterator == L'.'));

  tokens->push_back(Token{ TokenType::kValue, ExprType::kValue, number });
  return true;
}

bool TryOperator(std::wstring::const_iterator *iterator,
                    const std::wstring::const_iterator &end,
                    const std::wstring &buffer,
                    std::vector<Token> *tokens) {
  for (auto &match_string_op : kOperatorList) {
    if (buffer.compare(0, match_string_op.name.length(), match_string_op.name) == 0) {
      // unary op case
      if (buffer.compare(L"-") && 
            (tokens->size() == 0 || 
            (*tokens)[tokens->size() - 1].type == TokenType::kBinaryOp) ||
            (*tokens)[tokens->size() - 1].type == TokenType::kParenthesesOpen) {
        tokens->push_back(Token{ TokenType::kUnaryOp, ExprType::kNeg, match_string_op.name });
      }
      else {
        tokens->push_back(Token{ TokenType::kBinaryOp, match_string_op.exprtype, match_string_op.name });
      }
      *iterator += match_string_op.name.length();
      return true;
    }
  }
  return false;
}

bool TryPunctuation(std::wstring::const_iterator *iterator,
  const std::wstring::const_iterator &end,
  const std::wstring &buffer,
  std::vector<Token> *tokens) {
  if (**iterator == L'(') {
    tokens->push_back(Token{ TokenType::kParenthesesOpen, ExprType::kNone, L"" });
  }
  else if (**iterator == L')') {
    tokens->push_back(Token{ TokenType::kParenthesesClose, ExprType::kNone, L"" });
  }
  else {
    return false;
  }
  ++*iterator;
  return true;
}

Tokens Tokenizer(const std::wstring::const_iterator &begin,
                            const std::wstring::const_iterator &end) {
  auto it = begin;
  Tokens tokens;
  Token current_token;

  while (it != end) {
    if (*it == L' ') {
      ++it;
      continue;
    }

    auto posiblesize = std::min(distance(it, end), kBufferMaxSize);
    std::wstring buffer(it, it + posiblesize);

    if (TryFunction(&it, end, buffer, &tokens))
      continue;

    if (TryVariable(&it, end, buffer, &tokens))
      continue;

    if (TryNumber(&it, end, buffer, &tokens))
      continue;

    if (TryOperator(&it, end, buffer, &tokens))
      continue;

    if (TryPunctuation(&it, end, buffer, &tokens))
      continue;

    tokens.push_back(Token{ TokenType::kError, ExprType::kError, wstring(it, it + 1) });
    ++it;
  }
  return tokens;
}

Tokens Tokenizer(const std::wstring &expr_string) {
  return Tokenizer(expr_string.begin(), expr_string.end());
}

// Uplift parsing. 
// Yes it could be much better I know. But it will be less understandable and easy to write.
// Better is to use parser builder.

void PushBackError(Expressions *expressions, ParsingErrorType error) {
  Expression expr;
  expr.type = ExprType::kError;
  expr.parsingerror.type = error;
  expressions->push_back(expr);
}

void ParseValueVariable(TokenWithDataVector *tokenwithdatavector,
                        Expressions *expressions) {
  Expression expr;
  for (auto &tokenwithdata : (*tokenwithdatavector)) {
    if (tokenwithdata.token.type == TokenType::kValue) {
      expr.type = ExprType::kValue;
      try {
        expr.value = STR_TO_NUMBER(tokenwithdata.token.value);
        tokenwithdata.expridx = expressions->size();
        expressions->push_back(expr);
      }
      catch (...) {
        PushBackError(expressions, ParsingErrorType::kBadNumber);
      }
    }
    if (tokenwithdata.token.type == TokenType::kVariable) {
      expr.type = ExprType::kVariable;
      expr.variable.name = tokenwithdata.token.value;
      tokenwithdata.expridx = expressions->size();
      expressions->push_back(expr);
    }
  }

}

bool AbsorbRightArgument(TokenWithDataVector *tokenwithdatavector,
                        const TokenWithDataVector::iterator &token,
                        Expression *expr,
                        std::ptrdiff_t exprindex) {
  if (token == tokenwithdatavector->end() || ((token + 1)->expridx == -1))
    return false;

  auto nexttoken = token + 1;
  auto offset = 1 + (token + 1)->othersideoffset;
  auto rightsideofexpr = (token + offset);
  expr->operand[0] = nexttoken->expridx;
  token->expridx = exprindex;
  token->othersideoffset = offset;
  rightsideofexpr->expridx = token->expridx;
  rightsideofexpr->othersideoffset = -offset;
  return true;
}

bool AbsorbBothSideArguments(TokenWithDataVector *tokenwithdatavector,
                            const TokenWithDataVector::iterator &token,
                            Expression *expr,
                            std::ptrdiff_t exprindex) {
  if (token + 1 == tokenwithdatavector->end() || token == tokenwithdatavector->begin() || 
      ((token + 1)->expridx == -1) || ((token - 1)->expridx == -1))
    return false;

  auto prevtoken = token - 1;
  auto nexttoken = token + 1;
  auto leftoffset = -1 + (token - 1)->othersideoffset;
  auto rightoffset = 1 + (token + 1)->othersideoffset;
  auto leftsideofexpr = (token + leftoffset);
  auto rightsideofexpr = (token + rightoffset);
  expr->operand[0] = prevtoken->expridx;
  expr->operand[1] = nexttoken->expridx;
  token->expridx = exprindex;
  leftsideofexpr->expridx = exprindex;
  leftsideofexpr->othersideoffset = -leftoffset + rightoffset;
  rightsideofexpr->expridx = exprindex;
  rightsideofexpr->othersideoffset = leftoffset - rightoffset;
  return true;
}

TokenWithDataVector::iterator ParseParentheses(TokenWithDataVector *tokenwithdatavector,
                      const TokenWithDataVector::iterator &begin,
                      const TokenWithDataVector::iterator &end,
                      Expressions *expressions) {
  auto endofexpr = end;
  for (auto it = begin; it != end; ++it) {
    if (it->token.type == TokenType::kParenthesesOpen) {
      auto subexprend = ParseParentheses(tokenwithdatavector, it + 1, end, expressions);
      if ((it + 1)->othersideoffset != std::distance(it, subexprend) - 2)
        PushBackError(expressions, ParsingErrorType::kExpectOperatorFoundNothing);
      if (subexprend == end) {
        PushBackError(expressions, ParsingErrorType::kExpectClosingParenthesesFoundNothing);
        return end;
      }
      it->expridx = expressions->size() - 1;
      subexprend->expridx = expressions->size() - 1;
      it->othersideoffset = std::distance(it, subexprend);
      subexprend->othersideoffset = -it->othersideoffset;
      it = subexprend;
      continue;
    }

    if (it->token.type == TokenType::kParenthesesClose) {
      if (it == begin)
        PushBackError(expressions, ParsingErrorType::kExpectTokenFoundClosingParentheses);
      endofexpr = it;
      break;
    }
  }

  Expression expr;
  for (auto it = begin; it != endofexpr; ++it) {
    if (it->expridx == -1 && it->token.type == TokenType::kFunction) {
      expr.type = it->token.exprtype;
      if ((it + 1) != end && (it + 1)->token.type == TokenType::kParenthesesOpen &&
        AbsorbRightArgument(tokenwithdatavector, it, &expr, expressions->size())) {
        expressions->push_back(expr);
      } else {
        PushBackError(expressions, ParsingErrorType::kUnexpectedFunctionArgumenType);
      }
    }
  }

  for (auto it = begin; it != endofexpr; ++it) {
    if (it->expridx == -1 && it->token.type == TokenType::kUnaryOp && it->token.exprtype == ExprType::kNeg) {
      expr.type = it->token.exprtype;
      if (AbsorbRightArgument(tokenwithdatavector, it, &expr, expressions->size())) {
        expressions->push_back(expr);
      }
      else {
        PushBackError(expressions, ParsingErrorType::kUnexpectedUnaryNegArgumenType);
      }
    }
  }

  for (auto it = begin; it != endofexpr; ++it) {
    if (it->expridx == -1 && it->token.type == TokenType::kBinaryOp &&
      (it->token.exprtype == ExprType::kMul || it->token.exprtype == ExprType::kDiv)) {
      expr.type = it->token.exprtype;
      if (AbsorbBothSideArguments(tokenwithdatavector, it, &expr, expressions->size())) {
        expressions->push_back(expr);
      }
      else {
        PushBackError(expressions, ParsingErrorType::kUnexpectedBinaryOpArgumenType);
      }
    }
  }

  for (auto it = begin; it != endofexpr; ++it) {
    if (it->expridx == -1 && it->token.type == TokenType::kBinaryOp &&
      (it->token.exprtype == ExprType::kAdd || it->token.exprtype == ExprType::kSub)) {
      expr.type = it->token.exprtype;
      if (AbsorbBothSideArguments(tokenwithdatavector, it, &expr, expressions->size())) {
        expressions->push_back(expr);
      }
      else {
        PushBackError(expressions, ParsingErrorType::kUnexpectedBinaryOpArgumenType);
      }
    }
  }

  return endofexpr;
}
  
TokenWithDataVector TokenToTokenWithDataVector(const Tokens &tokens) {
  TokenWithDataVector tokenwithdatavector;

  tokenwithdatavector.reserve(tokens.size());

  for (auto &token : tokens)
    tokenwithdatavector.push_back({ token, -1, 0 });
  return tokenwithdatavector;
}

Expressions ParseTokens(const Tokens &tokens) {
  Expressions expressions;
  TokenWithDataVector tokenwithdatavector = TokenToTokenWithDataVector(tokens);
  
  expressions.reserve(tokens.size());

  ParseValueVariable(&tokenwithdatavector, &expressions);

  if (ParseParentheses(&tokenwithdatavector, tokenwithdatavector.begin(), tokenwithdatavector.end(), &expressions)
      != tokenwithdatavector.end())
    PushBackError(&expressions, ParsingErrorType::kUnexpectedClosingParentheses);
  if (tokenwithdatavector.size() != 0 && tokenwithdatavector[0].othersideoffset != tokenwithdatavector.size() - 1)
    PushBackError(&expressions, ParsingErrorType::kExpectOperatorFoundNothing);
  return expressions;
}

Expressions Parse(const std::wstring &expr_string) {
  return ParseTokens(Tokenizer(expr_string));
}

ExpressionResult Calculate(const Expressions &expressions, GetVarByNameFunction vargetter) {
  std::vector<NUMBER> values;
  ExpressionResult result;
  result.iscalculated = false;

  for (auto &expr : expressions) {
    switch (expr.type)
    {
    case ExprType::kNone: return result; //TODO(prog.dmitry@gmail.com) Uplift error codes
    case ExprType::kError: return result;
    case ExprType::kValue:
      values.push_back(expr.value);
      break;
    case ExprType::kVariable: {
      auto varresult = vargetter(expr.variable.name);
      if (varresult.iscalculated)
        values.push_back(varresult.value);
      else
        return varresult;
      break;
    }
    case ExprType::kNeg:
      values.push_back(-values[expr.operand[0]]);
      break;
    case ExprType::kCos:
      values.push_back(std::cos(values[expr.operand[0]]));
      break;
    case ExprType::kSin:
      values.push_back(std::cos(values[expr.operand[0]]));
      break;
    case ExprType::kMul:
      values.push_back(values[expr.operand[0]] * values[expr.operand[1]]);
      break;
    case ExprType::kDiv:
      values.push_back(values[expr.operand[0]] / values[expr.operand[1]]);
      break;
    case ExprType::kAdd:
      values.push_back(values[expr.operand[0]] + values[expr.operand[1]]);
      break;
    case ExprType::kSub:
      values.push_back(values[expr.operand[0]] - values[expr.operand[1]]);
      break;
    default:
      return result;
    }
  }

  if (values.size() != 0) {
    result.iscalculated = true;
    result.value = values.back();
  }

  return result;
}

ExpressionResult Calculate(const Expressions &expressions) {
  GetVarByNameFunction vargetter = [](const std::wstring &varname)->ExpressionResult {
    return{ false, NAN, { ParsingErrorType::kVariableNotDefined } };
  };
  return Calculate(expressions, vargetter);
}