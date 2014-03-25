
#include "stdafx.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define TEST_DEFINITION

#include "../Spreadsheet/cell.h"
#include "../Spreadsheet/simplecell.h"

#include "mock_cell.h"

using ::testing::_;
using std::wstring;
using std::get;

TEST(SimpleCellTest, IsNumType) {
  EXPECT_FALSE(NumCell::IsNewValueNumberType(L""));
  EXPECT_TRUE(NumCell::IsNewValueNumberType(L"1"));
  EXPECT_TRUE(NumCell::IsNewValueNumberType(L"-1"));
  EXPECT_TRUE(NumCell::IsNewValueNumberType(L" 1 "));
  EXPECT_TRUE(NumCell::IsNewValueNumberType(L" 1 abc"));
  EXPECT_FALSE(NumCell::IsNewValueNumberType(L"abc"));
}

TEST(SimpleCellTest, IsStrType) {
  EXPECT_FALSE(StrCell::IsNewValueStringType(L""));
  EXPECT_TRUE(StrCell::IsNewValueStringType(L"'1"));
  EXPECT_TRUE(StrCell::IsNewValueStringType(L"'-1"));
  EXPECT_TRUE(StrCell::IsNewValueStringType(L"' 1 "));
  EXPECT_TRUE(StrCell::IsNewValueStringType(L"' 1 abc"));
  EXPECT_FALSE(StrCell::IsNewValueStringType(L"abc"));
}

