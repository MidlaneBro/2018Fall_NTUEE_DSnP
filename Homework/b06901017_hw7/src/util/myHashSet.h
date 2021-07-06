/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(int a,int b,size_t c,vector<Data>* d):_bucket(a),_slot(b),_numBuckets(c),_buckets(d){}
      ~iterator() {}
      const Data& operator * () const { 
         if(_slot==-1||_bucket==_numBuckets){
            return Data();
         }
         return _buckets[_bucket][_slot]; 
      }
      iterator& operator ++ () {
         if(_bucket==_numBuckets){
            return (*this);
         }
         _slot++;
         if(_slot == _buckets[_bucket].size()){
            _slot=0;
            do{
               _bucket++;
            }while(_buckets[_bucket].empty()&&_bucket!=_numBuckets);
         }
         return (*this);
      }
      iterator operator ++ (int) {
         iterator tmp = *this;
         ++(*this);
         return tmp;
      }
      iterator& operator -- () { 
         if(_slot==-1){
            return (*this);
         }
         _slot--;
         if(_slot<0&&_bucket>0){
            do{
               _bucket--;
            }while(_buckets[_bucket].empty()&&_bucket!=0);
            _slot=_buckets[_bucket].size()-1;
         }
         return (*this);
      }
      iterator operator -- (int) {
         iterator tmp = *this;
         --(*this);
         return tmp;
      }
      iterator& operator = (const iterator& i) {
         _bucket = i._bucket;
         _slot = i._slot;
         _numBuckets = i._numBuckets;
         _buckets = i._buckets;
         return (*this);
      }
      bool operator == (const iterator& i) const {
         return (_bucket==i._bucket&&_slot==i._slot);
      }
      bool operator != (const iterator& i) const { 
         return !((*this)==i); 
      }
   private:
      int           _bucket;
      int           _slot;
      size_t        _numBuckets;
      vector<Data>* _buckets;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      if(empty()){
         return end();
      }
      for(size_t i=0;i<_numBuckets;i++){
         if(!_buckets[i].empty()){
            return iterator(i,0,_numBuckets,_buckets);
         }
      }
   }
   // Pass the end
   iterator end() const { return iterator(_numBuckets,0,_numBuckets,_buckets); }
   // return true if no valid data
   bool empty() const { return !size(); }
   // number of valid data
   size_t size() const { 
      size_t s = 0;
      for(size_t i=0;i<_numBuckets;i++){
         s+=_buckets[i].size();
      } 
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      int num=bucketNum(d);
      for(size_t i=0;i<_buckets[num].size();i++){
         if(d==_buckets[num][i]){
            return true;
         }
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      int num=bucketNum(d);
      for(size_t i=0;i<_buckets[num].size();i++){
         if(d==_buckets[num][i]){
            _buckets[num][i]=d;
            return true;
         }
      }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      int num=bucketNum(d);
      for(size_t i=0;i<_buckets[num].size();i++){
         if(d==_buckets[num][i]){
            _buckets[num][i]=d;
            return true;
         }
      }
      insert(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if(!check(d)){
         _buckets[bucketNum(d)].push_back(d);
         return true;
      }
      return false; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      int num=bucketNum(d);
      for(size_t i=0;i<_buckets[num].size();i++){
         if(d==_buckets[num][i]){
            _buckets[num].erase(_buckets[num].begin()+i);
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
