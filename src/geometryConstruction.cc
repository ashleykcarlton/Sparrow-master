#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include <sstream>

#include "geometryConstruction.hh"
#include "materialsLibrary.hh"
#include "ASIMScintillatorSD.hh"
#include "ASIMReadoutManager.hh"

geometryConstruction::geometryConstruction(G4bool UV)
  : InitialConstruction(true), UsingVisualization(UV),
    BevelCoreEdges(false), 
    NumPerimeterScints(4), FixPerimeterScintWidth(false),
    NumRejectionScints(2),
    NumPCBs(2), NumPCBPosts(4)
{
  // Initialize geometry parameters
  
  // The world
  WorldX = 5.*cm;
  WorldY = 5.*cm;
  WorldZ = 12*cm;
  
  // The detector case
  CaseX = 3.5*cm;
  CaseY = 3.5*cm;
  CaseZ = 6.*cm;
  
  Case_YTranslate = -0.3*cm;

  CaseThicknessX = 0.3*cm;
  CaseThicknessY = 0.3*cm;
  CaseThicknessZ = 0.3*cm;

  CaseCutoutX = 2.*cm;
  CaseCutoutY = 1.*cm;
  // CaseCutoutZ is dynamically calculated

  CaseCutout_YTranslate = 0.3*cm;

  // The scintillators

  // The wrap refers to white PTFE (Teflon) tape that provides diffuse
  // reflection for photons within the scintillator
  WrapThickness = 88.9*um;
  SingleWrapLayerThickness = WrapThickness;
  
  // The shield refers to light-black aluminum tape from ThorLabs to
  // provide complete light protection from external photons
  ShieldThickness = 90.*um;
  SingleShieldLayerThickness = ShieldThickness;

  Scintillators_YTranslate = 0.3*cm;
  
  CoreScintX = 0.9*cm;
  CoreScintY = 0.9*cm;
  CoreScintZ = 0.9*cm;

  CoreCutDepth = 0.3*cm;
  CoreCutAngle = 45.*degree;
  
  CoreCutX = CoreCutDepth;
  CoreCutY = CoreScintY + 1.*cm;
  CoreCutZ = CoreCutDepth;

  CoreCut_XTranslate = CoreScintX/2;
  CoreCut_YTranslate = CoreScintY/2;
  CoreCut_ZTranslate = CoreScintZ/2;

  PerimeterGap = 0.3*cm;

  PerimeterScintX = 0.9*cm;
  PerimeterScintY = 0.9*cm;
  PerimeterScintZ = 0.1*cm;

  RejectionScintX = 3.0*cm;
  RejectionScintY = 0.1*cm;
  RejectionScintZ = 3.0*cm;

  RejectionGap = 0.15*cm;

  // The PCBs

  // PCBX is dynamically calculated
  PCBY = 0.1*cm;
  // PCBZ is dynamically calculated

  PCBGap = 1.*cm;

  PCB_YTranslate[0] = Scintillators_YTranslate - CoreScintY/2 - WrapThickness - ShieldThickness - PCBY/2;
  PCB_YTranslate[1] = PCB_YTranslate[0] - PCBY - PCBGap;
  PCB_YTranslate[2] = PCB_YTranslate[0] - PCBY - PCBGap;
  PCB_YTranslate[3] = PCB_YTranslate[0] - PCBY - PCBGap;
  PCB_YTranslate[4] = PCB_YTranslate[0] - PCBY - PCBGap;

  PCBPostRMin = 0.;
  PCBPostRMax = 0.1*cm;
  PCBPostZ = PCBGap;

  CalculateGeometry();

  // Create a messenger for cmd-line based commands
  TheMessenger = new geometryMessenger(this);
}


geometryConstruction::~geometryConstruction()
{ delete TheMessenger; }


G4VPhysicalVolume *geometryConstruction::Construct()
{
  /////////////////////////////////////
  // Create the top-level world volumes
 
  if(InitialConstruction){

    // The simulation world
    
    World_S = new G4Box("World_S", WorldX/2, WorldY/2, WorldZ/2);
    
    World_L = new G4LogicalVolume(World_S, 
				  G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
				  "World_L");
    
    World_P = new G4PVPlacement(0, 
				G4ThreeVector(), 
				World_L, 
				"TheWorld",
				0,
				false,
				0);
  
    G4VisAttributes *WorldVisAtt = new G4VisAttributes();
    WorldVisAtt->SetVisibility(true);
    World_L->SetVisAttributes(WorldVisAtt);
  }


  /////////////////////////////
  // Build the Sparrow detector

  BuildCase();
  BuildCoreScintillator();
  BuildPerimeterScintillators();
  BuildRejectionScintillators();
  BuildPCBs();
  
  //////////////////////////////
  // Configure the ASIM readouts
  
  ConfigureReadouts();
  

  //////////////////////
  // Finish construction

  if(InitialConstruction)
    InitialConstruction = false;
  
  return World_P;
}


