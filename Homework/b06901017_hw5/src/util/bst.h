/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   
   BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0 ):
    _data(d), _parent(p), _left(l), _right(r) {}

   T              _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
  // TODO: design your own class!!
  public:
    BSTree() : _root(0),_head(0),_tail(0),_size(0) {}
    ~BSTree() { clear(); }
   
   class iterator { 
     friend class BSTree;

     public:
      iterator(BSTreeNode<T>* n = 0):_node(n), _OutOfRange(false) {}
      iterator(const iterator& i):_node(i._node), _OutOfRange(i._OutOfRange) {}
      ~iterator(){}

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
        if(!_node){
          _OutOfRange=!_OutOfRange;
        }
       if(_OutOfRange){
          _OutOfRange=false;
        }
        else if(_node->_right){
          _node=findMin(_node->_right);
        }
        else{
          BSTreeNode<T>* _tmp=findNextOnTop(_node);
          if(!_tmp){
            _OutOfRange=!_OutOfRange;
          }
          else{
            _node=_tmp;
          }
        }
        return *(this); 
      }
      iterator operator ++ (int) { 
        iterator tmp(_node);
        if(!_node){
          _OutOfRange=!_OutOfRange;
        }
        if(_OutOfRange){
          _OutOfRange=false;
        }
        else if(_node->_right){
          _node=findMin(_node->_right);
        }
        else{
          BSTreeNode<T>* _tmp=findNextOnTop(_node);
          if(!_tmp){
            _OutOfRange=!_OutOfRange;
          }
          else{
            _node=_tmp;
          }
        }
        return tmp; 
      }
      iterator& operator -- () { 
        if(!_node){
          _OutOfRange=!_OutOfRange;
        }
        if(_OutOfRange){
          _OutOfRange=false;
        }
        else if(_node->_left){
          _node=findMax(_node->_left);
        }
        else{
          BSTreeNode<T>* _tmp=findLastOnTop(_node);
          if(!_tmp){
            _OutOfRange=!_OutOfRange;
          }
          else{
            _node=_tmp;
          }
        }
        return *(this); 
      }
      iterator operator -- (int) { 
        iterator tmp(_node);
        if(!_node){
          _OutOfRange=!_OutOfRange;
        }
        if(_OutOfRange){
          _OutOfRange=false;
        }
        else if(_node->_left){
          _node=findMax(_node->_left);
        }
        else{
          BSTreeNode<T>* _tmp=findLastOnTop(_node);
          if(!_tmp){
            _OutOfRange=!_OutOfRange;
          }
          else{
            _node=_tmp;
          }
        }
        return tmp; 
      }

      iterator& operator = (const iterator& i) { 
        _node=i._node;
        _OutOfRange=i._OutOfRange;
        return *(this);
      }

      bool operator != (const iterator& i) const { 
        return ((_node!=i._node)||(_OutOfRange!=i._OutOfRange));
      }
      bool operator == (const iterator& i) const { 
        return ((_node==i._node)&&(_OutOfRange==i._OutOfRange));
      }

     private:
      BSTreeNode<T>* _node;
      bool           _OutOfRange;
      BSTreeNode<T>* findMin(BSTreeNode<T>* n){
        if(n->_left){
          return findMin(n->_left);
        }
        else{
          return n;
        }
      }
      BSTreeNode<T>* findMax(BSTreeNode<T>* n){
        if(n->_right){
          return findMax(n->_right);
        }
        else{
          return n;
        }
      }
      BSTreeNode<T>* findNextOnTop(BSTreeNode<T>* n){
        if(!n->_parent){
          return 0;
        }
        else if(n->_parent->_left==n){
          return n->_parent;
        }
        else{
          return findNextOnTop(n->_parent);
        }
      }
      BSTreeNode<T>* findLastOnTop(BSTreeNode<T>* n){
        if(!n->_parent){
          return 0;
        }
        else if(n->_parent->_right==n){
          return n->_parent;
        }
        else{
          return findLastOnTop(n->_parent);
        }
      }
   };

   iterator begin() const { 
     return iterator(_head);
   }
   iterator end() const {
     return ++iterator(_tail);
   }
   bool empty() const {
     return !_size;
   }
   size_t size() const {
     return _size;
   }
   void insert(const T& x) {
     if(empty()){
       _root=new BSTreeNode<T>(x);
       _head=_root;
       _tail=_root;
     }
     else{
       BSTreeNode<T>* _tmp=_root;
       while(1){
         if(x<=_tmp->_data){
           if(_tmp->_left){
             _tmp=_tmp->_left;
           }
           else{
             _tmp->_left=new BSTreeNode<T>(x,_tmp);
             break;
           }
         }
         if(x>_tmp->_data){
           if(_tmp->_right){
              _tmp=_tmp->_right;
           }
           else{
             _tmp->_right=new BSTreeNode<T>(x,_tmp);
             break;
           }
         }
       }
     }
     while(_head->_left){
       _head=_head->_left;
     }
     while(_tail->_right){
       _tail=_tail->_right;
     }
     _size++;
   }
   void pop_front(){
     if(!empty()){
        erase(iterator(_head));
     }
   }
   void pop_back(){
     if(!empty()){
        erase(iterator(_tail));
     }
   }

   bool erase(iterator pos) {
     if(empty()){
       return false;
     }
     if(pos._node->_left && pos._node->_right){
       pos._node->_data=(pos.findMin(pos._node->_right))->_data;
       return erase(++pos);
     }
     else if(pos._node->_left){
       if(pos._node==_root){
         _root=_root->_left;
       }
       if(pos._node==_tail){
         _tail=_tail->_left;
         while(_tail->_right){
           _tail=_tail->_right;
         }
       }
       if(pos._node->_parent){
          if(pos._node->_parent->_left == pos._node){
            pos._node->_parent->_left=pos._node->_left;
          }
          if(pos._node->_parent->_right == pos._node){
            pos._node->_parent->_right=pos._node->_left;
          }
       }
       pos._node->_left->_parent=pos._node->_parent;
       delete pos._node;
      _size--;
     }
     else if(pos._node->_right){
       if(pos._node==_root){
         _root=_root->_right;
       }
       if(pos._node==_head){
         _head=_head->_right;
         while(_head->_left){
           _head=_head->_left;
         }
       }
       if(pos._node->_parent){
          if(pos._node->_parent->_right == pos._node){
            pos._node->_parent->_right=pos._node->_right;
          }
          if(pos._node->_parent->_left == pos._node){
            pos._node->_parent->_left=pos._node->_right;
          }
       }
       pos._node->_right->_parent=pos._node->_parent;
       delete pos._node;
      _size--;
     }
     else{
       if(pos._node==_root){
         _root=0;
         _head=0;
         _tail=0;
       }
       if(pos._node==_head){
         _head=_head->_parent;
       }
       if(pos._node==_tail){
         _tail=_tail->_parent;
       }
       if(pos._node->_parent){
          if(pos._node->_parent->_left == pos._node){
            pos._node->_parent->_left=0;
          }
          if(pos._node->_parent->_right == pos._node){
            pos._node->_parent->_right=0;
          }
       }
       delete pos._node;
      _size--;
     }
     return true;
   }
   bool erase(const T& x) {
     iterator i=find(x);
     if(i==end()){
       return false;
     }
     erase(i);
     return true;
   }

   iterator find(const T& x){
     if(!empty()){
        BSTreeNode<T>* _tmp=_root;
        while(1){
          if(x==_tmp->_data){
            return iterator(_tmp);
          }
          else if(x<_tmp->_data){
            if(_tmp->_left){
              _tmp=_tmp->_left;
            }
            else{
              return end();
            }
          }
          else{
            if(_tmp->_right){
              _tmp=_tmp->_right;
            }
            else{
              return end();
            }
          }
        }
     }
     return end();
   }

   void clear(){
     while(_size){
       pop_back(); 
     }
   }

   void sort() const {}
   void print() const {
     cout<<_root->_data<<endl;
     traverse(_root->_left,2);
     traverse(_root->_right,2);
   }

   void traverse (BSTreeNode<T>* n,int tab) const {
     for(int i=0;i<tab;i++){
       cout<<" ";
     }
     cout<<n->_data<<endl;
     if(n->_left){
       traverse(n->_left,tab+2);
     }
     else{
       for(int i=0;i<tab+2;i++){
          cout<<" ";
       }
       cout<<"[0]"<<endl;
     }
     if(n->_right){
       traverse(n->_right,tab+2);
     }
     else{
       for(int i=0;i<tab+2;i++){
          cout<<" ";
       }
       cout<<"[0]"<<endl;
     }
   }

   private:
    BSTreeNode<T>* _root;
    BSTreeNode<T>* _head;
    BSTreeNode<T>* _tail;
    size_t _size;
};

#endif // BST_H
