/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <algorithm>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

int CirGate::Global_mark = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   int tab=50;
   int varid=VarId;
   int lineno=LineNo;
   cout<<"=================================================="<<endl;
   cout<<"= "<<getTypeStr()<<"("<<VarId<<")";
   do{
      tab--;
      varid/=10;
   }while(varid!=0);
   tab-=(2+getTypeStr().size()+2);
   if(!Name.empty()){
      cout<<"\""<<Name<<"\"";
      tab-=(2+Name.size());
   }
   do{
      tab--;
      lineno/=10;
   }while(lineno!=0);
   tab-=7;
   cout<<", line "<<LineNo<<setw(tab)<<"="<<endl;
   cout<<"=================================================="<<endl;
}

void
CirGate::printFanin(int current_level,int level,bool invert) const
{
   if(current_level>level){
      return;
   }
   for(int i=0;i<current_level;i++){
      cout<<"  ";
   }
   if(invert){
      cout<<"!";
   }
   cout<<getTypeStr()<<" "<<VarId;
   if(Mark==Global_mark&&Fanin.size()&&current_level!=level){
      cout<<" (*)";
      cout<<endl;
   }
   else{
      if(current_level!=level&&Fanin.size()){
         Mark=Global_mark;
      }
      cout<<endl;
      for(int i=0;i<Fanin.size();i++){
         Fanin[i]->gate()->printFanin(current_level+1,level,Fanin[i]->isInv());
      }
   }
   return;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   Global_mark++;
   printFanin(0,level,false);
}

void
CirGate::printFanout(int current_level,int level,bool invert) const
{
   if(current_level>level){
      return;
   }
   for(int i=0;i<current_level;i++){
      cout<<"  ";
   }
   if(invert){
      cout<<"!";
   }
   cout<<getTypeStr()<<" "<<VarId;
   if(Mark==Global_mark&&Fanout.size()&&current_level!=level){
      cout<<" (*)";
      cout<<endl;
   }
   else{
      if(current_level!=level&&Fanout.size()){
         Mark=Global_mark;
      }
      cout<<endl;
      for(int i=0;i<Fanout.size();i++){
         Fanout[i]->gate()->printFanout(current_level+1,level,Fanout[i]->isInv());
      }
   }
   return;
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   Global_mark++;
   printFanout(0,level,false);
}

void
CirGate::sort_fanout(){
   sort(Fanout.begin(),Fanout.end(),myfunction);
}