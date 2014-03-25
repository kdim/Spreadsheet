// spreadsheet.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ostream>
#include <istream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>

#include "sheet.h"

using std::wcout;
using std::wstring;
using std::wstringstream;

const int kMaxCharInString = 1000;
const int kMaxCharInValue = 1000;

template<class CHAR_TYPE>
void ReadInputSheet(std::basic_istream<CHAR_TYPE> &input, SheetSharedPtr sheet) {
  std::array<CHAR_TYPE, kMaxCharInString + 1> inputlinearr;
  std::array<wchar_t, kMaxCharInValue + 1> inputvaluearr;
  std::size_t x = 0, y = 0;

  while (input) {
    input.getline(inputlinearr.data(), kMaxCharInString);
    wstring inputline(inputlinearr.begin(), inputlinearr.end());
    inputline.erase(inputline.find(L'\0'));
    if (inputline.empty())
      break;

    wstringstream inputlinestream(inputline);
    while (inputlinestream) {
      inputlinestream.getline(inputvaluearr.data(), kMaxCharInValue, L'\t');
      wstring value(inputvaluearr.data());
      if (!value.empty())
        sheet->SetCellNewValue(x++, y, value);
    }
    ++y;
    x = 0;
  }
}

template<class CHAR_TYPE>
void WriteSheet(std::basic_ostream<CHAR_TYPE> &output, SheetSharedPtr sheet) {
  for (std::size_t y = 0; y < sheet->size_y(); ++y) 
    for (std::size_t x = 0; x < sheet->size_x(); ++x) {
      wstring valuewide = sheet->GetCell(x, y)->Print();
      std::basic_string<CHAR_TYPE> value(valuewide.begin(), valuewide.end());
      output << std::setw(16) << value;
      if (x == sheet->size_x() - 1 && y != sheet->size_y() - 1)
        output << std::endl;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
  auto sheet = std::make_shared<Sheet>();

  if (argc == 1) {
    wcout << L"Welcome to spreadsheet console&file tester.\n";
    wcout << L"Usage \"spreadsheet [input file] [output file]\n";
    wcout << L"Missing file parameters will be replace by console.\n";
    wcout << L"Start typing or exit by CTRL+C.\n";

    ReadInputSheet(std::wcin, sheet);
    WriteSheet(std::wcout, sheet);
    std::wcin.peek();
  } else {
    // do not want to implement or copy-paste trim&dequote myself
    std::ifstream input(argv[1]);
    if (input.is_open()) {
      ReadInputSheet(input, sheet);
    } else {
      wcout << L"Input file not found. Switched to console.\n";
      wcout << L"Start typing or exit by CTRL+C.\n";
      ReadInputSheet(std::wcin, sheet);
    }
    if (argc > 2) {
      std::ofstream output(argv[2]);
      if (output.is_open()) {
        WriteSheet(output, sheet);
      } else {
        wcout << L"Output file not found. Switched to console.\n";
        WriteSheet(std::wcout, sheet);
        std::wcin.peek();
      }
    } else {
      wcout << L"Output file not found. Switched to console.\n";
      WriteSheet(std::wcout, sheet);
      std::wcin.peek();
    }
  }

  return 0;
}

