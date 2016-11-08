#include"fwTSimpleList.hh"



template <class T> typename fw::TSimpleList<T>::iter_t
fw::TSimpleList<T>::GetNewEntry() {
  fList.push_back(T());
  return fList.rbegin();
}

template <class T> void fw::TSimpleList<T>::AddEntry(T ent) {
  fList.push_back(ent);
}



template <class T>
T* fw::TSimpleList<T>::GetNextP(T*p){
  iter_t it=iter_t(p);
  if(it==NULL){
    it=fList.begin();
  }else{
    ++it;
  }
  if(it==fList.end())
    return NULL;
  return it;

}
