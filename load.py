#!/usr/bin/python
#from tiles import test
import sys
sys.path.append(".")

import adv
setattr(__builtins__, "loadSpritesheet", adv.loadSpritesheet)
setattr(__builtins__, "createAnimation", adv.createAnimation)

from tiles import grass
from tiles import rock
from maps import level1
from monsters import player
