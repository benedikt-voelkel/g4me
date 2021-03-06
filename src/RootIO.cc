/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "RootIO.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
#include "TFile.h"
#include "TTree.h"
#include "PrimaryParticleInformation.hh"

namespace G4me {

RootIO *RootIO::mInstance = nullptr;

/*****************************************************************/

void
RootIO::InitMessenger()
{
  mDirectory = new G4UIdirectory("/io/");

  mFileNameCmd = new G4UIcmdWithAString("/io/prefix", this);
  mFileNameCmd->SetGuidance("Output file prefix.");
  mFileNameCmd->SetParameterName("prefix", false);
  mFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  mSaveParticlesCmd = new G4UIcmdWithABool("/io/saveParticles", this);
  mSaveParticlesCmd->SetGuidance("Save the generator particles.");
  mSaveParticlesCmd->SetParameterName("prefix", false);
  mSaveParticlesCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
};

/*****************************************************************/
  
void
RootIO::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mFileNameCmd)
    mFilePrefix = value;
  if (command == mSaveParticlesCmd)
    mSaveParticles = mSaveParticlesCmd->GetNewBoolValue(value);
}

/*****************************************************************/
  
void
RootIO::BeginOfRunAction(const G4Run *aRun)
{
  std::string filename = Form("%s.%03d.root", mFilePrefix.c_str(), aRun->GetRunID());
  Open(filename);
  ResetHits();
  ResetTracks();
  ResetParticles();
}

/*****************************************************************/

void
RootIO::EndOfRunAction(const G4Run *aRun)
{
  Close();
}

/*****************************************************************/

void
RootIO::BeginOfEventAction(const G4Event *aEvent)
{
}

/*****************************************************************/

void
RootIO::EndOfEventAction(const G4Event *aEvent)
{
  FillHits();
  FillTracks();
  FillParticles();
  ResetHits();
  ResetTracks();
  ResetParticles();
}

/*****************************************************************/

void
RootIO::Open(std::string filename) {
  
  mFile = TFile::Open(filename.c_str(), "RECREATE");
  
  mTreeHits = new TTree("Hits", "RootIO tree");
  mTreeHits->Branch("n"      , &mHits.n      , "n/I");
  mTreeHits->Branch("trkid"  , &mHits.trkid  , "trkid[n]/I");
  mTreeHits->Branch("trklen" , &mHits.trklen , "trklen[n]/F");
  mTreeHits->Branch("edep"   , &mHits.edep   , "edep[n]/F");
  mTreeHits->Branch("x"      , &mHits.x      , "x[n]/F");
  mTreeHits->Branch("y"      , &mHits.y      , "y[n]/F");
  mTreeHits->Branch("z"      , &mHits.z      , "z[n]/F");
  mTreeHits->Branch("t"      , &mHits.t      , "t[n]/F");
  mTreeHits->Branch("lyrid"  , &mHits.lyrid  , "lyrid[n]/I");
  
  mTreeTracks = new TTree("Tracks", "RootIO tree");
  mTreeTracks->Branch("n"        , &mTracks.n        , "n/I");
  mTreeTracks->Branch("proc"     , &mTracks.proc     , "proc[n]/B");
  mTreeTracks->Branch("sproc"    , &mTracks.sproc    , "sproc[n]/B");
  mTreeTracks->Branch("status"   , &mTracks.status   , "status[n]/I");
  mTreeTracks->Branch("parent"   , &mTracks.parent   , "parent[n]/I");
  mTreeTracks->Branch("particle" , &mTracks.particle , "particle[n]/I");
  mTreeTracks->Branch("pdg"      , &mTracks.pdg      , "pdg[n]/I");
  mTreeTracks->Branch("vt"       , &mTracks.vt       , "vt[n]/D");
  mTreeTracks->Branch("vx"       , &mTracks.vx       , "vx[n]/D");
  mTreeTracks->Branch("vy"       , &mTracks.vy       , "vy[n]/D");
  mTreeTracks->Branch("vz"       , &mTracks.vz       , "vz[n]/D");
  mTreeTracks->Branch("e"        , &mTracks.e        , "e[n]/D");
  mTreeTracks->Branch("px"       , &mTracks.px       , "px[n]/D");
  mTreeTracks->Branch("py"       , &mTracks.py       , "py[n]/D");
  mTreeTracks->Branch("pz"       , &mTracks.pz       , "pz[n]/D");

  if (mSaveParticles) {
    mTreeParticles = new TTree("Particles", "RootIO tree");
    mTreeParticles->Branch("n"      , &mParticles.n      , "n/I");
    mTreeParticles->Branch("parent" , &mParticles.parent , "parent[n]/I");
    mTreeParticles->Branch("pdg"    , &mParticles.pdg    , "pdg[n]/I");
    mTreeParticles->Branch("vt"     , &mParticles.vt     , "vt[n]/D");
    mTreeParticles->Branch("vx"     , &mParticles.vx     , "vx[n]/D");
    mTreeParticles->Branch("vy"     , &mParticles.vy     , "vy[n]/D");
    mTreeParticles->Branch("vz"     , &mParticles.vz     , "vz[n]/D");
    mTreeParticles->Branch("e"      , &mParticles.e      , "e[n]/D");
    mTreeParticles->Branch("px"     , &mParticles.px     , "px[n]/D");
    mTreeParticles->Branch("py"     , &mParticles.py     , "py[n]/D");
    mTreeParticles->Branch("pz"     , &mParticles.pz     , "pz[n]/D");
  }
    
};

