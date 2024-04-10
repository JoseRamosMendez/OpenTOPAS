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

#include "TsDoubleVectorFromValueTimesUnitless.hh"

TsDoubleVectorFromValueTimesUnitless::TsDoubleVectorFromValueTimesUnitless(TsParameterManager* pM, TsParameterFile* pf, TsTempParameter* tempParameter,
															 G4int nValues, G4String* values, const G4String& nameOfOtherParameter, const G4String& unit)
:TsVParameter(pM, pf, tempParameter), fUnit(unit)
{
	QuitIfMustBeAbsolute();

	CheckUnit(unit);

	fLength = nValues;
	fValues = new G4double [nValues];
	fValuesIfParameterNames = new G4String[nValues];
	ConvertToDoubleVector(nValues, values,  fValues, fValuesIfParameterNames);

	fNameOfOtherParameter = ParseNameOfOtherParameter(nameOfOtherParameter, "u");
}


TsDoubleVectorFromValueTimesUnitless::~TsDoubleVectorFromValueTimesUnitless()
{
	delete fValues;
	delete fValuesIfParameterNames;
}


G4String TsDoubleVectorFromValueTimesUnitless::GetType()
{
	G4String type = "dv";
	return type;
}


G4String TsDoubleVectorFromValueTimesUnitless::GetUnit() {
	return fUnit;
}


G4int TsDoubleVectorFromValueTimesUnitless::GetVectorLength()
{
	return fLength;
}


G4double* TsDoubleVectorFromValueTimesUnitless::GetDoubleVector()
{
	fUsed = true;
	G4double* values = new G4double[fLength];
	for (G4int iValue=0; iValue<fLength; iValue++)
		if (fValuesIfParameterNames[iValue]=="")
			values[iValue] = fValues[iValue] * fPm->GetUnitValue(fUnit) *
			EvaluateUnitlessParameter(fNameOfOtherParameter);
		else
			values[iValue] = EvaluateDoubleParameter(fValuesIfParameterNames[iValue]) *
			EvaluateUnitlessParameter(fNameOfOtherParameter);
	return values;
}


G4String TsDoubleVectorFromValueTimesUnitless::GetHTMLValue()
{
	G4String output;
	output += G4UIcommand::ConvertToString(fLength);
	for (G4int iValue=0; iValue<fLength; iValue++)
		if (fValuesIfParameterNames[iValue]=="")
			output+= " " + G4UIcommand::ConvertToString(fValues[iValue]);
		else
			output+= " " + EvaluateAsHTML(fValuesIfParameterNames[iValue]);
	output += " ";
	output += fUnit;
	output += " * ";
	output += EvaluateAsHTML(fNameOfOtherParameter);
	return output;
}
