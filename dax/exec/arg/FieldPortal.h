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
#ifndef __dax_exec_arg_FieldPortal_h
#define __dax_exec_arg_FieldPortal_h

#include <dax/Types.h>
#include <dax/cont/sig/Tag.h>

#include <boost/mpl/if.hpp>
#include <boost/utility/enable_if.hpp>


namespace dax { namespace exec { namespace arg {


namespace detail
{
template <typename T, typename ReferenceType,
          typename  Tags, class Enable = void>
struct ValueStorage
{
  //empty storage for read only portals

  template<typename PortalType>
  DAX_EXEC_EXPORT ReferenceType Get(int index, PortalType const& portal) const
    {
    return portal.Get(index);
    }

  template<typename PortalType>
  DAX_EXEC_EXPORT void Set(int,PortalType const&) const { }

  template<typename PortalType>
  DAX_EXEC_EXPORT void Set(int,PortalType const&,ReferenceType) const { }

};

template <typename T, typename ReferenceType, typename  Tags>
struct ValueStorage<T, ReferenceType, Tags,
    typename boost::enable_if<typename Tags::template Has<
    dax::cont::sig::Out> >::type>
{
  //actually store a value from the protal
  T Value;

  template<typename PortalType>
  DAX_EXEC_EXPORT ReferenceType Get(int index, PortalType const& portal)
    {
    this->Value = portal.Get(index);
    return this->Value;
    }

  template<typename PortalType>
  DAX_EXEC_EXPORT void Set(int index, PortalType const& portal) const
    {
    portal.Set(index,this->Value);
    }

  template<typename PortalType>
  DAX_EXEC_EXPORT void Set(int index, PortalType const& portal,
                           ReferenceType v) const
    {
    portal.Set(index,v);
    }

};
}

template <typename T, typename Tags, typename PortalExec, typename PortalConstExec>
struct FieldPortal
{
  typedef T ValueType;
protected:
  //What we have to do is use mpl::if_ to determine the type for
  //ExecArg
  typedef typename boost::mpl::if_<typename Tags::template Has<dax::cont::sig::Out>,
                                   PortalExec,
                                   PortalConstExec>::type PortalType;

  typedef typename boost::mpl::if_<typename Tags::template Has<dax::cont::sig::Out>,
                                   ValueType&,
                                   ValueType const&>::type ReferenceType;


  /*
  Todo for basic portals where the underlying storage is the same
  as the value type we want a flag so that we don't store a copy
  int the execPortal, but instead directly return a reference.
  */

  //determine if we need to store a local copy of the value we get from
  //the portal. Remember a portal can return Vector3 that can't be assigned
  //too as they actually might be created on the fly
  detail::ValueStorage< ValueType, ReferenceType, Tags > Storage;
public:
  PortalType Portal;

  typedef ReferenceType ReturnType;

  FieldPortal(): Storage(), Portal(){}

  DAX_EXEC_EXPORT ReturnType operator()(int index)
    {
    //if we have the In tag we have local store so use that value,
    //otherwise call the portal directly
    return this->Storage.Get(index,this->Portal);
    }

  //After needs to be tagged on out, since you get call .Set
  //on a input portal as that fails
  DAX_EXEC_EXPORT void SaveExecutionResult(int index) const
    {
    this->Storage.Set(index,this->Portal);
    }

  //After needs to be tagged on out, since you get call .Set
  //on a input portal as that fails
  DAX_EXEC_EXPORT void SaveExecutionResult(int index, ReferenceType v) const
    {
    this->Storage.Set(index,this->Portal,v);
    }
};


} } } //namespace dax::exec::arg



#endif //__dax_exec_arg_FieldPortal_h
