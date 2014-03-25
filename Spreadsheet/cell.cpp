#include "stdafx.h"
#include "cell.h"

using std::wstring;
using std::get;

CellAbstract::CellAbstract(std::wstring edited_value, CellCollectionWeakPtr collection, std::wstring name) :
    edited_value_(edited_value),
    collection_(collection),
    name_(name)
{
}

CellAbstract::~CellAbstract()
{
  DoNotifyListener(&CellAbstract::OnDependencyCellDestroy);
}

void CellAbstract::AddDependencyLink(CellWeakPtr dependency, unsigned counter)
{
  if (auto dependency_cell = dependency.lock()) {
    dependency_list_[dependency] = dependency_list_[dependency] + counter;
    dependency_cell->AddNotifyLink(shared_from_this(), counter);
  }
}

void CellAbstract::DeleteDependencyLink(CellWeakPtr dependency, unsigned counter)
{
  if (auto dependency_cell = dependency.lock()) {
    auto dependency_it = dependency_list_.find(dependency);
    if (dependency_it != dependency_list_.end() && --(dependency_it->second) <= 0)
      dependency_list_.erase(dependency_it);
    dependency_cell->DeleteNotifyLink(shared_from_this());
  }
} 

void CellAbstract::AddNotifyLink(CellWeakPtr listener, unsigned counter)
{
  listener_list_[listener] = listener_list_[listener] + counter;
}

void CellAbstract::DeleteNotifyLink(CellWeakPtr listener, unsigned counter)
{
  auto listener_it = listener_list_.find(listener);
  if (listener_it != listener_list_.end() && (listener_it->second - counter) <= 0)
    listener_list_.erase(listener_it);
}

void CellAbstract::OnDependencyCellDestroy(const CellAbstract &source)
{
  dependency_list_ = FilterLostPointer(&dependency_list_);
}

void CellAbstract::set_name(const std::wstring &new_name)
{
  using namespace std::placeholders;
  wstring old_name = name_;
  name_ = new_name;
  DoNotifyListener(std::bind(&CellAbstract::OnDependencyCellNameChange, _1, _2, old_name, new_name));
}

void CellAbstract::ReplaceThat(CellSharedPtr that) {
  name_ = that->name_;
  collection_ = that->collection_;

  for (auto &dependency : that->dependency_list_) {
    if (auto dependency_cell = dependency.first.lock()) {
      dependency_cell->DeleteNotifyLink(shared_from_this(), dependency.second);
    }
  }
  that->dependency_list_.clear();

  CellWeakPtrToCounter that_listener_list_{ std::move(that->listener_list_) };
  for (auto &listener : that_listener_list_) {
    if (auto listener_cell = listener.first.lock()) {
      listener_cell->AddDependencyLink(shared_from_this(), listener.second);
      listener_cell->DeleteDependencyLink(that, listener.second);
    }
  }
}

bool CellAbstract::TryChangeEditedValue(const std::wstring &new_value) {
  edited_value_ = new_value;
  if (OnChangeValue(new_value)) {
    DoNotifyListener(&CellAbstract::OnDependencyCellValueChange);
    return true;
  }
  return false;
};

void CellAbstract::set_edited_value(const std::wstring &new_value)
{
  TryChangeEditedValue(new_value);
}

void CellAbstract::DoNotifyListener(CellHandlerFunction handler)
{
  int lost_listener_count = 0;
  for (auto &listener_item : listener_list_)
    if (auto listener = get<0>(listener_item).lock()) 
      handler(*listener, *this);
    else
      ++lost_listener_count;

  if (lost_listener_count)
    listener_list_ = FilterLostPointer(&listener_list_);
}

void CellAbstract::DoNotifyListener(CellHandlerPtr handler)
{
  DoNotifyListener(std::mem_fn(handler));
}

CellWeakPtrToCounter CellAbstract::FilterLostPointer(CellWeakPtrToCounter *list)
{
  CellWeakPtrToCounter new_list;
  for (auto &item : *list)
    if (!get<0>(item).expired())
      new_list.insert(item);
  return new_list;
}
