#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "runAction.hh"
#include "runActionMessenger.hh"


runActionMessenger::runActionMessenger(runAction *RA)
  : RunAction(RA)
{
  rootNameCmd = new G4UIcmdWithAString("/Sparrow/dataOutput/setFileName",this);
  rootNameCmd->SetGuidance("Set the name of the run-level ROOT file");
  rootNameCmd->SetParameterName("Choice",false);

  rootInitCmd = new G4UIcmdWithoutParameter("/Sparrow/dataOutput/init",this);
  rootInitCmd->SetGuidance("Create a ROOT file with run-level information");
  rootInitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  rootWriteCmd = new G4UIcmdWithoutParameter("/Sparrow/dataOutput/write",this);
  rootWriteCmd->SetGuidance("Create a ROOT file with run-level information");
  rootWriteCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

runActionMessenger::~runActionMessenger()
{;}


void runActionMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if(cmd == rootNameCmd)
    RunAction->NameROOTFile(newValue);
  
  if(cmd == rootInitCmd)
    RunAction->InitROOTFile();
  
  if(cmd == rootWriteCmd)
    RunAction->WriteROOTFile();
}