void geometryConstruction::BuildCase()
{
  ////////////////
  // Build volumes

  CaseShell_S = new G4Box("CaseShell_S", CaseX/2, CaseY/2, CaseZ/2);

  CaseCutout_S = new G4Box("CaseWindowCutout_S", CaseCutoutX/2, CaseCutoutY/2, CaseCutoutZ/2);

  CaseChamber_S = new G4Box("CaseChamber_S", CaseChamberX/2, CaseChamberY/2, CaseChamberZ/2);

  Case_SS = new G4SubtractionSolid("Case_SS",
				   CaseShell_S,
				   CaseCutout_S,
				   new G4RotationMatrix(0., 0., 0.),
				   G4ThreeVector(0., CaseCutout_YTranslate, 0.));

  Case_SS = new G4SubtractionSolid("Case_SS",
				   Case_SS,
				   CaseCutout_S,
				   new G4RotationMatrix(90.*degree, -90.*degree, 90.*degree),
				   G4ThreeVector(0., CaseCutout_YTranslate, 0.));
  
  Case_SS = new G4SubtractionSolid("Case_SS",
				   Case_SS,
				   CaseChamber_S,
				   new G4RotationMatrix(0., 0., 0.),
				   G4ThreeVector(0., 0., 0.));

  Case_L = new G4LogicalVolume(Case_SS,
			       G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
			       "Case_L");

  Case_P = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.),
			     G4ThreeVector(0., Case_YTranslate, 0.),
			     Case_L,
			     "Case",
			     World_L,
			     false,
			     0);
  
  G4VisAttributes *CaseVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 1.0));
  CaseVisAtt->SetForceSolid(true);
  CaseVisAtt->SetVisibility(true);
  Case_L->SetVisAttributes(CaseVisAtt);


  CaseChamber_L = new G4LogicalVolume(CaseChamber_S,
				      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
				      "CaseChamber_L");
  
  CaseChamber_P = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.),
				    G4ThreeVector(0., Case_YTranslate, 0.),
				    CaseChamber_L,
				    "CaseChamber",
				    World_L,
				    false,
				    0);
  
  G4VisAttributes *ChamberVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 1.0));
  ChamberVisAtt->SetForceWireframe(true);
  ChamberVisAtt->SetVisibility(true);
  CaseChamber_L->SetVisAttributes(ChamberVisAtt);
}


