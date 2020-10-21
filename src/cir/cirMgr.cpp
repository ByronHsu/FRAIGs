/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <sstream>
#include <set>

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
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
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
   string line,word;
   ifstream ifs(fileName);

   if(!ifs.is_open()) {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   size_t piCount,poCount,aigCount,lineCount = 1,oId;
   vector<string> aig,po; // store aig input data
   bool c = 0;
   while(getline(ifs,line)){
      if(line == "c"){
         c = true;
      }
      istringstream ss(line);
      if(lineCount == 1){ //Header
         string tem;
         ss >> tem >> maxIndex >> piCount >> tem >> poCount >> aigCount;
         oId = maxIndex;
         gateList.resize(maxIndex+poCount+1,0);
         gateList[0] = new CirCONSTGate(0, 0);
      }
      if(lineCount > 1 && lineCount <= 1+piCount){ //PI_GATE
         size_t id;
         ss >> id;
         gateList[id/2] = new CirPIGate(lineCount, id/2);
         piOrder.push_back(id/2);
      }
      if(lineCount > 1+piCount && lineCount <= 1+piCount+poCount){ //PO_GATE
         oId++;
         size_t id;
         ss >> id;
         po.push_back(line);
         gateList[oId] = new CirPOGate(lineCount, oId);
         poOrder.push_back(oId);
      }
      if(lineCount > 1+piCount+poCount && lineCount <= 1+piCount+poCount+aigCount){ //AIG_GATE
         size_t idA,idI1,idI2;
         ss >> idA >> idI1 >> idI2;
         aig.push_back(line);
         gateList[idA/2] = new CirAIGGate(lineCount, idA/2);
      }
      if(lineCount > 1+piCount+poCount+aigCount){
         // Symbols
         if(!c){
            string io,s;
            ss >> io >> s;
            if(io[0] == 'i')  {gateList[piOrder[stoi(io.substr(1))]]->symbol = s;}
            if(io[0] == 'o')  {gateList[poOrder[stoi(io.substr(1))]]->symbol = s;}
         }
      }
      lineCount ++;
   }

   //Connect aig gate
   for(size_t i = 0,idA,idI1,idI2;i < aig.size() ; i++){
      istringstream ss(aig[i]);
      ss >> idA >> idI1 >> idI2;
      if(gateList[idI1/2] == 0)  gateList[idI1/2] = new CirUNDEFGate(0,idI1/2);
      if(gateList[idI2/2] == 0)  gateList[idI2/2] = new CirUNDEFGate(0,idI2/2);  
      gateList[idA/2] -> addFanin(gateList[idI1/2],idI1%2);
      gateList[idI1/2] -> addFanout(gateList[idA/2],idI1%2);
      gateList[idA/2] -> addFanin(gateList[idI2/2],idI2%2);
      gateList[idI2/2] -> addFanout(gateList[idA/2],idI2%2);
   }

   for(size_t i = 0,id; i < po.size() ; i++){
      istringstream ss(po[i]);
      ss >> id;
      if(gateList[id/2] == 0)  gateList[id/2] = new CirUNDEFGate(0,id/2);
      gateList[maxIndex+i+1] -> addFanin(gateList[id/2],id%2);
      gateList[id/2] -> addFanout(gateList[maxIndex+i+1],id%2);
   }

   // create dfsList
   for(size_t i = 0;i < poOrder.size(); i++){
      dfsPostOrder(gateList[poOrder[i]]);
   }
   resetFlag();

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
  size_t pi = 0,po = 0,aig = 0;
  for(size_t i = 1;i < gateList.size();i++){
    if(gateList[i] == 0) continue;
    GateType type = gateList[i]->type;
    if(type == PI_GATE) pi++;
    if(type == PO_GATE) po++;
    if(type == AIG_GATE) aig++;
  }
  cout << endl;
  cout << "Circuit Statistics" << endl;
  cout << "==================" << endl;
  cout << "  PI" << setw(12) << right << pi << endl;
  cout << "  PO" << setw(12) << right << po << endl;
  cout << "  AIG" << setw(11) << right << aig << endl;
  cout << "------------------" << endl;
  cout << "  Total" << setw(9) << right << pi+po+aig << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for (size_t i = 0,cnt = 0; i < dfsList.size(); ++i) {
      if(dfsList[i]->type != UNDEF_GATE){
         cout << "[" << cnt++ << "] ";
         dfsList[i]->printGate();
      }
   }
}


