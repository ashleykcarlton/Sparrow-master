#ifndef runAction_hh
#define runAction_hh 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"

#include <string>
using namespace std;

#include "TFile.h"
#include "TTree.h"

#include "runActionMessenger.hh"

class runAction : public G4UserRunAction
{

public:
  runAction();
  ~runAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  
  void NameROOTFile(G4String Name) {SparrowFileName = Name;}
  void InitROOTFile();
  void WriteROOTFile();
  
private:
  runActionMessenger *TheMessenger;
  
  TFile *SparrowFile;
  TTree *SparrowTree;
  string SparrowFileName;
  
  G4double WrapThickness, ShieldThickness, ParticleKE;
  G4int ParticleNum;
  
  enum{zCore, zNorth, zWest, zSouth, zEast};

  vector<G4int> ReadoutHits;
  vector<G4double> ReadoutEff, ReadoutEffErr;
  vector<G4double> ReadoutEDep, ReadoutEDepErr;
  
  vector<G4int> ArrayHits;
  vector<G4double> ArrayEff, ArrayEffErr;
  vector<G4double> ArrayEDep, ArrayEDepErr;
};


#endif
