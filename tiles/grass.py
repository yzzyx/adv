#!/usr/bin/python
# -*- coding: utf-8 -*-
import tile

class Grass(tile.Tile):
	spritesheet = loadSpritesheet("grass1.png")

	counter = 0

	def info(self):
		print "grass %d" % self.counter
		self.counter = self.counter + 1
