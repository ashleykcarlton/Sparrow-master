/////////////////////////////////////////////////////////////////////////////////
//
// name: KECorrectionGenerator.cc
// date: 15 Jan 16
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: C++ code to automatically create the Geant4 macro required to
//       generate the Sparrow kinetic energy correction factors. The
//       primary reasons this macro is generated specially is so that
//       the proton energy thresholds for accepting events in the ASIM
//       array for the West-Core scintillator combintation can be
//       dynamically adjusted as a function of kinetic energy. This is
//       necessary in order to ensure the correct kinetic energy is
//       calculated during event reconstruction when using the kinetic
//       energy correction factor. At present, the energy thresholds
//       for electrons is left at zero.
//
/////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int main(int argc, char *argv[]){

  int NParticles = 50000;

  double pKEMin = 5, pKEMax = 85, pKEStep = 2.5;
  double pThreshDiff = 5;
  
  double eKEMin = 1, eKEMax = 20, eKEStep = 0.5;
  double eThreshDiff = 1;

  ofstream Output("KECorrection.mac", ofstream::trunc);

  ifstream Input("KECorrectionTemplate.mac");

  string InputLine;

  stringstream SS;
  
  while(Input.good()){

    getline(Input, InputLine);

    if(Input.eof())
      break;

    if(InputLine == "$$ Insert proton commands $$"){

      for(double KE=pKEMin; KE<=pKEMax; KE=KE+pKEStep){
	SS.str("");
	SS << "/gps/ene/mono " << KE << " MeV";
	Output << SS.str() << endl;

	SS.str("");
	SS << "/ASIM/array/setLowerEnergyThreshold " << KE-pThreshDiff << " MeV";
	Output << SS.str() << endl;

	SS.str("");
	SS << "/run/beamOn " << NParticles << "\n";
	Output << SS.str() << endl;
      }
    }

    else if(InputLine == "$$ Insert electron commands $$"){

      for(double KE=eKEMin; KE<=eKEMax; KE=KE+eKEStep){
	SS.str("");
	SS << "/gps/ene/mono " << KE << " MeV";
	Output << SS.str() << endl;

	SS.str("");
	SS << "/ASIM/array/setLowerEnergyThreshold " << 0 << " MeV";
	Output << SS.str() << endl;

	SS.str("");
	SS << "/run/beamOn " << NParticles << "\n";
	Output << SS.str() << endl;
      }
    }

    else
      Output << InputLine << endl;
  }
  Input.close();

  Output.close();

  return 0;
}
