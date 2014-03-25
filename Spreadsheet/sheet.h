// sheet.h
#pragma once

#include <vector>
#include <memory>

#include "cell.h"

class Sheet;

typedef std::shared_ptr<Sheet> SheetSharedPtr;

class Sheet : public CellCollection
{
public:
  Sheet() : size_y_(0) {};
  ~Sheet() override {};
  CellSharedPtr GetCell(std::size_t x, std::size_t y);
  CellSharedPtr GetCell(const std::wstring &cellname) override;
  void SetCellNewValue(std::size_t x, std::size_t y, const std::wstring &newvalue);
  void SetCellNewValue(const std::wstring &cellname, const std::wstring &newvalue) override;

  std::size_t size_x() { return cells_.size(); }
  std::size_t size_y() { return size_y_; }
protected:
private:
  std::vector<std::vector<CellSharedPtr > > cells_;
  std::size_t size_y_;
};

bool StrToCoordinate(const std::wstring &cellname, std::size_t *x, std::size_t *y);
std::wstring CoordinateToStr(std::size_t x, std::size_t y);
