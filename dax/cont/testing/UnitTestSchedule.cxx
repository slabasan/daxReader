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

#define DAX_ARRAY_CONTAINER_CONTROL DAX_ARRAY_CONTAINER_CONTROL_BASIC
#define DAX_DEVICE_ADAPTER DAX_DEVICE_ADAPTER_SERIAL

#include <dax/cont/DeviceAdapterSerial.h>

#include <dax/cont/arg/FieldConstant.h>
#include <dax/cont/arg/FieldArrayHandle.h>

#include <dax/cont/Schedule.h>
#include <dax/cont/DeviceAdapterSerial.h>
#include <dax/exec/WorkletMapField.h>
#include <dax/cont/sig/Tag.h>

#include <dax/cont/internal/testing/Testing.h>

namespace{

struct ExampleWorklet : public dax::exec::WorkletMapField
{
  static float TestValue;
  typedef void ControlSignature(Field);
  typedef void ExecutionSignature(_1);

  template <typename T>
  void operator()(T v) const
    {
    TestValue = v;
    }
};
float ExampleWorklet::TestValue = 0;

struct Example2Worklet : public dax::exec::WorkletMapField
{
  static float TestValue;
  typedef void ControlSignature(Field(In), Field(In));
  typedef void ExecutionSignature(_1, _2);

  template <typename T>
  void operator()(T a, T b) const
    {
    TestValue = a * b;
    }
};
float Example2Worklet::TestValue = 0;

struct ExampleTupleWorklet : public dax::exec::WorkletMapField
{
  typedef void ControlSignature(Field(In));
  typedef void ExecutionSignature(_1);

  template <typename T>
  void operator()(T) const
    {
    }
};

struct ExampleSquare: public dax::exec::WorkletMapField
{
  typedef void ControlSignature(Field(In), Field(Out));
  typedef _2 ExecutionSignature(_1);

  template<typename T>
  T operator()(T t) const
    {
    return t * t;
    }
};

void VerifyConstantArgs()
{
  dax::cont::Schedule<>()(ExampleWorklet(),1); //convert to dax::Id
  DAX_TEST_ASSERT(ExampleWorklet::TestValue == 1, "TestValue is not 1");

  dax::cont::Schedule<>()(ExampleWorklet(),1.35); //convert double to dax::Scalar
  DAX_TEST_ASSERT(ExampleWorklet::TestValue == 1.35f, "TestValue is not 1.35f");

  dax::cont::Schedule<>()(Example2Worklet(),1.0f,3.0f); //convert double to dax::Scalar
  DAX_TEST_ASSERT(Example2Worklet::TestValue == 3.0f, "TestValue is not 3.0f");


  dax::Tuple<dax::Scalar,6> tuple6;
  tuple6[0]=0.0f; tuple6[1]=0.5f; tuple6[2]=0.25f;
  tuple6[0]=0.0f; tuple6[1]=-0.5f; tuple6[2]=-0.25f;
  dax::cont::Schedule<>()(ExampleTupleWorklet(),tuple6);

  dax::Vector2 vec2(-1, -2);
  dax::cont::Schedule<>()(ExampleTupleWorklet(),vec2);

  dax::Vector3 vec3(-1, -2, -3);
  dax::cont::Schedule<>()(ExampleTupleWorklet(),vec3);

  dax::Vector4 vec4(-1, -2, -3, -4);
  dax::cont::Schedule<>()(ExampleTupleWorklet(),vec4);

  dax::Id3 id3(1,2,3);
  dax::cont::Schedule<>()(ExampleTupleWorklet(),id3);
}

void VerifyArrayHandleArgs()
{
  std::vector<dax::Id> in(10);
  for(int i=0; i < 10; ++i){in[i]=i;}
  dax::cont::ArrayHandle<dax::Id> input = dax::cont::make_ArrayHandle(in);
  dax::cont::ArrayHandle<dax::Id> output;
  dax::cont::Schedule<>()(ExampleSquare(),input,output);

}

void Schedule()
{
  VerifyConstantArgs();
  VerifyArrayHandleArgs();
}

}

int UnitTestSchedule(int, char *[])
{
  return dax::cont::internal::Testing::Run(Schedule);
}