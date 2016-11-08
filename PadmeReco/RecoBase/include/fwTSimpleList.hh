#include<list>
#include<string>

#ifndef _fw_TSimpleList_hh_
#define _fw_TSimpleList_hh_ 0
namespace fw{
  template <class T>
    class TSimpleList{
      private:
        typedef typename std::list<T> cont_t;
        cont_t fList;
      public:
        typedef cont_t iter_t;
        T* GetNextP(T*);
        iter_t GetNewEntry();
        void AddEntry(T);

    };
}
#endif
