This repo contains several folders, representing different components

First we have simple_am_java, which was used at first to test
the connectivity between java and swim, and was used to test API calls.
It was also used at last and the hybrid planning algorithm (although
only the environment model generation) is included.

prism_fix_path has the code that given a path to the prism file, calls
prism and returns the result to the same folder.

planDB_connect has the code that given a path to the prism generated
files, parse them and store them in the database.

proactive, proactive_connect, proactive_pladapt were all folders related
to timeseries, and they were used to test the timeseries code when wrapped
using JNI.
