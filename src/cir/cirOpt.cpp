/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  // Here, CirGate::flag represents whether gate is in dfsList.(in:true)
  for(size_t i = 0; i < dfsList.size(); i++){
    dfsList[i]->flag = true;
  }

  for(size_t i = 0; i < gateList.size(); i++){
    if(gateList[i] != 0 && gateList[i]->flag == false){
      if(gateList[i]->type != CONST_GATE&& gateList[i]->type != PI_GATE){
        cout << "Sweeping: " << gateList[i]->getTypeStr() << "(" << gateList[i]->id << ")" << " removed..." << endl;
        gateList[i]->removeGate(-1,-1);
        gateList[i] = 0;
        delete gateList[i];
      }
    }
  }
  resetFlag(); // reset flags which represents the existence in dfsList

  // update dfsList
  dfsList.clear();
  for(size_t i = 0;i < poOrder.size(); i++){
    dfsPostOrder(gateList[poOrder[i]]);
  }
  resetFlag();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  // iterate dfsList to perform four type of optimization
  for(size_t i = 0; i < dfsList.size(); i++){
    int litL, litR, litSub = -1;
    if(dfsList[i]->type == AIG_GATE){
      litL = dfsList[i]->getFanin(0)->id*2 + dfsList[i]->isFaninInv(0);
      litR = dfsList[i]->getFanin(1)->id*2 + dfsList[i]->isFaninInv(1);
      if(litL == 1||litR == 1){
        if(litL == 1) litSub = litR;
        else litSub = litL;
      }
      if(litL == 0||litR == 0){
        litSub = 0;
      }
      if(litL/2 == litR/2){
        if(litL == litR) litSub = litL;
        else litSub = 0;
      }
      if(litSub != -1){
        dfsList[i]->removeGate(litSub,-1);
        gateList[dfsList[i]->id] = 0;
        string c;
        if(litSub%2) c = "!";
        cout << "Simplifying: " << litSub/2 <<" merging " << c << dfsList[i]->id << "..." << endl;
        delete dfsList[i];
      }
    }
  }
  for(size_t i = 0; i < dfsList.size(); i++){
    if(dfsList[i]->type == UNDEF_GATE){
      if(dfsList[i]->fanout.empty()){
        gateList[dfsList[i]->id] = 0;
        delete dfsList[i];
      }
    }
  }
  // update dfsList
  dfsList.clear();
  for(size_t i = 0;i < poOrder.size(); i++){
    dfsPostOrder(gateList[poOrder[i]]);
  }
  resetFlag();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