void geometryConstruction::BuildCoreScintillator()
{
  //////////////////////////////////
  // Build core scintillator volumes
  
  // The core scintillator light shield
  
  CoreShield_S = new G4Box("CoreShield_S", CoreShieldX/2, CoreShieldY/2, CoreShieldZ/2);
  
  CoreShield_L = new G4LogicalVolume(CoreShield_S,
				     G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
				     "CoreShield_L");
  
  CoreShield_P = new G4PVPlacement(0,
				   G4ThreeVector(0., Scintillators_YTranslate, 0.),
				   CoreShield_L,
				   "TheCoreShield",
				   CaseChamber_L,
				   false,
				   0);
  
  G4VisAttributes *CoreShieldVisAtt = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.4));
  CoreShieldVisAtt->SetForceSolid(true);
  CoreShield_L->SetVisAttributes(CoreShieldVisAtt);
  

  // The core scintillator wrapping
  
  CoreWrap_S = new G4Box("CoreWrap_S", CoreWrapX/2, CoreWrapY/2, CoreWrapZ/2);
  
  CoreWrap_L = new G4LogicalVolume(CoreWrap_S,
				   G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
				   "CoreWrap_L");
  
  CoreWrap_P = new G4PVPlacement(0,
				 G4ThreeVector(0., 0., 0.*cm),
				 CoreWrap_L,
				 "TheCoreWrap",
				 CoreShield_L,
				 false,
				 0);
  
  G4VisAttributes *CoreWrapVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.4));
  CoreWrapVisAtt->SetForceSolid(true);
  CoreWrap_L->SetVisAttributes(CoreWrapVisAtt);
  
  // The core scintillator

  CoreRaw_S = new G4Box("CoreRaw_S", CoreScintX/2, CoreScintY/2, CoreScintZ/2); 

  // If specified, create 45 degree bevels on the core scintillator
  // edges that lie in the X and Y planes

  if(BevelCoreEdges){

    vector<G4int> AMod{1, -1, -1};
    vector<G4int> BMod{-1, 1, -1};



    CoreCut_S = new G4Box("CoreCut_S", CoreCutX/2, CoreCutY/2, CoreCutZ/2);

    // First bevel the four edges in the X direction

    CoreScint_SS = new G4SubtractionSolid("CoreScint_SS",
					  CoreRaw_S,
					  CoreCut_S,
					  new G4RotationMatrix(0.*degree,
							       CoreCutAngle,
							       -90*degree),
					  G4ThreeVector(0.,
							CoreCut_YTranslate,
							CoreCut_ZTranslate));

    for(int i=0; i<3; i++){
      CoreScint_SS = new G4SubtractionSolid("CoreScint_SS",
					    CoreScint_SS,
					    CoreCut_S,
					    new G4RotationMatrix(0.*degree,
								 CoreCutAngle,
								 -90*degree),
					    G4ThreeVector(0,
							  CoreCut_YTranslate * AMod[i],
							  CoreCut_ZTranslate * BMod[i]));
    }

    // Then cut the four edges in the Y direction

    CoreScint_SS = new G4SubtractionSolid("CoreScint_SS",
					  CoreScint_SS,
					  CoreCut_S,
					  new G4RotationMatrix(90.*degree,
							       CoreCutAngle,
							       -90*degree),
					  G4ThreeVector(CoreCut_XTranslate,
							0.,
							CoreCut_ZTranslate));

    vector<G4int> XMod{1, -1, -1};
    vector<G4int> ZMod{-1, 1, -1};
    
    for(int i=0; i<3; i++){
      CoreScint_SS = new G4SubtractionSolid("CoreScint_SS",
					    CoreScint_SS,
					    CoreCut_S,
					    new G4RotationMatrix(90.*degree,
								 CoreCutAngle,
								 -90*degree),
					    G4ThreeVector(CoreCut_XTranslate * AMod[i],
							  0.,
							  CoreCut_ZTranslate * BMod[i]));
    }
    CoreScint_S = CoreScint_SS;
  }

  // Otherwise, leave the core scintillator with 90 degree edges

  else
    CoreScint_S = CoreRaw_S;
  
  CoreScint_L = new G4LogicalVolume(CoreScint_S,
				    G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO"),
				    "CoreScint_L");
  
  CoreScint_P = new G4PVPlacement(0,
				  G4ThreeVector(0., 0., 0.),
				  CoreScint_L,
				  "TheCoreScint",
				  CoreWrap_L,
				  false,
				  0);
  
  G4VisAttributes *ScintVisAtt = new G4VisAttributes(G4Colour(0.5, 0.15, 1.0, 0.9));
  ScintVisAtt->SetForceSolid(true);
  CoreScint_L->SetVisAttributes(ScintVisAtt);
  
  if(InitialConstruction){
    CoreScint_SD = new ASIMScintillatorSD("CoreScint");
    CoreScint_SD->SetHitRGBA(1.0, 0.0, 0.0, 0.5);
    CoreScint_SD->SetHitSize(6);

    G4SDManager::GetSDMpointer()->AddNewDetector(CoreScint_SD);
  }
  CoreScint_L->SetSensitiveDetector(CoreScint_SD);
}



