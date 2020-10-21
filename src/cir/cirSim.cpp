/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashMap.h"
#include <sstream>
#include <unordered_map>
#include <set>


using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  if(!hasSim) {initFecList();}
  vector<size_t> input;
  input.resize(piOrder.size());
  size_t cnt = 0, fecSize = fecList.size();
  size_t r = 0;
  const size_t LIMIT = 15;
  const int MY_RAND_MAX = 2147483647;
  while(1) {
    r++;
    for(size_t j = 0; j < input.size(); j++){
      input[j] = rnGen(MY_RAND_MAX);
      input[j] = (input[j] << 32) + rnGen(MY_RAND_MAX);
    }
    sim64(input);
    log64();
    if(fecList.size() == fecSize) cnt++;
    else cnt = 0, fecSize = fecList.size();
    if(cnt == LIMIT) break;
  }
  cout << "\r";
  cout << r*64 << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  if(!hasSim) {initFecList();}
  string s;
  vector<string> ptn;
  vector<size_t> input;
  ptn.resize(piOrder.size());
  input.resize(piOrder.size());
  size_t length = 0;
  while(patternFile >> s){
    stringstream ss(s);
    if(s.size() != ptn.size()){
      // Error: Pattern(010100110010) length(12) does not match the number of inputs(16) in a circuit!!
      cerr << "Error: Pattern(" << s << ") length(" << s.size() << ") does not match the number of inputs(" << ptn.size() << ") in a circuit!!" << endl;
      cout << "\r";
      cout << 0 << " patterns simulated." << endl;
      // clear fecList
      for(size_t i = 0; i < fecList.size(); i++)
        delete fecList[i];
      for(size_t i = 0; i < gateList.size(); i++)
        if(gateList[i])
          gateList[i]->fecGroup = 0;
      fecList.clear();
      return;
    }else{
      char c;
      for(size_t i = 0; i < ptn.size(); i++){
        ss >> c;
        if(c != '0' && c != '1'){
          // Error: Pattern(01010011x010) contains a non-0/1 character(‘x’).
          cerr << "Error: Pattern(" << s << ") contains a non-0/1 character(‘" << c << "‘)." << endl;
          cout << "\r";
          cout << 0 << " patterns simulated." << endl;
          // clear fecList
          for(size_t i = 0; i < fecList.size(); i++)
            delete fecList[i];
          for(size_t i = 0; i < gateList.size(); i++)
            if(gateList[i])
              gateList[i]->fecGroup = 0;
          fecList.clear();
          return;
        }
        ptn[i] += c;
      }
    }
    if(ptn[0].size() == 64){
      for(size_t i = 0; i < ptn.size(); i++){
        input[i] = binToDec(ptn[i]);
      }
      sim64(input);
      log64();
      for(size_t i = 0; i < ptn.size(); i++){
        ptn[i].clear();
      }
    }
    length++;
  }

  if(!ptn[0].empty()){
    size_t length = ptn[0].size();
    while(ptn[0].size() != 64){
      for(size_t i = 0; i < ptn.size(); i++){
        ptn[i] += '0';
      }
    }
    for(size_t i = 0; i < ptn.size(); i++){
      input[i] = binToDec(ptn[i]);
    }
    sim64(input);
    log64(length%64);
    for(size_t i = 0; i < ptn.size(); i++){
      ptn[i].clear();
    }
  }
  cout << "\r";
  // 111 patterns simulated.
  cout << length << " patterns simulated." << endl;

  return;
}

