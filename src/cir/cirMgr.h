/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <set>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class FecGroup
{
  friend class CirMgr;
  friend class CirGate;
  public:
    FecGroup() {}
    ~FecGroup() {}
    void removeMember(size_t);
    void setMember(size_t, size_t);
    void pushMember(size_t);
    void setBase();
  private:
    IdList member;
    size_t baseIdx;
};

class CirMgr
{
   friend class FecGroup;
public:
   CirMgr() { hasSim = 0; hasFraig = 0;}
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if(gid > maxIndex+poOrder.size()) return 0;
      if(gateList[gid] == 0) return 0; 
      else return gateList[gid];
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   //my functiom
   void dfsPostOrder(CirGate*);
   void resetFlag();

   // simulation
   void sim64(vector<size_t>&, bool = 0);
   void log64(size_t l = 64);
   void initFecList();
   bool removeFecGroup(size_t);
   
   //fraig
   void genProofModel(GateList&);
   bool isSAT(size_t,size_t,vector<size_t>&,GateList&);
   void updateDfsFraig();
   void findFaninCone(CirGate*,GateList&);

   //writeGate
   void writeGateDfs(CirGate*, vector<size_t>&) const;

private:
   ofstream           *_simLog;
   GateList gateList;
   GateList dfsList;
   vector<size_t> piOrder;
   vector<size_t> poOrder;
   size_t maxIndex;
   FecList fecList;
   bool hasSim;
   bool hasFraig;
};


#endif // CIR_MGR_H
