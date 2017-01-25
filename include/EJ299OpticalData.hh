#ifndef EJ299OpticalData_hh
#define EJ299OpticalData_hh 1

// name: EJ299OpticalData.hh
// date: 10 Jun 14
// auth: Zach Hartwig

// desc: This file contains material and optical data for the plastic
//       organic scintillator EJ299-33 (Eljen Technologies). "EJ299"
//       technically refers to their experimental scintillator
//       mixtures with, in this case, the "-33" referring to the pulse
//       shape discriminating plastic mixture based on Zaitseva's work
//       at LLNL. At present, there is a lot of spotty or absent data
//       on the scintillator, and the Eljen mixtures is still in
//       active development. All data below is references from the
//       following:
//
//       [1] http://www.eljentechnology.com/index.php/products/plastic-scintillators/114-ej-299-33
//
//       [2] Zaitseva et al. NIM A 668 (2012) 88-93. http://dx.doi.org/10.1016/j.nima.2011.11.071
//
//       [3] Nyibule et al. NIM A 728 (2013) 36-39. http://dx.doi.org/10.1016/j.nima.2013.06.020
//
//       [4] No data available! Values for EJ301 used as placeholder

namespace EJ299{

  // Density [1]
  const G4double EJ299_density = 1.08*g/cm3;

  // Number of atoms (x10^20) in a cm3 of scintillator [1]
  const G4int EJ299_CAtoms = 486;
  const G4int EJ299_HAtoms = 513;

  // Refractive index [4]
  const G4int EJ299_rIndexEntries = 2;
  G4double EJ299_rIndexSpectrum[2] = {1.*eV, 15.*eV};
  G4double EJ299_rIndex[2] = {1.505, 1.505};

  // Absorption length [4]
  const G4int EJ299_absLengthEntries = 2;
  G4double EJ299_absLengthSpectrum[4] = {1.*eV, 15.*eV};
  G4double EJ299_absLength[4] = {8.47*m, 8.47*m};

  // Fast time decay constant [4]
  const G4double EJ299_fastTimeC = 3.2*ns;

  // Slow time decay constant [4]
  const G4double EJ299_slowTimeC = 32.3*ns;

  // Fast/slow alpha/beta ratios [4]
  const G4double EJ299_fastComponent = 0.073; 
  const G4double EJ299_slowComponent = 0.098; 
  
  // Scaling factor to reduce number of scintillation photons
  // produced. Set to 1.0 until future work on scaling is implemented
  // for nonlinear scintillation
  const G4double EJ299_yieldScaleFactor = 1.0;
  
  // This parameter gives the relative strength of the fast component as
  // a fraction of total scintillation yield.  
  const G4double EJ299_yRatio = 1.0;

  // The number of optical photon scintillated is distributed in a
  // gaussian around the scint. yield.  This factor describes the
  // "broadening" beyond the gaussian (1.0 = gaussian).
  const G4double EJ299_rScale = 0.8;

  // The EJ299 emission spectrum [1]
  const G4int EJ299_entries = 46;

  G4double EJ299_eSpectrum[EJ299_entries];

  G4double EJ299_eWavelength[EJ299_entries] = {376.72*nm,
					       381.13*nm,
					       384.28*nm,
					       386.18*nm,
					       387.66*nm,
					       388.93*nm,
					       390.20*nm,
					       391.57*nm,
					       392.73*nm,
					       394.00*nm,
					       395.69*nm,
					       398.42*nm,
					       402.41*nm,
					       406.20*nm,
					       409.14*nm,
					       411.87*nm,
					       414.40*nm,
					       416.92*nm,
					       420.50*nm,
					       423.84*nm,
					       426.35*nm,
					       429.07*nm,
					       431.99*nm,
					       435.97*nm,
					       440.58*nm,
					       445.19*nm,
					       449.80*nm,
					       453.56*nm,
					       457.12*nm,
					       461.51*nm,
					       466.12*nm,
					       470.73*nm,
					       475.34*nm,
					       479.95*nm,
					       484.56*nm,
					       489.16*nm,
					       493.77*nm,
					       498.38*nm,
					       502.99*nm,
					       507.59*nm,
					       512.20*nm,
					       516.81*nm,
					       521.42*nm,
					       526.04*nm,
					       530.65*nm,
					       534.21*nm};

  G4double EJ299_prob[EJ299_entries] = {1.98,
					6.51,
					11.03,
					16.60,
					23.53,
					29.51,
					35.42,
					41.36,
					48.44,
					53.90,
					59.58,
					64.77,
					69.42,
					74.03,
					79.00,
					83.74,
					88.89,
					93.96,
					98.71,
					93.37,
					88.12,
					82.41,
					77.35,
					72.58,
					68.29,
					65.50,
					63.29,
					58.06,
					53.09,
					47.92,
					43.69,
					40.80,
					38.52,
					36.25,
					33.75,
					29.99,
					26.30,
					22.64,
					19.11,
					15.95,
					13.59,
					11.53,
					9.69,
					8.06,
					7.11,
					6.37};

  // Photon yield per MeV of electron energy deposited [1]
  const G4double EJ299_yield = 8000; // [photons MeVee^{-1}]

  // At present, there is very minimal data in the technical or
  // academic literature regarding the light response curves (energy
  // deposited vs photon production) for individual particles. The
  // only known source is from [3], where the most useful data is
  // published as fits to graphically presented data. Rather than
  // extract this data (difficult given the presentation), the
  // quadratic fit coefficients are assigned here and then used within
  // the materialsLibrary class to construct the light response
  // functions for the particles of interest. The coefficients have
  // the form: 
  //             L(E) [MeVee] = c0 + c1*E + c2*E
  //
  // where E is the energy deposited by the charged particle [MeV]

  const G4double EJ299_pCoeffs[3] = {-0.15, 0.25, 0.0096};  // protons
  const G4double EJ299_dCoeffs[3] = {-0.25, 0.26, 0.0039};  // deuterons
  const G4double EJ299_tCoeffs[3] = { 0.09, 0.21, 0.0022};  // tritons
  const G4double EJ299_aCoeffs[3] = {-0.14, 0.11, 0.0022};  // alphas
  const G4double EJ299_cCoeffs[3] = {-0.08, 0.01, 0.00006}; // carbons
}

#endif
