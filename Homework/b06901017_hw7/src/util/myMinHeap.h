/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      size_t t=_data.size();
      _data.push_back(d);
      while(t>0){
         size_t p=(t-1)/2;
         if(!(d<_data[p])){
            break;
         }
         _data[t]=_data[p];
         t=p;
      }
      _data[t]=d;
   }
   void delMin() {
      size_t i=0;
      size_t t=i*2+1;
      while(t<=_data.size()-1){
         if(t<_data.size()-1){
            if(!(_data[t]<_data[t+1])){
               ++t;
            }
         }
         if(!(_data[t]<_data.back())){
            break;
         }
         _data[i]=_data[t];
         i=t;
         t=2*i+1;
      }
      _data[i]=_data.back();
      _data.pop_back();
   }
   void delData(size_t i) {
      _data[i]=_data.back();
      _data.pop_back();
      while(1){
         size_t p=(i-1)/2;
         if(_data[i]<_data[p]){
            swap(_data[i],_data[p]);
            i=p;
         }
         else{
            size_t t=2*i+1;
            if(t>=_data.size()){
               break;
            }
            if(t<_data.size()-1){
               if(_data[t+1]<_data[t]){
                  ++t;
               }
            }
            if(!(_data[t]<_data[i])){
               break;
            }
            swap(_data[i],_data[t]);
            i=t;
         }
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
