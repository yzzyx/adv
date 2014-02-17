#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster

class Player(monster.Monster):
	x = 3
	y = 3
	speed = 5
	sprite_animation = loadAnimation("player1.png")
	print "Player animation-id: %d\n" % sprite_animation["__id__"]
