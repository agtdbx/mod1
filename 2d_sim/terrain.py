import pygame as pg

from define import WIN_W, WIN_H, MAX_MAP_W, MAX_MAP_H, MAP_DETAIL, PARAM_POINTS

class Terrain:
    def __init__(self):
        self.size = MAX_MAP_W * MAP_DETAIL
        self.draw_w = WIN_W / self.size
        self.draw_h = (WIN_H - 50) / MAX_MAP_H
        self.parse_params(PARAM_POINTS)


    def generate_points(self):
        self.points = []

        for i in range(0, len(self.heightmap)):
            x = int(i * self.draw_w)
            y = WIN_H - 20 - (self.heightmap[i][0] * self.draw_h)
            self.points.append((x, y))


    def parse_params(self, params):
        self.heightmap = [(0, 0)] * self.size

        next_points = set()

        for x, y in params:
            x *= MAP_DETAIL
            if y > MAX_MAP_H:
                y = MAX_MAP_H
            self.heightmap[x] = (y, (x, y))
            next_points.add(x)

        next_change = [-1, 1]

        while len(next_points) > 0:
            new_points = set()

            for point in next_points:
                # get value
                x = point

                value, origin = self.heightmap[x]

                ox, oy = origin

                for change in next_change:
                    nx = x + change

                    if nx < 0 or nx >= self.size:
                        continue

                    dist = (ox - nx) / MAP_DETAIL
                    if dist == 0:
                        continue

                    val = max(0, oy**2 - dist**2)
                    val /= oy**2
                    val = val**3
                    new_val = val * oy

                    last_val = self.heightmap[nx][0]
                    if new_val <= last_val:
                        continue

                    self.heightmap[nx] = (new_val, origin)
                    if nx not in new_points:
                        new_points.add(nx)

            next_points = new_points.copy()

        self.generate_points()


    def draw(self, window):
        for i in range(1, len(self.points)):
            pg.draw.line(window, (200, 200, 200), self.points[i - 1], self.points[i])

