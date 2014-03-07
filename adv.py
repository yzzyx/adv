import _adv

DIRECTION_DOWN = 0
DIRECTION_LEFT = 1
DIRECTION_UP = 2
DIRECTION_RIGHT = 3

"""
Different object types are
handled somewhat differently,
e.g. a weapon can be wielded, armor worn
"""
OBJECT_STANDARD = 0
OBJECT_WEAPON = 1
OBJECT_ARMOR = 2

loadSpritesheet = _adv.loadSpritesheet
createAnimation = _adv.createAnimation
getPlayer = _adv.getPlayer
getDistance = _adv.getDistance
getPath = _adv.getPath
isVisible = _adv.isVisible
monster_gotoPosition = _adv.monster_gotoPosition
monster_gotoDirection = _adv.monster_gotoDirection
monster_attack = _adv.monster_attack
