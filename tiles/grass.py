#!/usr/bin/python
# -*- coding: utf-8 -*-
import tile

class Grass(tile.Tile):
	"""Docstring for test. """

	sprite_animation = loadAnimation("grass1.png")
	print "Animation id: %d\n" % sprite_animation["__id__"]
	counter = 0

	def info(self):
		print "grass %d" % self.counter
		self.counter = self.counter + 1
