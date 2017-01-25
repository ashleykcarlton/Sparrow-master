#ifndef trackingAction_hh
#define trackingAction_hh 1

#include "G4UserTrackingAction.hh"

#include "TFile.h"
#include "TTree.h"


class trackingAction : public G4UserTrackingAction
{

public:
  trackingAction();
  ~trackingAction();

  void PreUserTrackingAction(const G4Track *);
  void PostUserTrackingAction(const G4Track *);

private:
  TFile *VertexFile;
  TTree *VertexTree;
  G4bool OutputVertexInfo;

  G4double VertexKE;
};

#endif
