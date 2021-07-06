/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  IdList removelist;//non-zero gate is to be removed
  for(unsigned i=0;i<gatelist.size();i++){
    if(gatelist[i]){
      removelist.push_back(gatelist[i]->getVarId());
    }
    else{
      removelist.push_back(0);
    }
  }
  for(unsigned i=0;i<dfslist.size();i++){
    removelist[dfslist[i]->getVarId()]=0;
  }
  for(unsigned i=0;i<gatelist.size();i++){
    if(removelist[i]&&(gatelist[i]->getTypeStr()!="PI")){
      cout<<"Sweeping: "<<gatelist[i]->getTypeStr()<<"("<<gatelist[i]->getVarId()<<") removed..."<<endl;
      remove(gatelist[i]);
    }
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  for(unsigned i=0;i<dfslist.size();i++){
    if(dfslist[i]->isAig()){
      bool flag=false;
      vector<GateV*> fanin;
      vector<GateV*> fanout;
      dfslist[i]->getFanin(fanin);
      dfslist[i]->getFanout(fanout);
      for(unsigned j=0;j<2;j++){
        if(fanin[j]->gate()->getTypeStr()=="CONST"){
          if(fanin[j]->isInv()){//case1
            cout<<"Simplifying: "<<fanin[1-j]->gate()->getVarId()<<" merging "<<(fanin[1-j]->isInv()? "!":"")<<dfslist[i]->getVarId()<<"..."<<endl;
            remove(dfslist[i]);
            dfslist[i]=0;
            for(unsigned k=0;k<fanout.size();k++){
              fanin[1-j]->gate()->addFanout(fanout[k]->gate(),!(fanin[1-j]->isInv()==fanout[k]->isInv()));
              fanin[1-j]->gate()->sort_fanout();
              fanout[k]->gate()->addFanin(fanin[1-j]->gate(),!(fanin[1-j]->isInv()==fanout[k]->isInv()));
            }
            flag=true;
            break;
          }
          else{//case2
            cout<<"Simplifying: 0 merging "<<dfslist[i]->getVarId()<<"..."<<endl;
            remove(dfslist[i]);
            dfslist[i]=0;
            for(unsigned k=0;k<fanout.size();k++){
              fanin[j]->gate()->addFanout(fanout[k]->gate(),fanout[k]->isInv());
              fanin[j]->gate()->sort_fanout();
              fanout[k]->gate()->addFanin(fanin[j]->gate(),fanout[k]->isInv());
            }
            flag=true;
            break;
          }
        }
      }
      if(fanin[0]->gate()==fanin[1]->gate()&&flag==false){
        if(fanin[0]->isInv()==fanin[1]->isInv()){//case3
          cout<<"Simplifying: "<<fanin[0]->gate()->getVarId()<<" merging "<<(fanin[0]->isInv()? "!":"")<<dfslist[i]->getVarId()<<"..."<<endl;
          remove(dfslist[i]);
          dfslist[i]=0;
          for(unsigned k=0;k<fanout.size();k++){
            fanin[0]->gate()->addFanout(fanout[k]->gate(),!(fanin[0]->isInv()==fanout[k]->isInv()));
            fanin[0]->gate()->sort_fanout();
            fanout[k]->gate()->addFanin(fanin[0]->gate(),!(fanin[0]->isInv()==fanout[k]->isInv()));
          }
        }
        else{//case4
          cout<<"Simplifying: 0 merging "<<dfslist[i]->getVarId()<<"..."<<endl;
          remove(dfslist[i]);
          dfslist[i]=0;
          for(unsigned k=0;k<fanout.size();k++){
            gatelist[0]->addFanout(fanout[k]->gate(),fanout[k]->isInv());
            gatelist[0]->sort_fanout();
            fanout[k]->gate()->addFanin(gatelist[0],fanout[k]->isInv());
          }
        }
      }
    }
  }
  for(int i=miloa[1]+miloa[3]+miloa[4]+1;i<idlist.size();i++){
    if(gatelist[idlist[i]]->unused_fanout()){
      remove(gatelist[idlist[i]]);
    }
  }
  dfslist.clear();
  CirGate::setGlobalMark();
  for(int i=miloa[1];i<miloa[1]+miloa[3];i++){
      dfs(gatelist[idlist[i]]);
  }
}


/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
