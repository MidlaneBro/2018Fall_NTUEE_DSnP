/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted=true;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
        _node=_node->_next;
        return *(this); 
      }
      iterator operator ++ (int) { 
        iterator tmp(_node);
        _node=_node->_next;
        return tmp; 
      }
      iterator& operator -- () { 
        _node=_node->_prev;
        return *(this); 
      }
      iterator operator -- (int) { 
        iterator tmp(_node);
        _node=_node->_prev;
        return tmp; 
      }

      iterator& operator = (const iterator& i) { 
        _node=i._node;
        return *(this);
      }

      bool operator != (const iterator& i) const { 
        return _node!=i._node;
      }
      bool operator == (const iterator& i) const { 
        return _node==i._node;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { 
    if(_head==_head->_next){
      return true;
    }
    return false;
   }
   size_t size() const {  
    size_t n=0;
    DListNode<T>* _tmp=_head;
    while(_tmp->_next!=_head){
      n++;
      _tmp=_tmp->_next;
    }
    return n;
   }

   void push_back(const T& x) {
    DListNode<T>* _p=new DListNode<T>(x,_head->_prev->_prev,_head->_prev);
    _p->_next->_prev=_p;
    _p->_prev->_next=_p;
    if(size()==1){
      _head=_head->_next;
    }
    else{
      _isSorted=false;
    }
   }
   void pop_front() {
    if(!empty()){
      DListNode<T>* _delete=_head;
      _delete->_next->_prev=_delete->_prev;
      _delete->_prev->_next=_delete->_next;
      _head=_head->_next;
      delete _delete;
    }
   }
   void pop_back() {
    if(!empty()){
      DListNode<T>* _delete=_head->_prev->_prev;
      _delete->_next->_prev=_delete->_prev;
      _delete->_prev->_next=_delete->_next;
      delete _delete;
    }
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
    if(empty()){
      return false;
    }
    pos._node->_prev->_next=pos._node->_next;
    pos._node->_next->_prev=pos._node->_prev;
    if(pos==begin()){
      _head=_head->_next;
    }
    delete pos._node;
    return true;
   }
   bool erase(const T& x) { 
     for(iterator i=begin();i!=end();i++){
       if(*i==x){
         erase(i);
         return true;
       }
     }
     return false; 
    }

   iterator find(const T& x) { 
     for(iterator i=begin();i!=end();i++){
       if(*i==x){
         return i;
       }
     }
     return end(); 
    }

   void clear() {
     while(!empty()){
       pop_front();
     }
   }  // delete all nodes except for the dummy node

   void sort() const {
     if(!_isSorted){
       for(iterator i=begin();i!=end();i++){
         for(iterator j=i;j!=end();j++){
           if(*i>*j){
             T tmp=*i;
             *i=*j;
             *j=tmp;
           }
         }
       }
     }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
