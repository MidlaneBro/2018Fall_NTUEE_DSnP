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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
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
private:
  size_t _gateV;
};

class CirGate
{
public:
   CirGate():VarId(0),LineNo(0),Mark(Global_mark){}
   CirGate(int id):VarId(id),LineNo(0),Mark(Global_mark){}
   CirGate(int id,int lineno):VarId(id),LineNo(lineno),Mark(Global_mark){}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   int getVarId() const { return VarId; }
   unsigned getLineNo() const { return LineNo; }
   string getName() const { return Name; }
   void give_Name(string& s){Name=s;}
   void addFanin(CirGate* g,bool phase){
     Fanin.push_back(new GateV(g,phase));
   }
   void addFanout(CirGate* g,bool phase){
     Fanout.push_back(new GateV(g,phase));
   }
   void getFanin(vector<GateV*>& in){
     for(int i=0;i<Fanin.size();i++){
       in.push_back(Fanin[i]);
     }
   }
   void getFanout(vector<GateV*>& out){
     for(int i=0;i<Fanout.size();i++){
       out.push_back(Fanout[i]);
     }
   }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void printFanin(int current_level,int level,bool invert) const;
   void printFanout(int current_level,int level,bool invert) const;

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
  int VarId;
  int LineNo;
  mutable int Mark;
  string Name;
  
protected:
  vector<GateV*> Fanin;
  vector<GateV*> Fanout;
};

class CirPiGate:public CirGate
{
public:
  CirPiGate(int id,int lineno):CirGate(id,lineno){}
  ~CirPiGate(){}
  string getTypeStr() const {return "PI";}
  bool floating_fanin(){return false;}
  bool unused_fanout(){
    if(Fanout.empty()){
      return true;
    }
  } 
  void printGate() const {}; 
};

class CirPoGate:public CirGate
{
public:
  CirPoGate(int id,int lineno):CirGate(id,lineno){}
  string getTypeStr() const {return "PO";}
  bool floating_fanin(){
     for(int i=0;i<Fanin.size();i++){
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
};

class CirAigGate:public CirGate
{
public:
  CirAigGate(int id,int lineno):CirGate(id,lineno){}
  string getTypeStr() const {return "AIG";}
  bool floating_fanin(){
     for(int i=0;i<Fanin.size();i++){
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
  }
  void printGate() const {}
};

class CirUndefGate:public CirGate
{
public:
  CirUndefGate(int id):CirGate(id){}
  string getTypeStr() const {return "UNDEF";}
  bool floating_fanin(){return false;}
  bool unused_fanout(){return false;}
  void printGate() const {}
};

class CirConstGate:public CirGate
{
public:
  CirConstGate():CirGate(0,0){}
  string getTypeStr() const {return "CONST";}
  bool floating_fanin(){return false;}
  bool unused_fanout(){return false;}
  void printGate() const {}
};

#endif // CIR_GATE_H
