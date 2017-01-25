#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhysics.hh"
#include "G4Scintillation.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "materialsLibrary.hh"
#include "EJ299OpticalData.hh"
using namespace EJ299;

/////////////////////////
//  ELEMENTS/ISOTOPES  //
/////////////////////////


G4Material *materialsLibrary::SiPM(G4String SiPMType)
{
  static G4bool init = true;
  static G4Material *SiliconPM = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  static G4MaterialPropertiesTable *SiliconPM_MPT = new G4MaterialPropertiesTable();
  
  if(init){

    // Create the photodiode quantum efficiency "QE" [%] as a function
    // of incident optical photon energy [eV] for the specified type
    // of Si-APD.  



    if(SiPMType=="S10985-050C"){
      const G4int PDEEntries = 23;
      
      // Optical photon wavelength [nm]
      G4double PDEPhotonWavelength[PDEEntries] = {318.5*nm,
						  324.1*nm,
						  331.4*nm,
						  337.9*nm,
						  357.2*nm,
						  371.7*nm,
						  387.9*nm,
						  412.0*nm,
						  445.9*nm,
						  474.1*nm,
						  503.2*nm,
						  526.6*nm,
						  562.9*nm,
						  598.3*nm,
						  626.6*nm,
						  663.7*nm,
						  700.0*nm,
						  743.5*nm,
						  785.4*nm,
						  826.6*nm,
						  866.9*nm,
						  900.0*nm};
      
      // Optical photon energy [eV]
      G4double PDEPhotonEnergy[PDEEntries];

      // Photodiode reflectivity [%] as a function of photon energy.
      // If reflectance is > 0, optical photons will scatter back into
      // scintillation volume, rattle around, and then return to Si-APD
      G4double R[PDEEntries];

      const G4double nm2eV = 1239.583*eV*nm;
      
      for(G4int i=0; i<PDEEntries; i++){
	PDEPhotonEnergy[i] =  nm2eV / PDEPhotonWavelength[i];	
	R[i] = 0.;
      }
      
      // SiPM Photon Detection Efficiency (PSD)
      G4double PDE[PDEEntries] = {13.6*perCent,
				  23.3*perCent,
				  32.2*perCent,
				  36.1*perCent,
				  37.9*perCent,
				  39.7*perCent,
				  46.3*perCent,
				  48.7*perCent,
				  50.8*perCent,
				  49.3*perCent,
				  46.5*perCent,
				  43.0*perCent,
				  37.0*perCent,
				  31.5*perCent,
				  27.3*perCent,
				  22.1*perCent,
				  17.9*perCent,
				  13.9*perCent,
				  10.6*perCent,
				  7.9*perCent,
				  5.6*perCent,
				  3.8*perCent};
      
      // Set the efficiency and reflectivity of the Si-APD material
      SiliconPM_MPT->AddProperty("EFFICIENCY", PDEPhotonEnergy, PDE, PDEEntries);
      SiliconPM_MPT->AddProperty("REFLECTIVITY", PDEPhotonEnergy, R, PDEEntries);
    }
    else
      G4Exception("masterialsLibrary::SiPM()",
		  "materialException002",
		  FatalException,
		  "SiPM type not found! Present choices are: 'S10985-050C'");
    
    const G4int RIentries = 14;
      
    G4double RIPhotonEnergy[RIentries] = {0.1*eV, 0.5*eV, 1.0*eV, 1.5*eV, 2.0*eV, 2.5*eV,
					  3.0*eV, 3.1*eV, 3.2*eV, 3.3*eV, 3.4*eV, 3.5*eV,
					  3.9*eV, 9.9*eV};
      
    G4double RI[RIentries] = {3.421, 3.443, 3.517, 3.660, 3.898, 4.318,
			      5.270, 5.632, 6.197, 6.888, 6.557, 5.675,
			      5.125, 5.125};

    // Set the refractive index of the Si-APD material
    SiliconPM_MPT->AddProperty("RINDEX", RIPhotonEnergy, RI, RIentries);
      
    // Set the MPT to the material
    SiliconPM->SetMaterialPropertiesTable(SiliconPM_MPT);
    
    init = false;
  }
  return SiliconPM;
}


//////////////
// MIXTURES //
//////////////


// Reference: http://www.kayelaby.npl.co.uk/general_physics/2_5/2_5_7.html
G4Material *materialsLibrary::Air()
{ 
  static G4bool init = true;
  static G4Material *Air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"); 
  static G4MaterialPropertiesTable *airMPT = new G4MaterialPropertiesTable();
  if(init){ 
    const G4int nEntries = 2;
    G4double photonEnergy[nEntries] = {1.*eV, 10.*eV};
    G4double refractiveIndex[nEntries] = {1.0003, 1.0003};

    airMPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
    Air->SetMaterialPropertiesTable(airMPT);
    
    init=false;
  }
  return Air;
}


