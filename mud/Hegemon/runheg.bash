#!/bin/bash
#  This will start the Hegemon Java Client.

# If you want to be able to run this script from anywhere,
# have this line do the right thing:

# cd ~/hegemon


# Don't need a CLASSPATH when running the JRE.
unset CLASSPATH

# Start up the GUI.
./jre/bin/java -cp heg.jar hegemon

