#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster

class Gnome(monster.Monster):
    x = 10
    y = 10
    target_x = x
    target_y = y
    speed = 4
    timer = 4
    has_directions = 0
    spritesheet = loadSpritesheet("player1.png")

    animation_stopped = [ createAnimation(spritesheet, 0, 1) ];
    animation_moving = [ createAnimation(spritesheet, 0, 5) ];

    def tick(self):
        pass
