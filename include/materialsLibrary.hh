#ifndef materialsLibrary_hh
#define materialsLibrary_hh 1

#include "G4Material.hh"

namespace materialsLibrary
{
  G4Material *Air();          
  G4Material *SiPM(G4String);
  G4Material *EJ299();
  G4Material *StainlessSteel();
}

#endif
