/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <map>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

void
CirMgr::randomSim()
{
  srand(time(NULL));
  int total_pattern=0;
  int same_time=0;
  bool flag=false;
  if(fecGrps->empty()){
    GateList* fec = new GateList;
    fec->push_back(gatelist[0]);
    for(unsigned i=0;i<dfslist.size();i++){
      if(dfslist[i]->isAig()){
        fec->push_back(dfslist[i]);
      }
    }
    fecGrps->push_back(fec);
  }
  while(1){
    size_t pi[miloa[1]]={0};
    vector<GateList*>* newFecGrps = new vector<GateList*>;
    for(int i=0;i<miloa[1];i++){
      for(int j=0;j<64;j++){
        pi[i]+=(size_t(rand()%2)<<j);
      }
      gatelist[idlist[i]]->setsignal(pi[i]);
    }
    for(unsigned i=0;i<dfslist.size();i++){
      dfslist[i]->simulate();
    }
    for(unsigned i=0;i<fecGrps->size();i++){
      map<size_t,GateList*> signal_map;
      map<size_t,GateList*>::iterator iter;
      for(unsigned j=0;j<((*fecGrps)[i]->size());j++){
        GateList* g = new GateList;
        iter=signal_map.find((*(*fecGrps)[i])[j]->getsignal());
        if(iter!=signal_map.end()){//exist
          delete g;
          g=iter->second;
          g->push_back((*(*fecGrps)[i])[j]);
        }
        else{
          iter=signal_map.find(~((*(*fecGrps)[i])[j]->getsignal()));
          if(iter!=signal_map.end()){
            delete g;
            g=iter->second;
            g->push_back((*(*fecGrps)[i])[j]);
          }
          else{
            g->push_back((*(*fecGrps)[i])[j]);
            signal_map.insert(pair<size_t,GateList*>((*(*fecGrps)[i])[j]->getsignal(),g));
          }
        }
      }
      for(iter=signal_map.begin();iter!=signal_map.end();iter++){
        GateList* g=iter->second;
        if(g->size()>1){
          newFecGrps->push_back(g);
        }
      }
    }
    if(fecGrps->size()==newFecGrps->size()){
      same_time++;
    }
    else{
      same_time=0;
    }
    for(unsigned i=0;i<fecGrps->size();i++){
      delete ((*fecGrps)[i]);
    }
    delete fecGrps;
    fecGrps=newFecGrps;
    cout<<"\r";
    cout<<"Total #FEC Group = "<< fecGrps->size();
    if(_simLog){
      size_t po[miloa[3]];
      for(int i=0;i<miloa[3];i++){
        po[i]=gatelist[idlist[i+miloa[1]]]->getsignal();
      }
      for(int i=0;i<(total_pattern%64==0? 64:total_pattern%64);i++){
        for(int j=0;j<miloa[1];j++){
          *_simLog<<pi[j]%2;
          pi[j]/=2;
        }
        *_simLog<<" ";
        for(int j=0;j<miloa[3];j++){
          *_simLog<<po[j]%2;
          po[j]/=2;
        }
        *_simLog<<endl;
      }
    }
    total_pattern+=64;
    if(same_time==16){
      break;
    }
  }
  for(unsigned i=0;i<fecGrps->size();i++){
    sort((*fecGrps)[i]->begin(),(*fecGrps)[i]->end(),mysort1);
  }
  sort(fecGrps->begin(),fecGrps->end(),mysort2);
  if(_simLog){
    _simLog->close();
  }
  cout<<"\r";
  cout<<total_pattern<<" patterns simulated."<<endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  int total_pattern=0;
  bool flag=false;
  if(fecGrps->empty()){
    GateList* fec = new GateList;
    fec->push_back(gatelist[0]);
    for(unsigned i=0;i<dfslist.size();i++){
      if(dfslist[i]->isAig()){
        fec->push_back(dfslist[i]);
      }
    }
    fecGrps->push_back(fec);
  }
  do{
    size_t pi[miloa[1]]={0};
    vector<GateList*>* newFecGrps = new vector<GateList*>;
    for(int i=0;i<64;i++){
      string s;
      patternFile>>s;
      if(s==""){
        flag=true;
        break;
      }
      if(s.length()!=miloa[1]){
        if(total_pattern<64){
          fecGrps=newFecGrps;
        }
        cout<<endl;
        cerr<<"Error: Pattern("<<s<<") length("<<s.length()<<") does not match the number of inputs("<<miloa[1]<<") in a circuit!!"<<endl;
        cout<<endl;
        cout<<total_pattern-i<<" patterns simulated."<<endl;
        return;
      }
      for(size_t j=0;j<s.length();j++){
        if(s[j]=='1'){
          pi[j]+=(size_t(1)<<i);
        }
        else if(s[j]!='0'){
          if(total_pattern<64){
            fecGrps=newFecGrps;
          }
          cout<<endl;
          cerr<<"Error: Pattern("<<s<<") contains a non-0/1 character('"<<s[j]<<"')."<<endl;
          cout<<endl;
          cout<<total_pattern-i<<" patterns simulated."<<endl;
          return;
        }
      }
      total_pattern++;
    }
    if(flag&&total_pattern%64==0){
      break;
    }
    for(int i=0;i<miloa[1];i++){
      gatelist[idlist[i]]->setsignal(pi[i]);
    }
    for(unsigned i=0;i<dfslist.size();i++){
      dfslist[i]->simulate();
    }
    for(unsigned i=0;i<fecGrps->size();i++){
      map<size_t,GateList*> signal_map;
      map<size_t,GateList*>::iterator iter;
      for(unsigned j=0;j<((*fecGrps)[i]->size());j++){
        GateList* g = new GateList;
        iter=signal_map.find((*(*fecGrps)[i])[j]->getsignal());
        if(iter!=signal_map.end()){//exist
          delete g;
          g=iter->second;
          g->push_back((*(*fecGrps)[i])[j]);
        }
        else{
          iter=signal_map.find(~((*(*fecGrps)[i])[j]->getsignal()));
          if(iter!=signal_map.end()){
            delete g;
            g=iter->second;
            g->push_back((*(*fecGrps)[i])[j]);
          }
          else{
            g->push_back((*(*fecGrps)[i])[j]);
            signal_map.insert(pair<size_t,GateList*>((*(*fecGrps)[i])[j]->getsignal(),g));
          }
        }
      }
      for(iter=signal_map.begin();iter!=signal_map.end();iter++){
        GateList* g=iter->second;
        if(g->size()>1){
          newFecGrps->push_back(g);
        }
      }
    }
    for(unsigned i=0;i<fecGrps->size();i++){
      delete ((*fecGrps)[i]);
    }
    delete fecGrps;
    fecGrps=newFecGrps;
    cout<<"\r";
    cout<<"Total #FEC Group = "<< fecGrps->size();
    if(_simLog){
      size_t po[miloa[3]];
      for(int i=0;i<miloa[3];i++){
        po[i]=gatelist[idlist[i+miloa[1]]]->getsignal();
      }
      for(int i=0;i<(total_pattern%64==0? 64:total_pattern%64);i++){
        for(int j=0;j<miloa[1];j++){
          *_simLog<<pi[j]%2;
          pi[j]/=2;
        }
        *_simLog<<" ";
        for(int j=0;j<miloa[3];j++){
          *_simLog<<po[j]%2;
          po[j]/=2;
        }
        *_simLog<<endl;
      }
    }
  }while(!flag);
  for(unsigned i=0;i<fecGrps->size();i++){
    sort((*fecGrps)[i]->begin(),(*fecGrps)[i]->end(),mysort1);
  }
  sort(fecGrps->begin(),fecGrps->end(),mysort2);
  if(_simLog){
    _simLog->close();
  }
  cout<<"\r";
  cout<<total_pattern<<" patterns simulated."<<endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