// Reference: See "../include/EJ299OpticalData.hh"
G4Material *materialsLibrary::EJ299()
{
  static G4bool init = true;
  static G4Material *EJ299 = new G4Material("EJ299", EJ299_density, 2);
  static G4MaterialPropertiesTable *EJ299_MPT = new G4MaterialPropertiesTable();
  
  if(init){

    EJ299->AddElement(G4NistManager::Instance()->FindOrBuildElement(6), EJ299_CAtoms);
    EJ299->AddElement(G4NistManager::Instance()->FindOrBuildElement(1), EJ299_HAtoms);
    
    
    // Transform the wavelength spectrum [nm] into energy spectrum [eV]
    const G4double nm2eV = 1239.583*eV*nm;
    for(G4int i=0; i<EJ299_entries; i++)
      EJ299_eSpectrum[i] = nm2eV / EJ299_eWavelength[i];

    // Construct the scintillation light response functions for each
    // particle from the published quadratic fit data

    const G4int EJ299_lightEntries = 100;

    G4double EJ299_energyDep[EJ299_lightEntries];
    
    G4double EJ299_eLight[EJ299_lightEntries], EJ299_pLight[EJ299_lightEntries];
    G4double EJ299_dLight[EJ299_lightEntries], EJ299_tLight[EJ299_lightEntries];
    G4double EJ299_aLight[EJ299_lightEntries], EJ299_cLight[EJ299_lightEntries];

    for(G4int i=0; i<EJ299_lightEntries; i++){

      // Energy deposition ranges from 0 to 50 MeV in steps of 0.5 MeV
      G4double E = i * 1.0 / 2; // [MeV]

      // Calculate the number of scintillation photons produced for
      // each particle type as a function of energy deposited. Note
      // that the electron response is assumed to be linear over the
      // entire deposition range of interest.
      EJ299_eLight[i] = E * EJ299_yield;
      EJ299_pLight[i] = EJ299_pCoeffs[0] + EJ299_pCoeffs[1] * E + EJ299_pCoeffs[2] * E * E;
      EJ299_dLight[i] = EJ299_dCoeffs[0] + EJ299_dCoeffs[1] * E + EJ299_dCoeffs[2] * E * E;
      EJ299_tLight[i] = EJ299_tCoeffs[0] + EJ299_tCoeffs[1] * E + EJ299_tCoeffs[2] * E * E;
      EJ299_aLight[i] = EJ299_aCoeffs[0] + EJ299_aCoeffs[1] * E + EJ299_aCoeffs[2] * E * E;
      EJ299_cLight[i] = EJ299_cCoeffs[0] + EJ299_cCoeffs[1] * E + EJ299_cCoeffs[2] * E * E;
    }
    
    EJ299_MPT->AddConstProperty("FASTTIMECONSTANT", EJ299_fastTimeC);
    EJ299_MPT->AddConstProperty("SLOWTIMECONSTANT", EJ299_slowTimeC);
    EJ299_MPT->AddConstProperty("YIELDRATIO",EJ299_yRatio);
    EJ299_MPT->AddConstProperty("RESOLUTIONSCALE",EJ299_rScale);
    EJ299_MPT->AddConstProperty("SCINTILLATIONYIELD", EJ299_yield);
    
    EJ299_MPT->AddProperty("ELECTRONSCINTILLATIONYIELD", EJ299_energyDep, EJ299_eLight, EJ299_lightEntries);
    EJ299_MPT->AddProperty("PROTONSCINTILLATIONYIELD", EJ299_energyDep, EJ299_pLight, EJ299_lightEntries);
    EJ299_MPT->AddProperty("DEUTERONSCINTILLATIONYIELD", EJ299_energyDep, EJ299_dLight, EJ299_lightEntries);
    EJ299_MPT->AddProperty("TRITONSCINTILLATIONYIELD", EJ299_energyDep, EJ299_tLight, EJ299_lightEntries);
    EJ299_MPT->AddProperty("ALPHASCINTILLATIONYIELD", EJ299_energyDep, EJ299_aLight, EJ299_lightEntries);
    EJ299_MPT->AddProperty("IONSCINTILLATIONYIELD", EJ299_energyDep, EJ299_cLight, EJ299_lightEntries);
    
    EJ299_MPT->AddProperty("RINDEX", EJ299_rIndexSpectrum, EJ299_rIndex, EJ299_rIndexEntries);
    EJ299_MPT->AddProperty("ABSLENGTH", EJ299_absLengthSpectrum, EJ299_absLength, EJ299_absLengthEntries);
    EJ299_MPT->AddProperty("FASTCOMPONENT", EJ299_eSpectrum, EJ299_prob, EJ299_entries);
        
    EJ299->SetMaterialPropertiesTable(EJ299_MPT);
    
    init = false;
  }
  return EJ299;
}


// Stainless Steel Grade 304L
// http://www.azom.com/details.asp?ArticleID=965
G4Material *materialsLibrary::StainlessSteel()
{
  static G4bool init = true;
  static G4Material *SS304 = new G4Material("Stainless Steel", 8.03*g/cm3, 9);
  if(init){
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe"), 66.495*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_C"), 0.08*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_Mn"), 2.0*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), 0.75*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_P"), 0.045*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_S"), 0.030*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_Cr"), 20.0*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_Ni"), 10.5*perCent);
    SS304->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_N"), 0.10*perCent);
    init = false;
  }
  return SS304;
}

