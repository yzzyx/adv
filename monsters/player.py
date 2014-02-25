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
	spritesheet = loadSpritesheet("player1.png")
	animation = createAnimation(spritesheet, 0, 1);

	def tick(self):
		pass
