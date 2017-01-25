#ifndef steppingAction_hh
#define steppingAction_hh 1

#include "G4UserSteppingAction.hh"

#include "ASIMReadoutManager.hh"

class steppingAction : public G4UserSteppingAction
{

public:
  steppingAction();
  ~steppingAction();

  void UserSteppingAction(const G4Step *);

private:
  ASIMReadoutManager *theReadoutManager;
};

#endif
