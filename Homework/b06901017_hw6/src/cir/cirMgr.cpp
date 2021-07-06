/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   lineNo=0;
   colNo=0;
   fstream file;
   file.open(fileName);
   if(!file){
      cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      return false;
   }
   string s;
   int newline;
   file.get(buf,sizeof(buf));
   if(buf[0]==0){
      errMsg="aag";
      return parseError(MISSING_IDENTIFIER);
   }
   if(isspace(buf[0])&&buf[0]!=32){
      errInt=buf[0];
      return parseError(ILLEGAL_WSPACE);
   }
   if(buf[0]==32){
      return parseError(EXTRA_SPACE);
   }
   if(buf[0]!='a'||buf[1]!='a'||buf[2]!='g'){
      s.clear();
      for(int i=0;i<sizeof(buf);i++){
         if(buf[i]==0||isspace(buf[i])){
            break;
         }
         s+=buf[i];
      }
      errMsg=s;
      return parseError(ILLEGAL_IDENTIFIER);
   }
   colNo=3;
   if(buf[3]==0){
      errMsg="number of variables";
      return parseError(MISSING_NUM);
   }
   if((buf[3]>=48&&buf[3]<=57)||buf[3]<32){
      return parseError(MISSING_SPACE);
   }
   if(buf[3]!=32){
      s.clear();
      for(int i=0;i<sizeof(buf);i++){
         if(buf[i]==0||isspace(buf[i])){
            break;
         }
         s+=buf[i];
      }
      errMsg=s;
      return parseError(ILLEGAL_IDENTIFIER);
   }
   colNo=4;
   if(buf[4]==0){
      errMsg="number of variables";
      return parseError(MISSING_NUM);
   }
   if(isspace(buf[4])&&buf[4]!=32){
      errInt=buf[4];
      return parseError(ILLEGAL_WSPACE);
   }
   if(buf[4]==32){
      return parseError(EXTRA_SPACE);
   }
   s.clear();
   for(int i=4;i<sizeof(buf);i++){
      if(buf[i]==0||isspace(buf[i])){
         break;
      }
      s+=buf[i];
   }
   if(!myStr2Int(s,miloa[0])){
      errMsg="number of variables("+s+")";
      return parseError(ILLEGAL_NUM);
   }
   if(miloa[0]<0){
      errMsg="number of variables("+s+")";
      return parseError(ILLEGAL_NUM);
   }
   colNo+=s.size();
   for(int i=1;i<5;i++){
      if(buf[colNo]==0){
         if(i==1){
            errMsg="number of PIs";
         }
         if(i==2){
            errMsg="number of latches";
         }
         if(i==3){
            errMsg="number of POs";
         }
         if(i==4){
            errMsg="number of AIGs";
         }
         return parseError(MISSING_NUM);
      }
      if(buf[colNo]!=32){
         return parseError(MISSING_SPACE);
      }
      colNo++;
      if(buf[colNo]==0){
         if(i==1){
            errMsg="number of PIs";
         }
         if(i==2){
            errMsg="number of latches";
         }
         if(i==3){
            errMsg="number of POs";
         }
         if(i==4){
            errMsg="number of AIGs";
         }
         return parseError(MISSING_NUM);
      }
      if(isspace(buf[colNo])&&buf[colNo]!=32){
         errInt=buf[colNo];
         return parseError(ILLEGAL_WSPACE);
      }
      if(buf[colNo]==32){
         return parseError(EXTRA_SPACE);
      }
      s.clear();
      for(int j=colNo;j<sizeof(buf);j++){
         if(buf[j]==0||isspace(buf[j])){
            break;
         }
         s+=buf[j];
      }
      if(!myStr2Int(s,miloa[i])){
         if(i==1){
            errMsg="number of PIs("+s+")";
         }
         if(i==2){
            errMsg="number of latches("+s+")";
         }
         if(i==3){
            errMsg="number of POs("+s+")";
         }
         if(i==4){
            errMsg="number of AIGs("+s+")";
         }
         return parseError(ILLEGAL_NUM);
      }
      if(miloa[i]<0){
         if(i==1){
            errMsg="number of PIs("+s+")";
         }
         if(i==2){
            errMsg="number of latches("+s+")";
         }
         if(i==3){
            errMsg="number of POs("+s+")";
         }
         if(i==4){
            errMsg="number of AIGs("+s+")";
         }
         return parseError(ILLEGAL_NUM);
      }
      colNo+=s.size();
   }
   if(buf[colNo]!=0){
      return parseError(MISSING_NEWLINE);
   }
   if(miloa[0]<miloa[1]+miloa[2]+miloa[4]){
      errMsg="number of variables";
      errInt=miloa[0];
      return parseError(NUM_TOO_SMALL);
   }
   if(miloa[2]!=0){
      errMsg="latches";
      return parseError(ILLEGAL_NUM);
   }
   file.get();
   lineNo++;
   int literalId;
   vector<int> po_record;
   gatelist.resize(miloa[0]+miloa[3]+1);
   for(int i=1;i<4;i++){
      for(int j=0;j<miloa[i];j++){
         colNo=0;
         newline=file.peek();
         if(newline==EOF){
            if(i==1){
               errMsg="PI";
            }
            if(i==3){
               errMsg="PO";
            }
            return parseError(MISSING_DEF);
         }
         if(newline==10){
            if(i==1){
               errMsg="PI literal ID";
            }
            if(i==3){
               errMsg="PO literal ID";
            }
            return parseError(MISSING_NUM);
         }
         fill(buf,buf+sizeof(buf),0);
         file.get(buf,sizeof(buf));
         if(isspace(buf[0])&&buf[0]!=32){
            errInt=buf[0];
            return parseError(ILLEGAL_WSPACE);
         }
         if(buf[0]==32){
            return parseError(EXTRA_SPACE);
         }
         s.clear();
         for(int k=0;k<sizeof(buf);k++){
            if(buf[k]==0||isspace(buf[k])){
               break;
            }
            s+=buf[k];
         }
         if(!myStr2Int(s,literalId)){
            if(i==1){
               errMsg="PI literal ID("+s+")";
            }
            if(i==3){
               errMsg="PO literal ID("+s+")";
            }
            return parseError(ILLEGAL_NUM);
         }
         if(literalId<0){
            if(i==1){
               errMsg="PI literal ID("+s+")";
            }
            if(i==3){
               errMsg="PO literal ID("+s+")";
            }
            return parseError(ILLEGAL_NUM);
         }
         if(i==1){
            if(literalId/2==0){
               errInt=literalId;
               return parseError(REDEF_CONST);
            }
            if(literalId%2!=0){
               errMsg="PI";
               errInt=literalId;
               return parseError(CANNOT_INVERTED);
            }
            if(gatelist[literalId/2]!=0){
               errInt=literalId;
               errGate=gatelist[literalId/2];
               return parseError(REDEF_GATE);
            }
         }
         if(literalId/2>miloa[0]){
            errInt=literalId;
            return parseError(MAX_LIT_ID);
         }
         colNo+=s.size();
         if(buf[colNo]!=0){
            return parseError(MISSING_NEWLINE);
         }
         file.get();
         lineNo++;
         if(i==1){
            idlist.push_back(literalId/2);
            gatelist[literalId/2]=new CirPiGate(literalId/2,lineNo);
         }
         if(i==3){
            po_record.push_back(literalId);
            idlist.push_back(miloa[0]+j+1);
            gatelist[miloa[0]+j+1]=new CirPoGate(miloa[0]+j+1,lineNo);
         }
      }
   }
   vector<int> net_record[3];
   for(int i=0;i<miloa[4];i++){
      colNo=0;
      newline=file.peek();
      if(newline==EOF){
         errMsg="AIG";
         return parseError(MISSING_DEF);
      }
      if(newline==10){
         errMsg="AIG literal ID";
         return parseError(MISSING_NUM);
      }
      fill(buf,buf+sizeof(buf),0);
      file.get(buf,sizeof(buf));
      if(isspace(buf[0])&&buf[0]!=32){
         errInt=buf[0];
         return parseError(ILLEGAL_WSPACE);
      }
      if(buf[0]==32){
         return parseError(EXTRA_SPACE);
      }
      s.clear();
      for(int i=0;i<sizeof(buf);i++){
         if(buf[i]==0||isspace(buf[i])){
            break;
         }
         s+=buf[i];
      }
      if(!myStr2Int(s,literalId)){
         errMsg="AIG literal ID("+s+")";
         return parseError(ILLEGAL_NUM);
      }
      if(literalId<0){
         errMsg="AIG literal ID("+s+")";
         return parseError(ILLEGAL_NUM);
      }
      if(literalId/2==0){
         errInt=literalId;
         return parseError(REDEF_CONST);
      }
      if(literalId%2!=0){
         errMsg="AIG";
         errInt=literalId;
         return parseError(CANNOT_INVERTED);
      }
      if(literalId/2>miloa[0]){
         errInt=literalId;
         return parseError(MAX_LIT_ID);
      }
      if(gatelist[literalId/2]!=0){
         errInt=literalId;
         errGate=gatelist[literalId/2];
         return parseError(REDEF_GATE);
      }
      net_record[0].push_back(literalId);
      idlist.push_back(literalId/2);
      gatelist[literalId/2]=new CirAigGate(literalId/2,lineNo+1);
      colNo+=s.size();
      for(int t=1;t<3;t++){
         if(buf[colNo]!=32){
            return parseError(MISSING_SPACE);
         }
         colNo++;
         if(buf[colNo]==0){
            errMsg="AIG input literal ID";
            return parseError(MISSING_NUM);
         }
         if(isspace(buf[colNo])&&buf[colNo]!=32){
            errInt=buf[colNo];
            return parseError(ILLEGAL_WSPACE);
         }
         if(buf[colNo]==32){
            return parseError(EXTRA_SPACE);
         }
         s.clear();
         for(int i=colNo;i<sizeof(buf);i++){
            if(buf[i]==0||isspace(buf[i])){
               break;
            }
            s+=buf[i];
         }
         if(!myStr2Int(s,literalId)){
            errMsg="AIG input literal ID("+s+")";
            return parseError(ILLEGAL_NUM);
         }
         if(literalId<0){
            errMsg="AIG input literal ID("+s+")";
            return parseError(ILLEGAL_NUM);
         }
         if(literalId/2>miloa[0]){
            errInt=literalId;
            return parseError(MAX_LIT_ID);
         }
         net_record[t].push_back(literalId);
         colNo+=s.size();
      }
      if(buf[colNo]!=0){
         return parseError(MISSING_NEWLINE);
      }
      file.get();
      lineNo++;
   }
   idlist.push_back(0);
   gatelist[0]=new CirConstGate();
   for(int i=0;i<net_record[0].size();i++){
      if(!gatelist[net_record[1][i]/2]){
         idlist.push_back(net_record[1][i]/2);
         gatelist[net_record[1][i]/2]=new CirUndefGate(net_record[1][i]/2);
      }
      if(!gatelist[net_record[2][i]/2]){
         idlist.push_back(net_record[2][i]/2);
         gatelist[net_record[2][i]/2]=new CirUndefGate(net_record[2][i]/2);
      }
      bool phase1=net_record[1][i]%2;
      bool phase2=net_record[2][i]%2;
      gatelist[net_record[0][i]/2]->addFanin(gatelist[net_record[1][i]/2],phase1);
      gatelist[net_record[0][i]/2]->addFanin(gatelist[net_record[2][i]/2],phase2);
      gatelist[net_record[1][i]/2]->addFanout(gatelist[net_record[0][i]/2],phase1);
      gatelist[net_record[2][i]/2]->addFanout(gatelist[net_record[0][i]/2],phase2);
   }
   for(int i=0;i<po_record.size();i++){
      if(!gatelist[po_record[i]/2]){
         idlist.push_back(po_record[i]/2);
         gatelist[po_record[i]/2]=new CirUndefGate(po_record[i]/2);
      }
      bool phase=po_record[i]%2;
      gatelist[miloa[0]+i+1]->addFanin(gatelist[po_record[i]/2],phase);
      gatelist[po_record[i]/2]->addFanout(gatelist[miloa[0]+i+1],phase);
   }
   while(1){
      newline=file.peek();
      if(newline==EOF||newline==99){
         break;
      }
      colNo=0;
      if(newline==10){
         errMsg="";
         return parseError(ILLEGAL_SYMBOL_TYPE);
      }
      fill(buf,buf+sizeof(buf),0);
      file.get(buf,sizeof(buf));
      if(isspace(buf[0])&&buf[0]!=32){
         errInt=buf[0];
         return parseError(ILLEGAL_WSPACE);
      }
      if(buf[0]==32){
         return parseError(EXTRA_SPACE);
      }
      if(buf[0]!='i'&&buf[0]!='o'){
         errMsg.clear();
         errMsg+=buf[0];
         return parseError(ILLEGAL_SYMBOL_TYPE);
      }
      colNo=1;
      if(buf[colNo]==32){
         return parseError(EXTRA_SPACE);
      }
      if(isspace(buf[colNo])&&buf[colNo]!=32){
         errInt=buf[colNo];
         return parseError(ILLEGAL_WSPACE);
      }
      s.clear();
      for(int i=1;i<sizeof(buf);i++){
         if(buf[i]==0||isspace(buf[i])){
            break;
         }
         s+=buf[i];
      }
      int order;
      if(!myStr2Int(s,order)){
         errMsg="symbol index ("+s+")";
         return parseError(ILLEGAL_NUM);
      }
      if(order<0){
         errMsg="symbol index ("+s+")";
         return parseError(ILLEGAL_NUM);
      }
      colNo+=s.size();
      if(buf[colNo]==0){
         errMsg="symbolic name";
         return parseError(MISSING_IDENTIFIER);
      }
      if(buf[colNo]!=32){
         return parseError(MISSING_SPACE);
      }
      colNo++;
      if(buf[colNo]==0){
         errMsg="symbolic name";
         return parseError(MISSING_IDENTIFIER);
      }
      if(buf[0]=='i'&&order>=miloa[1]){
         errMsg="PI index";
         errInt=order;
         return parseError(NUM_TOO_BIG);
      }
      if(buf[0]=='o'&&order>=miloa[3]){
         errMsg="PO index";
         errInt=order;
         return parseError(NUM_TOO_BIG);
      }
      string name;
      for(int i=colNo;i<sizeof(buf);i++){
         if(buf[i]==0){
            break;
         }
         if(buf[i]>0&&buf[i]<32){
            errInt=buf[i];
            return parseError(ILLEGAL_SYMBOL_NAME);
         }
         name+=buf[i];
         colNo++;
      }
      if(buf[0]=='i'){
         if(gatelist[idlist[order]]->getName()!=""){
            errMsg="i";
            errInt=order;
            return  parseError(REDEF_SYMBOLIC_NAME);
         }
         gatelist[idlist[order]]->give_Name(name);
      }
      if(buf[0]=='o'){
         if(gatelist[idlist[miloa[1]+order]]->getName()!=""){
            errMsg="o";
            errInt=order;
            return  parseError(REDEF_SYMBOLIC_NAME);
         }
         gatelist[idlist[miloa[1]+order]]->give_Name(name);
      }
      file.get();
      lineNo++;
   }
   if(newline==99){
      fill(buf,buf+sizeof(buf),0);
      file.get(buf,sizeof(buf));
      colNo=1;
      if(buf[1]!=0){
         return parseError(MISSING_NEWLINE);
      }
   }
   for(int i=0;i<gatelist.size();i++){
      if(gatelist[i]){
         gatelist[i]->sort_fanout();
      }
   }
   CirGate::setGlobalMark();
   for(int i=miloa[1];i<miloa[1]+miloa[3];i++){
      dfs(gatelist[idlist[i]]);
   }
   file.close();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout<<endl;
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<setw(4)<<"PI"<<setw(12)<<miloa[1]<<endl;
   cout<<setw(4)<<"PO"<<setw(12)<<miloa[3]<<endl;
   cout<<setw(5)<<"AIG"<<setw(11)<<miloa[4]<<endl;
   cout<<"------------------"<<endl;
   cout<<setw(7)<<"Total"<<setw(9)<<miloa[1]+miloa[3]+miloa[4]<<endl;
}

