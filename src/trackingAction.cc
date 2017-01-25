#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

#include "trackingAction.hh"


trackingAction::trackingAction()
  : OutputVertexInfo(false),
    VertexKE(0.)
{
  if(OutputVertexInfo){
    VertexFile = new TFile("VertexInfo.root", "recreate");

    VertexTree = new TTree("VertexTree", "Tree for event vertex information");
    VertexTree->Branch("VertexKE", &VertexKE);
  }
}


trackingAction::~trackingAction()
{
  if(OutputVertexInfo){
    VertexTree->Write();
    VertexFile->Close();

    delete VertexFile;
  }
}

void trackingAction::PreUserTrackingAction(const G4Track* currentTrack)
{
  if(OutputVertexInfo and currentTrack->GetTrackID() == 1){
    VertexKE = currentTrack->GetVertexKineticEnergy()/MeV;
    VertexTree->Fill();
  }
}


void trackingAction::PostUserTrackingAction(const G4Track *)
{;}
