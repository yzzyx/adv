#!/usr/bin/python
# -*- coding: utf-8 -*-
import tile

class Grass(tile.Tile):
	"""Docstring for test. """

	sprite_animation = loadAnimation("grass1.png")
	counter = 0

	def __init__(self):
		print "python:", self.sprite_animation

	def info(self):
		print "grass %d" % self.counter
		self.counter = self.counter + 1
