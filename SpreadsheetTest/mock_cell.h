class MockCellAbstract : public NothingCell {
 public:
  MOCK_METHOD0(Print,
      const std::wstring());
  MOCK_METHOD0(name,
      const std::wstring());
  MOCK_METHOD1(set_name,
      void(std::wstring new_name));
  MOCK_METHOD0(edited_value,
      const std::wstring());
  MOCK_METHOD1(set_edited_value,
      void(std::wstring new_value));
  MOCK_METHOD2(AddDependencyLink,
      void(CellWeakPtr dependency, unsigned counter));
  MOCK_METHOD2(DeleteDependencyLink,
      void(CellWeakPtr dependency, unsigned counter));
  MOCK_METHOD3(OnDependencyCellNameChange,
      void(const CellAbstract &source, const std::wstring &old_name, const std::wstring &new_name));
  MOCK_METHOD1(OnDependencyCellValueChange,
      void(const CellAbstract &source));
  MOCK_METHOD1(OnDependencyCellDestroy,
      void(const CellAbstract &source));
  MOCK_METHOD1(DoNotifyListener,
    void(CellHandlerFunction handler));
  MOCK_METHOD1(DoNotifyListener,
    void(CellHandlerPtr handler));
  MOCK_METHOD2(AddNotifyLink,
      void(CellWeakPtr listener, unsigned counter));
  MOCK_METHOD2(DeleteNotifyLink,
      void(CellWeakPtr listener, unsigned counter));
};
