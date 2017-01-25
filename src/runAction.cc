// Geant4
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"

// ASIM
#include "ASIMReadoutManager.hh"
#ifdef MPI_ENABLED
#include "MPIManager.hh"
#endif

// Sparrow
#include "runAction.hh"
#include "geometryConstruction.hh"
#include "PGA.hh"


runAction::runAction()
  : SparrowFile(NULL), SparrowTree(NULL), SparrowFileName("SparrowRun.root"),
    WrapThickness(0.), ShieldThickness(0.), ParticleKE(0.)
{
  TheMessenger = new runActionMessenger(this);
}
    
    
runAction::~runAction()
{
  delete TheMessenger;
}


void runAction::InitROOTFile()
{
  // Ability to write run summary to ROOT file is depracated at
  // present in CD1; readout/writing will need reimplementing although
  // all messenger commands/infrastructure are still perfectly good.
  
  /*
  SparrowFile = new TFile(SparrowFileName.c_str(),"recreate");
  
  SparrowTree = new TTree("SparrowTree","Run-level data on the Sparrow detector");
  
  // General variables
  SparrowTree->Branch("WrapThickness", &WrapThickness);
  SparrowTree->Branch("ShieldThickness", &ShieldThickness);
  SparrowTree->Branch("ParticleKE", &ParticleKE);

  // Core scintillator variables
  SparrowTree->Branch("CoreHits", &CoreHits);
  SparrowTree->Branch("CoreEDep", &CoreEDep);
  SparrowTree->Branch("CoreEDepErr", &CoreEDepErr);

  // West perimeter scintillator variables
  SparrowTree->Branch("WestHits", &WestHits);
  SparrowTree->Branch("WestEDep", &WestEDep);
  SparrowTree->Branch("WestEDepErr", &WestEDepErr);

  // East perimeter scintillator variables
  SparrowTree->Branch("EastHits", &EastHits);
  SparrowTree->Branch("EastEDep", &EastEDep);
  SparrowTree->Branch("EastEDepErr", &EastEDepErr);

  // Readout array variables
  SparrowTree->Branch("WestCoreHits", &WestCoreHits);
  SparrowTree->Branch("WestCoreEDep", &WestCoreEDep);

  SparrowTree->Branch("EastCoreHits", &EastCoreHits);
  SparrowTree->Branch("EastCoreEDep", &EastCoreEDep);
  
  SparrowTree->Branch("WestCoreEastHits", &WestCoreEastHits);
  SparrowTree->Branch("WestCoreEastEDep", &WestCoreEastEDep);
  */
}


void runAction::WriteROOTFile()
{
  /*
  SparrowTree->Write();
  SparrowFile->Close();
  if(SparrowFile != NULL)
    delete SparrowFile;
  
  SparrowFile = NULL;
  SparrowTree = NULL;
  */
}


void runAction::BeginOfRunAction(const G4Run *)
{
  // Initialize ASIM run-level variables for a new run
  ASIMReadoutManager::GetInstance()->InitializeForRun();

  // Initialize class member dat for a new run
  ReadoutHits.clear();
  ReadoutEff.clear();
  ReadoutEffErr.clear();
  ReadoutEDep.clear();
  ReadoutEDepErr.clear();
  ArrayHits.clear();
  ArrayEff.clear();
  ArrayEffErr.clear();
  ArrayEDep.clear();
  ArrayEDepErr.clear();
}