/*****************************************************************/

void
RootIO::Close()
{
  mFile->cd();
  mTreeHits->Write();
  mTreeTracks->Write();
  if (mSaveParticles) mTreeParticles->Write();
  mFile->Close();
}

/*****************************************************************/

void
RootIO::ResetTracks()
{
  mTracks.n = 0;
}

/*****************************************************************/

void
RootIO::FillTracks()
{
  mTreeTracks->Fill();
}

/*****************************************************************/

void
RootIO::AddTrack(const G4Track *aTrack) {
  auto id = aTrack->GetTrackID() - 1;
  if (mTracks.n != id) {
    std::cout << "--- oh dear, this can lead to hard times later: " << mTracks.n << " " << aTrack->GetTrackID() << std::endl;
  }
  int particleIndex = -1;
  if (aTrack->GetDynamicParticle()->GetPrimaryParticle()) { // this is a primary particle or a preassigned decay product
    auto info = dynamic_cast<PrimaryParticleInformation *>(aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());
    if (info) particleIndex = info->GetIndex();
  }
  mTracks.proc[id]     = aTrack->GetCreatorProcess() ? aTrack->GetCreatorProcess()->GetProcessType() : -1;
  mTracks.sproc[id]    = aTrack->GetCreatorProcess() ? aTrack->GetCreatorProcess()->GetProcessSubType() : -1;
  mTracks.status[id]   = 0;
  mTracks.parent[id]   = aTrack->GetParentID() - 1;
  mTracks.particle[id] = particleIndex;
  mTracks.pdg[id]      = aTrack->GetParticleDefinition()->GetPDGEncoding();
  mTracks.vt[id]       = aTrack->GetGlobalTime() / ns;
  mTracks.vx[id]       = aTrack->GetPosition().x()  / cm;
  mTracks.vy[id]       = aTrack->GetPosition().y()  / cm;
  mTracks.vz[id]       = aTrack->GetPosition().z()  / cm;
  mTracks.e[id]        = aTrack->GetTotalEnergy()   / GeV;
  mTracks.px[id]       = aTrack->GetMomentum().x()  / GeV;
  mTracks.py[id]       = aTrack->GetMomentum().y()  / GeV;
  mTracks.pz[id]       = aTrack->GetMomentum().z()  / GeV;
  mTracks.n++;
}

/*****************************************************************/

void
RootIO::AddStatus(const G4Track *aTrack, ETrackStatus_t status) {
  auto id = aTrack->GetTrackID() - 1;
  mTracks.status[id] |= status;
}

/*****************************************************************/

void
RootIO::ResetHits()
{
  mHits.n = 0;
}

/*****************************************************************/

void
RootIO::FillHits()
{
  mTreeHits->Fill();
}

/*****************************************************************/

void
RootIO::AddHit(const G4Step *aStep)
{
  auto point = aStep->GetPreStepPoint();
  auto track = aStep->GetTrack();

  mHits.trkid[mHits.n]  = track->GetTrackID() - 1;
  mHits.trklen[mHits.n] = track->GetTrackLength()  / cm;
  mHits.x[mHits.n]      = track->GetPosition().x() / cm;
  mHits.y[mHits.n]      = track->GetPosition().y() / cm;
  mHits.z[mHits.n]      = track->GetPosition().z() / cm;
  mHits.t[mHits.n]      = track->GetGlobalTime()   / ns;
  mHits.lyrid[mHits.n]  = point->GetTouchableHandle()->GetCopyNumber();
  mHits.n++;
}

/*****************************************************************/

void
RootIO::ResetParticles()
{
  if (!mSaveParticles) return;
  mParticles.n = 0;
}

/*****************************************************************/

void
RootIO::FillParticles()
{
  if (!mSaveParticles) return;
  mTreeParticles->Fill();
}

/*****************************************************************/

void
RootIO::AddParticle(int id, int pdg, int parent,
		    double px, double py, double pz, double et,
		    double vx, double vy, double vz, double vt)
{
  if (!mSaveParticles) return;
  
  if (mParticles.n != id) {
    std::cout << "--- oh dear, this can lead to hard times later: " << mParticles.n << " " << id << std::endl;
  }
  mParticles.parent[id] = parent;
  mParticles.pdg[id]    = pdg;
  mParticles.vt[id]     = vt;
  mParticles.vx[id]     = vx;
  mParticles.vy[id]     = vx;
  mParticles.vz[id]     = vz;
  mParticles.e[id]      = et;
  mParticles.px[id]     = px;
  mParticles.py[id]     = py;
  mParticles.pz[id]     = pz;
  mParticles.n++;
}

/*****************************************************************/

} /** namespace G4me **/
