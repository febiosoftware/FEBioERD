/*This file is part of the FEBio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio.txt for details.

Copyright (c) 2019 University of Utah, The Trustees of Columbia University in
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
#include "FEGrowthTensorERD.h"
#include <FEBioMech/FEElasticMaterial.h>
#include <FECore/FEMaterialPoint.h>
#include <FECore/FEModelComponent.h>

//-----------------------------------------------------------------------------
//! Define a material point that stores the multiplicative decomposition
//! of the deformation gradient and the solid referential density
//
class FEKinematicMaterialPointERD : public FEMaterialPointData
{
public:
    FEKinematicMaterialPointERD(FEMaterialPointData *pt) : FEMaterialPointData(pt) {}
    
    FEMaterialPointData* Copy() override;
    
    void Init() override;

    void Update(const FETimeInfo& timeInfo) override;
    
    void Serialize(DumpStream& ar) override;
    
public:
    mat3d    m_Fe;       //!< elastic deformation
    mat3d    m_Fg;       //!< growth deformation
    double   m_Je;
    double   m_Jg;
    double   m_theta;    //!< growth rate
    double   m_theta_p;  //!< growth rate on previous step
    double   m_K_res;    //!< stiffness component of the residual
    FEParamDouble   m_rhor;     //!< solid referential density
};

//-----------------------------------------------------------------------------
//! Material class that implements Kinematic growth model.
//
class FEBIOERD_API FEKinematicGrowthRateDependent : public FEElasticMaterial
{
public:
    FEKinematicGrowthRateDependent(FEModel* pfem);
    
    //! Initialization routine
    bool Init() override;
    
    //! get the elastic base material
    FEElasticMaterial* GetBaseMaterial() { return m_pBase; }
    
    //! get the growth tensor base material
    FEGrowthTensorERD* GetGrowthMaterial() { return m_pGrowth; }
    
    //! Returns the Cauchy stress
    mat3ds Stress(FEMaterialPoint& mp) override;
    
    //! Returns the spatial tangent
    tens4ds Tangent(FEMaterialPoint& mp) override;

    //! Returns the spatial tangent
    mat3ds dSdtheta(FEMaterialPoint& mp);
    
    //! Returns dSdFg
    tens4ds dSdFg(FEMaterialPoint& mp);

    //! Returns dTdc
    mat3ds dTdc(FEMaterialPoint& mp, int sol);

    //! Returns the strain energy density
    double StrainEnergyDensity(FEMaterialPoint& mp) override;
    
    // returns a pointer to a new material point object
    FEMaterialPointData* CreateMaterialPointData() override;

    // update material point at each iteration
    void UpdateSpecializedMaterialPoints(FEMaterialPoint& mp, const FETimeInfo& tp) override;

private:
    FEElasticMaterial*  m_pBase;        //!< pointer to elastic solid material
    FEGrowthTensorERD*     m_pGrowth;      //!< pointer to growth tensor

    DECLARE_FECORE_CLASS();
};