void geometryConstruction::BuildPerimeterScintillators()
{
  G4double PerimeterPos = CoreShieldZ/2 + PerimeterShieldZ/2 + PerimeterGap + ShieldThickness + WrapThickness;
  G4double PerimeterNandSOffset = PerimeterShieldZ;
  vector<G4double> XMultiplier{1., 0., -1., 0.};
  vector<G4double> ZMultiplier{0., 1., 0., -1.};
  
  vector<G4String> PerimeterPrefixes;
  PerimeterPrefixes.push_back("North");
  PerimeterPrefixes.push_back("West");
  PerimeterPrefixes.push_back("South");
  PerimeterPrefixes.push_back("East");

  stringstream SS;

  
  ///////////////////////////////////////
  // Build perimeter scintillator volumes

  PerimeterShield_S = new G4Box("PerimeterShield_S",
				PerimeterShieldX/2,
				PerimeterShieldY/2,
				PerimeterShieldZ/2);

  // Build the four perimeter scintillators in this order:
  // 0. The "north perimeter scintillator" (+X axis)
  // 1. The "west" perimeter scintillator" (+Z axis)
  // 2. The "south" perimeter scintillator (-X axis)
  // 3. The "east" perimeter scintillator (-Z axis)
  
  for(G4int scint=0; scint<NumPerimeterScints; scint++){
    
    SS << PerimeterPrefixes.at(scint) << "PerimeterShield_L";
    G4String LVName = SS.str();
    
    PerimeterShield_L.push_back(new G4LogicalVolume(PerimeterShield_S,
						    G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
						    LVName));
    
    G4VisAttributes *PerimeterShieldVisAtt = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.4));
    PerimeterShieldVisAtt->SetForceSolid(true);
    PerimeterShield_L.at(scint)->SetVisAttributes(PerimeterShieldVisAtt);
    
    SS.str("");
    SS << "The" << PerimeterPrefixes.at(scint) << "PerimeterShield";
    G4String PVName = SS.str();

    G4double Rotation = 90.*degree;
    if(scint==1 or scint==3)
      Rotation = 0.;

    G4double Rot0 = Rotation;
    G4double Rot1 = -1. * Rotation;
    G4double Rot2 = Rotation;

    G4double Pos = PerimeterPos;
    
    if(scint==0 or scint==2)
      Pos -= PerimeterNandSOffset;
    
    G4double PosX = Pos * XMultiplier.at(scint);
    G4double PosY = Scintillators_YTranslate;
    G4double PosZ = Pos * ZMultiplier.at(scint);
    
    PerimeterShield_P.push_back(new G4PVPlacement(new G4RotationMatrix(Rot0, Rot1, Rot2),
						  G4ThreeVector(PosX, PosY, PosZ),
						  PerimeterShield_L.at(scint),
						  PVName,
						  CaseChamber_L,
						  false,
						  0));
  }
  
  // The perimeter scintillator wrapping
  
  PerimeterWrap_S = new G4Box("PerimeterWrap_S",
			      PerimeterWrapX/2,
			      PerimeterWrapY/2,
			      PerimeterWrapZ/2);
  
  for(G4int scint=0; scint<NumPerimeterScints; scint++){

    SS.str("");
    SS << PerimeterPrefixes.at(scint) << "PerimeterWrap_L";
    G4String LVName = SS.str();

    PerimeterWrap_L.push_back(new G4LogicalVolume(PerimeterWrap_S,
						  G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
						  LVName));
    
    G4VisAttributes *PerimeterWrapVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.4));
    PerimeterWrapVisAtt->SetForceSolid(true);
    PerimeterWrap_L.at(scint)->SetVisAttributes(PerimeterWrapVisAtt);

    SS.str("");
    SS << "The" << PerimeterPrefixes.at(scint) << "PerimeterWrap";
    G4String PVName = SS.str();
    
    PerimeterWrap_P.push_back(new G4PVPlacement(0,
						G4ThreeVector(0., 0., 0.*cm),
						PerimeterWrap_L.at(scint),
						PVName,
						PerimeterShield_L.at(scint),
						false,
						0));
  }
   
  // The perimeter scintillator
  
  PerimeterScint_S = new G4Box("PerimeterScint_S",
			       PerimeterScintX/2,
			       PerimeterScintY/2,
			       PerimeterScintZ/2);
  
  for(G4int scint=0; scint<NumPerimeterScints; scint++){

    SS.str("");
    SS << PerimeterPrefixes.at(scint) << "PerimeterScint_L";
    G4String LVName = SS.str();
    
    PerimeterScint_L.push_back(new G4LogicalVolume(PerimeterScint_S,
						   materialsLibrary::EJ299(),
						   LVName));
    
    SS.str("");
    SS << "The" << PerimeterPrefixes.at(scint) << "PerimeterScintillator";
    G4String PVName = SS.str();
    
    PerimeterScint_P.push_back(new G4PVPlacement(0,
						 G4ThreeVector(0., 0., 0.),
						 PerimeterScint_L.at(scint),
						 PVName,
						 PerimeterWrap_L.at(scint),
						 false,
						 0));

    G4VisAttributes *ScintVisAtt = new G4VisAttributes(G4Colour(0.3, 0.5, 1.0, 0.7));
    ScintVisAtt->SetForceSolid(true);
    PerimeterScint_L.at(scint)->SetVisAttributes(ScintVisAtt);
    
    if(InitialConstruction){
      SS.str("");
      SS << PerimeterPrefixes.at(scint) << "PerimeterScint";
      G4String SDName = SS.str();
      
      PerimeterScint_SD.push_back(new ASIMScintillatorSD(SDName));
      PerimeterScint_SD.at(scint)->SetHitRGBA(1.0, 1.0, 0.0, 0.4);
      PerimeterScint_SD.at(scint)->SetHitSize(6);

      G4SDManager::GetSDMpointer()->AddNewDetector(PerimeterScint_SD.at(scint));
    }
    PerimeterScint_L.at(scint)->SetSensitiveDetector(PerimeterScint_SD.at(scint));
  }
}


