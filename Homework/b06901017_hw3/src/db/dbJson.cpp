/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string s1;
   string s2;
   string s3;
   int value;
   while(1){
   	is>>s1;
   	if(s1=="}"){
   		break;
   	}
   	is>>s2;
    if(s1=="{"&&s2=="}"){
      break;
    }
    is>>s3>>value;
   	string key;
   	key=key.assign(s2,1,s2.size()-2);
   	j._obj.emplace_back(key,value);
   }
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
  	os<<"{"<<endl;
    for(int i=0;i<j.size();i++){
      os<<"  "<<j._obj[i];
      if(i!=j.size()-1){
        os<<",";
      }
      os<<endl;
    }
    os<<"}"<<endl;
    os<<"Total JSON elements: "<<j.size()<<endl;
  	return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
  _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
  for(int i=0;i<_obj.size();i++){
    if(_obj[i].key()==elm.key()&&_obj[i].value()==elm.value()){
      return false;
    }
  }
  _obj.emplace_back(elm.key(),elm.value());
  return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
  if(_obj.empty()){
    return NAN;
  }
  else{
    float ave=0.0;
    for(int i=0;i<_obj.size();i++){
      ave+=_obj[i].value();
    }
    ave/=_obj.size();
    return ave;
  }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if(_obj.empty()){
    idx=size();
   }
   else{
     for(int i=0;i<_obj.size();i++){
       if(_obj[i].value()>maxN){
         maxN=_obj[i].value();
         idx=i;
       }
     }
   }
   return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if(_obj.empty()){
    idx=size();
  }
  else{
    for(int i=0;i<_obj.size();i++){
      if(_obj[i].value()<minN){
        minN=_obj[i].value();
        idx=i;
      }
    }
  }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
  int s = 0;
  if(!_obj.empty()){
    for(int i=0;i<_obj.size();i++){
      s+=_obj[i].value();
    }
  }
  return s;
}
