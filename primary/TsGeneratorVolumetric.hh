//
// ********************************************************************
// *                                                                  *
// * Copyright 2024 The OpenTOPAS Collaboration                       *
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

#ifndef TsGeneratorVolumetric_hh
#define TsGeneratorVolumetric_hh

#include "TsVGenerator.hh"

#include "G4VPhysicalVolume.hh"

class G4Navigator;
class G4Material;

class TsGeneratorVolumetric : public TsVGenerator
{
public:
	TsGeneratorVolumetric(TsParameterManager* pM, TsGeometryManager* gM, TsGeneratorManager* pgM, G4String sourceName);
	~TsGeneratorVolumetric();

	void ResolveParameters();
	void UpdateForNewRun(G4bool rebuiltSomeComponents);

	void GeneratePrimaries(G4Event* );
	
private:
	void CalculateExtent();
	
	G4bool fRecursivelyIncludeChildren;
	G4Navigator* fNavigator;
	G4Material* fActiveMaterial;
	G4int fMaxNumberOfPointsToSample;
	
	std::vector<G4VPhysicalVolume*> fVolumes;

	G4bool fNeedToCalculateExtent;

	G4double fXMin;
	G4double fXMax;
	G4double fYMin;
	G4double fYMax;
	G4double fZMin;
	G4double fZMax;
};

#endif
