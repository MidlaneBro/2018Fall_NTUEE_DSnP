/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class GateV
{
#define NEG 0x1
public:
  GateV(CirGate* g,size_t phase):_gateV(size_t(g)+phase){};
  CirGate* gate() const{
    return (CirGate*)(_gateV & ~size_t(NEG));
  }
  bool isInv() const {
    return (_gateV & NEG);
  }
  size_t get_gateV(){return _gateV;}
private:
  size_t _gateV;
};

class CirGate
{
public:
   CirGate():VarId(0),LineNo(0),Mark(Global_mark),signal(0){}
   CirGate(unsigned id):VarId(id),LineNo(0),Mark(Global_mark),signal(0){}
   CirGate(unsigned id,unsigned lineno):VarId(id),LineNo(lineno),Mark(Global_mark),signal(0){}
   virtual ~CirGate(){}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getVarId() const { return VarId; }
   unsigned getLineNo() const { return LineNo; }
   string getName() const { return Name; }
   void give_Name(string& s){Name=s;}
   void addFanin(CirGate* g,bool phase){
     for(unsigned i=0;i<Fanin.size();i++){
       if(!Fanin[i]){
         Fanin[i]=new GateV(g,phase);
         return;
       }
     }
     Fanin.push_back(new GateV(g,phase));
     assert(Fanin.size()<=2);
   }
   void addFanout(CirGate* g,bool phase){
     Fanout.push_back(new GateV(g,phase));
   }
   void getFanin(vector<GateV*>& in) const{
     for(unsigned i=0;i<Fanin.size();i++){
       if(Fanin[i]){
         in.push_back(Fanin[i]);
       }
     }
   }
   void getFanout(vector<GateV*>& out) const{
     for(unsigned i=0;i<Fanout.size();i++){
       out.push_back(Fanout[i]);
     }
   }
   void removeFanin(CirGate* g){
     for(unsigned i=0;i<Fanin.size();i++){
       if(Fanin[i]){
         if(Fanin[i]->gate()==g){
            delete Fanin[i];
            Fanin[i]=0;
            return;
         }
       }
     }
   }
   void removeFanout(CirGate* g){
     for(vector<GateV*>::iterator i=Fanout.begin();i!=Fanout.end();i++){
       if((*i)->gate()==g){
         delete *i;
         Fanout.erase(i);
         return;
       }
     }
   }
   virtual bool isAig() const = 0;
   void setsignal(size_t s){ signal=s; }
   size_t getsignal() const { return signal; }
   virtual void simulate() = 0;

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void printFanin(int current_level,int level,bool invert) const;
   void printFanout(int current_level,int level,bool invert) const;
   unsigned FaninSize() const{
     unsigned sum=0;
     for(unsigned i=0;i<Fanin.size();i++){
       if(Fanin[i]){
         sum++;
       }
     }
     return sum;
   }

   //helpful functions
   virtual bool floating_fanin() = 0;
   virtual bool unused_fanout() = 0;
   static bool myfunction(GateV* a,GateV* b){
      return a->gate()->getVarId() < b->gate()->getVarId();
   }
   void sort_fanout();
   bool isGlobalMark(){return Global_mark==Mark;};
   void setToGlobalMark(){Mark=Global_mark;}
   static void setGlobalMark(){Global_mark++;}

   //public data member
   static int Global_mark;

private:
  unsigned VarId;
  unsigned LineNo;
  mutable int Mark;
  string Name;
  
protected:
  vector<GateV*> Fanin;
  vector<GateV*> Fanout;
  size_t signal;
};

class CirPiGate:public CirGate
{
public:
  CirPiGate(int id,int lineno):CirGate(id,lineno){}
  ~CirPiGate(){}
  string getTypeStr() const {return "PI";}
  bool isAig() const {return false;}
  bool floating_fanin(){return false;}
  bool unused_fanout(){
    if(Fanout.empty()){
      return true;
    }
    return false;
  } 
  void printGate() const {};
  void simulate(){}
};

class CirPoGate:public CirGate
{
public:
  CirPoGate(int id,int lineno):CirGate(id,lineno){}
  ~CirPoGate(){}
  string getTypeStr() const {return "PO";}
  bool isAig() const {return false;}
  bool floating_fanin(){
     for(unsigned i=0;i<Fanin.size();i++){
       if(!Fanin[i]){
         return true;
       }
       else if(Fanin[i]->gate()->getTypeStr()=="UNDEF"){
         return true;
       }
     }
     return false;
   } 
   bool unused_fanout(){
     return false;
   }
   void printGate() const {}
   void simulate(){
      signal=Fanin[0]->isInv()? ~(Fanin[0]->gate()->getsignal()) : Fanin[0]->gate()->getsignal();
   }
};

class CirAigGate:public CirGate
{
public:
  CirAigGate(int id,int lineno):CirGate(id,lineno){}
  ~CirAigGate(){}
  string getTypeStr() const {return "AIG";}
  bool isAig() const {return true;}
  bool floating_fanin(){
     for(unsigned i=0;i<Fanin.size();i++){
       if(!Fanin[i]){
         return true;
       }
       else if(Fanin[i]->gate()->getTypeStr()=="UNDEF"){
         return true;
       }
     }
     return false;
   }
  bool unused_fanout(){
    if(Fanout.empty()){
      return true;
    }
    return false;
  }
  void printGate() const {}
  void simulate(){
      size_t signal1=Fanin[0]->isInv()? ~(Fanin[0]->gate()->getsignal()) : Fanin[0]->gate()->getsignal();
      size_t signal2=Fanin[1]->isInv()? ~(Fanin[1]->gate()->getsignal()) : Fanin[1]->gate()->getsignal();
      signal=signal1&signal2;
  }
};

class CirUndefGate:public CirGate
{
public:
  CirUndefGate(int id):CirGate(id){}
  ~CirUndefGate(){}
  string getTypeStr() const {return "UNDEF";}
  bool isAig() const {return false;}
  bool floating_fanin(){return false;}
  bool unused_fanout(){
    if(Fanout.empty()){
      return true;
    }
    return false;
  }
  void printGate() const {}
  void simulate(){}
};

class CirConstGate:public CirGate
{
public:
  CirConstGate():CirGate(0,0){}
  ~CirConstGate(){}
  string getTypeStr() const {return "CONST";}
  bool isAig() const {return false;}
  bool floating_fanin(){return false;}
  bool unused_fanout(){return false;}
  void printGate() const {}
  void simulate(){}
};

#endif // CIR_GATE_H
