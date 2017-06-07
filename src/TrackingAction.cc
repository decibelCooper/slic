#include "TrackingAction.hh"

// LCDD
#include "lcdd/detectors/CurrentTrackState.hh"

// SLIC
#include "TrackManager.hh"
#include "TrackUtil.hh"
#include "UserTrackInformation.hh"

// Geant4
#include "G4TransportationManager.hh"

namespace slic {

void TrackingAction::PreUserTrackingAction(const G4Track* track) {

    G4int trackID = track->GetTrackID();
    G4int parentID = track->GetParentID();

    G4bool isTruePrimary = (track->GetParentID() == 0);
    UserRegionInformation* regionInfo = TrackUtil::getRegionInformation(track);
    G4bool insideTrackerRegion = regionInfo->getStoreSecondaries();
    UserTrackInformation* trackInfo = TrackUtil::getUserTrackInformation(track);
    bool hasTrackInfo = (trackInfo != NULL);
    bool aboveEnergyThreshold = (track->GetKineticEnergy() > regionInfo->getThreshold());

    // This happens when backscattering track are un-suspended.
    if (hasTrackInfo) {
        // Track is backscattering?
        if (trackInfo->getTrackSummary()->getBackScattering()) {
            // Update current track ID for calorimeter SDs and return.
            CurrentTrackState::setCurrentTrackID(trackInfo->getTrackSummary()->getTrackID());
            return;
        }
        return;
    } else
	trackInfo = TrackUtil::setupUserTrackInformation(track, false);

    // Primary OR in tracker region and above energy threshold?
    if (isTruePrimary || (insideTrackerRegion && aboveEnergyThreshold)) {

        // Update track ID for calorimeter SDs.
        CurrentTrackState::setCurrentTrackID(trackInfo->getTrackSummary()->getTrackID());

        // Turn on trajectory storage.
        fpTrackingManager->SetStoreTrajectory(true);

	// Save it.
	trackInfo->getTrackSummary()->setToBeSaved();

    } else {
        // Inside tracker region and below energy threshold?
        if (!insideTrackerRegion) {
            // Track is outside tracking region so turn off trajectory storage.
            // These are generally tracks from calorimeter showers.
            fpTrackingManager->SetStoreTrajectory(false);
        }
    }

    m_pluginManager->preTracking(track);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {

    // Get the track information and summary.
    UserTrackInformation* trackInfo = TrackUtil::getUserTrackInformation(track);
    TrackSummary *trackSummary = trackInfo->getTrackSummary();

    G4int nSecondaries = fpTrackingManager->GimmeSecondaries()->size();
    if (nSecondaries == 0 /*trackSummary->getNSecondaries() == 0*/ && !trackSummary->getToBeSaved()) {
	TrackSummary *parent = trackSummary->findParent();
	if (parent) {
	    parent->addAssociatedTrackID(trackSummary->getTrackID());
	    parent->incrementUnsavedSecondaries();

	    TrackManager::instance()->removeTrackSummary(trackSummary->getTrackID());
	    delete trackSummary;
	    trackSummary = NULL;
	} else
	    std::cout << "No summary for " << track->GetParentID() << " in PostUserTrackingAction()" << std::endl;
    } else {
	trackSummary->setNSecondaries(nSecondaries);

	// Update the track summary.
	trackSummary->update(track);
    }

    m_pluginManager->postTracking(track);
}

} // namespace slic
