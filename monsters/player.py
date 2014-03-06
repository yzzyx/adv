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

    spritesheet_naked = adv.loadSpritesheet("dansken.png")
    spritesheet_clothing = adv.loadSpritesheet("dansken_pakladd_sprites.png")

    animation_stopped = [adv.createAnimation(spritesheet_clothing, 0, 2),
                         adv.createAnimation(spritesheet_clothing, 0, 2),
                         adv.createAnimation(spritesheet_clothing, 0, 2),
                         adv.createAnimation(spritesheet_clothing, 0, 2)]
    animation_moving = [adv.createAnimation(spritesheet_clothing, 0, 2),
                        adv.createAnimation(spritesheet_clothing, 0, 2),
                        adv.createAnimation(spritesheet_clothing, 0, 2),
                        adv.createAnimation(spritesheet_clothing, 0, 2)]

    def tick(self):
        pass
