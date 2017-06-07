#include "TrackManager.hh"

// SLIC
#include "LcioManager.hh"
#include "MCParticleManager.hh"
#include "StdHepGenerator.hh"

// LCDD
#include "lcdd/hits/TrackerHit.hh"

// LCIO
#include "EVENT/LCIO.h"
#include "EVENT/LCEvent.h"

#include <cassert>

using IMPL::LCCollectionVec;
using EVENT::LCCollection;
using EVENT::LCEvent;

namespace slic {

void TrackManager::saveTrackSummaries(const G4Event* anEvent, LCEvent* lcEvent) {

    /* If Geant4 event generation was used, then an empty MCParticle collection needs to be created. */
    G4bool geant4Generator = false;
    if (MCParticleManager::instance()->getMCParticleCollection() == NULL) {
        //std::cout << "creating empty MCParticle collection" << std::endl;
        MCParticleManager::instance()->createMCParticleCollection();
        geant4Generator = true;
    }

    /* Copy MCParticle pointers into the corresponding TrackSummary objects for primary particles. */
    PrimaryParticleMap* particleMap = MCParticleManager::instance()->getPrimaryParticleMap();
    for (PrimaryParticleMapIterator it = particleMap->begin(); it != particleMap->end(); it++) {
        if (it->second == 0) {
            G4Exception("", "", FatalException, "MCParticle was mapped to null G4PrimaryParticle.");
        }
	
	TrackSummary *primary = (*_trackSummaryMap)[it->second->GetTrackID()];
	if (primary)
	    primary->setMCParticle(dynamic_cast<MCParticleImpl*>(it->first));
    }

    G4bool writeCompleteEvent = LcioManager::instance()->getWriteCompleteEvent();

    // Set parents to be saved on particles that will be persisted.
    for (auto it = _trackSummaryMap->begin(); it != _trackSummaryMap->end(); it++)
	if (it->second && it->second->getToBeSaved())
		it->second->setParentToBeSaved();

    /* Get the MCParticle collection created by event generation. */
    LCCollectionVec* mcpVec = MCParticleManager::instance()->getMCParticleCollection();

    /* Save TrackSummary objects to LCIO collection. */
#ifdef SLIC_LOG
    log() << LOG::okay << "TrackManager processing " << _trackSummaryMap->size() << " TrackSummary objects." << LOG::done;
#endif

    TrackSummary* trackSummary;
    for (auto it = _trackSummaryMap->begin(); it != _trackSummaryMap->end(); it++) {
	if (!(trackSummary = it->second))
	    continue;

	/* Build the MCParticle in case it hasn't been created yet. */
	if (trackSummary->getMCParticle() == 0) {
	    trackSummary->buildMCParticle();
	}


	/* Associate the track ID to the MCParticle for the hit maker. */
	trackSummary->associateTrackIDs();

	/*
	 * Only particles created in the simulation need to be added, as the generator MCParticles
	 * associated with primaries were already.  But if a Geant4 internal generator was used
	 * such as the GPS or ParticleGun, then all particles with the save flag on should be added
	 * here, as they will all be flagged as sim particles and would not have been added yet.
	 */
	if(trackSummary->getMCParticle()->getGeneratorStatus() == 0 || geant4Generator) {
	    if (trackSummary->getMCParticle() != NULL) {
		mcpVec->push_back(trackSummary->getMCParticle());
		// DEBUG
		//if (trackSummary->getParentID() <= 0)
		//    G4cout << "WARNING: sim particle with track ID " << trackSummary->getTrackID() << " has no parent!" << G4endl;
	    }
	}
    }

    /* Set flag so it is saved. */
    mcpVec->setFlag(0xe0000000);

    /* Save the MCParticle collection into the event. */
    lcEvent->addCollection((LCCollection*)mcpVec, "MCParticle");

#ifdef SLIC_LOG
    log() << LOG::always << "Saved " << mcpVec->getNumberOfElements() << " MCParticles." << LOG::done;
#endif

}

TrackSummary* TrackManager::findFirstSavedAncestor(TrackSummary* trackSummary) {
    TrackSummary* parent = trackSummary->findParent();
    while (parent != 0) {
        if (parent->getToBeSaved() == true) {
            /* Associate the track ID to the first MCParticle up the tree that will be saved. */
            MCParticleManager::instance()->addMCParticleTrackID(
                    trackSummary->getTrackID(),
                    parent->getMCParticle());
            break;
        }
        parent = parent->findParent();
    }
    return parent;
}

}
