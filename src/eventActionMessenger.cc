#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "eventAction.hh"
#include "eventActionMessenger.hh"


eventActionMessenger::eventActionMessenger(eventAction *EA)
  : EventAction(EA)
{
  detDataDirectory = new G4UIdirectory("/Sparrow/dataOutput/");
  detDataDirectory->SetGuidance("Sparrow data output control");
  
  eventFreqCmd = new G4UIcmdWithAnInteger("/Sparrow/dataOutput/setEventInfoFreq",this);
  eventFreqCmd->SetGuidance("Sets how often event information is printed to terminal");
  eventFreqCmd->SetGuidance("Event will print every <int> events");
  eventFreqCmd->AvailableForStates(G4State_Idle, G4State_PreInit);
}


eventActionMessenger::~eventActionMessenger()
{
  delete eventFreqCmd;
  delete detDataDirectory;
}


void eventActionMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if(cmd == eventFreqCmd)
    EventAction->SetEventInfoFreq(eventFreqCmd->GetNewIntValue(newValue));
}
