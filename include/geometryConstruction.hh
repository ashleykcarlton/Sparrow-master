#ifndef geometryConstruction_hh
#define geometryConstruction_hh 1

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"

#include <vector>
using namespace std;

#include "ASIMScintillatorSD.hh"

#include "geometryMessenger.hh"

class geometryConstruction : public G4VUserDetectorConstruction
{
  
public:
  geometryConstruction(G4bool); 
  ~geometryConstruction();
  
  G4VPhysicalVolume *Construct();

  void BuildCase();
  void BuildCoreScintillator();
  void BuildPerimeterScintillators();
  void BuildRejectionScintillators();
  void BuildPCBs();

  void CalculateGeometry();
  void ConfigureReadouts();
  
  G4double GetShieldThickness() { return ShieldThickness;}
  G4double GetWrapThickness() { return WrapThickness; }
  
  // Messenger methods
  void SetDetectorPos(G4ThreeVector);
  void SetDetectorRot(G4ThreeVector);
  
  void SetCoreScintSize(G4ThreeVector);
  void SetPerimeterScintSize(G4ThreeVector);
  void SetPerimeterGap(G4double);
  void SetWrapThickness(G4double);
  void SetWrapLayers(G4int);
  void SetShieldThickness(G4double);
  void SetShieldLayers(G4int);
  void RebuildGeometry();
  G4bool CheckForGeometryOverlaps();

private:
  G4bool InitialConstruction, UsingVisualization;
  
  // Volumes and parameters for the world
  
  G4Box *World_S;
  G4LogicalVolume *World_L;
  G4VPhysicalVolume *World_P;
  G4double WorldX, WorldY, WorldZ;

  // Volumes and parameters for the outer case

  G4Box *CaseShell_S;
  G4Box *CaseCutout_S;
  G4SubtractionSolid *Case_SS;
  G4LogicalVolume *Case_L;
  G4VPhysicalVolume *Case_P;
  G4double CaseX, CaseY, CaseZ;
  G4double CaseThicknessX, CaseThicknessY, CaseThicknessZ;
  G4double CaseCutoutX, CaseCutoutY, CaseCutoutZ;
  G4double CaseCutout_YTranslate;
  
  G4Box *CaseChamber_S;
  G4LogicalVolume *CaseChamber_L;
  G4VPhysicalVolume *CaseChamber_P;
  G4double CaseChamberX, CaseChamberY, CaseChamberZ;

  G4double Case_YTranslate;

  // General scintillator parameters
  
  G4double Scintillators_YTranslate;

  // Volumes and parameters for the core scintillator

  G4bool BevelCoreEdges;

  G4Box *CoreCut_S;
  G4SubtractionSolid *CoreScint_SS;
  G4double CoreCutDepth, CoreCutAngle;
  G4double CoreCutX, CoreCutY, CoreCutZ;
  G4double CoreCut_XTranslate, CoreCut_YTranslate, CoreCut_ZTranslate;

  G4Box *CoreRaw_S;

  G4VSolid *CoreScint_S;
  G4LogicalVolume *CoreScint_L;
  G4VPhysicalVolume *CoreScint_P;
  ASIMScintillatorSD *CoreScint_SD;
  G4double CoreScintX, CoreScintY, CoreScintZ;

  G4Box *CoreShield_S;
  G4LogicalVolume *CoreShield_L;
  G4VPhysicalVolume *CoreShield_P;
  G4double CoreShieldX, CoreShieldY, CoreShieldZ;

  G4Box *CoreWrap_S;
  G4LogicalVolume *CoreWrap_L;
  G4VPhysicalVolume *CoreWrap_P;
  G4double CoreWrapX, CoreWrapY, CoreWrapZ;

  // Volumes and parameters for the perimeter scintillators
  
  const G4int NumPerimeterScints;
  G4bool FixPerimeterScintWidth;
  
  G4Box *PerimeterScint_S;
  vector<G4LogicalVolume *> PerimeterScint_L;
  vector<G4VPhysicalVolume *> PerimeterScint_P;
  vector<ASIMScintillatorSD *> PerimeterScint_SD;
  G4double PerimeterScintX, PerimeterScintY, PerimeterScintZ;
  
  G4Box *PerimeterShield_S;
  vector<G4LogicalVolume *> PerimeterShield_L;
  vector<G4VPhysicalVolume *> PerimeterShield_P;
  G4double PerimeterShieldX, PerimeterShieldY, PerimeterShieldZ;
  
  G4Box *PerimeterWrap_S;
  vector<G4LogicalVolume *> PerimeterWrap_L;
  vector<G4VPhysicalVolume *> PerimeterWrap_P;
  G4double PerimeterWrapX, PerimeterWrapY, PerimeterWrapZ;
  G4double PerimeterGap;

  // Volumes for the coincidence rejection scintillators

  const G4int NumRejectionScints;
  
  G4Box *RejectionScint_S;
  vector<G4LogicalVolume *> RejectionScint_L;
  vector<G4VPhysicalVolume *> RejectionScint_P;
  vector<ASIMScintillatorSD *> RejectionScint_SD;
  G4double RejectionScintX, RejectionScintY, RejectionScintZ;
  
  G4Box *RejectionShield_S;
  vector<G4LogicalVolume *> RejectionShield_L;
  vector<G4VPhysicalVolume *> RejectionShield_P;
  G4double RejectionShieldX, RejectionShieldY, RejectionShieldZ;
  
  G4Box *RejectionWrap_S;
  vector<G4LogicalVolume *> RejectionWrap_L;
  vector<G4VPhysicalVolume *> RejectionWrap_P;
  G4double RejectionWrapX, RejectionWrapY, RejectionWrapZ;

  G4double RejectionGap, RejectionPosTop, RejectionPosBot;

  // General scintillator variables
  
  G4double WrapThickness, SingleWrapLayerThickness;
  G4double ShieldThickness, SingleShieldLayerThickness;
  
  // Volumes and parameters for the PCBs

  const G4int NumPCBs;
  const G4int NumPCBPosts;
  
  G4Box *PCB_S;
  G4LogicalVolume *PCB_L;
  vector<G4VPhysicalVolume *> PCB_P;
  G4double PCBX, PCBY, PCBZ;
  G4double PCBGap;
  G4double PCB_YTranslate[5];
  
  G4Tubs *PCBPost_S;
  G4LogicalVolume *PCBPost_L;
  vector<G4VPhysicalVolume *> PCBPost_P;
  G4double PCBPostRMin, PCBPostRMax, PCBPostZ;
  G4double PCBPost_XTranslate, PCBPost_YTranslate, PCBPost_ZTranslate;
  
  geometryMessenger *TheMessenger;
};

#endif
