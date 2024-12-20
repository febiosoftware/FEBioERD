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



#include "stdafx.h"
#include <FECore/sdk.h>
#include <FECore/FEModule.h>
#include <FECore/FEModel.h>
#include <FECore/FETimeStepController.h>

#include "FEElasticReactionDiffusionStandard.h"
#include <FEBioMix/FESolute.h>
#include <FEBioMix/FEDiffConstIso.h>
#include <FEBioMix/FESoluteFlux.h>
#include <FEBioMix/FEFixedConcentration.h>
#include <FEBioMix/FEPrescribedConcentration.h>

#include "FEReactionERD.h"
#include "FEReactionRateConstERD.h"
#include "FEConcentrationIndependentReactionERD.h"
#include "FEMassActionForwardERD.h"
#include "FEMassActionReversibleERD.h"
#include "FEHillActivation.h"
#include "FEHillInhibition.h"
#include "FEHillActivationANDActivation.h"
#include "FEHillActivationANDInhibition.h"
#include "FEHillActivationORActivation.h"
#include "FEReactionRateStressSensitiveERD.h"
#include "FEPrescribedStressSensitiveConcentrationERD.h"

#include "FEGrowthTensorERD.h"
#include "FEKinematicGrowthRateDependent.h"

#include "FEElasticReactionDiffusionSolver.h"

#include "FEBioERDPlot.h"
#include <FEBioMix/FEBioMixPlot.h>
#include <FEBioMix/FEBioMixData.h>

#include "FEERDDomainFactory.h"
#include "FEElasticReactionDiffusionSolidDomain.h"

#include <FEBioMix/FEBiphasicModule.h>
#include <FEBioMix/FEBiphasicAnalysis.h>
#include <FEBioMix/FEBiphasicSoluteAnalysis.h>
#include <FEBioMix/FEMultiphasicAnalysis.h>
#include "FEERDModule.h"
#include "FEElasticReactionDiffusionAnalysis.h"

class FEBioERDModule : public FEModule
{
public:
	FEBioERDModule() {}
	void InitModel(FEModel* fem)
	{
		DOFS& dofs = fem->GetDOFS();
		int var = dofs.AddVariable("concentration", VAR_ARRAY);
	}
};

FECORE_PLUGIN int GetSDKVersion()
{
	return FE_SDK_VERSION;
}

FECORE_PLUGIN void GetPluginVersion(int& major, int& minor, int& patch)
{
	major = 1;
	minor = 0;
	patch = 0;
}

FECORE_PLUGIN void PluginInitialize(FECoreKernel& febio)
{
	//-----------------------------------------------------------------------------
	// Domain factory
	// 
	FECoreKernel::SetInstance(&febio);
	febio.RegisterDomain(new FEERDDomainFactory);

	// Reaction-diffusion module
	const char* info = \
		"{ "
		"   \"title\" : \"Elastic Reaction Diffusion Analysis\","
		"   \"info\"  : \"Transient analysis with solutes.\","
		"   \"author\": \"Steven LaBelle\","
		"   \"version\": \"1.0\""
		"}";

	//======================================================================
// setup the "elastic reaction diffusion" module
	febio.CreateModule(new FEERDModule, "elastic-reaction-diffusion", info);

	febio.AddModuleDependency("multiphasic"); // also pulls in solid, biphasic, solutes
		//-----------------------------------------------------------------------------
	// Global data classes

	//-----------------------------------------------------------------------------
	// analyis classes (default type must match module name!)
	REGISTER_FECORE_CLASS(FEElasticReactionDiffusionAnalysis, "elastic-reaction-diffusion");

	//-----------------------------------------------------------------------------
	// solver classes
	REGISTER_FECORE_CLASS(FEElasticReactionDiffusionSolver, "elastic-reaction-diffusion");

	//-----------------------------------------------------------------------------
	// Domain classes
	REGISTER_FECORE_CLASS(FEElasticReactionDiffusionSolidDomain, "elastic-reaction-diffusion-solid");

	//-----------------------------------------------------------------------------
	// Materials
	REGISTER_FECORE_CLASS(FEVolumeGrowthERD, "volume growth");
	REGISTER_FECORE_CLASS(FEAreaGrowthERD, "area growth");
	REGISTER_FECORE_CLASS(FEFiberGrowthERD, "fiber growth");
	REGISTER_FECORE_CLASS(FEKinematicGrowthRateDependent, "kinematic growth");
	REGISTER_FECORE_CLASS(FEElasticReactionDiffusionStandard, "elastic-reaction-diffusion");
	REGISTER_FECORE_CLASS(FEReactionRateConstERD, "constant reaction rate erd");
	REGISTER_FECORE_CLASS(FEConcentrationIndependentReactionERD, "concentration-independent-erd");
	REGISTER_FECORE_CLASS(FEMassActionForwardERD, "mass-action-forward-erd");
	REGISTER_FECORE_CLASS(FEMassActionReversibleERD, "mass-action-reversible-erd");
	REGISTER_FECORE_CLASS(FEReactionRateStressSensitiveERD, "stress sensitive reaction rate erd");

	REGISTER_FECORE_CLASS(FEHillActivation, "Hill activation");
	REGISTER_FECORE_CLASS(FEHillInhibition, "Hill inhibition");
	REGISTER_FECORE_CLASS(FEHillActivationANDActivation, "Hill activation AND activation");
	REGISTER_FECORE_CLASS(FEHillActivationANDInhibition, "Hill activation AND inhibition");
	REGISTER_FECORE_CLASS(FEHillActivationORActivation, "Hill activation OR activation");

	REGISTER_FECORE_CLASS(FEReactantSpeciesRefERD, "vR");
	REGISTER_FECORE_CLASS(FEProductSpeciesRefERD, "vP");

	//-----------------------------------------------------------------------------
	// Surface loads

	//-----------------------------------------------------------------------------
	// Boundary conditions
	REGISTER_FECORE_CLASS(FEPrescribedStressSensitiveConcentrationERD, "prescribed stress sensitive concentration erd");
	//-----------------------------------------------------------------------------
	// classes derived from FEPlotData
	// growth variables
	REGISTER_FECORE_CLASS(FEPlotGrowthRatio, "growth ratio");
	REGISTER_FECORE_CLASS(FEPlotGrowthTensor, "growth tensor");
	REGISTER_FECORE_CLASS(FEPlotTraceStresses, "stress trace");
	REGISTER_FECORE_CLASS(FEPlotGrowthElasticDeformationGradient, "growth elastic deformation gradient");
	REGISTER_FECORE_CLASS(FEPlotGrowthDeformationGradient, "growth deformation gradient");
	REGISTER_FECORE_CLASS(FEPlotJacobian, "Jacobian");
	REGISTER_FECORE_CLASS(FEPlotGrowthElasticJacobian, "growth elastic Jacobian");
	REGISTER_FECORE_CLASS(FEPlotGrowthJacobian, "growth Jacobian");
	REGISTER_FECORE_CLASS(FEPlotGrowthK, "growth activation");
	REGISTER_FECORE_CLASS(FEPlotGrowthPhi, "growth phi");

	REGISTER_FECORE_CLASS(FEPlotActualSoluteConcentrationERD, "solute concentration erd");
	REGISTER_FECORE_CLASS(FEPlotEffectiveSoluteConcentrationERD, "effective solute concentration erd");
	REGISTER_FECORE_CLASS(FEPlotSoluteFluxERD, "solute flux erd");
	REGISTER_FECORE_CLASS(FEPlotSPREffectiveConcentrationERD, "effective SPR solute concentration erd");

	febio.SetActiveModule(0);
}