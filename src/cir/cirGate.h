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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.
/*******************************/
/*   Global variable and enum  */
/*******************************/

enum FanType
{
   FAN_IN = 0,
   FAN_OUT = 1,
};
enum UpdateMode
{
   NORMAL = 0,
   ADD = 1,
   DELETE = 2,
};

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
   friend class CirMgr;
   friend class CirAIGGate;
   friend class CirPOGate;
   friend class FecGroup;
public:
   CirGate() {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { 
    if (type == UNDEF_GATE)	return "UNDEF";
    if (type == PI_GATE)	return "PI";
    if (type == PO_GATE)	return "PO";
    if (type == AIG_GATE)	return "AIG";
    if (type == CONST_GATE)	return "CONST";
    if (type == TOT_GATE)	return "TOT";
    return "UNDEF";
  }
   unsigned getLineNo() const { return line; }
   virtual bool isAig() const { if(type == AIG_GATE) return true; else return false; }
   // Printing functions
   void printGate() const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   //my hw6 function
   void dfsFanin(int,string = "",bool = 0) const;
   void dfsFanout(int,string = "",bool = 0) const;

   void addFanin(CirGate* g, bool inv = false) {
      if (inv)	g = (CirGate*)((size_t)g + 1);
      fanin.push_back(g);
   }
   CirGate* getFanin(size_t i) const {
      if (i >= fanin.size())	return 0;
      return (CirGate*)(((size_t)fanin[i]) & ~size_t(0x1));
   }
   bool isFaninInv(size_t i) const{
      return (size_t)fanin[i] & (0x1);
   }
   void addFanout(CirGate* g,bool inv = false) {
      if (inv)	g = (CirGate*)((size_t)g + 1);
      fanout.push_back(g);
   }
   CirGate* getFanout(size_t i) const {
      if (i >= fanout.size())	return 0;
      return (CirGate*)(((size_t)fanout[i]) & ~size_t(0x1));
   }
   bool isFanoutInv(size_t i) const{
      return (size_t)fanout[i] & (0x1);
   }
   // my fraig function
   void updateFan(FanType,CirGate*,CirGate*);
   void removeGate(int, int);

   // simulation
   virtual void simulate() = 0;

   // fraig
   void setVar(size_t& v){var = v;}
   size_t getVar(){return var;}

   //dfsfanout
   static bool cmpFanout(CirGate*,CirGate*);
private:

protected: 
   GateList fanin;
   GateList fanout;
   GateType type;
   string symbol;
   size_t line;
   size_t id;
   mutable bool flag;
   size_t simVal;
   FecGroup* fecGroup;
   size_t var;
   size_t fIdx;
   int dOrder;
};

class CirUNDEFGate : public CirGate{
public:
   CirUNDEFGate(size_t l,size_t i){ 
      type = UNDEF_GATE,line = l,id = i;flag = 0; 
      simVal = 0, fecGroup = 0;
   }
   ~CirUNDEFGate(){}
   void simulate(){
     // do nothing
   }
};

class CirPIGate : public CirGate{
public:
   CirPIGate(size_t l,size_t i){ 
      type = PI_GATE,line = l,id = i;flag = 0;
      simVal = 0, fecGroup = 0;
   }
   ~CirPIGate(){}
   void simulate(){
     // do nothing
   }
};

class CirPOGate : public CirGate{
public:
   CirPOGate(size_t l,size_t i){ 
      type = PO_GATE,line = l,id = i;flag = 0;
      simVal = 0, fecGroup = 0;
   }
   ~CirPOGate(){}
   void simulate(){
      size_t i = getFanin(0)->simVal;
      if(isFaninInv(0)) i = ~i;
      simVal = i;
   }
};

class CirAIGGate : public CirGate{
public:
   CirAIGGate(size_t l,size_t i){ 
      type = AIG_GATE,line = l,id = i;flag = 0; 
      simVal = 0, fecGroup = 0;
   }
   ~CirAIGGate(){}
   void simulate(){
      size_t l = getFanin(0)->simVal;
      size_t r = getFanin(1)->simVal;
      if(isFaninInv(0)) l = ~l;
      if(isFaninInv(1)) r = ~r;
      simVal = l & r;
   }
};

class CirCONSTGate : public CirGate{
public:
   CirCONSTGate(size_t l,size_t i){ 
      type = CONST_GATE,line = l,id = i,flag = 0; 
      simVal = 0, fecGroup = 0;
   }
   ~CirCONSTGate(){}
   void simulate(){
     simVal = 0;
   }
};

#endif // CIR_GATE_H
