#include "stdafx.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define TEST_DEFINITION

#include "../Spreadsheet/sheet.h"
#include "../Spreadsheet/simplecell.h"
#include "../Spreadsheet/expressioncell.h"

using ::testing::_;
using std::wstring;
using std::get;

TEST(SheetTest, Coordinates) {
  EXPECT_STREQ(L"A1", CoordinateToStr(0, 0).c_str());
  EXPECT_STREQ(L"B1", CoordinateToStr(1, 0).c_str());
  EXPECT_STREQ(L"B2", CoordinateToStr(1, 1).c_str());
  EXPECT_STREQ(L"A99", CoordinateToStr(0, 98).c_str());
  EXPECT_STREQ(L"AA1", CoordinateToStr(26, 0).c_str());
}

TEST(SheetTest, Coordinates2) {
  std::size_t x, y;
  StrToCoordinate(L"A1", &x, &y);
  EXPECT_EQ(x, 0);
  EXPECT_EQ(y, 0);

  StrToCoordinate(L"B1", &x, &y);
  EXPECT_EQ(x, 1);
  EXPECT_EQ(y, 0);

  StrToCoordinate(L"B2", &x, &y);
  EXPECT_EQ(x, 1);
  EXPECT_EQ(y, 1);

  StrToCoordinate(L"A99", &x, &y);
  EXPECT_EQ(x, 0);
  EXPECT_EQ(y, 98);

  StrToCoordinate(L"AA1", &x, &y);
  EXPECT_EQ(x, 26);
  EXPECT_EQ(y, 0);
}

TEST(SheetTest, Creation) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"123");
  sheetptr->SetCellNewValue(0, 1, L"'abc");
  sheetptr->SetCellNewValue(1, 0, L"");
  sheetptr->SetCellNewValue(1, 1, L"=1+2+3");

  EXPECT_STRCASEEQ(typeid(*(sheetptr->GetCell(0, 0))).name(), typeid(NumCell).name());
  EXPECT_STRCASEEQ(typeid(*(sheetptr->GetCell(0, 1))).name(), typeid(StrCell).name());
  EXPECT_STRCASEEQ(typeid(*(sheetptr->GetCell(1, 0))).name(), typeid(NothingCell).name());
  EXPECT_STRCASEEQ(typeid(*(sheetptr->GetCell(1, 1))).name(), typeid(ExpressionCell).name());

  EXPECT_STREQ(sheetptr->GetCell(0, 0)->Print().c_str(), L"123");
  EXPECT_STREQ(sheetptr->GetCell(0, 1)->Print().c_str(), L"abc");
  EXPECT_STREQ(sheetptr->GetCell(1, 0)->Print().c_str(), L"");
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(1, 1)->Print()), 6);
}

TEST(SheetTest, ExpressionWithVariable) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"1");
  sheetptr->SetCellNewValue(0, 1, L"2");
  sheetptr->SetCellNewValue(1, 0, L"3");
  sheetptr->SetCellNewValue(1, 1, L"=A1+A2*B1");

  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(1, 1)->Print()), 7);
}

TEST(SheetTest, ExpressionWithVariable2) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"=(1)");
  sheetptr->SetCellNewValue(0, 1, L"=2");
  sheetptr->SetCellNewValue(1, 0, L"3");
  sheetptr->SetCellNewValue(1, 1, L"=(A1+B1)");

  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(0, 0)->Print()), 1);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(0, 1)->Print()), 2);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(1, 0)->Print()), 3);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(1, 1)->Print()), 4);
}

TEST(SheetTest, ExpressionWithVariable3) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"1"); // A1
  sheetptr->SetCellNewValue(0, 1, L"2"); // A2
  sheetptr->SetCellNewValue(1, 0, L"3"); // B1
  sheetptr->SetCellNewValue(1, 1, L"=(A1 + A2 * B1) / A1 + 1");
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(1, 1)->Print()), 8);
}

TEST(SheetTest, ComplexExpression) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"=B1 - A2");           // A1 = 1
  sheetptr->SetCellNewValue(1, 0, L"=((C1 / A3))");           // B1 = 3
  sheetptr->SetCellNewValue(2, 0, L"=(A2 + (B2 * B3 +1)) / A3"); // C1 = 9
  sheetptr->SetCellNewValue(0, 1, L"=B2 / C3");           // A2 = 2 
  sheetptr->SetCellNewValue(1, 1, L"=C3 + B3 / A3");      // B2 = 4 
  sheetptr->SetCellNewValue(2, 1, L"0");                  // C2 = 0
  sheetptr->SetCellNewValue(0, 2, L"3");                  // A3 = 3
  sheetptr->SetCellNewValue(1, 2, L"6");                  // B3 = 6
  sheetptr->SetCellNewValue(2, 2, L"2");                  // C3 = 2

  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"A1")->Print()), 1);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"B1")->Print()), 3);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"C1")->Print()), 9);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"A2")->Print()), 2);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"B2")->Print()), 4);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"C2")->Print()), 0);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"A3")->Print()), 3);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"B3")->Print()), 6);
  EXPECT_EQ(STR_TO_NUMBER(sheetptr->GetCell(L"C3")->Print()), 2);
}

TEST(SheetTest, BadExpression1) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"1");
  sheetptr->SetCellNewValue(0, 1, L"2");
  sheetptr->SetCellNewValue(1, 0, L"3");
  sheetptr->SetCellNewValue(1, 1, L"=A1+A2*B1+B2");

  EXPECT_STREQ(sheetptr->GetCell(1, 1)->Print().c_str(), L"#ERROR");
}

TEST(SheetTest, BadExpression2) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"=A2");
  sheetptr->SetCellNewValue(0, 1, L"=B2");
  sheetptr->SetCellNewValue(1, 0, L"=A1");
  sheetptr->SetCellNewValue(1, 1, L"=B1");

  EXPECT_STREQ(sheetptr->GetCell(1, 1)->Print().c_str(), L"#ERROR");
}

TEST(SheetTest, BadExpression3) {
  auto sheetptr = std::make_shared<Sheet>();

  sheetptr->SetCellNewValue(0, 0, L"'abc");
  sheetptr->SetCellNewValue(0, 1, L"=A1");
  sheetptr->SetCellNewValue(1, 0, L"=B2");
  sheetptr->SetCellNewValue(1, 1, L"=A1+B1");

  EXPECT_STREQ(sheetptr->GetCell(1, 1)->Print().c_str(), L"#ERROR");
}

