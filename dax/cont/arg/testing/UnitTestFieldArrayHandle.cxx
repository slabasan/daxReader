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

#include <dax/cont/arg/FieldArrayHandle.h>
#include <dax/cont/internal/Testing.h>

#include <dax/cont/internal/Bindings.h>
#include <dax/cont/sig/Tag.h>
#include <dax/exec/WorkletMapField.h>

namespace{
using dax::cont::arg::Field;


struct Worklet1: public dax::exec::WorkletMapField
{
  typedef void ControlSignature(Field);
};

template<typename T>
void verifyBindingExists(T value)
{
  typedef Worklet1 Invocation1(T);
  dax::cont::internal::Bindings<Invocation1> binded(value);
  (void)binded;
}

void ArrayHandle()
{
  //confirm that we can bind to the following types:

  //integer
  typedef dax::cont::ArrayHandle<dax::Id> IdAType;
  verifyBindingExists<IdAType>( IdAType() );

  //scalar
  typedef dax::cont::ArrayHandle<dax::Scalar> ScalarAType;
  verifyBindingExists<ScalarAType>( ScalarAType() );

  //vector
  typedef dax::cont::ArrayHandle<dax::Vector2> VecAType;
  verifyBindingExists<VecAType>( VecAType() );

}

}

int UnitTestFieldArrayHandle(int, char *[])
{
  return dax::cont::internal::Testing::Run(ArrayHandle);
}
