#!/usr/bin/python
#from tiles import test
import sys
sys.path.append("./python/stdlib.zip")
sys.path.append(".")

DIRECTION_DOWN = 0
DIRECTION_LEFT = 1
DIRECTION_UP = 2
DIRECTION_RIGHT = 3

import adv
setattr(__builtins__, "loadSpritesheet", adv.loadSpritesheet)
setattr(__builtins__, "createAnimation", adv.createAnimation)
setattr(__builtins__, "getPlayer", adv.getPlayer)
setattr(__builtins__, "DIRECTION_UP", DIRECTION_UP)
setattr(__builtins__, "DIRECTION_LEFT", DIRECTION_LEFT)
setattr(__builtins__, "DIRECTION_DOWN", DIRECTION_DOWN)
setattr(__builtins__, "DIRECTION_RIGHT", DIRECTION_RIGHT)

from tiles import grass
from tiles import rock
from maps import level1
from monsters import player
