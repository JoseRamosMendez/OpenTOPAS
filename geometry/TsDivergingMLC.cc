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

#include "TsDivergingMLC.hh"

#include "TsParameterManager.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4UIcommand.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trap.hh"
#include "G4GenericTrap.hh"

DivergingMLC::DivergingMLC(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
								 TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name)
	: TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume, name)
{;}

DivergingMLC::~DivergingMLC()
{;}

void DivergingMLC::UpdateForSpecificParameterChange(G4String parameter)
{
	if (parameter == GetFullParmNameLower("NegativeFieldSetting") || parameter == GetFullParmNameLower("PositiveFieldSetting")) {
		G4double* xPlusOpenNew  = fPm->GetDoubleVector(GetFullParmName("PositiveFieldSetting"), "Length");
		G4double* xMinusOpenNew = fPm->GetDoubleVector(GetFullParmName("NegativeFieldSetting"), "Length");
		G4double* leaf_widths   = fPm->GetDoubleVector(GetFullParmName("LeafWidths"), "Length");

		G4double CurrentWidth = -fHalfTotalMLCWidth;
		for (int i= 0; i < fNbOfLeavesPerSide; i++) {
			G4double posOpen = xPlusOpenNew[i];
			G4double negOpen = xMinusOpenNew[i];
			G4double widht   = leaf_widths[i];

			 //Check if any of the leaves changed position
			if ((posOpen != fXPlusLeavesOpen[i]) || (negOpen != fXMinusLeavesOpen[i])) {
				if ( (posOpen - negOpen) < 0.0 ) {
					std::cerr << "Detected Leaf collision at "<< i <<" th leaf: XPlusLeaf "
					<< posOpen/cm << " (cm), XMinusLeaf "
					<< negOpen/cm << " (cm) "<< std::endl;
					fPm->AbortSession(1);
				}

				G4ThreeVector xpl = G4ThreeVector((fLeafHalfLength + posOpen)/fMagnification,    (CurrentWidth + (widht*0.5))/fMagnification, 0);
				G4ThreeVector xml = G4ThreeVector((-(fLeafHalfLength) + negOpen)/fMagnification, (CurrentWidth + (widht*0.5))/fMagnification, 0);

				// Construct the new Leaves
				std::pair<G4GenericTrap*,G4GenericTrap*> Leaves = ConstructLeafPair(i, fLeafThickness,posOpen,negOpen,widht,CurrentWidth);


				*(fGenericTrapXPlusLeaves[i])  = *(Leaves.first);
				*(fGenericTrapXMinusLeaves[i]) = *(Leaves.second);

				(fPhysicalXPlusLeaves[i])->SetTranslation(xpl);
				(fPhysicalXMinusLeaves[i])->SetTranslation(xml);

				fXPlusLeavesOpen[i]  = posOpen;
				fXMinusLeavesOpen[i] = negOpen;
			}
			CurrentWidth += widht; 
		}
		AddToReoptimizeList(fEnvelopeLog);

	} else {
		// For any other parameters, fall back to the base class Update method
		TsVGeometryComponent::UpdateForSpecificParameterChange(parameter);
	}
}


