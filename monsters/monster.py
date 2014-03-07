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
    speed = 5
    attack_speed = 5
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
        """ Called from engine whenever self.timer has timed out """
        pass

    def canSeePlayer(self):
        pass

    def isHit(self):
        """ Called when monster is hit and loses HP """
        pass

    def isDead(self):
        """
        Called when monster has a HP of 0

        When this call returns, the monster will be deleted
        """
        pass

    def getMovementSpeed(self):
        """
        Should always return the current movement speed

        Note that this function must also factor in any
        modifiers that is a result from  objects in the
        monsters invetory

        Is called by the engine when the is_dirty-flag
        has been updated
        """
        return self.speed

    def getAttackSpeed(self):
        """
        Should always return the current attack speed

        Note that this function must also factor in any
        modifiers that is a result from  objects in the
        monsters invetory

        Is called by the engine when the is_dirty-flag
        has been updated
        """
        return self.attack_speed;
        pass

    def hpUpdate(self):
        pass

    def mpUpdate(self):
        pass

    def gotoPosition(self, x, y):
        """
        Try to move the monster to position (x,y)
        If the pathfinder can't, it will start walking towards
        the closest possible tile
        """
        adv.monster_gotoPosition(self, x, y)

    def gotoDirection(self, direction):
        """
        Walk one step in 'direction', which is one of
        adv.DIRECTION_DOWN, adv.DIRECTION_LEFT,
        adv.DIRECTION_UP and adv.DIRECTION_RIGHT
        """
        adv.monster_gotoDirection(self, direction)

    def attack(self, x, y):
        """
        Attack tile at x,y
        """
        adv.monster_attack(self, x, y)

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