void geometryConstruction::BuildRejectionScintillators()
{
  stringstream SS;

  vector<G4String> RejectionPrefixes{"Top", "Bottom"};
  
  RejectionShield_S = new G4Box("RejectionShield_S",
				RejectionShieldX/2,
				RejectionShieldY/2,
				RejectionShieldZ/2);
  
  for(int scint=0; scint<2; scint++){
    
    SS << RejectionPrefixes.at(scint) << "RejectionShield_L";
    G4String LVName = SS.str();
    
    RejectionShield_L.push_back(new G4LogicalVolume(RejectionShield_S,
						    G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
						    LVName));
    
    SS.str("");
    SS << "The" << RejectionPrefixes.at(scint) << "RejectionShield";
    G4String PVName = SS.str();
    
    G4double YPosition = RejectionPosTop;
    if(scint==1)
      YPosition = RejectionPosBot;
    
    RejectionShield_P.push_back(new G4PVPlacement(0,
						  G4ThreeVector(0., YPosition, 0),
						  RejectionShield_L.at(scint),
						  PVName,
						  CaseChamber_L,
						  false,
						  0));
    
    G4VisAttributes *RejectionShieldVisAtt = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.4));
    RejectionShieldVisAtt->SetForceSolid(true);
    RejectionShield_L.at(scint)->SetVisAttributes(RejectionShieldVisAtt);
  }

  // The rejection scintillator wrapping

  RejectionWrap_S = new G4Box("RejectionWrap_S",
			      RejectionWrapX/2,
			      RejectionWrapY/2,
			      RejectionWrapZ/2);
  
  for(int scint=0; scint<2; scint++){
    
    SS << RejectionPrefixes.at(scint) << "RejectionWrap_L";
    G4String LVName = SS.str();
    
    RejectionWrap_L.push_back(new G4LogicalVolume(RejectionWrap_S,
						  G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
						  LVName));
    
    SS.str("");
    SS << "The" << RejectionPrefixes.at(scint) << "RejectionWrap";
    G4String PVName = SS.str();
    
    RejectionWrap_P.push_back(new G4PVPlacement(0,
						G4ThreeVector(0., 0., 0.),
						RejectionWrap_L.at(scint),
						PVName,
						RejectionShield_L.at(scint),
						false,
						0));

    G4VisAttributes *RejectionWrapVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.4));
    RejectionWrapVisAtt->SetForceSolid(true);
    RejectionWrap_L.at(scint)->SetVisAttributes(RejectionWrapVisAtt);
  }

  // The rejection scintillator
  
  RejectionScint_S = new G4Box("RejectionScint_S",
			       RejectionScintX/2,
			       RejectionScintY/2,
			       RejectionScintZ/2);
  
  for(int scint=0; scint<2; scint++){
    
    SS << RejectionPrefixes.at(scint) << "RejectionScint_L";
    G4String LVName = SS.str();
    
    RejectionScint_L.push_back(new G4LogicalVolume(RejectionScint_S,
						    G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
						    LVName));

    SS.str("");
    SS << "The" << RejectionPrefixes.at(scint) << "RejectionScint";
    G4String PVName = SS.str();
    
    RejectionScint_P.push_back(new G4PVPlacement(0,
						 G4ThreeVector(0., 0., 0.),
						 RejectionScint_L.at(scint),
						 PVName,
						 RejectionWrap_L.at(scint),
						 false,
						 0));
    
    G4VisAttributes *RejectionScintVisAtt = new G4VisAttributes(G4Colour(0.3, 0.5, 1.0, 0.7));
    RejectionScintVisAtt->SetForceSolid(true);
    RejectionScint_L.at(scint)->SetVisAttributes(RejectionScintVisAtt);
    
    if(InitialConstruction){
      SS.str("");
      SS << RejectionPrefixes.at(scint) << "RejectionScint";
      G4String SDName = SS.str();
      
      RejectionScint_SD.push_back(new ASIMScintillatorSD(SDName));
      RejectionScint_SD.at(scint)->SetHitRGBA(1.0, 0.65, 0., 0.6);
      RejectionScint_SD.at(scint)->SetHitSize(6);
      
      G4SDManager::GetSDMpointer()->AddNewDetector(RejectionScint_SD.at(scint));
    }
    RejectionScint_L.at(scint)->SetSensitiveDetector(RejectionScint_SD.at(scint));
  }
}


