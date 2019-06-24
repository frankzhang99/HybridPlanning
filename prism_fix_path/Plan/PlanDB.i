/* PlanDB.i */
#ifdef SWIG
%module DB
%{
/* Includes the header in the wrapper code */
#include <string>
#include <vector>
#include "PlanDB.h"
%}
#endif

/* Parse the header file to generate wrappers */
%include "std_string.i"
%include "std_vector.i"
%include "PlanDB.h"
