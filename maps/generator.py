import random
from monsters.gnome import GreenGoblin, GreenGoblinKing, Gnome
from monsters.woodsmen import CrazyLumberjack
from tiles import grass

class WeightedRandom:
    def __init__(self, pairs):
        self.pairs = pairs
        self.total = sum(self.pairs.values())

    def choice(self):
        r = random.randint(1, self.total)

        for value, weight in self.pairs.items():
            r -= weight
            if r <= 0:
                return value

    def addChance(self, key, chance, ignore_missing=True):
        if ignore_missing and key not in self.pairs:
            return
        self.pairs[key] += chance
        self.total = sum(self.pairs.values())

class MapGenerator:
    def generateLevel(self, m):
        return []


class RandomGrassGenerator(MapGenerator):
    def generateLevel(self, m):
        monsters = []
        for row_n, row in enumerate(m.tiles):
            for col_n, tile in enumerate(row):
                if isinstance(tile, grass.Grass):
                    wr = WeightedRandom(
                        {grass.Grass: 10,
                         grass.MediumGrass: 5,
                         grass.DarkGrass: 2,
                         grass.YellowGrass: 3}
                    )

                    if row_n > 0:
                        if col_n > 0:
                            wr.addChance(m.tiles[row_n - 1][col_n - 1].__class__, 7)
                        wr.addChance(m.tiles[row_n - 1][col_n].__class__, 7)
                        if col_n < len(row) - 1:
                            wr.addChance(m.tiles[row_n - 1][col_n + 1].__class__, 7)
                    if col_n > 0:
                        wr.addChance(m.tiles[row_n][col_n - 1].__class__, 7)
                    m.tiles[row_n][col_n] = wr.choice()()

        for _ in range(0, m.width * m.height // 75):
            wr = WeightedRandom(
                {CrazyLumberjack: 5,
                 GreenGoblin: 10,
                 GreenGoblinKing: 2,
                 Gnome: 3}
            )

            while True:
                x, y = random.randint(0, m.width - 1), random.randint(0, m.height - 1)
                for monster in monsters:
                    if monster.x == x and monster.y == y:
                        continue
                monsters.append(wr.choice()(x, y))
                break

        return monsters