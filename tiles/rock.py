#!/usr/bin/python
# -*- coding: utf-8 -*-
import tile

class Rock(tile.Tile):
	"""Docstring for test. """

	sprite_animation = loadAnimation("rock1.png")
	print "Animation id: %d\n" % sprite_animation["__id__"]

	def playerEnter(self):
		print "rock enter"
		pass
	def playerExit(self):
		print "rock exit"
		pass
