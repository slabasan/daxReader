/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __daxSystemIncludes_h
#define __daxSystemIncludes_h

# include <iostream>  // Include real ansi istream and ostream.
# include <fstream>   // Include real ansi ifstream and ofstream.
# include <iomanip>   // Include real ansi io manipulators.

// Need these in global namespace so the same code will work with ansi
// and old-style streams.
using std::dec;
using std::hex;
using std::setw;
using std::setfill;
using std::setprecision;
using std::cerr;
using std::cout;
using std::cin;
using std::ios;
using std::endl;
using std::ends;
using std::ostream;
using std::istream;
using std::ofstream;
using std::ifstream;
using std::fstream;

#include <string>
#endif