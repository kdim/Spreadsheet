// cell.h
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <tuple>
#include <functional>

class CellAbstract;
class CellCollection;

typedef std::weak_ptr<CellAbstract> CellWeakPtr;
typedef std::shared_ptr<CellAbstract> CellSharedPtr;
typedef std::map<CellWeakPtr, int, std::owner_less<CellWeakPtr>> CellWeakPtrToCounter;
typedef std::function<void(CellAbstract&, const CellAbstract&)> CellHandlerFunction;
typedef void (CellAbstract::*CellHandlerPtr)(const CellAbstract&);
typedef std::weak_ptr<CellCollection> CellCollectionWeakPtr;
typedef std::shared_ptr<CellCollection> CellCollectionSharedPtr;

class CellAbstract: public std::enable_shared_from_this<CellAbstract>
{
public:
  explicit CellAbstract(std::wstring edited_value = L"", 
      CellCollectionWeakPtr collection = std::weak_ptr<CellCollection>(), 
      std::wstring name = L"");
  CellAbstract(const CellAbstract &copysource) = delete;
  CellAbstract(CellAbstract &&movesource) = delete;
  CellAbstract& operator=(CellAbstract &arg) = delete;
  virtual ~CellAbstract();

  virtual const std::wstring Print() { return edited_value_; };
  virtual void ReplaceThat(CellSharedPtr that);
  virtual bool TryChangeEditedValue(const std::wstring &new_value);

  VIRTUAL_FOR_TEST const std::wstring name() { return name_; };
  virtual void set_name(const std::wstring &new_name);
  VIRTUAL_FOR_TEST const std::wstring edited_value() { return edited_value_; };
  virtual void set_edited_value(const std::wstring &new_value);

protected: PUBLIC_FOR_TEST
  virtual bool OnChangeValue(const std::wstring &new_value) = 0;

  virtual void OnDependencyCellNameChange(const CellAbstract &source,
                                          const std::wstring &old_name,
                                          const std::wstring &new_name) {};
  virtual void OnDependencyCellValueChange(const CellAbstract &source) {};
  virtual void OnDependencyCellDestroy(const CellAbstract &source);

  VIRTUAL_FOR_TEST void AddDependencyLink(CellWeakPtr dependency, unsigned counter = 1);
  VIRTUAL_FOR_TEST void DeleteDependencyLink(CellWeakPtr dependency, unsigned counter = 1);

  VIRTUAL_FOR_TEST void DoNotifyListener(CellHandlerFunction handler);
  // because of http://connect.microsoft.com/VisualStudio/feedback/details/694400
  VIRTUAL_FOR_TEST void DoNotifyListener(CellHandlerPtr handler);

  CellCollectionWeakPtr collection_;

private: PUBLIC_FOR_TEST
  std::wstring name_;
  std::wstring edited_value_;
  CellWeakPtrToCounter listener_list_;
  CellWeakPtrToCounter dependency_list_;

  VIRTUAL_FOR_TEST CellWeakPtrToCounter FilterLostPointer(CellWeakPtrToCounter *list);
  VIRTUAL_FOR_TEST void AddNotifyLink(CellWeakPtr listener, unsigned counter = 1);
  VIRTUAL_FOR_TEST void DeleteNotifyLink(CellWeakPtr listener, unsigned counter = 1);
};

class CellCollection : public std::enable_shared_from_this<CellCollection>
{
public:
  virtual ~CellCollection() {};
  virtual CellSharedPtr GetCell(const std::wstring &cellname) = 0;
  virtual void SetCellNewValue(const std::wstring &cellname, const std::wstring &newvalue) = 0;
};
