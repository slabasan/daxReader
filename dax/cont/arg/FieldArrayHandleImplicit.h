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
#ifndef __dax_cont_arg_FieldArrayHandleImplicit_h
#define __dax_cont_arg_FieldArrayHandleImplicit_h

#include <dax/cont/arg/FieldArrayHandle.h>
#include <dax/cont/ArrayHandleImplicit.h>

namespace dax { namespace cont { namespace arg {

/// \headerfile FieldArrayHandle.h dax/cont/arg/FieldArrayHandle.h
/// \brief Map permutation array handle to \c Field worklet parameters.
template <typename Tags, typename Value, typename Functor, typename Device>
class ConceptMap< Field(Tags), dax::cont::ArrayHandleImplicit<Value,
                                                               Functor,
                                                               Device> > :
  public ConceptMap< Field(Tags),
    dax::cont::ArrayHandle<
      Value,
      typename dax::cont::internal::ArrayHandleImplicitTraits<Value,
        Functor>::Tag,
      Device >
    >
{
  typedef ConceptMap< Field(Tags),
    dax::cont::ArrayHandle<
      Value,
      typename dax::cont::internal::ArrayHandleImplicitTraits<Value,
        Functor>::Tag,
      Device > > superclass;
  typedef dax::cont::ArrayHandleImplicit<Value, Functor, Device> HandleType;
public:
  ConceptMap(HandleType handle):
    superclass(handle)
    {}
};

/// \headerfile FieldArrayHandle.h dax/cont/arg/FieldArrayHandle.h
/// \brief Map permutation array handle to \c Field worklet parameters.
template <typename Tags, typename Value, typename Functor, typename Device>
class ConceptMap< Field(Tags), const dax::cont::ArrayHandleImplicit<Value,
                                                               Functor,
                                                               Device> > :
  public ConceptMap< Field(Tags),
    const dax::cont::ArrayHandle<
      Value,
      typename dax::cont::internal::ArrayHandleImplicitTraits<Value,
        Functor>::Tag,
      Device >
    >
{
  typedef ConceptMap< Field(Tags),
    const dax::cont::ArrayHandle<
      Value,
      typename dax::cont::internal::ArrayHandleImplicitTraits<Value,
        Functor>::Tag,
      Device > > superclass;

    typedef dax::cont::ArrayHandleImplicit<Value, Functor, Device> HandleType;
public:
  ConceptMap(HandleType handle):
    superclass(handle)
    {}
};

} } } //namespace dax::cont::arg

#endif //__dax_cont_arg_FieldArrayHandleImplicit_h
