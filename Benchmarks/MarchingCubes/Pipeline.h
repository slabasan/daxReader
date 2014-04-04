//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2012 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================

#include "ArgumentsParser.h"

#include <dax/cont/ArrayHandle.h>
#include <dax/cont/DispatcherGenerateInterpolatedCells.h>
#include <dax/cont/DispatcherMapCell.h>
#include <dax/cont/DispatcherMapField.h>
#include <dax/cont/Timer.h>
#include <dax/cont/UniformGrid.h>
#include <dax/cont/UnstructuredGrid.h>
#include <dax/cont/VectorOperations.h>

#include <dax/worklet/Magnitude.h>
#include <dax/worklet/MarchingCubes.h>

#include <iostream>
#include <vector>
#include <fstream>

#define MAKE_STRING2(x) #x
#define MAKE_STRING1(x) MAKE_STRING2(x)
#define DEVICE_ADAPTER MAKE_STRING1(DAX_DEFAULT_DEVICE_ADAPTER_TAG)


namespace 
{
dax::Scalar ISOVALUE = 3.0;
dax::Scalar GRID_SIZE = 50;

//typedef dax::cont::ArrayHandle<dax::Scalar> ArrayHandleScalar;

void PrintResults(int pipeline, double time)
{
  std::cout << "Elapsed time: " << time << " seconds." << std::endl;
  std::cout << "CSV," DEVICE_ADAPTER ","
            << pipeline << "," << time << std::endl;
}

void ReadData(std::vector<dax::Scalar> &buffer)
{
    assert(sizeof(float) == sizeof(dax::Scalar));
    
    FILE *f = fopen("/home/slabasan/Dropbox/DaxDatasets/visit_ex_db", "rb");
    assert(f != NULL);

    buffer.resize(GRID_SIZE*GRID_SIZE*GRID_SIZE);
    fread(&buffer.front(), sizeof(float), GRID_SIZE*GRID_SIZE*GRID_SIZE, f);
    assert(ferror(f) == 0);

    //SL: Confirming validity of data read in from file
    //printf("grid size: %lf\n", GRID_SIZE);
    std::cout << "buffer: " << buffer.at(0) << std::endl;
    std::cout << "buffer: " << buffer.at(1) << std::endl;
    std::cout << "buffer: " << buffer.at(2) << std::endl;
    std::cout << "buffer: " << buffer.at(3) << std::endl;
    std::cout << "buffer: " << buffer.at(4) << std::endl;
    std::cout << "buffer: " << buffer.at(5) << std::endl;

    fclose(f);
}

void RunDAXPipeline(const dax::cont::UniformGrid<> &grid, int pipeline, std::vector<dax::Scalar> &buffer)
{
  std::cout << "Running pipeline " << pipeline << ": Magnitude -> MarchingCubes" << std::endl;
 
  dax::cont::UnstructuredGrid<dax::CellTagTriangle> outGrid;

  dax::cont::ArrayHandle<dax::Scalar> inArray = dax::cont::make_ArrayHandle(buffer);
  std::cout << "size of input data: " << inArray.GetNumberOfValues() << std::endl;
  assert(grid.GetNumberOfPoints() == inArray.GetNumberOfValues());

  //dax::cont::ArrayHandle<dax::Scalar> intermediate1;
  //dax::cont::DispatcherMapField< dax::worklet::Magnitude > magDispatcher;
  //magDispatcher.Invoke( grid.GetPointCoordinates(), inArray);

  dax::cont::Timer<> timer;

  //dispatch marching cubes worklet generate step
  typedef dax::cont::DispatcherGenerateInterpolatedCells< dax::worklet::MarchingCubesGenerate > DispatcherIC;
  typedef DispatcherIC::CountHandleType  CountHandleType;

  dax::worklet::MarchingCubesCount classifyWorklet(ISOVALUE);
  dax::worklet::MarchingCubesGenerate generateWorklet(ISOVALUE);

  //run the first step
  CountHandleType count; //array handle for the first step count
  dax::cont::DispatcherMapCell<dax::worklet::MarchingCubesCount > cellDispatcher( classifyWorklet );
  cellDispatcher.Invoke(grid, inArray, count);

  //construct the topology generation worklet
  DispatcherIC icDispatcher(count, generateWorklet );
  icDispatcher.SetRemoveDuplicatePoints(
                pipeline == dax::testing::ArgumentsParser::MARCHING_CUBES_REMOVE_DUPLICATES);

  //run the second step
  icDispatcher.Invoke(grid, outGrid, inArray);

  double time = timer.GetElapsedTime();

  std::cout << "isovalue: " << ISOVALUE << std::endl;
  std::cout << "number of coordinates in: " << grid.GetNumberOfPoints() << std::endl;
  std::cout << "number of coordinates out: " << outGrid.GetNumberOfPoints() << std::endl;
  std::cout << "number of cells out: " << outGrid.GetNumberOfCells() << std::endl;
  PrintResults(pipeline, time);

}


} // Anonymous namespace