void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i = 0;i < piOrder.size(); i++){
     cout << " " << piOrder[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i = 0;i < poOrder.size(); i++){
     cout << " " << poOrder[i];
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
  vector<size_t> f1,f2;
  for(size_t i = 1;i < gateList.size(); i++){
    if(gateList[i] == 0) continue;
    for(size_t j = 0;j < gateList[i]->fanin.size(); j++){
      if(gateList[i]->getFanin(j)->type == UNDEF_GATE) {f1.push_back(gateList[i]->id);break;}
    }
    if(gateList[i]->type != PO_GATE){
      if(gateList[i]->fanout.size() == 0)
        f2.push_back(gateList[i]->id);
    }
  }
  if(!f1.empty()){
    cout << "Gates with floating fanin(s):";
    for(size_t i = 0; i < f1.size(); i++){
      cout << " " << f1[i];
    }
    cout << endl;
  }
  if(!f2.empty()){
    cout << "Gates defined but not used  :";
    for(size_t i = 0; i < f2.size(); i++){
      cout << " " << f2[i];
    }
    cout << endl;
  }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   vector<size_t> aigOut;
   for (size_t i = 0; i < dfsList.size(); ++i) {
      if(dfsList[i]->type == AIG_GATE){
         aigOut.push_back(dfsList[i]->id);
      }
   }
   //HEADER
   outfile << "aag" << " " << maxIndex << " " << piOrder.size() << " " << 0 << " " << poOrder.size() << " " << aigOut.size() << endl;
   //PI_GATE
   for(size_t i = 0;i < piOrder.size(); i++){
      outfile << piOrder[i]*2 << endl;
   }
   //PO_GATE
   for(size_t i = 0;i < poOrder.size(); i++){
      if(gateList[poOrder[i]]->isFaninInv(0))
         outfile << gateList[poOrder[i]]->getFanin(0)->id*2 + 1;
      else
         outfile << gateList[poOrder[i]]->getFanin(0)->id*2;
      outfile << endl;
   }
   //AIG_GATE
   for(size_t i = 0;i < aigOut.size(); i++){
      outfile << aigOut[i]*2;
      if(gateList[aigOut[i]]->isFaninInv(0))
         outfile << " " << gateList[aigOut[i]]->getFanin(0)->id*2 + 1;
      else
         outfile << " " << gateList[aigOut[i]]->getFanin(0)->id*2;
      if(gateList[aigOut[i]]->isFaninInv(1))
         outfile << " " << gateList[aigOut[i]]->getFanin(1)->id*2 + 1;
      else
         outfile << " " << gateList[aigOut[i]]->getFanin(1)->id*2;
      outfile << endl;
   }
   //SYMBOL
   for(size_t i = 0;i < piOrder.size(); i++){
      if(!gateList[piOrder[i]]->symbol.empty())
         outfile << "i" << i << " " << gateList[piOrder[i]]->symbol << endl;
   }
   for(size_t i = 0;i < poOrder.size(); i++){
      if(!gateList[poOrder[i]]->symbol.empty())
         outfile << "o" << i << " " << gateList[poOrder[i]]->symbol << endl;
   }
   outfile << "c" << endl;
   //TEST
   outfile << "AAG output by Chung-Yang (Ric) Huang" << endl; 
   // outfile << "AAG output by Byron Hsu" << endl; 
}

void
CirMgr::dfsPostOrder(CirGate* g)
{
   g->flag = 1;
   for(size_t i = 0;i < g->fanin.size(); i++){
      if(!g->getFanin(i)->flag)
         dfsPostOrder(g->getFanin(i));
   }
   dfsList.push_back(g);
}

void 
CirMgr::resetFlag(){
  for(size_t i = 0;i < gateList.size(); i++){
    if(gateList[i] != 0){
      gateList[i]->flag = 0;
    }
  }
}

void 
CirMgr::initFecList(){
  //全部屬於同個group
  FecGroup* f = new FecGroup;
  //CONST
  f->member.push_back(myLit(0, false));
  gateList[0]->fecGroup = f;
  //AIG(ONLY THOSE IN DFSLIST)
  for(size_t i = 0; i < dfsList.size(); i++){
    if(dfsList[i]->type == AIG_GATE){
      f->pushMember(myLit(dfsList[i]->id,false));
      dfsList[i]->fecGroup = f;
    }
  }

  fecList.push_back(f);
}

bool cmp(IdList L1, IdList L2){
  return L1[0] < L2[0];
}

void
CirMgr::printFECPairs() const
{
  size_t cnt = 0;

  vector<IdList> sortList;
  
  for(size_t i = 0; i < fecList.size(); i++){
    IdList temp = fecList[i]->member;
    sort(temp.begin(),temp.end());
    sortList.push_back(temp);
  }
  sort(sortList.begin(), sortList.end(), cmp);
  for(size_t i = 0; i < sortList.size(); i++){
    cout << "[" << cnt++ << "]";
    bool is1stInv = sortList[i][0]%2;
    for(size_t j = 0; j < sortList[i].size(); j++){
      string c;
      c = (sortList[i][j]%2 ^ is1stInv)? "!" : "\0";
      cout << " " << c << sortList[i][j]/2;
    }
    cout << endl;
  }
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   vector<size_t> aigOut,pi;
   size_t m = 0;

   writeGateDfs(g, aigOut);

   for(size_t i = 0; i < gateList.size(); i++){
      if(gateList[i]){
         if(gateList[i]->flag){
            if(gateList[i]->type != UNDEF_GATE && gateList[i]->id > m) m = gateList[i]->id;
         }
      }
   }
   for(size_t i = 0; i < piOrder.size(); i++){
      if(gateList[piOrder[i]]->flag) pi.push_back(piOrder[i]);
   }
   
   //resetFlag
   for(size_t i = 0;i < gateList.size(); i++){
      if(gateList[i] != 0){
         gateList[i]->flag = 0;
      }
   }

   outfile << "aag" << " " << m << " " << pi.size() << " 0" << " " << 1 << " " << aigOut.size() << endl;
   //PI_GATE
   for(size_t i = 0;i < pi.size(); i++){
      outfile << pi[i]*2 << endl;
   }
   //PO_GATE
   outfile << g->id*2 << endl;
   //AIG_GATE
   for(size_t i = 0;i < aigOut.size(); i++){
      outfile << aigOut[i]*2;
      if(gateList[aigOut[i]]->isFaninInv(0))
         outfile << " " << gateList[aigOut[i]]->getFanin(0)->id*2 + 1;
      else
         outfile << " " << gateList[aigOut[i]]->getFanin(0)->id*2;
      if(gateList[aigOut[i]]->isFaninInv(1))
         outfile << " " << gateList[aigOut[i]]->getFanin(1)->id*2 + 1;
      else
         outfile << " " << gateList[aigOut[i]]->getFanin(1)->id*2;
      outfile << endl;
   }
   //SYMBOL
   for(size_t i = 0;i < pi.size(); i++){
      if(!gateList[pi[i]]->symbol.empty())
         outfile << "i" << i << " " << gateList[pi[i]]->symbol << endl;
   }
   outfile << "o0 " << g->id << endl;
   outfile << "c" << endl;
   //TEST
   //Write gate (2)
   outfile << "Write gate (" << g->id << ") " << "by Chung-Yang (Ric) Huang" << endl; 
   // outfile << "AAG output by Byron Hsu" << endl; 
}
void
CirMgr::writeGateDfs(CirGate* g, vector<size_t>& gL) const{
  g->flag = 1;
  for(size_t i = 0;i < g->fanin.size(); i++){
    if(!g->getFanin(i)->flag)
        writeGateDfs(g->getFanin(i), gL);
  }
  if(g->isAig())
   gL.push_back(g->id);
}

CirMgr::~CirMgr(){
  for(size_t i = 0; i < gateList.size(); i++)
    if(gateList[i])
      delete gateList[i];
  for(size_t i = 0; i < fecList.size(); i++)
    if(fecList[i])
      delete fecList[i];
}