void geometryConstruction::BuildPCBs()
{
  ////////////////
  // Build volumes 
  
  PCB_S = new G4Box("PCB_S", PCBX/2, PCBY/2, PCBZ/2);

  PCB_L = new G4LogicalVolume(PCB_S,
			      materialsLibrary::StainlessSteel(),
			      "PCB_L");

  for(G4int p=0; p<NumPCBs; p++){
    
    stringstream SS;
    SS << "PCB" << p;
    G4String PCBName = SS.str();
    
    PCB_P.push_back(new G4PVPlacement(new G4RotationMatrix(0., 0., 0.),
				      G4ThreeVector(0., PCB_YTranslate[p], 0.),
				      PCB_L,
				      PCBName,
				      CaseChamber_L,
				      false,
				      0));
  }
  
  G4VisAttributes *PCBVisAtt = new G4VisAttributes(G4Colour(0.0, 0.7, 0.0, 1.0));
  PCBVisAtt->SetForceSolid(true);
  PCB_L->SetVisAttributes(PCBVisAtt);

  PCBPost_S = new G4Tubs("PCB_S", PCBPostRMin, PCBPostRMax, PCBPostZ/2, 0.*degree, 360.*degree);

  PCBPost_L = new G4LogicalVolume(PCBPost_S,
				  G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
				  "PCBPost_L");


  // Multipliers to correctly place one post at each corner
  vector<G4int> XMod{1, 1, -1, -1};
  vector<G4int> ZMod{1, -1, -1, 1};
  
  for(G4int p=0; p<NumPCBPosts; p++)
    PCBPost_P.push_back(new G4PVPlacement(new G4RotationMatrix(0., 90.*degree, 0.),
					  G4ThreeVector(PCBPost_XTranslate * XMod[p],
							PCBPost_YTranslate,
							PCBPost_ZTranslate * ZMod[p]),
					  PCBPost_L,
					  "PCBPost",
					  CaseChamber_L,
					  false,
					  p));
  
  G4VisAttributes *PostVisAtt = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8, 0.8));
  PostVisAtt->SetForceSolid(true);
  PCBPost_L->SetVisAttributes(PostVisAtt);
}


void geometryConstruction::ConfigureReadouts()
{
  // Get the ASIM readout manager singleton
  ASIMReadoutManager *ARMgr = ASIMReadoutManager::GetInstance();


  //////////////////////////
  // Create ASIM readouts //
  //////////////////////////

  // Reset all readouts to account for geometry rebuild at runtime

  ARMgr->ClearReadoutsAndArrays();

  // Register readouts for all three scintillators
  
  ARMgr->RegisterNewReadout("CScint",
			    CoreScint_P);
  
  ARMgr->RegisterNewReadout("NScint",
			    PerimeterScint_P.at(0));
  
  ARMgr->RegisterNewReadout("WScint",
			    PerimeterScint_P.at(1));

  ARMgr->RegisterNewReadout("SScint",
			    PerimeterScint_P.at(2));
  
  ARMgr->RegisterNewReadout("EScint",
			    PerimeterScint_P.at(3));
  
  ARMgr->RegisterNewReadout("TScint",
			    RejectionScint_P.at(0));
  
  ARMgr->RegisterNewReadout("BScint",
			    RejectionScint_P.at(1));

  ////////////////////////
  // Create ASIM arrays //
  ////////////////////////

  // Create a convenient integer ID for each scintillator; the order
  // must correspond to the registration order
  enum{Core, North, West, South, East, Top, Bottom};
  
  // Create four ASIM readout arrays, one for each combination of
  // perimeter scintillor + core scintillator

  G4bool CoincidenceMode = true;

  vector<G4int> NorthCore{North, Core};
  ARMgr->CreateArray("NorthCore", NorthCore, CoincidenceMode);
  
  vector<G4int> WestCore{West, Core};
  ARMgr->CreateArray("WestCore", WestCore, CoincidenceMode);
  
  vector<G4int> SouthCore{South, Core};
  ARMgr->CreateArray("SouthCore", SouthCore, CoincidenceMode);
  
  vector<G4int> EastCore{East, Core};
  ARMgr->CreateArray("EastCore", EastCore, CoincidenceMode);
}


