/* AdaptationPlanner.i */
#ifdef SWIG
%module AP
%{
/* Includes the header in the wrapper code */
#include <string>
#include <vector>
#include <set>
#include "AdaptationPlanner.h"
%}
#endif

/* Parse the header file to generate wrappers */
%include "std_string.i"
%include "std_vector.i"
%include "std_set.i"
%include "AdaptationPlanner.h"
