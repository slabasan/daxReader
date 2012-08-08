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

#define DAX_DEVICE_ADAPTER DAX_DEVICE_ADAPTER_ERROR
#define BOOST_SP_DISABLE_THREADS

// Tests math functions that rely on system math functions in the Cuda runtime
// environment. Ensures that the Cuda versions of the functions are behaving
// the same as the standard C math library functions.

#include <dax/cuda/cont/DeviceAdapterCuda.h>

#include <dax/exec/math/Compare.h>
#include <dax/exec/math/Exp.h>
#include <dax/exec/math/Precision.h>
#include <dax/exec/math/Sign.h>
#include <dax/exec/math/Trig.h>

#include <dax/exec/internal/ErrorMessageBuffer.h>

#include <dax/exec/Assert.h>

#include <dax/cuda/cont/internal/Testing.h>

namespace ut_CudaMath {

#define MY_ASSERT(condition, message) \
  if (!(condition)) \
    { \
    this->ErrorMessage.RaiseError( \
          __FILE__ ":" __DAX_ASSERT_EXEC_STRINGIFY(__LINE__) ": " message \
          " (" #condition ")"); \
    return; \
    }

struct TestCompareKernel
{
  DAX_EXEC_EXPORT void operator()(dax::Id) const
  {
    MY_ASSERT(dax::exec::math::Min(3, 8) == 3, "Got wrong min.");
    MY_ASSERT(dax::exec::math::Min(-0.1f, -0.7f) == -0.7f, "Got wrong min.");
    MY_ASSERT(dax::exec::math::Max(3, 8) == 8, "Got wrong max.");
    MY_ASSERT(dax::exec::math::Max(-0.1f, -0.7f) == -0.1f, "Got wrong max.");
  }

  dax::exec::internal::ErrorMessageBuffer ErrorMessage;
  DAX_CONT_EXPORT
  void SetErrorMessageBuffer(
      const dax::exec::internal::ErrorMessageBuffer &errorMessage)
  {
    this->ErrorMessage = errorMessage;
  }
};

struct TestExpKernel
{
  DAX_EXEC_EXPORT void operator()(dax::Id) const
  {
    MY_ASSERT(test_equal(dax::exec::math::Pow(0.25, 2.0), dax::Scalar(0.0625)),
              "Bad power result.");
    MY_ASSERT(test_equal(dax::exec::math::Sqrt(3.75),
                         dax::exec::math::Pow(3.75, 0.5)),
              "Bad sqrt result.");
    MY_ASSERT(test_equal(dax::exec::math::RSqrt(3.75),
                         dax::exec::math::Pow(3.75, -0.5)),
              "Bad reciprocal sqrt result.");
    MY_ASSERT(test_equal(dax::exec::math::Cbrt(3.75),
                         dax::exec::math::Pow(3.75, 1.0/3.0)),
              "Bad cbrt result.");
    MY_ASSERT(test_equal(dax::exec::math::RCbrt(3.75),
                         dax::exec::math::Pow(3.75, -1.0/3.0)),
              "Bad reciprocal cbrt result.");
    MY_ASSERT(test_equal(dax::exec::math::Exp(3.75),
                         dax::exec::math::Pow(2.71828183, 3.75)),
              "Bad exp result.");
    MY_ASSERT(test_equal(dax::exec::math::Exp2(3.75),
                         dax::exec::math::Pow(2.0, 3.75)),
              "Bad exp2 result.");
    MY_ASSERT(test_equal(dax::exec::math::ExpM1(3.75),
                         dax::exec::math::Pow(2.71828183, 3.75)-dax::Scalar(1)),
              "Bad expm1 result.");
    MY_ASSERT(test_equal(dax::exec::math::Exp10(3.75),
                         dax::exec::math::Pow(10.0, 3.75)),
              "Bad exp2 result.");
    MY_ASSERT(test_equal(dax::exec::math::Log2(dax::Scalar(0.25)),
                         dax::Scalar(-2.0)),
              "Bad value from Log2");
    MY_ASSERT(
          test_equal(dax::exec::math::Log2(dax::make_Vector4(0.5, 1.0, 2.0, 4.0)),
                     dax::make_Vector4(-1.0, 0.0, 1.0, 2.0)),
          "Bad value from Log2");
    MY_ASSERT(test_equal(dax::exec::math::Log(3.75),
                         dax::exec::math::Log2(3.75)/dax::exec::math::Log2(2.71828183)),
              "Bad log result.");
    MY_ASSERT(test_equal(dax::exec::math::Log10(3.75),
                         dax::exec::math::Log(3.75)/dax::exec::math::Log(10.0)),
              "Bad log10 result.");
    MY_ASSERT(test_equal(dax::exec::math::Log1P(3.75),
                         dax::exec::math::Log(4.75)),
              "Bad log1p result.");
  }

  dax::exec::internal::ErrorMessageBuffer ErrorMessage;
  DAX_CONT_EXPORT
  void SetErrorMessageBuffer(
      const dax::exec::internal::ErrorMessageBuffer &errorMessage)
  {
    this->ErrorMessage = errorMessage;
  }
};

struct TestPrecisionKernel
{
  DAX_EXEC_EXPORT void operator()(dax::Id) const
  {
    dax::Scalar zero = 0.0;
    dax::Scalar finite = 1.0;
    dax::Scalar nan = dax::exec::math::Nan();
    dax::Scalar inf = dax::exec::math::Infinity();
    dax::Scalar neginf = dax::exec::math::NegativeInfinity();
    dax::Scalar epsilon = dax::exec::math::Epsilon();

    // General behavior.
    MY_ASSERT(nan != nan, "Nan not equal itself.");
    MY_ASSERT(!(nan >= zero), "Nan not greater or less.");
    MY_ASSERT(!(nan <= zero), "Nan not greater or less.");
    MY_ASSERT(!(nan >= finite), "Nan not greater or less.");
    MY_ASSERT(!(nan <= finite), "Nan not greater or less.");

    MY_ASSERT(neginf < inf, "Infinity big");
    MY_ASSERT(zero < inf, "Infinity big");
    MY_ASSERT(finite < inf, "Infinity big");
    MY_ASSERT(zero > neginf, "-Infinity small");
    MY_ASSERT(finite > neginf, "-Infinity small");

    MY_ASSERT(zero < epsilon, "Negative epsilon");
    MY_ASSERT(finite > epsilon, "Large epsilon");

    // Math check functions.
    MY_ASSERT(!dax::exec::math::IsNan(zero), "Bad IsNan check.");
    MY_ASSERT(!dax::exec::math::IsNan(finite), "Bad IsNan check.");
    MY_ASSERT(dax::exec::math::IsNan(nan), "Bad IsNan check.");
    MY_ASSERT(!dax::exec::math::IsNan(inf), "Bad IsNan check.");
    MY_ASSERT(!dax::exec::math::IsNan(neginf), "Bad IsNan check.");
    MY_ASSERT(!dax::exec::math::IsNan(epsilon), "Bad IsNan check.");

    MY_ASSERT(!dax::exec::math::IsInf(zero), "Bad infinity check.");
    MY_ASSERT(!dax::exec::math::IsInf(finite), "Bad infinity check.");
    MY_ASSERT(!dax::exec::math::IsInf(nan), "Bad infinity check.");
    MY_ASSERT(dax::exec::math::IsInf(inf), "Bad infinity check.");
    MY_ASSERT(dax::exec::math::IsInf(neginf), "Bad infinity check.");
    MY_ASSERT(!dax::exec::math::IsInf(epsilon), "Bad infinity check.");

    MY_ASSERT(dax::exec::math::IsFinite(zero), "Bad finite check.");
    MY_ASSERT(dax::exec::math::IsFinite(finite), "Bad finite check.");
    MY_ASSERT(!dax::exec::math::IsFinite(nan), "Bad finite check.");
    MY_ASSERT(!dax::exec::math::IsFinite(inf), "Bad finite check.");
    MY_ASSERT(!dax::exec::math::IsFinite(neginf), "Bad finite check.");
    MY_ASSERT(dax::exec::math::IsFinite(epsilon), "Bad finite check.");

    MY_ASSERT(test_equal(dax::exec::math::FMod(6.5, 2.3), dax::Scalar(1.9)),
              "Bad fmod.");
    MY_ASSERT(test_equal(dax::exec::math::Remainder(6.5, 2.3),
                         dax::Scalar(-0.4)),
              "Bad remainder.");
    dax::Scalar remainder, quotient;
    remainder = dax::exec::math::RemainderQuotient(6.5, 2.3, quotient);
    MY_ASSERT(test_equal(remainder, dax::Scalar(-0.4)), "Bad remainder.");
    MY_ASSERT(test_equal(quotient, dax::Scalar(3.0)), "Bad quotient.");
    dax::Scalar integral, fractional;
    fractional = dax::exec::math::ModF(4.6, integral);
    MY_ASSERT(test_equal(integral, dax::Scalar(4.0)), "Bad integral.");
    MY_ASSERT(test_equal(fractional, dax::Scalar(0.6)), "Bad fractional.");
    MY_ASSERT(test_equal(dax::exec::math::Floor(4.6), dax::Scalar(4.0)),
              "Bad floor.");
    MY_ASSERT(test_equal(dax::exec::math::Ceil(4.6), dax::Scalar(5.0)),
              "Bad ceil.");
    MY_ASSERT(test_equal(dax::exec::math::Round(4.6), dax::Scalar(5.0)),
              "Bad round.");
  }

  dax::exec::internal::ErrorMessageBuffer ErrorMessage;
  DAX_CONT_EXPORT
  void SetErrorMessageBuffer(
      const dax::exec::internal::ErrorMessageBuffer &errorMessage)
  {
    this->ErrorMessage = errorMessage;
  }
};

struct TestSignKernel
{
  DAX_EXEC_EXPORT void operator()(dax::Id) const
  {
    MY_ASSERT(dax::exec::math::Abs(-1) == 1, "Bad abs.");
    MY_ASSERT(dax::exec::math::Abs(dax::Scalar(-0.25)) == 0.25, "Bad abs.");
    MY_ASSERT(dax::exec::math::IsNegative(-3.1), "Bad negative.");
    MY_ASSERT(!dax::exec::math::IsNegative(3.2), "Bad positive.");
    MY_ASSERT(!dax::exec::math::IsNegative(0.0), "Bad non-negative.");
    MY_ASSERT(dax::exec::math::CopySign(-0.25, 100.0) == 0.25, "Copy sign.");
  }

  dax::exec::internal::ErrorMessageBuffer ErrorMessage;
  DAX_CONT_EXPORT
  void SetErrorMessageBuffer(
      const dax::exec::internal::ErrorMessageBuffer &errorMessage)
  {
    this->ErrorMessage = errorMessage;
  }
};

struct TestTrigKernel
{
  DAX_EXEC_EXPORT void operator()(dax::Id) const
  {
    MY_ASSERT(test_equal(dax::exec::math::Pi(), dax::Scalar(3.14159265)),
              "Pi not correct.");

    MY_ASSERT(test_equal(dax::exec::math::ATan2(0.0, 1.0),
                         dax::Scalar(0.0)),
              "ATan2 x+ axis.");
    MY_ASSERT(test_equal(dax::exec::math::ATan2(1.0, 0.0),
                         dax::Scalar(0.5*dax::exec::math::Pi())),
              "ATan2 y+ axis.");
    MY_ASSERT(test_equal(dax::exec::math::ATan2(-1.0, 0.0),
                         dax::Scalar(-0.5*dax::exec::math::Pi())),
              "ATan2 y- axis.");

    MY_ASSERT(test_equal(dax::exec::math::ATan2(1.0, 1.0),
                         dax::Scalar(0.25*dax::exec::math::Pi())),
              "ATan2 Quadrant 1");
    MY_ASSERT(test_equal(dax::exec::math::ATan2(1.0, -1.0),
                         dax::Scalar(0.75*dax::exec::math::Pi())),
              "ATan2 Quadrant 2");
    MY_ASSERT(test_equal(dax::exec::math::ATan2(-1.0, -1.0),
                         dax::Scalar(-0.75*dax::exec::math::Pi())),
              "ATan2 Quadrant 3");
    MY_ASSERT(test_equal(dax::exec::math::ATan2(-1.0, 1.0),
                         dax::Scalar(-0.25*dax::exec::math::Pi())),
              "ATan2 Quadrant 4");

    dax::Scalar angle = (1.0/3.0)*dax::exec::math::Pi();
    dax::Scalar opposite = dax::exec::math::Sqrt(3.0);
    dax::Scalar adjacent = 1.0;
    dax::Scalar hypotenuse = 2.0;
    MY_ASSERT(test_equal(dax::exec::math::Sin(angle), opposite/hypotenuse),
              "Sin failed test.");
    MY_ASSERT(test_equal(dax::exec::math::Cos(angle), adjacent/hypotenuse),
              "Cos failed test.");
    MY_ASSERT(test_equal(dax::exec::math::Tan(angle), opposite/adjacent),
              "Tan failed test.");
    MY_ASSERT(test_equal(dax::exec::math::ASin(opposite/hypotenuse), angle),
              "Arc Sin failed test.");
    MY_ASSERT(test_equal(dax::exec::math::ACos(adjacent/hypotenuse), angle),
              "Arc Cos failed test.");
    MY_ASSERT(test_equal(dax::exec::math::ATan(opposite/adjacent), angle),
              "Arc Tan failed test.");
  }

  dax::exec::internal::ErrorMessageBuffer ErrorMessage;
  DAX_CONT_EXPORT
  void SetErrorMessageBuffer(
      const dax::exec::internal::ErrorMessageBuffer &errorMessage)
  {
    this->ErrorMessage = errorMessage;
  }
};

template<class Functor>
void TestSchedule(Functor functor)
{
  dax::cont::internal::Schedule(functor,
                                1,
                                dax::cuda::cont::DeviceAdapterTagCuda());
}

void TestCudaMath()
{
  std::cout << "Compare functions" << std::endl;
  TestSchedule(TestCompareKernel());

  std::cout << "Exponential functions" << std::endl;
  TestSchedule(TestExpKernel());

  std::cout << "Precision functions" << std::endl;
  TestSchedule(TestPrecisionKernel());

  std::cout << "Sign functions" << std::endl;
  TestSchedule(TestSignKernel());

  std::cout << "Trig functions" << std::endl;
  TestSchedule(TestTrigKernel());
}

} // namespace ut_CudaMath

//-----------------------------------------------------------------------------
int UnitTestCudaMath(int, char *[])
{
  return dax::cuda::cont::internal::Testing::Run(ut_CudaMath::TestCudaMath);
}
