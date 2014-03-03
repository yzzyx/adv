#!/usr/bin/python
# -*- coding: utf-8 -*-
import map
from tiles.grass import Grass
from tiles.rock import Rock
from monsters.gnome import Gnome

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

    def generate(self):
        self.monsters = []

        self.monsters.append(Gnome(10, 10))

    def getTiles(self):
        pass
 

