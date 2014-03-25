#include "stdafx.h"
#include "sheet.h"

#include <cwctype>
#include <algorithm>

#include "simplecell.h"
#include "cellfactory.h"

bool StrToCoordinate(const std::wstring &cellname, std::size_t *x, std::size_t *y) {
  auto it = cellname.begin();
  *x = 0;
  *y = 0;
  int state = 0;
  while (it != cellname.end() && std::towupper(*it) >= L'A' && std::towupper(*it) <= L'Z') {
    *x = (*x) * (L'Z' - L'A' + 1) + std::towupper(*it) - L'A' + 1;
    state = 1;
    ++it;
  }
  --(*x);
  if (state == 1) {
    while (it != cellname.end() && std::iswdigit(*it)) {
      *y = (*y) * (L'9' - L'0' + 1) + *it - L'0';
      state = 2;
      ++it;
    }
    --(*y);
  }

  return state == 2;
}

std::wstring CoordinateToStr(std::size_t x, std::size_t y) {
  std::wstring result = std::to_wstring(y + 1);
  std::wstring alpha;
  ++x;
  while (x > 0) {
    alpha.push_back(static_cast<wchar_t>(x % (L'Z' - L'A' + 1) - 1 + L'A'));
    x = (x - 1) / (L'Z' - L'A' + 1);
  }
  result.insert(result.begin(), alpha.rbegin(), alpha.rend());
  return result;
}

CellSharedPtr Sheet::GetCell(std::size_t x, std::size_t y) {
  if (x >= cells_.size())
    cells_.resize(x + 1);
  if (y >= cells_[x].size()) {
    size_y_ = std::max(y + 1, size_y_);
    cells_[x].resize(y + 1, std::make_shared<NothingCell>(L"", shared_from_this(), CoordinateToStr(x, y)));
  }
  return cells_[x][y];
}

CellSharedPtr Sheet::GetCell(const std::wstring &cellname) {
  std::size_t x, y;
  if (StrToCoordinate(cellname, &x, &y))
    return GetCell(x, y);
  else
    return std::shared_ptr<NothingCell>();
}

void Sheet::SetCellNewValue(std::size_t x, std::size_t y, const std::wstring &newvalue) {
  if (!(GetCell(x, y)->TryChangeEditedValue(newvalue)))
    cells_[x][y] = GetCellFactory().ReplaceCellSharedPtr(cells_[x][y], newvalue);
}

void Sheet::SetCellNewValue(const std::wstring &cellname, const std::wstring &newvalue) {
  std::size_t x, y;
  if (StrToCoordinate(cellname, &x, &y))
    SetCellNewValue(x, y, newvalue);
}


