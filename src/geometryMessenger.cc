#include "G4UIdirectory.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "geometryMessenger.hh"
#include "geometryConstruction.hh"


geometryMessenger::geometryMessenger(geometryConstruction *GC)
  : TheGeometry(GC)
{
  TopLevelDirectory = new G4UIdirectory("/Sparrow/");
  TopLevelDirectory->SetGuidance("Custom Sparrow simulation controls");

  GeometryDirectory = new G4UIdirectory("/Sparrow/geometry/");
  GeometryDirectory->SetGuidance("Simulation geometry control");
  
  DetectorPosCmd = 
    new G4UIcmdWith3VectorAndUnit("/Sparrow/geometry/setDetectorPos",this);
  DetectorPosCmd->SetGuidance("Set the position of the Detector");
  DetectorPosCmd->SetParameterName("X Position","Y Position","Z Position",true);
  DetectorPosCmd->SetDefaultValue(G4ThreeVector(0.*cm, 0.*cm, 0.*cm));
  DetectorPosCmd->SetUnitCategory("Length");
  DetectorPosCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  DetectorRotCmd = 
    new G4UIcmdWith3VectorAndUnit("/Sparrow/geometry/setDetectorRot",this);
  DetectorRotCmd->SetGuidance("Set the rotation of the Detector");
  DetectorRotCmd->SetParameterName("Angle","Angle","Angle",true);
  DetectorRotCmd->SetDefaultValue(G4ThreeVector(0., 0., 0));
  DetectorRotCmd->SetUnitCategory("Angle");
  DetectorRotCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  WrapThickCmd = 
    new G4UIcmdWithADoubleAndUnit("/Sparrow/geometry/setWrapThickness",this);
  WrapThickCmd->SetGuidance("Set the thickness of the scintillator wrapping. Note that");
  WrapThickCmd->SetGuidance("1 layer of Teflon (PTFE) tape is 88.9 microns.");
  WrapThickCmd->SetParameterName("Thickness",true);
  WrapThickCmd->SetDefaultValue(88.9*um);
  WrapThickCmd->SetUnitCategory("Length");
  WrapThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  WrapLayersCmd = 
    new G4UIcmdWithAnInteger("/Sparrow/geometry/setWrapLayers",this);
  WrapLayersCmd->SetGuidance("Set the number of layers of the scintillator wrapping. Note that");
  WrapLayersCmd->SetGuidance("1 layer of Teflon (PTFE) tape is 88.9 microns.");
  WrapLayersCmd->SetParameterName("Layers",true);
  WrapLayersCmd->SetDefaultValue(1);
  WrapLayersCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  ShieldThickCmd = 
    new G4UIcmdWithADoubleAndUnit("/Sparrow/geometry/setShieldThickness",this);
  ShieldThickCmd->SetGuidance("Set the thickness of the scintillator light shield. Note that");
  ShieldThickCmd->SetGuidance("1 layer of light-black aluminum tape is 90.0 microns.");
  ShieldThickCmd->SetParameterName("Thickness",true);
  ShieldThickCmd->SetDefaultValue(90.*um);
  ShieldThickCmd->SetUnitCategory("Length");
  ShieldThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  ShieldLayersCmd = 
    new G4UIcmdWithAnInteger("/Sparrow/geometry/setShieldLayers",this);
  ShieldLayersCmd->SetGuidance("Set the number of layers of the scintillator light shield. Note that");
  ShieldThickCmd->SetGuidance("1 layer of light-black aluminum tape is 90.0 microns.");
  ShieldLayersCmd->SetParameterName("Layers",true);
  ShieldLayersCmd->SetDefaultValue(1);
  ShieldLayersCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  CoreScintSizeCmd = 
    new G4UIcmdWith3VectorAndUnit("/Sparrow/geometry/setCoreScintillatorSize",this);
  CoreScintSizeCmd->SetGuidance("Set the size (X,Y,Z) of the core scintillators");
  CoreScintSizeCmd->SetParameterName("X Size","Y Size","Z Size",true);
  CoreScintSizeCmd->SetDefaultValue(G4ThreeVector(0., 0., 0));
  CoreScintSizeCmd->SetUnitCategory("Length");
  CoreScintSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  PerimeterScintSizeCmd = 
    new G4UIcmdWith3VectorAndUnit("/Sparrow/geometry/setPerimeterScintillatorSize",this);
  PerimeterScintSizeCmd->SetGuidance("Set the size (X,Y,Z) of the perimeter scintillators");
  PerimeterScintSizeCmd->SetParameterName("X Size","Y Size","Z Size",true);
  PerimeterScintSizeCmd->SetDefaultValue(G4ThreeVector(0., 0., 0));
  PerimeterScintSizeCmd->SetUnitCategory("Length");
  PerimeterScintSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  PerimeterGapCmd = 
    new G4UIcmdWithADoubleAndUnit("/Sparrow/geometry/setPerimeterGap",this);
  PerimeterGapCmd->SetGuidance("Set the distance between the center of the perimeter scintillators");
  PerimeterGapCmd->SetGuidance(" and center of the core scintillator.");
  PerimeterGapCmd->SetParameterName("Thickness",true);
  PerimeterGapCmd->SetDefaultValue(0.5*cm);
  PerimeterGapCmd->SetUnitCategory("Length");
  PerimeterGapCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

geometryMessenger::~geometryMessenger()
{
  delete PerimeterGapCmd;
  delete PerimeterScintSizeCmd;
  delete CoreScintSizeCmd;
  delete ShieldLayersCmd;
  delete ShieldThickCmd;
  delete WrapLayersCmd;
  delete WrapThickCmd;
  delete DetectorRotCmd;
  delete DetectorPosCmd;
  delete GeometryDirectory;
  delete TopLevelDirectory;
}


void geometryMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if(cmd == DetectorPosCmd)
    TheGeometry->SetDetectorPos(DetectorPosCmd->GetNew3VectorValue(newValue));
  
  if(cmd == DetectorRotCmd)
    TheGeometry->SetDetectorRot(DetectorRotCmd->GetNew3VectorValue(newValue));
  
  if(cmd == WrapThickCmd)
    TheGeometry->SetWrapThickness(WrapThickCmd->GetNewDoubleValue(newValue));
  
  if(cmd == WrapLayersCmd)
    TheGeometry->SetWrapLayers(WrapLayersCmd->GetNewIntValue(newValue));

  if(cmd == ShieldThickCmd)
    TheGeometry->SetShieldThickness(ShieldThickCmd->GetNewDoubleValue(newValue));

  if(cmd == ShieldLayersCmd)
    TheGeometry->SetShieldThickness(ShieldLayersCmd->GetNewIntValue(newValue));
  
  if(cmd == CoreScintSizeCmd)
    TheGeometry->SetCoreScintSize(CoreScintSizeCmd->GetNew3VectorValue(newValue));
  
  if(cmd == PerimeterScintSizeCmd)
    TheGeometry->SetPerimeterScintSize(PerimeterScintSizeCmd->GetNew3VectorValue(newValue));

  if(cmd == PerimeterGapCmd)
    TheGeometry->SetPerimeterGap(PerimeterGapCmd->GetNewDoubleValue(newValue));
}

    
  
