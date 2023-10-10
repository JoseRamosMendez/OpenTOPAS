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

#ifndef TsScoreDoseToMaterial_hh
#define TsScoreDoseToMaterial_hh

#include "TsVBinnedScorer.hh"

#include "G4EmCalculator.hh"

class G4Material;

class TsScoreDoseToMaterial : public TsVBinnedScorer
{
public:
	TsScoreDoseToMaterial(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM,
					   G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer);

	virtual ~TsScoreDoseToMaterial();

	void UpdateForSpecificParameterChange(G4String parameter);

	G4bool ProcessHits(G4Step*,G4TouchableHistory*);

private:
	G4EmCalculator fEmCalculator;
	G4Material* fReferenceMaterial;

	G4ParticleDefinition* fProtonSubstituteForNeutrals;
	G4double fSubstituteForNeutralsEnergy;

	G4bool fPreCalculateStoppingPowerRatios;
	G4double fProtonEnergyBinSize;
	G4int fProtonEnergyNBins;
	G4double fElectronEnergyBinSize;
	G4int fElectronEnergyNBins;
	std::vector<std::vector<G4double> > fProtonStoppingPowerRatio;
	std::vector<std::vector<G4double> > fElectronStoppingPowerRatio;
	std::vector<G4double> fOtherStoppingPowerRatio;
	G4double fMinProtonEnergyForRatio;
	G4double fMaxProtonEnergyForRatio;
	G4double fMinElectronEnergyForRatio;
	G4double fMaxElectronEnergyForRatio;
};
#endif
