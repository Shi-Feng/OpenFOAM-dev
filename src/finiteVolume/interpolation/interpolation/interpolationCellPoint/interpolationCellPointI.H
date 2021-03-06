/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
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

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
inline Type Foam::interpolationCellPoint<Type>::interpolate
(
    const cellPointWeight& cpw
) const
{
    const FixedList<scalar, 4>& weights = cpw.weights();
    const FixedList<label, 3>& faceVertices = cpw.faceVertices();

    Type t = this->psi_[cpw.cell()]*weights[0];
    t += psip_[faceVertices[0]]*weights[1];
    t += psip_[faceVertices[1]]*weights[2];
    t += psip_[faceVertices[2]]*weights[3];

    return t;
}


template<class Type>
inline Type Foam::interpolationCellPoint<Type>::interpolate
(
    const vector& position,
    const label celli,
    const label facei
) const
{
    return interpolate(cellPointWeight(this->pMesh_, position, celli, facei));
}


template<class Type>
inline Type Foam::interpolationCellPoint<Type>::interpolate
(
    const vector& position,
    const tetIndices& tetIs,
    const label facei
) const
{
    // Assumes that the position is consistent with the supplied
    // tetIndices.  Does not pay attention to whether or not facei is
    // supplied or not - the result will be essentially the same.
    // Performs a consistency check, however.

    if (facei >= 0)
    {
        if (facei != tetIs.face())
        {
            FatalErrorInFunction
                << "specified face " << facei << " inconsistent with the face "
                << "stored by tetIndices: " << tetIs.face()
                << exit(FatalError);
        }
    }

    FixedList<scalar, 4> weights;

    tetIs.tet(this->pMesh_).barycentric(position, weights);

    const faceList& pFaces = this->pMesh_.faces();

    const face& f = pFaces[tetIs.face()];

    // Order of weights is the same as that of the vertices of the tet, i.e.
    // cellCentre, faceBasePt, facePtA, facePtB.

    Type t = this->psi_[tetIs.cell()]*weights[0];

    t += psip_[f[tetIs.faceBasePt()]]*weights[1];

    t += psip_[f[tetIs.facePtA()]]*weights[2];

    t += psip_[f[tetIs.facePtB()]]*weights[3];

    return t;
}


// ************************************************************************* //
