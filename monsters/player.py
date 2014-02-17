#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster

class Player(monster.Monster):
	x = 3
	y = 3
	speed = 3
	sprite_animation = loadAnimation("player1.png")
