/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __fncExecutive_h
#define __fncExecutive_h

#include "fncObject.h"

class fncModule;
class fncPort;
fncDefinePtrMacro(fncModule);
fncDefinePtrMacro(fncPort);

/// fncExecutive is the executive. One typically creates a single executive and
/// then registers module connections with that execute. To trigger an
/// execution, one finally calls fncExecutive::Execute().
class fncExecutive : public fncObject
{
public:
  fncExecutive();
  virtual ~fncExecutive();
  fncTypeMacro(fncExecutive, fncObject);

  /// Executes the pipelines. Return false if there's some error.
  bool Execute();

  /// Register a connection. Returns true if the connection was setup correctly.
  bool Connect(
    fncModulePtr sourceModule, fncPortPtr sourcePort,
    fncModulePtr sinkModule, fncPortPtr sinkPort);
  bool Connect(
    fncModulePtr sourceModule, const std::string& sourcename,
    fncModulePtr sinkModule, const std::string& sinkname);

  /// Resets the executive. This is the only way to break connections until we
  /// start supporting de-connecting.
  void Reset();

private:
  fncDisableCopyMacro(fncExecutive);

  class fncInternals;
  fncInternals* Internals;
};

fncDefinePtrMacro(fncExecutive);

#endif
