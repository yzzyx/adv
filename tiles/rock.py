#!/usr/bin/python
# -*- coding: utf-8 -*-
import tile

class Rock(tile.Tile):
	"""Docstring for test. """

	sprite_animation = loadAnimation("rock1.png")
	walkable = 0

	def playerEnter(self):
		print "rock enter"
		pass
	def playerExit(self):
		print "rock exit"
		pass
