/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   if(!CmdExec::lexOptions(option,tokens)){
        return CMD_EXEC_ERROR;
   }
   size_t n=tokens.size();
   if(n==0){
        return CmdExec::errorOption(CMD_OPT_MISSING,"");
   }
   int numObj;
   int arrsize;
   if(myStr2Int(tokens[0],numObj)){
           if(numObj<=0){
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[0]); 
           }
           else if(n==1){
                try{
                        mtest.newObjs(numObj);
                }catch(bad_alloc){
                        return CMD_EXEC_ERROR;
                }
           }
           else if(!myStrNCmp("-Array",tokens[1],2)){
                if(n==2){
                        return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);
                }
                else if(myStr2Int(tokens[2],arrsize)){
                        if(arrsize<=0){
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                        }
                        else if(n==3){
                                try{
                                        mtest.newArrs(numObj,arrsize);
                                }catch(bad_alloc){
                                        return CMD_EXEC_ERROR;
                                }
                        }
                        else{
                                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[3]);
                        }
                        
                }
                else{
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                }
           }
           else{
                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[1]);
           }
   }
   else if(!myStrNCmp("-Array",tokens[0],2)){
           if(n==1){
                return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]); 
           }
           else if(myStr2Int(tokens[1],arrsize)){
                if(arrsize<=0){
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
                }
                else if(n==2){
                        return CmdExec::errorOption(CMD_OPT_MISSING,"");
                }
                else if(myStr2Int(tokens[2],numObj)){
                        if(numObj<=0){
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                        }
                        else if(n==3){
                                try{
                                        mtest.newArrs(numObj,arrsize);
                                }catch(bad_alloc){
                                        return CMD_EXEC_ERROR;
                                }
                        }
                        else{
                                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[3]);
                        }
                }
                else if(!myStrNCmp("-Array",tokens[2],2)){
                        return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[2]);
                }
                else{
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]); 
                }
           }
           else{
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
           }
   }
   else{
           return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[0]);
   }
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   if(!CmdExec::lexOptions(option,tokens)){
        return CMD_EXEC_ERROR;
   }
   size_t n=tokens.size();
   if(n==0){
        return CmdExec::errorOption(CMD_OPT_MISSING,"");
   }
   int objId;
   int numRandId;
   if(!myStrNCmp("-Index",tokens[0],2)){
        if(n==1){
                return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]);
        }
        else if(myStr2Int(tokens[1],objId)){
                if(objId<0){
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
                }
                else if(n==2){
                        if(objId>=mtest.getObjListSize()){
                                cerr<<"Size of object list ("<<mtest.getObjListSize()<<") is <= "<<objId<<"!!"<<endl;
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
                        }
                        else{
                                mtest.deleteObj(objId);
                        }
                }
                else{
                        if(!myStrNCmp("-Array",tokens[2],2)){
                                if(n==3){
                                        if(objId>=mtest.getArrListSize()){
                                                cerr<<"Size of array list ("<<mtest.getArrListSize()<<") is <= "<<objId<<"!!"<<endl;
                                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
                                        }
                                        else{
                                                mtest.deleteArr(objId);
                                        }
                                }
                                else if(!myStrNCmp("-Index",tokens[3],2)||!myStrNCmp("-Random",tokens[3],2)||!myStrNCmp("-Array",tokens[3],2)){
                                        return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[3]);
                                }
                                else{
                                       return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[3]);
                                }
                        }
                        else if(!myStrNCmp("-Index",tokens[2],2)||!myStrNCmp("-Random",tokens[2],2)){
                                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[2]);
                        }
                        else{
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                        }
                }
        }
        else{
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
        }
   }
   else if(!myStrNCmp("-Random",tokens[0],2)){
        if(n==1){
                return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]);
        }
        else if(myStr2Int(tokens[1],numRandId)){
                if(numRandId<=0){
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]); 
                }
                else if(n==2){
                        if(!mtest.getObjListSize()){
                                cerr<<"Size of object list is 0!!"<<endl;
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[0]); 
                        }
                        else{
                                for(int i=0;i<numRandId;i++){
                                        mtest.deleteObj(rnGen(mtest.getObjListSize()));
                                }
                        }
                }
                else{
                        if(!myStrNCmp("-Array",tokens[2],2)){
                                if(n==3){
                                        if(!mtest.getArrListSize()){
                                                cerr<<"Size of array list is 0!!"<<endl;
                                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[0]); 
                                        }
                                        else{
                                                for(int i=0;i<numRandId;i++){
                                                        mtest.deleteArr(rnGen(mtest.getArrListSize()));
                                                }
                                        }
                                }
                                else if(!myStrNCmp("-Index",tokens[3],2)||!myStrNCmp("-Random",tokens[3],2)||!myStrNCmp("-Array",tokens[3],2)){
                                        return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[3]);
                                }
                                else{
                                       return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[3]);
                                }
                        }
                        else if(!myStrNCmp("-Index",tokens[2],2)||!myStrNCmp("-Random",tokens[2],2)){
                                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[2]);
                        }
                        else{
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                        }
                }
        }
        else{
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
        }
   }
   else if(!myStrNCmp("-Array",tokens[0],2)){
           if(n==1){
                return CmdExec::errorOption(CMD_OPT_MISSING,"");
           }
           else if(!myStrNCmp("-Index",tokens[1],2)){
                if(n==2){
                        return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);
                }
                else if(myStr2Int(tokens[2],objId)){
                        if(objId<0){
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                        }
                        else if(n==3){
                                if(objId>=mtest.getArrListSize()){
                                        cerr<<"Size of array list ("<<mtest.getArrListSize()<<") is <= "<<objId<<"!!"<<endl;
                                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                                }
                                else{
                                        mtest.deleteArr(objId);
                                }
                        }
                        else if(!myStrNCmp("-Index",tokens[3],2)||!myStrNCmp("-Random",tokens[3],2)||!myStrNCmp("-Array",tokens[3],2)){
                                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[3]);
                        }
                        else{
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[3]);
                        }
                }
                else{
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                }
           }
           else if(!myStrNCmp("-Random",tokens[1],2)){
                if(n==2){
                        return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);
                }
                else if(myStr2Int(tokens[2],numRandId)){
                        if(numRandId<=0){
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                        }
                        else if(n==3){
                                if(!mtest.getArrListSize()){
                                        cerr<<"Size of array list is 0!!"<<endl;
                                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
                                }
                                else{
                                        for(int i=0;i<numRandId;i++){
                                                mtest.deleteArr(rnGen(mtest.getArrListSize()));
                                        }
                                }
                        }
                        else if(!myStrNCmp("-Index",tokens[3],2)||!myStrNCmp("-Random",tokens[3],2)||!myStrNCmp("-Array",tokens[3],2)){
                                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[3]);
                        }
                        else{
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[3]);
                        }
                }
                else{
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
                }
           }
           else if(!myStrNCmp("-Array",tokens[1],2)){
                return CmdExec::errorOption(CMD_OPT_EXTRA,tokens[1]);
           }
           else{
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
           }
   }
   else{
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[0]);
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


