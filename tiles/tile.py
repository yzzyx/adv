#!/usr/bin/python
# -*- coding: utf-8 -*-

class Tile():
	walkable = 1		# Can the player walk on it?
	visibility = 1		# Can the player see through it?
	is_dirty = 0
	spritesheet = None
	spriteid = 0		# Tiles are static - just one sprite per tile

	def playerEnter(self):
		pass
	def playerExit(self):
		pass
	def tick(self):
		pass


