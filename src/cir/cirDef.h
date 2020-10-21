/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include <bitset>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum
#define myLit(x,y) (y? 2*x+1: 2*x) 
#define decToBin(x) (bitset<64>(x).to_string())
#define binToDec(x) (bitset<64>(x).to_ulong())

class CirGate;
class CirMgr;
class FecGroup;

typedef vector<CirGate*>           GateList;
typedef vector<unsigned>           IdList;
typedef vector<FecGroup*>          FecList;

enum GateType
{
   UNDEF_GATE = 0,
   PI_GATE    = 1,
   PO_GATE    = 2,
   AIG_GATE   = 3,
   CONST_GATE = 4,

   TOT_GATE
};

class SatSolver;

#endif // CIR_DEF_H
