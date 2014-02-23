#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster

class Player(monster.Monster):
	x = 1
	y = 4
	target_x = x
	target_y = y
	speed = 4
	timer = 4
	sprite_animation = loadAnimation("player1.png")

	def tick(self):
		print "player TICK"
