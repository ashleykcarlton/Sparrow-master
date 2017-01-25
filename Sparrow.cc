/////////////////////////////////////////////////////////////////////////////////
//
// name: Sparrow.cc
// date: 29 Dec 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc:
//
/////////////////////////////////////////////////////////////////////////////////

// Geant4
#include "G4RunManager.hh" 
#include "G4VisManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4TrajectoryDrawByParticleID.hh"

// C++
#include <ctime>

// ASIM
#include <ASIMReadoutManager.hh>
#ifdef MPI_ENABLED
#include "MPIManager.hh"
#endif

// Sparrow
#include "geometryConstruction.hh"
#include "physicsList.hh"
#include "PGA.hh"
#include "steppingAction.hh"
#include "stackingAction.hh"
#include "trackingAction.hh"
#include "eventAction.hh"
#include "runAction.hh"


int main(int argc, char *argv[])
{
  ////////////////////////////////
  // Parse command line options //
  ////////////////////////////////

  // Option to print help information to the cmd line
  if(argc==2 and (string)argv[1]=="-help"){
    G4cout << "\nThe Geant4 Sparrow simulation may be run with the following cmd line options:\n"
	   <<   "  arg0 == -vis     : activate Qt visualization and console     (default: no vis/cmd line only)\n"
	   <<   "  arg1 == -hp      : use high precision (HP) neutron transport (default: HP transport off)\n"
	   <<   "  arg2 == -optical : use optical physics models                (default: optical physics off)\n"
	   <<   "  arg3 == -seed    : seed the RNG with the present time        (default: standard G4 RNG seed\n"
	   <<   "  arg4 == <fname>  : autorun specified macro; parallel only!   (default: runtime/SDE.mpi.mac)\n"
	   << G4endl;
    return 0;
  }

  // Option to turn visualization
  G4bool useVisualization = false;
  if(argc>1){
    std::string arg1 = argv[1];
    if(arg1=="-vis")
      useVisualization = true;
  }
  
  // Option to enable high precision (HP) neutron transport
  G4bool useNeutronHP = false;
  if(argc>2){
    std::string arg2 = argv[2];
    if(arg2=="-hp")
      useNeutronHP = true;
  }

  // Option to enable optical physics
  G4bool useOptical = false;
  if(argc>3){
    std::string arg3 = argv[3];
    if(arg3=="-optical")
      useOptical = true;
  }

  // Option to enable seeding the RNG from the current time
  if(argc>4){
    std::string arg4 = argv[4];
    if(arg4=="-seed")
      CLHEP::HepRandom::setTheSeed(time(0));
  }
  
  // Option to specify start macro for parallel processing. Note user
  // should input macro with relative path to SDE_TOPDIR
  G4String MPIMacroName = "runtime/Sparrow.mpi.mac";
  if(argc>5)
    MPIMacroName.assign("runtime/" + (string)argv[5]);
  
  
  ////////////////////////////////////
  // Sequential/parallel processing //
  ////////////////////////////////////
  
  G4bool useParallelProcessing = false;

#ifdef MPI_ENABLED
  useParallelProcessing = true;
  
  // Declare faux command line arguments to pass to MPI; use static to
  // ensure that only one copy is created to prevent memory issues
  static const G4int argcMPI = 2;
  static char *argvMPI[argcMPI];
  argvMPI[0] = argv[0]; // binary name
  argvMPI[1] = (char *)"slaveForum/Slave"; // slave file base name
  
  MPIManager *theMPIManager= new MPIManager(argcMPI, argvMPI);
#endif

  /////////////////////////////////////////////////
  // Initialize the singleton ASIMStorageManager //
  /////////////////////////////////////////////////

  ASIMReadoutManager *theReadoutManager = new ASIMReadoutManager;
  if(useParallelProcessing)
    theReadoutManager->EnableParallelMode();
  

  //////////////////////////////////////////////////
  // Initialize mandatory and option user classes //
  //////////////////////////////////////////////////
    
  G4RunManager* theRunManager = new G4RunManager;
  
  theRunManager->SetUserInitialization(new geometryConstruction(useVisualization));
  theRunManager->SetUserInitialization(new physicsList(useNeutronHP,
						       useOptical));
  theRunManager->SetUserAction(new PGA);
  theRunManager->Initialize();
  
  theRunManager->SetUserAction(new runAction);
  theRunManager->SetUserAction(new steppingAction);
  theRunManager->SetUserAction(new trackingAction);
  theRunManager->SetUserAction(new stackingAction);
  theRunManager->SetUserAction(new eventAction);
  
  
  ///////////////////////////////////
  // Initialize the user interface //
  ///////////////////////////////////
  
  G4UImanager* UI = G4UImanager::GetUIpointer();
  {
    G4String Alias = "/control/alias";
    G4String Exec = "/control/execute";

    // Aliases for macro files (e.g. run particles)
    UI->ApplyCommand(Alias + " mac " + Exec + " runtime/Sparrow.mac");
    UI->ApplyCommand(Alias + " wrap " + Exec + " runtime/WrappingScan.mac");
    UI->ApplyCommand(Alias + " gap " + Exec + " runtime/PerimeterGapScan.mac");
    UI->ApplyCommand(Alias + " coin " + Exec + " runtime/Coincidence.mac");

    // Aliases for GPS macros (e.g. setup source but do not run)
    UI->ApplyCommand(Alias + " standard " + Exec + " runtime/Sparrow.gps");
    UI->ApplyCommand(Alias + " beamPosZ " + Exec + " runtime/BeamPosZ.gps");
    UI->ApplyCommand(Alias + " beamNegZ " + Exec + " runtime/BeamNegZ.gps");
    UI->ApplyCommand(Alias + " monoIso " + Exec + " runtime/MonoIso.gps");
    UI->ApplyCommand(Alias + " constIso " + Exec + " runtime/ConstIso.gps");
    UI->ApplyCommand(Alias + " issProton " + Exec + " runtime/ISSAverageProton.gps");
    UI->ApplyCommand(Alias + " issElectron " + Exec + " runtime/ISSAverageElectron.gps");

    UI->ApplyCommand("/control/verbose 1");
    UI->ApplyCommand("/run/verbose 0");
    UI->ApplyCommand("/event/verbose 0");
    UI->ApplyCommand("/hits/verbose 0");
    UI->ApplyCommand("/tracking/verbose 0");
    
    UI->ApplyCommand("{standard}");
  }

  
  /////////////////////////
  // Parallel processing //
  /////////////////////////
  
  if(useParallelProcessing){
    
    G4String macroCmd = "/control/execute " + MPIMacroName;
    UI->ApplyCommand(macroCmd);
    
#ifdef MPI_ENABLED    
    delete theMPIManager;
#endif
    
  }
  
  
  ///////////////////////////
  // Sequential processing //
  ///////////////////////////
  else{
  
    G4VisManager *visManager = NULL;
    G4TrajectoryDrawByParticleID *colorModel = NULL;
    
    // Create a session with Qt vizualization
    if(useVisualization){
      visManager = new G4VisExecutive;
      visManager->Initialize();
      
      // Color particle by particle type
      colorModel = new G4TrajectoryDrawByParticleID;
      colorModel->Set("deuteron", "yellow");
      colorModel->Set("neutron", "cyan");
      colorModel->Set("gamma", "green");
      colorModel->Set("e-", "red");
      colorModel->Set("e+", "blue");
      colorModel->Set("opticalphoton","magenta");
      colorModel->SetDefault("gray");
      visManager->RegisterModel(colorModel);
      visManager->SelectTrajectoryModel(colorModel->Name());
    
      G4UIExecutive *theUIExecutive = new G4UIExecutive(argc, argv, "Qt");
    
      UI->ApplyCommand("/vis/open OGL");
      UI->ApplyCommand("/control/execute runtime/Sparrow.vis");
    
      theUIExecutive->SessionStart();

      delete theUIExecutive;
      delete colorModel;
    }
  
    // Create a tcsh-like terminal only session
    else{

      // Create a decent 'tcsh'-like prompt for tab completion, command
      // history, etc.  Also, style points for cooler prompt
      G4String prompt = "Sparrow >> ";
      G4int histories = 200;
      G4UItcsh *shell = new G4UItcsh(prompt, histories);
      G4UIsession* session = new G4UIterminal(shell);
    
      // As Gallagher famously said: "STYYYYYYYYYYYYYLE!!"
      G4cout << "\n\n \t ******************************************************************\n"
	     <<     " \t **********                                               *********\n"
	     <<     " \t ********   \"A sparrow has all the same working parts      ********\n"
	     <<     " \t ******          as an ostrich\" - Chinese proverb            ******\n"
	     <<     " \t ****                                                          ****\n"
	     <<     " \t **         WELCOME TO THE SPARROW DETECTOR SIMULATION           **\n"
	     <<     " \t *                                                                *\n"
	     <<     " \t ******************************************************************\n\n";
    
      session->SessionStart();
    
      ////////////////////////
      // Garbage collection //
      ////////////////////////
    
      delete session;
    }
  }
  
  if(theReadoutManager->CheckForOpenASIMFile())
    theReadoutManager->WriteASIMFile(true);
  delete theReadoutManager;
  
  delete theRunManager;
  
  G4cout << G4endl;
  
  return 0;
}
