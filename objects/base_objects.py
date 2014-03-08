#!/usr/bin/python
# -*- coding: utf-8 -*-
import adv

class Object(object):
    x = 0
    y = 0
    timer = -1
    has_directions = 0
    spritesheet = None
    animation = None
    type = adv.OBJECT_STANDARD

    def __init__(self, x = -1, y = -1):
        if x != -1:
            self.x = x
        if y != -1:
            self.y = y

    def tick(self):
        """ Called from engine whenever self.timer has timed out """
        pass

    def movementSpeedModifier(self):
        """
        Called from monster.getMovementSpeed,

        this allows you to return a movement speed modifier
        that will be applied to the monster.
        Returns the modifier as an integer
        """
        return 0

    def attackSpeedModifier(self):
        """
        Called from monster.getAttackSpeed,

        this allows you to return an attack speed modifier
        that will be applied to the monster.
        Returns the modifier as an integer
        """
        return 0

    def use(self):
        """
        Called when a player/monster uses the object
        """
        pass

    def drop(self):
        """
        Called when a player/monster drops the object
        """
        pass

    def throw(self):
        """
        Called when a player/monster throws the object
        """
        pass


class Weapon(Object):
    type = adv.OBJECT_WEAPON

class Armor(Object):
    type = adv.OBJECT_ARMOR

