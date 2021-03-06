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

#include <dax/cont/testing/TestingGridGenerator.h>
#include <dax/cont/testing/Testing.h>

#include <dax/worklet/PointDataToCellData.h>

#include <dax/Types.h>
#include <dax/cont/ArrayHandle.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/DispatcherMapCell.h>

#include <vector>

namespace {

const dax::Id DIM = 8;

//-----------------------------------------------------------------------------
template<typename CellTag>
void verifyCellData(
              const dax::cont::testing::CellConnections<CellTag> &cellVertices,
              const dax::Scalar& computedCellData)
{
  dax::Scalar expectedCellData = 0.0;
  for (int vertexIndex = 0;
       vertexIndex < cellVertices.NUM_VERTICES;
       ++vertexIndex)
  {
    expectedCellData += cellVertices[vertexIndex];
  }
  expectedCellData /= cellVertices.NUM_VERTICES;
  DAX_TEST_ASSERT(test_equal(computedCellData,expectedCellData),"Got bad average");
}

//-----------------------------------------------------------------------------
struct TestPointDataToCellDataWorklet
{
  //----------------------------------------------------------------------------
  template<typename GridType>
  DAX_CONT_EXPORT
  void operator()(const GridType&) const
    {
    dax::cont::testing::TestGrid< GridType > grid(DIM);

    std::vector<dax::Scalar> field(grid->GetNumberOfPoints());
    for (dax::Id pointIndex = 0;
         pointIndex < grid->GetNumberOfPoints();
         pointIndex++)
      {
      field[pointIndex] = pointIndex;
      }

    dax::cont::ArrayHandle<dax::Scalar> fieldHandle =
        dax::cont::make_ArrayHandle(field);

    dax::cont::ArrayHandle<dax::Scalar> resultHandle;

    std::cout << "Running PointDataToCellData worklet" << std::endl;
    dax::cont::DispatcherMapCell< dax::worklet::PointDataToCellData> dispatcher;
    dispatcher.Invoke(grid.GetRealGrid(), fieldHandle, resultHandle);

    std::cout << "Checking result" << std::endl;
    std::vector<dax::Scalar> cellData(grid->GetNumberOfCells());
    resultHandle.CopyInto(cellData.begin());
    for (dax::Id cellIndex = 0;
         cellIndex < grid->GetNumberOfCells();
         cellIndex++)
      {
      verifyCellData(grid.GetCellConnections(cellIndex),
                     cellData[cellIndex]);
      }
    }
};

//-----------------------------------------------------------------------------
void TestPointDataToCellData()
  {
  dax::cont::testing::GridTesting::TryAllGridTypes(
                                           TestPointDataToCellDataWorklet() );
  }


} // Anonymous namespace

//-----------------------------------------------------------------------------
int UnitTestWorkletPointDataToCellData(int, char *[])
{
  return dax::cont::testing::Testing::Run(TestPointDataToCellData);
}
