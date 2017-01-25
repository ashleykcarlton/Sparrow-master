#include "ASIMReadoutManager.hh"

#include "steppingAction.hh"


steppingAction::steppingAction()
{ theReadoutManager = ASIMReadoutManager::GetInstance(); }


steppingAction::~steppingAction()
{;}

 
void steppingAction::UserSteppingAction(const G4Step *currentStep)
{
  theReadoutManager->HandleOpticalPhotonDetection(currentStep);
}
