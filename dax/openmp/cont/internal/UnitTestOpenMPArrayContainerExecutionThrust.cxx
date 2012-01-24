/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <dax/openmp/cont/internal/ArrayContainerExecutionThrust.h>

#include <dax/cont/internal/IteratorContainer.h>

#include <dax/cont/internal/Testing.h>

#include <thrust/for_each.h>
#include <thrust/sequence.h>

namespace dax {
namespace openmp {
namespace cont {
namespace internal {
namespace ut_arraycontainer {

const dax::Id ARRAY_SIZE = 10;

//-----------------------------------------------------------------------------
struct AddOne
{
  DAX_EXEC_EXPORT void operator()(dax::Scalar &x) {
    x += 1;
  }
};

//-----------------------------------------------------------------------------
static void TestBasicTransfers()
{
  // Create original input array.
  dax::Scalar inputArray[ARRAY_SIZE];
  ::thrust::sequence(&inputArray[0], &inputArray[ARRAY_SIZE]);
  dax::cont::internal::IteratorContainer<dax::Scalar*>
      inputContainer(&inputArray[0], &inputArray[ARRAY_SIZE]);

  // Create the managed container for the array in the execution environment.
  dax::openmp::cont::internal::ArrayContainerExecutionThrust<dax::Scalar>
      executionContainer;
  executionContainer.Allocate(ARRAY_SIZE);

  // Copy the data to the execution environment.
  executionContainer.CopyFromControlToExecution(inputContainer);

  // Do something with the array on the device.
  ::thrust::for_each(executionContainer.GetBeginThrustIterator(),
                     executionContainer.GetEndThrustIterator(),
                     AddOne());

  // Make a destination to check results.
  dax::Scalar outputArray[ARRAY_SIZE];
  dax::cont::internal::IteratorContainer<dax::Scalar*>
      outputContainer(&outputArray[0], &outputArray[ARRAY_SIZE]);

  // Copy the results data back from the execution environment.
  executionContainer.CopyFromExecutionToControl(outputContainer);

  // Check the results.
  for (dax::Id index = 0; index < ARRAY_SIZE; index++)
    {
    std::cout << inputArray[index] << ", " << outputArray[index] << std::endl;
    DAX_TEST_ASSERT(outputArray[index] == index+1, "Bad result.");
    }
}

}
}
}
}
}

//-----------------------------------------------------------------------------
int UnitTestOpenMPArrayContainerExecutionThrust(int, char *[])
{
  using namespace dax::openmp::cont::internal::ut_arraycontainer;
  return dax::cont::internal::Testing::Run(TestBasicTransfers);
}
