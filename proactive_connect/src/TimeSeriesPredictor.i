/* TimeSeriesPredictor.i */
#ifdef SWIG
%module TSP
%{
/* Includes the header in the wrapper code */
#include <string>
#include <vector>
#include "TimeSeriesPredictor.h"
%}
#endif

/* Parse the header file to generate wrappers */
%include "std_string.i"
%include "std_vector.i"
%include "TimeSeriesPredictor.h"
