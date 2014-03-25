
#include "stdafx.h"
#include <ostream>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define TEST_DEFINITION

#include "../Spreadsheet/expression.h"
#include "../Spreadsheet/expression_impl.h"

using ::testing::_;
using std::wstring;
using std::get;

TEST(TokenTest, Function) {
  wstring str = L"sin";
  std::vector<Token> tokens;
  EXPECT_TRUE(TryFunction(&(str.begin()), str.end(), str, &tokens));
  ASSERT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].type, TokenType::kFunction);
  EXPECT_TRUE(tokens[0].value.compare(str) == 0);
}

TEST(TokenTest, Variable) {
  wstring str = L"$A$1";
  std::vector<Token> tokens;
  EXPECT_TRUE(TryVariable(&(str.begin()), str.end(), str, &tokens));
  ASSERT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].type, TokenType::kVariable);
  EXPECT_TRUE(tokens[0].value.compare(str) == 0);
}

TEST(TokenTest, Tokenizer) {
  wstring str = L" ( 9 + 3 ) * - $A1  ";
  std::vector<Token> tokens = Tokenizer(str);
  ASSERT_EQ(tokens.size(), 8);
  EXPECT_EQ(tokens[0].type, TokenType::kParenthesesOpen);
  //  EXPECT_TRUE(tokens[0].value.compare(L"(") == 0);
  EXPECT_EQ(tokens[1].type, TokenType::kValue);
  EXPECT_TRUE(tokens[1].value.compare(L"9") == 0);
  EXPECT_EQ(tokens[2].type, TokenType::kBinaryOp);
  EXPECT_TRUE(tokens[2].value.compare(L"+") == 0);
  EXPECT_EQ(tokens[3].type, TokenType::kValue);
  EXPECT_TRUE(tokens[3].value.compare(L"3") == 0);
  EXPECT_EQ(tokens[4].type, TokenType::kParenthesesClose);
  //  EXPECT_TRUE(tokens[4].value.compare(L")") == 0);
  EXPECT_EQ(tokens[5].type, TokenType::kBinaryOp);
  EXPECT_TRUE(tokens[5].value.compare(L"*") == 0);
  EXPECT_EQ(tokens[6].type, TokenType::kUnaryOp);
  EXPECT_TRUE(tokens[6].value.compare(L"-") == 0);
  EXPECT_EQ(tokens[7].type, TokenType::kVariable);
  EXPECT_TRUE(tokens[7].value.compare(L"$A1") == 0);
}

TEST(ParseTest, AbsorbRightArgument) {
  wstring str = L"-5";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  ASSERT_EQ(expr.size(), 2);
  EXPECT_EQ(expr[0].type, ExprType::kValue);
  EXPECT_EQ(expr[0].value, 5);
  EXPECT_EQ(expr[1].type, ExprType::kNeg);
}

TEST(ParseTest, TokenToTokenWithDataVector) {
  wstring str = L"6-5";
  auto tokens = Tokenizer(str);
  auto expr = TokenToTokenWithDataVector(tokens);
  ASSERT_EQ(expr.size(), 3);
  EXPECT_EQ(expr[0].token.type, TokenType::kValue);
  EXPECT_TRUE(expr[0].token.value.compare(L"6") == 0);
  EXPECT_EQ(expr[2].token.type, TokenType::kValue);
  EXPECT_TRUE(expr[2].token.value.compare(L"5") == 0);
}

TEST(ParseTest, AbsorbBothSideArguments) {
  wstring str = L"6-5";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  ASSERT_EQ(expr.size(), 3);
  EXPECT_EQ(expr[0].type, ExprType::kValue);
  EXPECT_EQ(expr[0].value, 6);
  EXPECT_EQ(expr[1].type, ExprType::kValue);
  EXPECT_EQ(expr[1].value, 5);
  EXPECT_EQ(expr[2].type, ExprType::kSub);
}

TEST(ParseTest, ParseTokens) {
  wstring str = L"6-5*2";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  ASSERT_EQ(expr.size(), 5);
  EXPECT_EQ(expr[0].type, ExprType::kValue);
  EXPECT_EQ(expr[0].value, 6);
  EXPECT_EQ(expr[1].type, ExprType::kValue);
  EXPECT_EQ(expr[1].value, 5);
  EXPECT_EQ(expr[2].type, ExprType::kValue);
  EXPECT_EQ(expr[2].value, 2);
  EXPECT_EQ(expr[3].type, ExprType::kMul);
  EXPECT_EQ(expr[3].operand[0], 1);
  EXPECT_EQ(expr[3].operand[1], 2);
  EXPECT_EQ(expr[4].type, ExprType::kSub);
  EXPECT_EQ(expr[4].operand[0], 0);
  EXPECT_EQ(expr[4].operand[1], 3);
}

TEST(CalculateTest, Calculate1) {
  wstring str = L"6-5*2";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, -4);
}

TEST(CalculateTest, Calculate2) {
  wstring str = L"(6-5)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 1);
}

TEST(CalculateTest, Calculate2_1) {
  wstring str = L"((6-5)-1)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 0);
}

TEST(CalculateTest, Calculate2_2) {
  wstring str = L"((1+((6-5)-1))+1)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 2);
}

TEST(CalculateTest, Calculate2_3) {
  wstring str = L"(((1)))";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 1);
}

TEST(CalculateTest, Calculate2_4) {
  wstring str = L"(-(((1))))";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, -1);
}

TEST(CalculateTest, Calculate2_5) {
  wstring str = L"(-((())))";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_FALSE(result.iscalculated);
}

TEST(CalculateTest, Function) {
  wstring str = L"cos(0)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 1);
}

TEST(CalculateTest, Nothing) {
  wstring str = L"";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_FALSE(result.iscalculated);
}

TEST(CalculateTest, WithVars) {
  GetVarByNameFunction getvar = [](const std::wstring &varname) -> ExpressionResult {
    if (varname.compare(L"A") == 0)
      return{ true, 1, { ParsingErrorType::kNone } };
    else if (varname.compare(L"B") == 0)
      return{ true, 2, { ParsingErrorType::kNone } };
    else if (varname.compare(L"C") == 0)
      return{ true, 3, { ParsingErrorType::kNone } };
    return{ false, NAN, { ParsingErrorType::kVariableNotDefined } };
  };
  wstring str = L"A+B*C";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr, getvar);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 7);
}

TEST(CalculateTest, Calculate3) {
  wstring str = L"6+3-5*2 + cos(0)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  ASSERT_TRUE(result.iscalculated);
  EXPECT_DOUBLE_EQ(result.value, 0);
}

TEST(CalculateTest, BadCalculate1) {
  wstring str = L"6-5*2cos";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

TEST(CalculateTest, BadCalculate2) {
  wstring str = L"6-5 5*2";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

TEST(CalculateTest, BadCalculate2_1) {
  wstring str = L"(6-5 5*2)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

TEST(CalculateTest, BadCalculate3) {
  wstring str = L"(6-5*2";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

TEST(CalculateTest, BadCalculate4) {
  wstring str = L"6-5*2)";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

TEST(CalculateTest, BadCalculate5) {
  wstring str = L"6-5*2 *";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

TEST(CalculateTest, BadCalculate6) {
  wstring str = L"+*+-()";
  auto tokens = Tokenizer(str);
  auto expr = ParseTokens(tokens);
  auto result = Calculate(expr);
  EXPECT_FALSE(result.iscalculated);
}

