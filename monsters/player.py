#!/usr/bin/python
# -*- coding: utf-8 -*-
import adv
import monster

class Player(monster.Monster):
    x = 1
    y = 8
    target_x = x
    target_y = y
    speed = 4
    timer = 4
    has_directions = 1
    direction = adv.DIRECTION_DOWN

    spritesheet = adv.loadSpritesheet("player.png")

    animation_stopped = [adv.createAnimation(spritesheet, 0, 1),
                         adv.createAnimation(spritesheet, 5, 1),
                         adv.createAnimation(spritesheet, 10, 1),
                         adv.createAnimation(spritesheet, 15, 1)]
    animation_moving = [adv.createAnimation(spritesheet, 0, 5),
                        adv.createAnimation(spritesheet, 5, 5),
                        adv.createAnimation(spritesheet, 10, 5),
                        adv.createAnimation(spritesheet, 15, 5)]

    def tick(self):
        pass
