//
// ********************************************************************
// *                                                                  *
// * Copyright 2022 The TOPAS Collaboration                           *
// *                                                                  *
// * Permission is hereby granted, free of charge, to any person      *
// * obtaining a copy of this software and associated documentation   *
// * files (the "Software"), to deal in the Software without          *
// * restriction, including without limitation the rights to use,     *
// * copy, modify, merge, publish, distribute, sublicense, and/or     *
// * sell copies of the Software, and to permit persons to whom the   *
// * Software is furnished to do so, subject to the following         *
// * conditions:                                                      *
// *                                                                  *
// * The above copyright notice and this permission notice shall be   *
// * included in all copies or substantial portions of the Software.  *
// *                                                                  *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,  *
// * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES  *
// * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND         *
// * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT      *
// * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,     *
// * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
// * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR    *
// * OTHER DEALINGS IN THE SOFTWARE.                                  *
// *                                                                  *
// ********************************************************************
//

#include "TsScoreEffectiveCharge.hh"

#include "TsVGeometryComponent.hh"

TsScoreEffectiveCharge::TsScoreEffectiveCharge(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM,
										   G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer)
: TsVBinnedScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer)
{
	SetUnit("e+");
}


TsScoreEffectiveCharge::~TsScoreEffectiveCharge() {;}


G4bool TsScoreEffectiveCharge::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
	if (!fIsActive) {
		fSkippedWhileInactive++;
		return false;
	}

	G4bool status = false;

	// Add charge when particle stops
	if (aStep->GetPostStepPoint()->GetKineticEnergy() == 0) {
		   G4double CellCharge = aStep->GetPreStepPoint()->GetCharge();
		   CellCharge *= aStep->GetPreStepPoint()->GetWeight();
		   AccumulateHit(aStep, CellCharge);
		   status = true;
	   }

	// Subtract charge when particle is generated, except for primaries
	if (aStep->GetTrack()->GetCurrentStepNumber() == 1 && aStep->GetTrack()->GetParentID() != 0) {
		G4double CellCharge = aStep->GetPreStepPoint()->GetCharge();
		CellCharge *= aStep->GetPreStepPoint()->GetWeight();
		CellCharge *= -1.0;
		AccumulateHit(aStep, CellCharge);
		status = true;
	}

	return status;
}