void runAction::EndOfRunAction(const G4Run *currentRun)
{
  ASIMReadoutManager *ARMgr = ASIMReadoutManager::GetInstance();

  G4int nodeRank = 0;
#ifdef MPI_ENABLED
  MPIManager *MPIMgr = MPIManager::GetInstance();
  MPIMgr->ForceBarrier("runAction::EndOfRunAction()");
  nodeRank = MPIMgr->GetRank();
#endif
  
  // Create a summary of run-level variables 
  ARMgr->CreateRunSummary(currentRun);
  
  // In sequential or parallel architectures only the master should be
  // responsible for the output of any information
  if(nodeRank == 0){
    
    G4RunManager *theRunManager = G4RunManager::GetRunManager();

    // Get the scintillator wrap and shield thickness
    
    geometryConstruction *theGeometry = (geometryConstruction *)
      theRunManager->GetUserDetectorConstruction();
    WrapThickness = theGeometry->GetWrapThickness()/um;
    ShieldThickness = theGeometry->GetShieldThickness()/um;

    // Get the vertex particle information
    
    PGA *thePGA = (PGA *)theRunManager->GetUserPrimaryGeneratorAction();
    G4GeneralParticleSource *GPS = thePGA->GetSource();
    ParticleKE = GPS->GetParticleEnergy();
    ParticleNum = theRunManager->GetNumberOfEventsToBeProcessed();

    // Get the ASIM readout information

    const G4int NumReadouts = ARMgr->GetNumReadouts();
    for(G4int r=0; r<NumReadouts; r++){
      
      ReadoutHits.push_back(ARMgr->GetReadoutHits(r));
      
      ReadoutEff.push_back(ReadoutHits[r] * 1. / ParticleNum);
      ReadoutEffErr.push_back(ReadoutEff[r] / sqrt(ReadoutHits[r]));
      
      ReadoutEDep.push_back(ARMgr->GetReadoutEDep(r) / ReadoutHits[r]);
      ReadoutEDepErr.push_back(ReadoutEDep[r] / sqrt(ReadoutHits[r]));
      
      if(ReadoutHits[r] == 0){
	ReadoutEffErr[r] = 0.;
	ReadoutEDep[r] = 0.;
	ReadoutEDepErr[r] = 0.;
      }
    }

    // Get the ASIM array information

    const G4int NumArrays = ARMgr->GetNumArrays();
    for(G4int a=0; a<NumArrays; a++){
      
      ArrayHits.push_back(ARMgr->GetArrayHits(a));
      
      ArrayEff.push_back(ArrayHits[a] * 1. / ParticleNum);
      ArrayEffErr.push_back(ArrayEff[a] / sqrt(ArrayHits[a]));
      
      ArrayEDep.push_back(ARMgr->GetArrayEDep(a) / ArrayHits[a]);
      ArrayEDepErr.push_back(ArrayEDep[a] / sqrt(ArrayHits[a]));
      
      if(ArrayHits[a] == 0){
	ArrayEffErr[a] = 0.;
	ArrayEDep[a] = 0.;
	ArrayEDepErr[a] = 0.;
      }
    }


    /////////////////////////////////
    // Fill the Sparrow ROOT TTree //
    /////////////////////////////////

    // Depracated; See not above in runAction::InitROOTFile()
    
    //  if(SparrowTree != NULL)
    //    SparrowTree->Fill();

    //////////////////////////////
    // Output results to stdout //
    //////////////////////////////

    G4cout << setprecision(5)
	   << "\n\n"
	   << "\n======================== Sparrow run-level results ========================\n\n"
	   << "    Wrap thickness   : " << WrapThickness << " um\n"
	   << "    Shield thickness : " << ShieldThickness << " um\n"
	   << "    Vertex energy    : " << ParticleKE/MeV << " MeV\n"
	   << "    Vertex number    : " << ParticleNum << " particles\n\n"
	   << G4endl;

    G4cout << "    ======  Single readouts ======\n\n";
    for(G4int r=0; r<NumReadouts; r++)
      G4cout << "    Readout[" << r << "] : " << ARMgr->GetReadoutName(r) << "\n"
	     << "      Hits : " << ReadoutHits[r] << "\n"
	     << "      Effi : " << ReadoutEff[r] << " +/- " << ReadoutEffErr[r] << "\n"
	     << "      EDep : " << ReadoutEDep[r]/MeV << " +/- " << ReadoutEDepErr[r] << "\n"
	     << G4endl;

    G4cout << "    ======  Arrays readouts  ======\n\n";
    for(G4int a=0; a<NumArrays; a++)
      G4cout << "    Readout[" << a << "] : " << ARMgr->GetArrayName(a) << "\n"
	     << "      Hits : " << ArrayHits[a] << "\n"
	     << "      Effi : " << ArrayEff[a] << " +/- " << ArrayEffErr[a] << "\n"
	     << "      EDep : " << ArrayEDep[a]/MeV << " +/- " << ArrayEDepErr[a] << "\n"
	     << G4endl;

    G4cout << "\n===========================================================================\n\n"
	   << G4endl;
  }
}
