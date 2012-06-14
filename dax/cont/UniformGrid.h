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
#ifndef __dax__cont__UniformGrid_h
#define __dax__cont__UniformGrid_h

#include <dax/cont/ArrayContainerControl.h>
#include <dax/cont/DeviceAdapter.h>

#include <dax/exec/CellVoxel.h>

#include <dax/exec/internal/TopologyUniform.h>

namespace dax {
namespace cont {

/// A tag you can use to identify when a grid is a uniform grid.
///
struct UniformGridTag {  };

/// This class defines the topology of a uniform grid. A uniform grid is axis
/// aligned and has uniform spacing between grid points in every dimension. The
/// grid can be shifted and scaled in space by defining and origin and spacing.
///
template <class ArrayContainerControlTag = DAX_DEFAULT_ARRAY_CONTAINER_CONTROL,
          class DeviceAdapterTag = DAX_DEFAULT_DEVICE_ADAPTER>
class UniformGrid
{
private:
  dax::Vector3 Origin;
  dax::Vector3 Spacing;
  dax::Extent3 Extent;

public:
  typedef dax::exec::CellVoxel CellType;
  typedef UniformGridTag GridTypeTag;

  typedef dax::exec::internal::TopologyUniform ExecutionTopologyStruct;

  UniformGrid()
    : Origin(dax::make_Vector3(0.0, 0.0, 0.0)),
      Spacing(dax::make_Vector3(1.0, 1.0, 1.0))
  {
    this->SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(0, 0, 0));
  }

  /// The extent defines the minimum and maximum (inclusive) indices in each
  /// dimension.
  ///
  const dax::Extent3 &GetExtent() const { return this->Extent; }
  void SetExtent(const dax::Extent3 &extent) { this->Extent = extent; }
  void SetExtent(const dax::Id3 &min, const dax::Id3 &max) {
    this->Extent.Min = min;
    this->Extent.Max = max;
  }

  /// The origin is the location in space of the point at grid position
  /// (0, 0, 0).  This position may or may not actually be in the extent.
  ///
  const dax::Vector3 &GetOrigin() const { return this->Origin; }
  void SetOrigin(const dax::Vector3 &coords) { this->Origin = coords; }

  /// The spacing is the distance between grid points. Each component in the
  /// vector refers to a spacing along the associated axis, which can vary.
  ///
  const dax::Vector3 &GetSpacing() const { return this->Spacing; }
  void SetSpacing(const dax::Vector3 &distances) { this->Spacing = distances; }

  // Helper functions

  /// Get the number of points.
  ///
  dax::Id GetNumberOfPoints() const {
    dax::Id3 dims = dax::extentDimensions(this->GetExtent());
    return dims[0]*dims[1]*dims[2];
  }

  /// Get the number of cells.
  ///
  dax::Id GetNumberOfCells() const {
    dax::Id3 dims = dax::extentDimensions(this->GetExtent())
                    - dax::make_Id3(1, 1, 1);
    return dims[0]*dims[1]*dims[2];
  }

  /// Converts an i, j, k point location to a point index.
  ///
  dax::Id ComputePointIndex(const dax::Id3 &ijk) const {
    return dax::index3ToFlatIndex(ijk, this->GetExtent());
  }

  /// Converts an i, j, k cell location to a cell index.
  ///
  dax::Id ComputeCellIndex(const dax::Id3 &ijk) const {
    return dax::index3ToFlatIndexCell(ijk, this->GetExtent());
  }

  /// Converts a flat point index to an i, j, k point location.
  ///
  dax::Id3 ComputePointLocation(dax::Id index) const {
    return dax::flatIndexToIndex3(index, this->GetExtent());
  }

  /// Converts a flat cell index to an i, j, k cell location.
  ///
  dax::Id3 ComputeCellLocation(dax::Id index) const {
    return dax::flatIndexToIndex3Cell(index, this->GetExtent());
  }

  /// Given a point i, j, k location, computes the coordinates.
  ///
  dax::Vector3 ComputePointCoordinates(dax::Id3 location) const {
    dax::Vector3 coord(
          this->GetOrigin()[0] + this->GetSpacing()[0]*location[0],
          this->GetOrigin()[1] + this->GetSpacing()[1]*location[1],
          this->GetOrigin()[2] + this->GetSpacing()[2]*location[2]);
    return coord;
  }

  /// Given a point idnex, computes the coordinates.
  ///
  dax::Vector3 ComputePointCoordinates(dax::Id index) const {
    return this->ComputePointCoordinates(this->ComputePointLocation(index));
  }

  /// This class just serves as a placeholde (in lieu of something like an
  /// ArrayHandle) so that a uniform grid can be templated as any other grid
  /// with points. Eventually there needs to be specialization to handle the
  /// case that there is no real array.
  ///
  struct PointCoordinatesArrayPlaceholder {
    // Probably no one actually needs these typedefs. They are here mostly to
    // document why we need the template parameters in the first place (which
    // is to capture the arrays/device to copy data over to the execution
    // environment).
    typedef ArrayContainerControlTag ArrayContainerControl;
    typedef DeviceAdapterTag DeviceAdapter;
  };

  typedef PointCoordinatesArrayPlaceholder PointCoordinatesType;

  PointCoordinatesArrayPlaceholder GetPointCoordinates() const {
    return PointCoordinatesArrayPlaceholder();
  }
};

}
}

#endif //__dax__cont__UniformGrid_h