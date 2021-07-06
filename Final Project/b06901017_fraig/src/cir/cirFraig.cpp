/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/

class StrashKey
{
  #define NEG 0x1
  public:
    StrashKey():in0(0),in1(0){}
    StrashKey(CirGate* g){
      assert(g->getTypeStr()=="AIG");
      vector<GateV*> fanin;
      g->getFanin(fanin);
      for(unsigned i=0;i<fanin.size();i++){
        if(i==0){
          in0=fanin[0]->get_gateV();
        }
        if(i==1){
          in1=fanin[1]->get_gateV();
        }
      }
    }
    ~StrashKey(){}
    size_t operator() () const{
      return in0+in1;
    }
    bool operator == (const StrashKey& key) const{
      return ((in0==key.in0&&in1==key.in1)||(in1==key.in0&&in0==key.in1));
    }
  private:
    size_t in0;
    size_t in1;
};

// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  HashMap<StrashKey,CirGate*> Strash(getHashSize(miloa[4]));
  for(unsigned i=0;i<dfslist.size();i++){
    if(dfslist[i]->isAig()){
      StrashKey key(dfslist[i]);
      if(!Strash.insert(key,dfslist[i])){
        CirGate* g;
        Strash.query(key,g);
        vector<GateV*> fanout;
        dfslist[i]->getFanout(fanout);
        cout<<"Strashing: "<<g->getVarId()<<" merging "<<dfslist[i]->getVarId()<<"..."<<endl;
        remove(dfslist[i]);
        dfslist[i]=0;
        for(unsigned j=0;j<fanout.size();j++){
          g->addFanout(fanout[j]->gate(),fanout[j]->isInv());
          fanout[j]->gate()->addFanin(g,fanout[j]->isInv());
        }
      }
    }
  }
  dfslist.clear();
  CirGate::setGlobalMark();
  for(int i=miloa[1];i<miloa[1]+miloa[3];i++){
      dfs(gatelist[idlist[i]]);
  }
}

void
CirMgr::fraig()
{
  
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
