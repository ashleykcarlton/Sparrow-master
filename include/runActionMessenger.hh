#ifndef runActionMessenger_hh
#define runActionMessenger_hh 1

#include "G4UImessenger.hh"

class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

class runAction;


class runActionMessenger : public G4UImessenger
{

public:
  runActionMessenger(runAction *);
  ~runActionMessenger();

  void SetNewValue(G4UIcommand *, G4String);
  
private:
  G4UIcmdWithAString *rootNameCmd;
  G4UIcmdWithoutParameter *rootInitCmd, *rootWriteCmd;
  
  runAction *RunAction;
};

#endif
