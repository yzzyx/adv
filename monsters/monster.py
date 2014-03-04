#!/usr/bin/python
# -*- coding: utf-8 -*-


# Functions available:
#
# moveTowards(x, y) - Calculate shortest path, and move in that direction
# attack(x, y)      - attack player/monster on tile x,y
# getTileType()     - get type of current tile
# throw(obj, x, y)  - throw object obj towards x, y
# drop(obj)         - drop object obj
# getTileObjectList() - get list of objects on current tile
# pickup(obj)        - pick up object obj
#
# Callbacks:
# enteredTile()
# canSeePlayer()
# lostPlayerVision()
# isHit()
#
import random
import adv

class Monster(object):
    x = 0
    y = 0
    target_x = 0
    target_y = 0
    hp = 100
    mp = 100
    speed = 2
    timer = -1
    is_dirty = 1
    has_directions = 1        # Monsters usually have 4 directions
    direction = adv.DIRECTION_DOWN

    inventory = None
    spritesheet = None
    animation = None
    animation_stopped = None
    animation_moving = None

    def __init__(self, x = -1, y = -1):
        if x != -1:
            self.x = x
        if y != -1:
            self.y = y
#        print "Created monster @ %d, %d" % (x,y)

    def tick(self):
        pass

    def canSeePlayer(self):
        pass

    def isHit(self):
        pass

    def hpUpdate(self):
        pass
    
    def mpUpdate(self):
        pass

#    def gotoPosition(self, x, y)
#        monster_gotoPosition(self, x, y)

class RandomAggro(object):
    def getDistanceTo(self, x, y):
        return abs(self.x - x) + abs(self.y - y)

    def tick(self):
        player = adv.getPlayer()
        if self.getDistanceTo(player.x, player.y) < 6:
            self.target_x, self.target_y = player.x, player.y
        else:
            self.target_x, self.target_y = self.x + random.randint(-1, 1), self.y + random.randint(-1, 1)
        self.is_dirty = True