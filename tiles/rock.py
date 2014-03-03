#!/usr/bin/python
# -*- coding: utf-8 -*-
import tile
import adv

class Rock(tile.Tile):
    spritesheet = adv.loadSpritesheet("rock1.png")
    walkable = 0
    visibility = 0

    def playerEnter(self):
        print "rock enter"
        pass
    def playerExit(self):
        print "rock exit"
        pass
