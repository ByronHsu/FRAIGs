/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <climits>
#include <unordered_map>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include "algorithm"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
SatSolver solver;
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed

void
CirMgr::strash()
{
  unordered_map<size_t, CirGate*> map;

  for(size_t i = 0; i < dfsList.size(); i++){
    size_t litL, litR;
    if(dfsList[i]->type == AIG_GATE){
      litL = dfsList[i]->getFanin(0)->id*2 + dfsList[i]->isFaninInv(0);
      litR = dfsList[i]->getFanin(1)->id*2 + dfsList[i]->isFaninInv(1);
      size_t key = (litL < litR)? (litL<<32)+litR : (litR<<32)+litL;
      if(map.count(key)){
        //already exist in map
        gateList[dfsList[i]->id]->removeGate(myLit(map[key]->id,false),-1);
        gateList[dfsList[i]->id] = 0;
        delete dfsList[i];
        cout << "Strashing: " << map[key]->id << " merging " << dfsList[i]->id << "..." << endl;
      }else{
        map[key] = gateList[dfsList[i]->id];
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
void
CirMgr::fraig()
{
   // clear fecList
   if(hasFraig) {
      for(size_t i = 0; i < fecList.size(); i++){
         for(size_t j = 0; j < fecList[i]->member.size(); j++){
            size_t VID = fecList[i]->member[j]/2;
            gateList[VID]->fecGroup = 0;
         }
         if(fecList[i])
            delete fecList[i];
      }
      fecList.clear();
      return;
   }
   hasFraig = true;
   updateDfsFraig();
   for(size_t i = 0; i < fecList.size(); i++)
      fecList[i]->setBase();
      
   for(size_t i = 0; i < dfsList.size();){
      CirGate* nowGate = dfsList[i];
      // cout << dfsList[i]->id << endl;
      if(nowGate->type == AIG_GATE){
         if(nowGate->fecGroup != 0){
            GateList coneList;
            size_t baseIdx = nowGate->fecGroup->baseIdx;
            size_t base = nowGate->fecGroup->member[baseIdx];
            size_t now = nowGate->fecGroup->member[nowGate->fIdx];

            // [DEBUG]
            // string s1,s2;
            // s1 = (base%2)? "!" : "\0";
            // s2 = (now%2)? "!" : "\0";
            // cout << "NOW: " << s1 << now/2 << " " << "BASE: " << s2 << base/2 << endl;

            if(base/2 == now/2) {
               i++;continue;
            }
            solver.initialize();
            if(!gateList[now/2]->flag)
               findFaninCone(gateList[now/2],coneList);
            if(!gateList[base/2]->flag)
               findFaninCone(gateList[base/2],coneList);
            // // [DEBUG]
            // cout << "=====CONELIST======" << endl;
            // for(size_t i = 0; i < coneList.size(); i++)
            //    cout << " " << coneList[i]->id;
            // cout << endl;
            // cout << "===================" << endl;
            genProofModel(coneList);
            vector<size_t> ret;
            bool isSat = isSAT(now,base,ret,coneList);
            resetFlag();
            if(isSat){
               sim64(ret,true);
               for(size_t i = 0; i < fecList.size(); i++){
                  fecList[i]->setBase();
                  removeFecGroup(i);
               }
               string s1,s2;
               s1 = (base%2)? "!" : "\0";
               s2 = (now%2)? "!" : "\0";
               cout << "SAT: " << s1 << base/2 << " and " << s2 << now/2 << " differ." << endl;
              //  [DEBUG]
              //  cout << "== PRINT FECLIST ==" << endl;
              //  printFECPairs();
              //  cout << "===================" << endl;
              //  cout << "== PRINT NETLIST ==" << endl;
              //  printNetlist();
              //  cout << "===================" << endl;
            }else{
               string s1,s2;
               s1 = (base%2)? "!" : "\0";
               s2 = (now%2)? "!" : "\0";
               cout << "Fraig: " << s1 << base/2 << " merging " << s2 << now/2 << "..." << endl;
               //remove from fecgroup
               nowGate->fecGroup->removeMember(nowGate->fIdx);
               //if size is one, remove from fecList
               for(size_t i = 0; i < fecList.size(); i++){
                  if(fecList[i] == nowGate->fecGroup){
                     if(nowGate->fecGroup->member.size() == 1){
                        size_t VID = fecList[i]->member[0]/2;
                        delete gateList[VID]->fecGroup;
                        gateList[VID]->fecGroup = 0;
                        fecList[i] = fecList.back();
                        fecList.pop_back();
                        break;
                     }
                  }
               }
               nowGate->fecGroup = 0;

               //remove gate
               bool inv = base%2 ^ now%2;
               size_t NEWLID = 2*(base/2)+inv;
               nowGate->removeGate(NEWLID,-1);
               gateList[nowGate->id] = 0;
               delete nowGate;

               updateDfsFraig();
               for(size_t i = 0; i < fecList.size(); i++){
                  fecList[i]->setBase();
                  removeFecGroup(i);
               }
               i = 0;
              //  [DEBUG]
              //  cout << "== PRINT FECLIST ==" << endl;
              //  printFECPairs();
              //  cout << "===================" << endl;
              //  cout << "== PRINT NETLIST ==" << endl;
              //  printNetlist();
              //  cout << "===================" << endl;
              
               continue;
            }
         }
      }
      i++;
   }

  //  hasSim = false;
   return;
}
bool 
CirMgr::isSAT(size_t LID1, size_t LID2, vector<size_t>& ret, GateList& coneList){
   size_t newV = solver.newVar();
   solver.addXorCNF(newV, gateList[LID1/2]->getVar(), LID1%2, gateList[LID2/2]->getVar(), LID2%2);
   
   solver.assumeRelease();
   if(gateList[0]->flag) 
      solver.assumeProperty(gateList[0]->getVar(), false);
   solver.assumeProperty(newV, true);

   bool result = solver.assumpSolve();
   //STORE SAT VALUE
   if(result){
      for(size_t i = 0; i < piOrder.size(); i++){
         if(gateList[piOrder[i]]->flag)
            ret.push_back(solver.getValue(gateList[piOrder[i]]->getVar()));
         else
            ret.push_back(0);
      }
      for(size_t i = 0; i < coneList.size(); i++){
         if(coneList[i]->type == UNDEF_GATE){
            coneList[i]->simVal = solver.getValue(coneList[i]->getVar());
         }
      }
   }
   
   return result;
}
void
CirMgr::genProofModel(GateList &vec)
{

   for (size_t i = 0; i < vec.size(); i++) {
      if(vec[i] != 0){
         size_t v = solver.newVar();
         vec[i]->setVar(v);
      }
   }

   for (size_t i = 0; i < vec.size(); i++) {
      if(vec[i] != 0){
         if(vec[i]->type == AIG_GATE){
            solver.addAigCNF(
               vec[i]->getVar(), 
               vec[i]->getFanin(0)->getVar(), vec[i]->isFaninInv(0),
               vec[i]->getFanin(1)->getVar(), vec[i]->isFaninInv(1)
            );
         }
      }
   }
}
void
CirMgr::updateDfsFraig(){
   // create dfsList
   dfsList.clear();
   for(size_t i = 0;i < poOrder.size(); i++){
      dfsPostOrder(gateList[poOrder[i]]);
   }
   resetFlag();
   // set dOrder
   for(size_t i = 0;i < gateList.size(); i++)
      if(gateList[i])
         gateList[i]->dOrder = -1;
   for(size_t i = 0;i < dfsList.size(); i++)
      dfsList[i]->dOrder = i;
      
}

void
CirMgr::findFaninCone(CirGate* g, GateList& v){
   g->flag = 1;
   for(size_t i = 0;i < g->fanin.size(); i++){
      if(!g->getFanin(i)->flag)
         findFaninCone(g->getFanin(i), v);
   }
   v.push_back(g);
}
/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
FecGroup::setBase(){
   size_t min = INT_MAX;
   size_t minIdx = 0;
   for(size_t i = 0; i < member.size();){
      if(member[i]/2 != 0 && cirMgr->gateList[member[i]/2]->dOrder == -1)
         removeMember(i);
      else
         i++;
   }
   for(size_t i = 0; i < member.size(); i++){
      if(member[i] == 0){
        baseIdx = 0; return;
      }
      if(cirMgr->gateList[member[i]/2]->dOrder < (int)min){
         min = cirMgr->gateList[member[i]/2]->dOrder;
         minIdx = i;
      }
   }
   baseIdx = minIdx;
}
