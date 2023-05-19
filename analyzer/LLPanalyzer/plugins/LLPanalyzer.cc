// -*- C++ -*-
//
// Package:    analyzer/LLPanalyzer
// Class:      LLPanalyzer
//
/**\class LLPanalyzer LLPanalyzer.cc analyzer/LLPanalyzer/plugins/LLPanalyzer.cc

 Description: recasting LLP HNL analysis with trilepton final states: https://cms.cern.ch/iCMS/analysisadmin/cadilines?line=EXO-20-009&tp=an&id=2360&ancode=EXO-20-009

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Kyungmin Park
//         Created:  Wed, 17 May 2023 23:30:53 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/Utilities/interface/RegexMatch.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/IsolatedTrack.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


//using reco::TrackCollection;
using pat::ElectronCollection;
using pat::ElectronRef;

class LLPanalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit LLPanalyzer(const edm::ParameterSet&);
      ~LLPanalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
      int nEvents;
      int hasElec;
      int hasPromptElec;
      int hasDisplacedElec;
      int passTrig;
      int passTwoDisplacedLep;
      int passDR;
      std::vector<ElectronRef> vPromptElec_;
      std::vector<ElectronRef> vDisplacedElec_;

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<ElectronCollection> electronsToken_;  //used to select what tracks to read from configuration file
      edm::EDGetTokenT<edm::TriggerResults> triggerToken_;  //used to select what tracks to read from configuration file
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
LLPanalyzer::LLPanalyzer(const edm::ParameterSet& iConfig)
 :
  electronsToken_(consumes<ElectronCollection>(iConfig.getUntrackedParameter<edm::InputTag>("electrons"))),
  triggerToken_(consumes<edm::TriggerResults>(iConfig.getUntrackedParameter<edm::InputTag>("trgResults")))
{
   //now do what ever initialization is needed
    nEvents = 0;
    hasElec = 0;
    hasPromptElec = 0;
    hasDisplacedElec = 0;
    passTrig = 0;
    passTwoDisplacedLep = 0;
    passDR = 0;
}


LLPanalyzer::~LLPanalyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

bool isTightDisplaced(ElectronRef iElec) {
    
    // Tight Displaced Requirements
    if (std::abs(iElec->eta()) >= 2.5) return false;
    if (iElec->pt() <= 7) return false; 

    auto trk = iElec->gsfTrack();  // track associated with the electron 

    if (trk.isNull() || std::abs(trk->dxy()) <= 0.01) return false;
    
    // I_rel
    if (std::abs(iElec->dr03EcalRecHitSumEt()/iElec->p4().Pt()) >= 0.2) return false;

    // shower shape variables for ID
    if (iElec->full5x5_sigmaIetaIeta() >= 0.011) return false;
    if (iElec->hadronicOverEm() >= 0.10) return false;

    // track-cluster matching
    if (iElec->deltaEtaSuperClusterTrackAtVtx() >= 0.01) return false;
    if (iElec->deltaPhiSuperClusterTrackAtVtx() >= 0.04) return false;

    // 1/E - 1/p
    double elecE = iElec->ecalEnergy();
    double elecP = elecE / iElec->eSuperClusterOverP();
    if (std::abs(1/elecE - 1/elecP) >= 0.010) return false;

    return true;
}

bool isTightPrompt(ElectronRef iElec) {
    
    // Tight Prompt Requirements
    if (std::abs(iElec->eta()) >= 2.5) return false;
    if (iElec->pt() <= 30) return false; 

    auto trk = iElec->gsfTrack();  // track associated with the electron 

    if (trk.isNull() || std::abs(trk->dxy()) >= 0.05) return false;
    if (trk.isNull() || std::abs(trk->dz()) >= 0.1) return false;
    
    // I_rel
    if (std::abs(iElec->dr03EcalRecHitSumEt()/iElec->p4().Pt()) >= 0.1) return false;

    // MVA estimator

    return true;
}




// ------------ method called for each event  ------------
void
LLPanalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    nEvents++;

    /*
    * Trigger
    */

    Handle<edm::TriggerResults> trgs;
    iEvent.getByToken( triggerToken_, trgs );

    const TriggerNames &triggerNames = iEvent.triggerNames( *trgs );
    //std::cout << "N(triggers) = " << trgs->size() << std::endl;
    //for ( unsigned int iT = 0; iT < trgs->size(); iT++ ) {
    //    std::cout << " name["<<iT<<"]:"<<triggerNames.triggerName(iT)<< std::endl;
    //}

    std::string trgName = "HLT_Ele27_WPTight_Gsf*";
    std::vector< std::vector<std::string>::const_iterator > trgMatches = regexMatch( triggerNames.triggerNames(), trgName );
    
    //std::cout << "Number of trigger matches = " << trgMatches.size() << std::endl;

    if ( !trgMatches.empty() ) {
        for ( auto const& iT : trgMatches ) {
            //std::cout << " name["<<triggerNames.triggerIndex(*iT)<<"]:"<< *iT << " -> " << trgs->accept(triggerNames.triggerIndex(*iT)) << std::endl;
            if ( trgs->accept(triggerNames.triggerIndex(*iT)) ) passTrig++;
            break;
        }
    } 

    /* 
    * Electron cuts
    */
    Handle<ElectronCollection> electrons;
    iEvent.getByToken(electronsToken_, electrons);

    int nElectrons = 0;
    int nElec = 0;
    int nAntiElec = 0;

    int nPromptElectrons = 0;
    int nPromptElec = 0;
    int nPromptAntiElec = 0;

    int nDisplacedElectrons = 0;
    int nDisplacedElec = 0;
    int nDisplacedAntiElec = 0;

    vPromptElec_.clear();
    vDisplacedElec_.clear();


    for ( unsigned int iE = 0; iE < electrons->size(); iE++) {
        ElectronRef iElec( electrons, iE );
        nElectrons++;

        if (iElec->charge() > 0) nAntiElec++;
        else nElec++;

        if (isTightDisplaced(iElec)) {
            nDisplacedElectrons++;
            if (iElec->charge() > 0) nDisplacedAntiElec++;
            else nDisplacedElec++; 
            vDisplacedElec_.push_back(iElec);
        }

        else if (isTightPrompt(iElec)) {
            nPromptElectrons++;
            if (iElec->charge() > 0) nPromptAntiElec++;
            else nPromptElec++;
            vPromptElec_.push_back(iElec);
        }
    }

    if (nElectrons > 0) hasElec++;
    if (nDisplacedElectrons > 0) hasDisplacedElec++;
    if (nPromptElectrons > 0) hasPromptElec++;

    if (vDisplacedElec_.size() == 2) {
        passTwoDisplacedLep++;

        // sort by pT
        std::sort( vDisplacedElec_.begin(), vDisplacedElec_.end(), [](auto a, auto b) { return a->pt() > b->pt(); } );

        //std::cout << "Leading e" << std::endl;
        //std::cout << "pT = " << vDisplacedElec_[0]->pt() << std::endl;
        //std::cout << "Sub-Leading e" << std::endl;
        //std::cout << "pT = " << vDisplacedElec_[1]->pt() << std::endl;

        double dR = 0.;
        
        double dEta = vDisplacedElec_[0]->eta() - vDisplacedElec_[1]->eta();
        double dPhi = vDisplacedElec_[0]->phi() - vDisplacedElec_[1]->phi();

        dR = std::sqrt( dEta*dEta + dPhi*dPhi );
        //std::cout << "deltaR = " << dR << std::endl;

        if (dR < 1.) passDR++;
    }

#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void
LLPanalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
LLPanalyzer::endJob()
{
    std::cout << "Events Analyzed = " << nEvents << std::endl;
    //std::cout << "Events with Electrons = " << hasElec << std::endl;
    //std::cout << "Events with Tight Displaced Electrons = " << hasDisplacedElec << std::endl;
    //std::cout << "Events with Tight Prompt Electrons = " << hasPromptElec << std::endl;

    std::cout << "Events passing HLT Single Electron trigger = " << passTrig << std::endl; 
    std::cout << "Events with Two Displaced Electrons = " << passTwoDisplacedLep << std::endl;
    std::cout << "Events with dR(l2, l3) < 1 = " << passDR << std::endl;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
LLPanalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(LLPanalyzer);
