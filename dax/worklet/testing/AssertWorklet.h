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
#ifndef __Assert_worklet_
#define __Assert_worklet_

#include <dax/exec/WorkletMapField.h>

#include <dax/exec/Assert.h>

namespace dax {
namespace worklet {
namespace testing {

class Assert : public dax::exec::WorkletMapField
{
public:
  typedef void ControlSignature(Field);
  typedef void ExecutionSignature(_1);

  template<typename ValueType>
  DAX_EXEC_EXPORT
  void operator()(const ValueType &) const
  {
    DAX_ASSERT_EXEC(0 == 1, *this);
  }
};

}
}
}
#endif
