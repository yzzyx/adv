#!/usr/bin/python
# -*- coding: utf-8 -*-
import random
import monster
import adv

class CrazyLumberjack(monster.Monster):
    x = 10
    y = 10
    speed = 4
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
        distance = self.getDistanceTo(player.x, player.y)
        if distance <= 2:
            self.attack(player.x, player.y)
        elif self.getDistanceTo(player.x, player.y) < 12:
            self.gotoPosition(player.x, player.y)
        else:
            dirs = range(0,4)
            random.shuffle(dirs)
            for direction in dirs:
                if self.gotoDirection(direction):
                    break
        self.is_dirty = True
