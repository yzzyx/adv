#!/usr/bin/python
# -*- coding: utf-8 -*-
import map
from monsters.woodsmen import CrazyLumberjack
from tiles.grass import Grass
from maps.generator import RandomGrassGenerator
from tiles.rock import Rock
from monsters.gnome import Gnome, GreenGoblin
from objects.sword import SimpleSword

class level1(map.Map):
    tiles = [
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Rock()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(),  Rock(),  Rock(),  Rock(),  Rock(),  Rock(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
            [ Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass(), Grass()  ],
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
    width = 24
    height = 23

    objects = None

    def generate(self):
        grassgen = RandomGrassGenerator()
        self.monsters = grassgen.generateLevel(self)
        self.objects = []
        self.objects.append(SimpleSword(4,3))

    def getTiles(self):
        pass
 

