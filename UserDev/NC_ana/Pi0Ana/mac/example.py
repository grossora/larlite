import sys
from ROOT import gSystem
gSystem.Load("libNC_ana_Pi0Ana")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing Pi0Ana..."

sys.exit(0)

