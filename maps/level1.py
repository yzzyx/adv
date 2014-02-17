#!/usr/bin/python
# -*- coding: utf-8 -*-
import map
from tiles.grass import Grass
from tiles.rock import Rock

class level1(map.Map):
	tiles = [
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(),  Rock(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			[ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass() ],
			]
	"""
	tiles = [ [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Rock, Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  [ Grass(), Grass(), Grass(), Grass() ],
			  ]
	"""
	width = 10
	height = 22

	def generate(self):
		pass
	def getTiles(self):
		pass
 

