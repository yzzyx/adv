#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster

class Player(monster.Monster):
    x = 1
    y = 8
    target_x = x
    target_y = y
    speed = 4
    timer = 4
    has_directions = 1
    direction = DIRECTION_DOWN

    spritesheet = loadSpritesheet("player.png")

    animation_stopped = [createAnimation(spritesheet, 0, 1),
                         createAnimation(spritesheet, 5, 1),
                         createAnimation(spritesheet, 10, 1),
                         createAnimation(spritesheet, 15, 1),
                         ];
    animation_moving = [ createAnimation(spritesheet, 0, 5),
                         createAnimation(spritesheet, 5, 5),
                         createAnimation(spritesheet, 10, 5),
                         createAnimation(spritesheet, 15, 5) ] ;

    def tick(self):
        pass
