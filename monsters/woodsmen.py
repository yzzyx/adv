#!/usr/bin/python
# -*- coding: utf-8 -*-
import random
import monster
import adv

class CrazyLumberjack(monster.Monster):
    x = 10
    y = 10
    target_x = x
    target_y = y
    speed = 2
    timer = 2
    has_directions = 1
    spritesheet = adv.loadSpritesheet("crazy_lumberjack.png")

    animation_stopped = [adv.createAnimation(spritesheet, 0, 2),
                         adv.createAnimation(spritesheet, 2, 2),
                         adv.createAnimation(spritesheet, 4, 2),
                         adv.createAnimation(spritesheet, 6, 2)]
    animation_moving = [adv.createAnimation(spritesheet, 0, 2),
                        adv.createAnimation(spritesheet, 2, 2),
                        adv.createAnimation(spritesheet, 4, 2),
                        adv.createAnimation(spritesheet, 6, 2)]

    def getDistanceTo(self, x, y):
        return abs(self.x - x) + abs(self.y - y)

    def tick(self):
        player = adv.getPlayer()
        if self.getDistanceTo(player.x, player.y) < 6:
            self.target_x, self.target_y = player.x, player.y
        else:
            self.target_x, self.target_y = self.x + random.randint(-1, 1), self.y + random.randint(-1, 1)
            self.target_x = max(0, self.target_x)
            self.target_y = max(0, self.target_y)
            self.target_x = min(24, self.target_x)
            self.target_y = min(32, self.target_y)
        self.is_dirty = True
