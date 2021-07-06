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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

int CirGate::Global_mark = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   size_t p[64];
   size_t value=signal;
   for(int i=0;i<64;i++){
      p[63-i]=value%2;
      value/=2;
   }
   for(int i=0;i<80;i++){
      cout<<"=";
   }
   cout<<endl;
   cout<<"= "<<getTypeStr()<<"("<<VarId<<")";
   if(!Name.empty()){
      cout<<"\""<<Name<<"\"";
   }
   cout<<", line "<<LineNo<<endl;
   cout<<"= FECs:"<<endl;
   cout<<"= Value: ";
   for(int i=0;i<64;i++){
      if(i%8==0&&i!=0){
         cout<<"_";
      }
      cout<<p[i];
   }
   cout<<endl;
   for(int i=0;i<80;i++){
      cout<<"=";
   }
   cout<<endl;
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
   if(Mark==Global_mark&&FaninSize()&&current_level!=level){
      cout<<" (*)";
      cout<<endl;
   }
   else{
      if(current_level!=level&&FaninSize()){
         Mark=Global_mark;
      }
      cout<<endl;
      for(unsigned i=0;i<Fanin.size();i++){
         if(Fanin[i]){
            Fanin[i]->gate()->printFanin(current_level+1,level,Fanin[i]->isInv());
         }
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
      for(unsigned i=0;i<Fanout.size();i++){
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