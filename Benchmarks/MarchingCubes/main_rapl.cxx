//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
// //  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2012 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================

#include "ArgumentsParser.h"
#include "Pipeline.h"

#include <iostream>
using std::cerr;
using std::endl;
#include "/usr/include/mpich2/mpi.h"
#include <unistd.h>

dax::cont::UniformGrid<> CreateInputStructure(dax::Id dim)
{
  dax::cont::UniformGrid<> grid;
  grid.SetOrigin(dax::make_Vector3(0.0, 0.0, 0.0));
  grid.SetSpacing(dax::make_Vector3(1.0, 1.0, 1.0));
  grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(dim-1, dim-1, dim-1));
  return grid;
}

int main(int argc, char* argv[])
  {
      MPI_Init(&argc, &argv);
      sleep(5);
      cerr << "Done sleeping" << endl;

  dax::testing::ArgumentsParser parser;
  if (!parser.parseArguments(argc, argv))
    {
    return 1;
    }

  //init grid vars from parser
  const dax::Id MAX_SIZE = 400;

  dax::cont::UniformGrid<> grid = CreateInputStructure(MAX_SIZE);

  RunDAXPipeline(grid, parser.pipeline());

  sleep(5);
  cerr << "Done sleeping" << endl;
  MPI_Finalize();

  return 0;
}
