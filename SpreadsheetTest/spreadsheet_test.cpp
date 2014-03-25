// SpreadsheetTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main()\n");
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

