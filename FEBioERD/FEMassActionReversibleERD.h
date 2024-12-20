/*This file is part of the FEBio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio.txt for details.

Copyright (c) 2021 University of Utah, The Trustees of Columbia University in
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/



#pragma once
#include "FEChemicalReactionERD.h"

//-----------------------------------------------------------------------------
//! Law of mass action for reversible chemical reaction
//! using  concentrations.
class FEBIOERD_API FEMassActionReversibleERD : public FEChemicalReactionERD
{
public:
    //! constructor
    FEMassActionReversibleERD(FEModel* pfem);

    //! molar supply at material point
    double ReactionSupply(FEMaterialPoint& pt) override;

    //! tangent of molar supply with strain (J) at material point
    mat3ds Tangent_ReactionSupply_Strain(FEMaterialPoint& pt) override;

    //! tangent of molar supply with  concentration at material point
    double Tangent_ReactionSupply_Concentration(FEMaterialPoint& pt, const int sol) override;

    //! tangent of molar supply with Cauchy stress (sigma) at material point
    mat3ds Tangent_ReactionSupply_Stress(FEMaterialPoint& pt) override;

    //! molar supply at material point
    double FwdReactionSupply(FEMaterialPoint& pt);

    //! molar supply at material point
    double RevReactionSupply(FEMaterialPoint& pt);

    DECLARE_FECORE_CLASS();
};
