/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "arcUniform.H"
#include "sampledSet.H"
#include "meshSearch.H"
#include "DynamicList.H"
#include "polyMesh.H"
#include "addToRunTimeSelectionTable.H"
#include "word.H"
#include "unitConversion.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace sampledSets
{
    defineTypeNameAndDebug(arcUniform, 0);
    addToRunTimeSelectionTable(sampledSet, arcUniform, word);
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::sampledSets::arcUniform::calcSamples
(
    DynamicList<point>& samplingPts,
    DynamicList<label>& samplingCells,
    DynamicList<label>& samplingFaces,
    DynamicList<label>& samplingSegments,
    DynamicList<scalar>& samplingCurveDist
) const
{
    const vector axis1 = radial_ - (radial_ & normal_)*normal_;
    const vector axis2 = normal_ ^ axis1;
    const scalar radius = mag(axis1);

    for (label i = 0; i < nPoints_; ++ i)
    {
        const scalar t = scalar(i)/(nPoints_ - 1);
        const scalar theta = (1 - t)*startAngle_ + t*endAngle_;
        const scalar c = cos(theta), s = sin(theta);

        const point pt = centre_ + c*axis1 + s*axis2;
        const label celli = searchEngine().findCell(pt);

        if (celli != -1)
        {
            samplingPts.append(pt);
            samplingCells.append(celli);
            samplingFaces.append(-1);
            samplingSegments.append(samplingSegments.size());
            samplingCurveDist.append(radius*theta);
        }
    }
}


void Foam::sampledSets::arcUniform::genSamples()
{
    // Storage for sample points
    DynamicList<point> samplingPts;
    DynamicList<label> samplingCells;
    DynamicList<label> samplingFaces;
    DynamicList<label> samplingSegments;
    DynamicList<scalar> samplingCurveDist;

    calcSamples
    (
        samplingPts,
        samplingCells,
        samplingFaces,
        samplingSegments,
        samplingCurveDist
    );

    samplingPts.shrink();
    samplingCells.shrink();
    samplingFaces.shrink();
    samplingSegments.shrink();
    samplingCurveDist.shrink();

    setSamples
    (
        samplingPts,
        samplingCells,
        samplingFaces,
        samplingSegments,
        samplingCurveDist
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sampledSets::arcUniform::arcUniform
(
    const word& name,
    const polyMesh& mesh,
    const meshSearch& searchEngine,
    const dictionary& dict
)
:
    sampledSet(name, mesh, searchEngine, dict),
    centre_(dict.lookup("centre")),
    normal_(normalised(dict.lookup<vector>("normal"))),
    radial_(dict.lookup<vector>("radial")),
    startAngle_(dict.lookup<scalar>("startAngle")),
    endAngle_(dict.lookup<scalar>("endAngle")),
    nPoints_(dict.lookup<scalar>("nPoints"))
{
    genSamples();

    if (debug)
    {
        write(Info);
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::sampledSets::arcUniform::~arcUniform()
{}


// ************************************************************************* //
