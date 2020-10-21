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

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void 
CirGate::printGate() const {
   switch(type){
      case PI_GATE:{
         cout << "PI  " << id;
         if(!symbol.empty())
         cout << " (" << symbol << ")";
         cout << endl;
      break;
      }
      case PO_GATE:{
         cout << "PO  " << id;
         stringstream ss;
         if(getFanin(0)->type == UNDEF_GATE)
            ss << "*";
         if(isFaninInv(0))
            ss << "!";
         ss << getFanin(0)->id;
         cout << " " << ss.str();
         if(!symbol.empty())
         cout << " (" << symbol << ")";
         cout << endl;
      break;
      }
      case AIG_GATE:{
         cout << "AIG " << id;
         for(size_t i = 0;i < 2;i++){
            stringstream ss;
            if(getFanin(i)->type == UNDEF_GATE)
               ss << "*";
            if(isFaninInv(i))
               ss << "!";
            ss << getFanin(i)->id;
            cout << " " << ss.str();
         }
         cout << endl;
      break;
      }
      case CONST_GATE:{
         cout << "CONST0" << endl;
      break;
      }
      default:
      break;
   }
}
void
CirGate::reportGate() const
{
  cout << "================================================================================" << endl;
  stringstream ss;
  ss << "= " + getTypeStr() << "(" << id << ")";
	if (!symbol.empty())	ss << "\"" << symbol << "\"";
	ss << ", line " << line;
	cout << ss.str() << endl;

  // FEC
  cout << "= FECs:";
  if(fecGroup != 0){
    bool isGateInv = 0;
    IdList temp = fecGroup->member;
    sort(temp.begin(), temp.end());
    for(size_t i = 0; i < temp.size(); i++){
      if(temp[i]/2 == id)
        isGateInv = temp[i]%2;
    }
    for(size_t i = 0; i < temp.size(); i++){
      if(temp[i]/2 == id) continue;
      string c;
      c = (temp[i]%2 ^ isGateInv)? "!" : "\0";
      cout << " " << c << temp[i]/2;
    }
  }
  cout << endl;

  // VALUE
  cout << "= Value: ";
  string val = decToBin(simVal);
  for(int i = 63; i >= 0; i--){
    cout << val[i];
    if(i%8 == 0 && i != 0) cout << "_";
  }
  cout << endl;
  // cout << val << endl;
  // cout << "= fIdx: " << fIdx << endl;
	cout << "================================================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   dfsFanin(level);
   cirMgr->resetFlag();
   assert (level >= 0);
}
void 
CirGate::dfsFanin(int level,string space,bool inv) const{
   cout << space;
   if(inv) cout << "!";
   cout << getTypeStr();
   cout << " " << id;
   
   if(level <= 0) {cout << endl; return;}

   if(flag)
      {cout << " (*)" << endl; return;}
   else
      cout << endl; 
   for(size_t i = 0;i < fanin.size(); i++){
      flag = 1;
      getFanin(i)->dfsFanin(level-1,"  "+space,isFaninInv(i));
   }
}
void
CirGate::reportFanout(int level) const
{
   dfsFanout(level);
   cirMgr->resetFlag();
   assert (level >= 0);
}
bool 
CirGate::cmpFanout(CirGate* A,CirGate* B){
   CirGate* AA = (CirGate*)(((size_t)A) & ~size_t(0x1));
   CirGate* BB = (CirGate*)(((size_t)B) & ~size_t(0x1));
   return  AA->id < BB->id;
}

void 
CirGate::dfsFanout(int level,string space,bool inv) const{
   cout << space;
   if(inv) cout << "!";
   cout << getTypeStr();
   cout << " " << id;

   if(level <= 0) {cout << endl; return;}

   if(flag)
      {cout << " (*)" << endl; return;}
   else
      cout << endl;

  // [NOTE] sort in order to be the same as ric
   GateList sortList = fanout;
   sort(sortList.begin(),sortList.end(),cmpFanout);

   for(size_t i = 0;i < sortList.size(); i++){
      flag = 1;
      CirGate* A = (CirGate*)(((size_t)sortList[i]) & ~size_t(0x1));
      bool IA = (size_t)sortList[i] & (0x1);
      A->dfsFanout(level-1,"  "+space,IA);
   }
}

void
CirGate::updateFan(FanType type, CirGate* A, CirGate* B){
  // [NOTE] A B may have been inverted
  // replace A with B
  // if(B == 0) directly delete A
  // if(A == 0) directly add A in fan

  UpdateMode mode = NORMAL;
  if(A == 0) mode = ADD;
  if(B == 0 || (size_t)B == 1) mode = DELETE;

  if(type == FAN_OUT){
    if(mode == ADD){
      fanout.push_back(B);
      return;
    }
    for(size_t i = 0; i < fanout.size(); i++){
      if(fanout[i] == A){
        if(mode == DELETE) fanout.erase(fanout.begin() + i);
        if(mode == NORMAL) fanout[i] = B;
        break;
      }
    }
  }
  else{
    if(mode == ADD){
      fanin.push_back(B);
      return;
    }
    for(size_t i = 0; i < fanin.size(); i++){
      if(fanin[i] == A){
        if(mode == DELETE) fanin.erase(fanin.begin() + i);
        if(mode == NORMAL) fanin[i] = B;
        break;
      }
    }
  }
}

void
CirGate::removeGate(int litA, int litB){
  // litA : 應該用誰來取代他原本接出去的fanout的fanin
  // litB : 應該用誰來取代他原本接入的fanin的fanout
  // 如果為-1, 代表空(i.e.沒東西接入or接出)
  for(size_t i = 0; i < fanout.size(); i++){
    CirGate *tar, *sub, *out, *A;
    A = (litA == -1)? 0 : cirMgr->getGate(litA/2);
    bool inv = isFanoutInv(i), aInv = litA % 2;
    tar = (CirGate*)((size_t)this + inv), sub = (CirGate*)((size_t)A + (inv^aInv) ), out = (CirGate*)((size_t)getFanout(i) + (inv^aInv) );

    getFanout(i)->updateFan(FAN_IN, tar, sub);
    if(A != 0)
      A->updateFan(FAN_OUT, 0, out);
  }

  for(size_t i = 0; i < fanin.size(); i++){
    CirGate *tar, *sub, *B, *in;
    B = (litB == -1)? 0 : cirMgr->getGate(litB/2);
    bool inv = isFaninInv(i), bInv = litB % 2;
    tar = (CirGate*)((size_t)this + inv), sub = (CirGate*)((size_t)B + (inv^bInv) ), in = (CirGate*)((size_t)getFanin(i) + (inv^bInv) );

    getFanin(i)->updateFan(FAN_OUT, tar, sub);
    if(B != 0)
      B->updateFan(FAN_IN, 0, in);
  }
}