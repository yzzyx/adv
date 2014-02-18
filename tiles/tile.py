#!/usr/bin/python
# -*- coding: utf-8 -*-

class Tile():
	sprite_animation = None
	walkable = 1		# Can the player walk on it?
	visibility = 1		# Can the player see through it?
	is_dirty = 0

	def playerEnter(self):
		pass
	def playerExit(self):
		pass
	def tick(self):
		pass


