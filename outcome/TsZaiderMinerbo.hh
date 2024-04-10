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

#ifndef TsZaiderMinerbo_hh
#define TsZaiderMinerbo_hh

#include "TsVOutcomeModel.hh"

#include <vector>

class TsParameterManager;

class TsZaiderMinerbo : public TsVOutcomeModel
{
public:
	TsZaiderMinerbo(TsParameterManager* pM, G4String parName);
	~TsZaiderMinerbo();

	G4double Initialize(std::vector<G4double> dose, std::vector<G4double> volume);
	G4double LQPredictionAfterFraction(G4int fraction, G4double dose_i);

private:
	TsParameterManager* fPm;
	G4String fModelName;
	G4double fAlpha;
	G4double fBeta;
	G4double fLambda;
	G4int    fNbOfFractions;
	G4double* fTimek;

};
#endif
