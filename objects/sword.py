#!/usr/bin/python
# -*- coding: utf-8 -*-
import adv
import base_objects


class SimpleSword(base_objects.Weapon):
    spritesheet = adv.loadSpritesheet("./img/sword.png")
    animation = adv.createAnimation(spritesheet, 0, 1)