void geometryConstruction::CalculateGeometry()
{
  ///////////////////////////////////////
  // Calculate new geometry parameters //
  ///////////////////////////////////////

  // PCB and PCBPosts

  PCBX = CoreScintX/2 + 3.*cm;
  PCBZ = CoreScintZ + 6*WrapThickness + 6*ShieldThickness + 2*PerimeterScintZ + 2*PerimeterGap + 1.5*cm;
  PCBPost_ZTranslate = PCBZ/2 - PCBPostRMax;
  
  PCBPost_XTranslate = PCBX/2 - PCBPostRMax;
  PCBPost_YTranslate = PCB_YTranslate[0] - PCBY/2 - PCBPostZ/2;
  PCBPost_ZTranslate = PCBZ/2 - PCBPostRMax;
  
  // Perimeter scintillator volumes

  if(!FixPerimeterScintWidth){
    PerimeterScintX = CoreScintX + 2*WrapThickness + 2*ShieldThickness + 2*PerimeterGap;
    PerimeterScintY = CoreScintY;
    PerimeterScintZ = 0.1*cm;
  }
  
  PerimeterWrapX = PerimeterScintX + 2*WrapThickness;
  PerimeterWrapY = PerimeterScintY + 2*WrapThickness;
  PerimeterWrapZ = PerimeterScintZ + 2*WrapThickness;
  
  PerimeterShieldX = PerimeterWrapX + 2*ShieldThickness;
  PerimeterShieldY = PerimeterWrapY + 2*ShieldThickness;
  PerimeterShieldZ = PerimeterWrapZ + 2*ShieldThickness;

  // Core scintillator volumes

  CoreWrapX = CoreScintX + 2*WrapThickness;
  CoreWrapY = CoreScintY + 2*WrapThickness;
  CoreWrapZ = CoreScintZ + 2*WrapThickness;

  CoreShieldX = CoreWrapX + 2*ShieldThickness;
  CoreShieldY = CoreWrapY + 2*ShieldThickness;
  CoreShieldZ = CoreWrapZ + 2*ShieldThickness;

    // Rejection scintillators

  RejectionWrapX = RejectionScintX + 2*WrapThickness;
  RejectionWrapY = RejectionScintY + 2*WrapThickness;
  RejectionWrapZ = RejectionScintZ + 2*WrapThickness;

  RejectionShieldX = RejectionScintX + 2*WrapThickness;
  RejectionShieldY = RejectionScintY + 2*WrapThickness;
  RejectionShieldZ = RejectionScintZ + 2*WrapThickness;
  
  RejectionPosTop = Scintillators_YTranslate + CoreShieldY/2 + RejectionShieldY/2 + RejectionGap;
  RejectionPosBot = Scintillators_YTranslate - CoreShieldY/2 - RejectionShieldY/2 - RejectionGap;
  

  // Case volumes

  CaseX = PCBX + 1.*cm;
  CaseZ = PCBZ + 1.*cm;

  CaseChamberX = CaseX - CaseThicknessX*2;
  CaseChamberY = CaseY - CaseThicknessY*2;
  CaseChamberZ = CaseZ - CaseThicknessZ*2;

  CaseCutoutZ = CaseZ + 1.*cm;
}


// Messenger methods

void geometryConstruction::SetDetectorPos(G4ThreeVector)
{
  RebuildGeometry();
}


void geometryConstruction::SetDetectorRot(G4ThreeVector)
{
  RebuildGeometry();
}



void geometryConstruction::SetCoreScintSize(G4ThreeVector Size)
{  
  CoreScintX = Size.getX();
  CoreScintY = Size.getY();
  CoreScintZ = Size.getZ();
  
  RebuildGeometry();
}


void geometryConstruction::SetPerimeterScintSize(G4ThreeVector Size)
{ 
  PerimeterScintX = Size.getX();
  PerimeterScintY = Size.getY();
  PerimeterScintZ = Size.getZ();
 
  RebuildGeometry();
}

void geometryConstruction::SetPerimeterGap(G4double Gap)
{
  PerimeterGap = Gap;

  RebuildGeometry();
}


void geometryConstruction::SetWrapThickness(G4double Thickness)
{
  WrapThickness = Thickness;
  
  RebuildGeometry();
}


void geometryConstruction::SetWrapLayers(G4int Layers)
{
  WrapThickness = SingleWrapLayerThickness * Layers;
  
  RebuildGeometry();
}


void geometryConstruction::SetShieldThickness(G4double Thickness)
{
  ShieldThickness = Thickness;
  
  RebuildGeometry();
}


void geometryConstruction::SetShieldLayers(G4int Layers)
{
  ShieldThickness = SingleShieldLayerThickness * Layers;
  
  RebuildGeometry();
}