void
CirMgr::sim64(vector<size_t>& input, bool fraig)
{
  for(size_t i = 0; i < piOrder.size(); i++)
    gateList[piOrder[i]]->simVal = input[i];
    
  for(size_t i = 0; i < dfsList.size(); i++)
    dfsList[i]->simulate();

  for(size_t i = 0; i < fecList.size();){
    //[NOTE] speed up a lotttttt

    //如果它的大小是1，砍掉，continue
    if(removeFecGroup(i)){
      continue;
    }
    
    unordered_map<size_t, FecGroup*> map;
    
    for(size_t j = 0; j < fecList[i]->member.size();){
      size_t nowLit = fecList[i]->member[j];
      CirGate* gate = gateList[nowLit/2];
      size_t f1 = gate->simVal, f2 = ~gate->simVal;
      //讓第一個留在原group
      if(j == 0){
        map[f1] = gate->fecGroup;
        j++;
        continue;
      }
      FecGroup* qF;
      if(map.count(f1)){
        qF = map[f1];
        if(qF == gate->fecGroup) {
          if(qF->member[gate->fIdx]%2 == 1 && hasSim){
            fecList[i]->removeMember(j);
            FecGroup* nF = new FecGroup;
            fecList.push_back(nF);
            nF->pushMember(myLit(gate->id, false));
            map[f1] = nF;
          }else{
            fecList[i]->setMember(j, myLit(gate->id, false));
            j++;
          }
        }
        else  {
          fecList[i]->removeMember(j);
          qF->pushMember(myLit(gate->id, false));
        }
      }else
      if(map.count(f2)){
        qF = map[f2];
        if(qF == gate->fecGroup) {
          if(qF->member[gate->fIdx]%2 == 0 && hasSim){
            fecList[i]->removeMember(j);
            FecGroup* nF = new FecGroup;
            fecList.push_back(nF);
            nF->pushMember(myLit(gate->id, false));
            map[f1] = nF;
          }else{
            fecList[i]->setMember(j, myLit(gate->id, true));
            j++;
          }
        }
        else  {
          fecList[i]->removeMember(j);
          qF->pushMember(myLit(gate->id, true));
        }
      }else{
        fecList[i]->removeMember(j);
        FecGroup* nF = new FecGroup;
        fecList.push_back(nF);
        nF->pushMember(myLit(gate->id, false));
        map[f1] = nF;
      }
    }
    //如果它的大小是1，砍掉，continue
    if(removeFecGroup(i)){
      continue;
    }

    i++;
  }
  hasSim = true;
  if(!fraig)
    cout << "\rTotal #FEC Group = " << fecList.size() << flush;
  return;
}
void
CirMgr::log64(size_t length)
{
  vector<string> logger;
  for(size_t i = 0; i < piOrder.size(); i++)
    logger.push_back(decToBin(gateList[piOrder[i]]->simVal));
  for(size_t i = 0; i < poOrder.size(); i++)
    logger.push_back(decToBin(gateList[poOrder[i]]->simVal)); 
  
  size_t wall = piOrder.size();
  ofstream& outFile = *_simLog;

  if(_simLog != NULL){
    for(size_t i = 0; i < length; i++){
      for(size_t j = 0; j < logger.size(); j++){
        if(j == wall){
          outFile << " ";
        }
        outFile << logger[j][i];
      }
      outFile << endl;
    }
  }

  return;
}
bool
CirMgr::removeFecGroup(size_t i){
  if(fecList[i]->member.size() == 1){
    size_t VID = fecList[i]->member[0]/2;
    delete gateList[VID]->fecGroup;
    gateList[VID]->fecGroup = 0;
    fecList[i] = fecList.back();
    fecList.pop_back();
    return true;
  }else
  if(fecList[i]->member.empty()){
    fecList[i] = fecList.back();
    fecList.pop_back();
    return true; 
  }
  else
    return false;

}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void 
FecGroup::removeMember(size_t idx){
  cirMgr->gateList[member.back()/2]->fIdx = idx;
  member[idx] = member.back();
  member.pop_back();
}
void
FecGroup::setMember(size_t idx, size_t LID) {
  member[idx] = LID;
  cirMgr->gateList[LID/2]->fIdx = idx;
}
void 
FecGroup::pushMember(size_t LID){
  member.push_back(LID);
  cirMgr->gateList[LID/2]->fIdx = member.size()-1;
  cirMgr->gateList[LID/2]->fecGroup = this;
}