G4VPhysicalVolume* DivergingMLC::Construct()
{
	BeginConstruction();
	fPhysicalXPlusLeaves.clear();
	fPhysicalXMinusLeaves.clear();
	fLogicXPlusLeaves.clear();
	fLogicXMinusLeaves.clear();
	fGenericTrapXPlusLeaves.clear();
	fGenericTrapXMinusLeaves.clear();

	//Count leaves
	fNbOfLeavesPerSide = fPm->GetVectorLength(GetFullParmName("LeafWidths"));
	const G4int n_xl  = fPm->GetVectorLength(GetFullParmName("PositiveFieldSetting"));
	const G4int n_xr  = fPm->GetVectorLength(GetFullParmName("NegativeFieldSetting"));
	if (fNbOfLeavesPerSide != n_xl) {
		std::cerr << "Number of Width does not Match to XPlusLeaves: Widths = " << fNbOfLeavesPerSide << ", XPlusLeavesOpen = "<< n_xl << std::endl;
		fPm->AbortSession(1);
	} else {
		if (fNbOfLeavesPerSide != n_xr) {
				std::cerr << "Number of Width does not Match to XMinusLeaves: Widths = " << fNbOfLeavesPerSide << ", XPlusLeavesOpen = "<< n_xr << std::endl;
				fPm->AbortSession(1);
		}
	}

	//LeafHalfLength: x of leaf, leaf_thickness: z of leaf, and vt: various y of leaf
	fLeafHalfLength = (0.5)*fPm->GetDoubleParameter(GetFullParmName("Length"), "Length"); //x
	fLeafThickness  = fPm->GetDoubleParameter(GetFullParmName("Thickness"), "Length");    //z
	G4double* leaf_widths   = fPm->GetDoubleVector(GetFullParmName("LeafWidths"), "Length");      //y
	if ( fPm->ParameterExists(GetFullParmName("MaximumLeafOpen")) ) {
		fMaximumLeafOpen = fPm->GetDoubleParameter(GetFullParmName("MaximumLeafOpen"), "Length");
	} 
	else {
		fMaximumLeafOpen = 6.0*fLeafHalfLength;
	}

	fXPlusLeavesOpen     = fPm->GetDoubleVector(GetFullParmName("PositiveFieldSetting"), "Length");
	fXMinusLeavesOpen    = fPm->GetDoubleVector(GetFullParmName("NegativeFieldSetting"), "Length");
	fSAD                 = fPm->GetDoubleParameter(GetFullParmName("SAD"), "Length");
	fSUSD                = fPm->GetDoubleParameter(GetFullParmName("SourceToUpstreamSurfaceDistance"), "Length");
	fMagnification       = fSAD/fSUSD;
	fLeafThickness  = fPm->GetDoubleParameter(GetFullParmName("Thickness"), "Length") * fMagnification;    //z
	G4String Travelangle = fPm->GetStringParameter(GetFullParmName("LeafTravelAxis"));
	G4String TransZName  = "dc:Ge/" + fName + "/TransZ";
	G4double DistanceSourceToMLCcenter = fSUSD + (fLeafThickness * 0.5/fMagnification);
	G4double TransZVal = (fSAD - DistanceSourceToMLCcenter)/mm;
	G4String TransZValString = G4UIcommand::ConvertToString(TransZVal);
	G4String transValue = "0. deg";

	if (Travelangle == "Yb") {
		transValue = "270. deg";
	}
	else if (Travelangle == "Xb") {
		transValue = "0. deg";
	}
	else {
		std::cerr << "LeafTravelAxis need to be either Xb or Yb. LeafTravelAxis = " << Travelangle << std::endl;
		fPm->AbortSession(1); 	
	}
	fPm->AddParameter("dc:Ge/" + fName + "/RotZ", transValue);
	fPm->AddParameter(TransZName, TransZValString + " mm");
	if (fSAD < 0) {
		std::cerr << "SAD must be positive. SAD = " << fSAD << std::endl;
		fPm->AbortSession(1); 										// Need to be modified to return false
	}
	//test
	G4double TotalMLCWidth = 0.0;
	for (G4int i = 0; i < fNbOfLeavesPerSide; i++) {
		TotalMLCWidth += leaf_widths[i];
		//Prevent leaf opening over its limit.
		if ( fabs(fXPlusLeavesOpen[i]) > fMaximumLeafOpen || fabs(fXMinusLeavesOpen[i]) > fMaximumLeafOpen) {
				std::cerr << i << " th Leaf tried to open more than the limit: " << fMaximumLeafOpen/cm << " (cm)" << std::endl;
				fPm->AbortSession(1);
		}
		//Leaf collision detection.
		if ( (fXPlusLeavesOpen[i] - fXMinusLeavesOpen[i]) < 0.0 ) {
				std::cerr << "Detected Leaf collision at "<< i <<" th leaf: XPlusLeaf "
				<< fXPlusLeavesOpen[i]/cm << " (cm), XMinusLeaf "
			 	<< fXMinusLeavesOpen[i]/cm << " (cm) "<< std::endl;
				fPm->AbortSession(1);
		}
	}

	G4String envelopeMaterialName = fParentComponent->GetResolvedMaterialName();
	G4Box* svWholeBox = new G4Box(fName, fLeafHalfLength + fMaximumLeafOpen, TotalMLCWidth/2, 0.5 * fLeafThickness / fMagnification);
	fEnvelopeLog  = CreateLogicalVolume(fName, envelopeMaterialName, svWholeBox);
	fEnvelopePhys = CreatePhysicalVolume(fEnvelopeLog);

	fHalfTotalMLCWidth = TotalMLCWidth/2;

	G4double CurrentWidth = -fHalfTotalMLCWidth;
	for (G4int i = 0; i < fNbOfLeavesPerSide; i++) {
		G4String id_string = G4UIcommand::ConvertToString(i);
		G4double xPosOpen  = fXPlusLeavesOpen[i];
		G4double xNegOpen  = fXMinusLeavesOpen[i];
		G4double leafWidth = leaf_widths[i];
	
		std::pair<G4GenericTrap*,G4GenericTrap*> Leaves = ConstructLeafPair(i,fLeafThickness,xPosOpen,xNegOpen,leafWidth, CurrentWidth);

		G4String volName                = "X+Leaf"+id_string;
		G4GenericTrap* positivesvLeaf   = Leaves.first;
		G4LogicalVolume* positivelLeaf  = CreateLogicalVolume(positivesvLeaf);
		G4ThreeVector* threeVecPlus     = new G4ThreeVector(((fLeafHalfLength) + xPosOpen)/fMagnification, (CurrentWidth + (leafWidth*0.5))/fMagnification, 0);
		G4VPhysicalVolume* pPlusLeaf    = CreatePhysicalVolume(volName, positivelLeaf, 0, threeVecPlus, fEnvelopePhys);

		volName                         = "X-Leaf"+id_string;
		G4GenericTrap* negativesvLeaf   = Leaves.second;
		G4LogicalVolume* negativelLeaf  = CreateLogicalVolume(negativesvLeaf);
		G4ThreeVector* threeVecMinus    = new G4ThreeVector((-(fLeafHalfLength) + xNegOpen)/fMagnification, (CurrentWidth + (leafWidth*0.5))/fMagnification, 0);
		G4VPhysicalVolume* pMinusLeaf   = CreatePhysicalVolume(volName, negativelLeaf, 0, threeVecMinus, fEnvelopePhys);
		
		fPhysicalXPlusLeaves.push_back(pPlusLeaf);
		fPhysicalXMinusLeaves.push_back(pMinusLeaf);
		fLogicXPlusLeaves.push_back(positivelLeaf);
		fLogicXMinusLeaves.push_back(negativelLeaf);
		fGenericTrapXPlusLeaves.push_back(Leaves.first);
		fGenericTrapXMinusLeaves.push_back(Leaves.second);

		CurrentWidth += leafWidth;
	}
	return fEnvelopePhys;
}


