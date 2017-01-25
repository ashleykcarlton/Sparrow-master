#ifndef geometryMessenger_hh
#define geometryMessenger_hh 1

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;


class geometryConstruction;


class geometryMessenger : public G4UImessenger
{
public:
  geometryMessenger(geometryConstruction *);
  ~geometryMessenger();
  
  void SetNewValue(G4UIcommand *, G4String);
private:
  geometryConstruction *TheGeometry;
  
  G4UIdirectory *TopLevelDirectory;
  G4UIdirectory *GeometryDirectory;
  
  G4UIcmdWith3VectorAndUnit *DetectorPosCmd;
  G4UIcmdWith3VectorAndUnit *DetectorRotCmd;
  G4UIcmdWithADoubleAndUnit *WrapThickCmd;
  G4UIcmdWithAnInteger *WrapLayersCmd;
  G4UIcmdWithADoubleAndUnit *ShieldThickCmd;
  G4UIcmdWithAnInteger *ShieldLayersCmd;
  G4UIcmdWith3VectorAndUnit *CoreScintSizeCmd;
  G4UIcmdWith3VectorAndUnit *PerimeterScintSizeCmd;
  G4UIcmdWithADoubleAndUnit *PerimeterGapCmd;

};


#endif
