#!/usr/bin/python
# -*- coding: utf-8 -*-
import monster

class Player(monster.Monster):
	x = 1
	y = 8
	speed = 3
	sprite_animation = loadAnimation("player1.png")