void geometryConstruction::RebuildGeometry()
{
  //////////////////////////
  // Recalculate geometry //
  //////////////////////////

  CalculateGeometry();
  

  ////////////////////////////
  // Clear previous volumes //
  ////////////////////////////

  ///////////////////////////////
  // Remove the core scintillator

  if(CoreScint_P){
    CoreWrap_L->RemoveDaughter(CoreScint_P);
    delete CoreScint_P;
    CoreScint_P = NULL;
    delete CoreScint_L;
    delete CoreScint_S;
  }

  if(CoreWrap_P){
    CoreShield_L->RemoveDaughter(CoreWrap_P);
    delete CoreWrap_P;
    CoreWrap_P = NULL;
    delete CoreWrap_L;
    delete CoreWrap_S;
  }

  if(CoreShield_P){
    CaseChamber_L->RemoveDaughter(CoreShield_P);
    delete CoreShield_P;
    CoreShield_P = NULL;
    delete CoreShield_L;
    delete CoreShield_S;
  }

  /////////////////////////////////////
  // Remove the perimeter scintillators
  
  for(G4int scint=0; scint<NumPerimeterScints; scint++){
    
    if(PerimeterScint_P.at(scint)){
      PerimeterWrap_L.at(scint)->RemoveDaughter(PerimeterScint_P.at(scint));
      delete PerimeterScint_P.at(scint);
      PerimeterScint_P.at(scint) = NULL;
      delete PerimeterScint_L.at(scint);
    }
    
    if(PerimeterWrap_P.at(scint)){
      PerimeterShield_L.at(scint)->RemoveDaughter(PerimeterWrap_P.at(scint));
      delete PerimeterWrap_P.at(scint);
      PerimeterWrap_P.at(scint) = NULL;
      delete PerimeterWrap_L.at(scint);
    }

    if(PerimeterShield_P.at(scint)){
      CaseChamber_L->RemoveDaughter(PerimeterShield_P.at(scint));
      delete PerimeterShield_P.at(scint);
      PerimeterShield_P.at(scint) = NULL;
      delete PerimeterShield_L.at(scint);
    }
  }

  PerimeterScint_L.clear();
  PerimeterScint_P.clear();
  PerimeterWrap_L.clear();
  PerimeterWrap_P.clear();
  PerimeterShield_L.clear();
  PerimeterShield_P.clear();
  
  delete PerimeterScint_S;
  delete PerimeterWrap_S;
  delete PerimeterShield_S;

  ////////////////////////////////////
  // Remove the rejection scintillator
  
  for(G4int scint=0; scint<NumRejectionScints; scint++){
    
    if(RejectionScint_P.at(scint)){
      RejectionWrap_L.at(scint)->RemoveDaughter(RejectionScint_P.at(scint));
      delete RejectionScint_P.at(scint);
      RejectionScint_P.at(scint) = NULL;
      delete RejectionScint_L.at(scint);
    }
    
    if(RejectionWrap_P.at(scint)){
      RejectionShield_L.at(scint)->RemoveDaughter(RejectionWrap_P.at(scint));
      delete RejectionWrap_P.at(scint);
      RejectionWrap_P.at(scint) = NULL;
      delete RejectionWrap_L.at(scint);
    }

    if(RejectionShield_P.at(scint)){
      CaseChamber_L->RemoveDaughter(RejectionShield_P.at(scint));
      delete RejectionShield_P.at(scint);
      RejectionShield_P.at(scint) = NULL;
      delete RejectionShield_L.at(scint);
    }
  }
  
  RejectionScint_L.clear();
  RejectionScint_P.clear();
  RejectionWrap_L.clear();
  RejectionWrap_P.clear();
  RejectionShield_L.clear();
  RejectionShield_P.clear();
  
  delete RejectionScint_S;
  delete RejectionWrap_S;
  delete RejectionShield_S;

  //////////////////////////////
  // Remove the PCB and PCBPosts

  for(G4int p=0; p<NumPCBs; p++){
    if(PCB_P.at(p)){
      CaseChamber_L->RemoveDaughter(PCB_P.at(p));
      delete PCB_P.at(p);
      PCB_P.at(p) = NULL;
    }
  }

  for(G4int p=0; p<NumPCBPosts; p++){
    if(PCBPost_P.at(p)){
      CaseChamber_L->RemoveDaughter(PCBPost_P.at(p));
      delete PCBPost_P.at(p);
      PCBPost_P.at(p) = NULL;
    }
  }

  PCB_P.clear();
  PCBPost_P.clear();

  delete PCB_L;
  delete PCB_S;

  delete PCBPost_L;
  delete PCBPost_S;

  //////////////////
  // Remove the case
  
  if(CaseChamber_P){
    World_L->RemoveDaughter(CaseChamber_P);
    delete CaseChamber_P;
    CaseChamber_P = NULL;
    delete CaseChamber_L;
    delete CaseChamber_S;
  }
  
  if(Case_P){
    World_L->RemoveDaughter(Case_P);
    delete Case_P;
    Case_P = NULL;
    delete Case_L;
    delete Case_SS;
    delete CaseCutout_S;
    delete CaseShell_S;
  }
  
  //////////////////////////
  // Rebuild the geometry //
  //////////////////////////
  
  if(!CheckForGeometryOverlaps()){
    Construct();
    
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    if(UsingVisualization)
      G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  }
}


G4bool geometryConstruction::CheckForGeometryOverlaps()
{
  // Method still needs proper implementation with settable position
  // and rotation for proper checking. For now, always return
  // false. ZSH (28 Jun 15).
  /*
  G4bool geometryOverlaps = true;
  
  G4VPhysicalVolume *overlapCheck_P =
    new G4PVPlacement(new G4RotationMatrix(0., 0., 0.),
		      G4ThreeVector(0., 0., 0.),
		      Shield_L,
		      "GeometryOverlapChecker",
		      World_L,
		      false,
		      0);
  
  geometryOverlaps = overlapCheck_P->CheckOverlaps();
  
  delete overlapCheck_P;
  */
  return false;
}