void
CirMgr::printNetlist() const
{ 
   cout<<endl;
   for(int i=0;i<dfslist.size();i++){
      cout<<"["<<i<<"] ";
      string s=dfslist[i]->getTypeStr();
      cout<<s;
      int tab=4;
      tab-=s.size();
      for(int j=0;j<tab;j++){
         cout<<" ";
      }
      cout<<dfslist[i]->getVarId();
      vector<GateV*> in;
      dfslist[i]->getFanin(in);
      for(int j=0;j<in.size();j++){
         cout<<" ";
         if(in[j]->gate()->getTypeStr()=="UNDEF"){
            cout<<"*";
         }
         in[j]->isInv()? cout<<"!":cout<<"";
         cout<<in[j]->gate()->getVarId();
      }
      if(!(dfslist[i]->getName().empty())){
         cout<<" ("<<dfslist[i]->getName()<<")";
      }
      cout<<endl;
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i=0;i<miloa[1];i++){
      cout<<" "<<idlist[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i=miloa[1];i<miloa[1]+miloa[3];i++){
      cout<<" "<<idlist[i];
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   IdList type1,type2;
   for(int i=0;i<gatelist.size();i++){
      if(gatelist[i]){
         if(gatelist[i]->floating_fanin()){
            type1.push_back(i);
         }
         if(gatelist[i]->unused_fanout()){
            type2.push_back(i);
         }
      }
   }
   if(!type1.empty()){
      cout<<"Gates with floating fanin(s):";
      for(int i=0;i<type1.size();i++){
         cout<<" "<<type1[i];
      }
      cout<<endl;
   }
   if(!type2.empty()){
      cout<<"Gates defined but not used  :";
      for(int i=0;i<type2.size();i++){
         cout<<" "<<type2[i];
      }
      cout<<endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   GateList aig;
   for(int i=0;i<dfslist.size();i++){
      if(dfslist[i]->getTypeStr()=="AIG"){
         aig.push_back(dfslist[i]);
      }
   }
   outfile<<"aag";
   for(int i=0;i<4;i++){
      outfile<<" "<<miloa[i];
   }
   outfile<<" "<<aig.size();
   outfile<<endl;
   for(int i=0;i<miloa[1];i++){
      outfile<<idlist[i]*2<<endl;
   }
   for(int i=miloa[1];i<miloa[1]+miloa[3];i++){
      vector<GateV*> g;
      gatelist[idlist[i]]->getFanin(g);
      for(int j=0;j<g.size();j++){
         outfile<<g[j]->gate()->getVarId()*2+g[j]->isInv();
      }
      outfile<<endl;
   }
   for(int i=0;i<aig.size();i++){
      outfile<<aig[i]->getVarId()*2;
      vector<GateV*> g;
      aig[i]->getFanin(g);
      for(int j=0;j<g.size();j++){
         outfile<<" ";
         outfile<<g[j]->gate()->getVarId()*2+g[j]->isInv();
      }
      outfile<<endl;
   }
   for(int i=0;i<miloa[1];i++){
      if(!gatelist[idlist[i]]->getName().empty()){
         outfile<<"i"<<i<<" "<<gatelist[idlist[i]]->getName()<<endl;
      }
   }
   for(int i=miloa[1];i<miloa[1]+miloa[3];i++){
      if(!gatelist[idlist[i]]->getName().empty()){
         outfile<<"o"<<i-miloa[1]<<" "<<gatelist[idlist[i]]->getName()<<endl;
      }
   }
   outfile<<"c"<<endl;
   outfile<<"AAG output by Min-Hsien (Jacky) Chung"<<endl;
}

void 
CirMgr::dfs(CirGate* node)
{
   if(node->getTypeStr()!="UNDEF"){
      vector<GateV*> children;
      node->getFanin(children);
      for(int i=0;i<children.size();i++){
         if(!children[i]->gate()->isGlobalMark()){
            dfs(children[i]->gate());
         }
      }
      node->setToGlobalMark();
      dfslist.push_back(node);
   }
}