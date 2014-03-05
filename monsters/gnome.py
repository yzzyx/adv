#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster
import adv
import random

class Gnome(monster.Monster):
    x = 10
    y = 10
    speed = 4
    timer = 1
    has_directions = 0
    spritesheet = adv.loadSpritesheet("player1.png")

    animation_stopped = [adv.createAnimation(spritesheet, 0, 1)]
    animation_moving = [adv.createAnimation(spritesheet, 0, 1)]

    def tick(self):
        self.gotoDirection(random.randint(0, 4))

class GreenGoblin(monster.Monster):
    x = 10
    y = 10
    target_x = x
    target_y = y
    speed = 1
    timer = 4
    has_directions = 1
    spritesheet = adv.loadSpritesheet("green_goblin.png")

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
            self.gotoPosition(player.x, player.y)
        else:
            dirs = range(0,4)
            random.shuffle(dirs)
            for direction in dirs:
                if self.gotoDirection(direction):
                    break
        self.is_dirty = True


class GreenGoblinKing(monster.Monster):
    x = 10
    y = 10
    speed = 1
    timer = 4
    has_directions = 1
    spritesheet = adv.loadSpritesheet("green_goblin_king.png")

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
            self.gotoPosition(player.x, player.y)
        else:
            dirs = range(0,4)
            random.shuffle(dirs)
            for direction in dirs:
                if self.gotoDirection(direction):
                    break
            self.target_x, self.target_y = player.x, player.y
        self.is_dirty = True

"""
class SwarmGoblin(monster.Monster):
    x = 10
    y = 10
    target_x = x
    target_y = y
    speed = 4
    timer = 4
    has_directions = 0
    spritesheet = adv.loadSpritesheet("swarm_goblin.png")

    animation_stopped = [adv.createAnimation(spritesheet, 0, 1) ]
    animation_moving = [adv.createAnimation(spritesheet, 0, 2) ]

    def tick(self):
        player = adv.getPlayer()
        player_dist = self.getDistanceTo(player.x, player.y)
        if player_dist < 10:
            if player_dist == 1:
                print "%s attacked!" % self.__class__.__name__
            elif player_dist < 5:
                close_monsters = adv.getMonstersWithin(self.__class__, 8)
                if len(close_monsters) > 2:
                    self.target_x, self.target_y = player.x, player.y
                elif close_monsters:
                    self.target_x, self.target_y = close_monsters[0].x, close_monsters[0].y
                else:
                    self._flee(player)
            self.target_x, self.target_y = player.x, player.y
        else:
            self.target_x = self.x + random.randint(-1, 1)
            self.target_y = self.y + random.randint(-1, 1)

    def _flee(self, target):
        flee_x = self.x - (self.x - target.x)
        if flee_x == 0:
            self.target_x = self.x
        else:
            self.target_x = self.x - 1 if flee_x < 0 else self.x + 1

        flee_y = self.y - (self.y - target.y)
        if flee_y == 0:
            self.target_y = self.y
        else:
            self.target_y = self.y - 1 if flee_y < 0 else self.y + 1
"""