std::pair<G4GenericTrap*,G4GenericTrap*> DivergingMLC::ConstructLeafPair(G4int leafID, G4double thick, G4double posOpen, G4double negOpen, G4double leafWidth, G4double currentWidth) {
	// X Deformation: Between the leaf openings
	G4double positiveXDeformation = 0;
	G4double negativeXDeformation = 0;
	
	// Y Deformation: Between Two consecutive leaves
	G4double forwardYDeformation  = 0;
	G4double backwardYDeformation = 0;

	// Calculate Deformations
	positiveXDeformation =  thick * posOpen / fSAD;
	negativeXDeformation = -thick * negOpen / fSAD;

	forwardYDeformation  = thick*(currentWidth + leafWidth)/fSAD;
	backwardYDeformation = thick*(currentWidth)/fSAD;

	std::vector<G4TwoVector> posVertexes;
	//Posterior To Source
	posVertexes.push_back(G4TwoVector(-fLeafHalfLength+positiveXDeformation,backwardYDeformation-leafWidth*0.5)/fMagnification); // -X, -Y, -Z
	posVertexes.push_back(G4TwoVector(-fLeafHalfLength+positiveXDeformation,forwardYDeformation+leafWidth*0.5)/fMagnification);  // -X, +Y, -Z
	posVertexes.push_back(G4TwoVector( fLeafHalfLength,                     forwardYDeformation+leafWidth*0.5)/fMagnification);  // +X, +Y, -Z
	posVertexes.push_back(G4TwoVector( fLeafHalfLength,                     backwardYDeformation-leafWidth*0.5)/fMagnification); // +X, -Y, -Z

	//Anterior to Source
	posVertexes.push_back(G4TwoVector(-fLeafHalfLength,-leafWidth*0.5)/fMagnification); // -X, -Y, +Z
	posVertexes.push_back(G4TwoVector(-fLeafHalfLength,+leafWidth*0.5)/fMagnification); // -X, +Y, +Z
	posVertexes.push_back(G4TwoVector( fLeafHalfLength,+leafWidth*0.5)/fMagnification); // +X, +Y, +Z
	posVertexes.push_back(G4TwoVector( fLeafHalfLength,-leafWidth*0.5)/fMagnification); // +X, -Y, +Z

	std::vector<G4TwoVector> negVertexes;
	//Posterior to Source
	negVertexes.push_back(G4TwoVector(-fLeafHalfLength,                     backwardYDeformation-leafWidth*0.5)/fMagnification); // -X -Y, -Z
	negVertexes.push_back(G4TwoVector(-fLeafHalfLength,                     forwardYDeformation+leafWidth*0.5)/fMagnification);  // -X +Y, -Z
	negVertexes.push_back(G4TwoVector( fLeafHalfLength-negativeXDeformation,forwardYDeformation+leafWidth*0.5)/fMagnification);  // +X +Y, -Z
	negVertexes.push_back(G4TwoVector( fLeafHalfLength-negativeXDeformation,backwardYDeformation-leafWidth*0.5)/fMagnification); // +X -Y, -Z

	//Anterior to Source
	negVertexes.push_back(G4TwoVector(-fLeafHalfLength,-leafWidth*0.5)/fMagnification); // -X -Y, +Z
	negVertexes.push_back(G4TwoVector(-fLeafHalfLength,+leafWidth*0.5)/fMagnification); // -X +Y, +Z
	negVertexes.push_back(G4TwoVector( fLeafHalfLength,+leafWidth*0.5)/fMagnification); // +X +Y, +Z
	negVertexes.push_back(G4TwoVector( fLeafHalfLength,-leafWidth*0.5)/fMagnification); // +X -Y, +Z

	G4GenericTrap* posLeaf = new G4GenericTrap("gLeaf_pos",thick * 0.5/fMagnification,posVertexes);
	G4GenericTrap* negLeaf = new G4GenericTrap("gLeaf_neg",thick * 0.5/fMagnification,negVertexes);
	return std::make_pair(posLeaf, negLeaf);
}
