#!/usr/bin/python
#from tiles import test
import sys
sys.path.append(".")

import adv
setattr(__builtins__, "loadAnimation", adv.loadAnimation)

from tiles import grass
from tiles import rock
from maps import level1
from monsters import player
