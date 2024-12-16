import pygame as pg

from pygame.math import Vector2 as vec2
from define import  WIN_W, WIN_H, MAX_MAP_W, MAX_MAP_H, MAP_DETAIL,\
                    PARAM_POINTS, MAP_COLOR, MAP_SPLIT_SIZE

class Terrain:
    def __init__(self):
        self.size = MAX_MAP_W * MAP_DETAIL
        self.draw_w = WIN_W / (self.size - 1)
        self.draw_h = (WIN_H - 50) / MAX_MAP_H
        self.parse_params(PARAM_POINTS)
        self.generate_points()
        self.generate_lines()
        # self.merge_lines()
        self.split_lines()


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


    def generate_points(self):
        self.points = []

        for i in range(0, len(self.heightmap)):
            x = int(i * self.draw_w)
            y = WIN_H - 20 - (self.heightmap[i][0] * self.draw_h)
            self.points.append((x, y))


    def generate_lines(self):
        self.lines = []
        for i in range(0, len(self.points) - 1):
            p1 = vec2(self.points[i])
            p2 = vec2(self.points[i + 1])
            dir = p1 - p2
            n = vec2(-dir.y, dir.x).normalize()
            line = (p1, p2, n, -dir, max(p1.y, p2.y))
            self.lines.append(line)


    def merge_lines(self):
        print(f"nb lines : {len(self.lines)}")

        lines = []
        currLine = None
        for line in self.lines:
            if currLine == None:
                currLine = line
                continue

            if currLine[2] != line[2]:
                lines.append(currLine)
                currLine = line
                continue

            currLine = (currLine[0], line[1], currLine[2])

        lines.append(currLine)
        self.lines = lines

        print(f"nb lines : {len(self.lines)}")


    def split_lines(self):
        self.splited_lines = []
        self.nb_splited_lines = WIN_W // MAP_SPLIT_SIZE + 1
        for i in range(self.nb_splited_lines):
            lines = []
            for line in self.lines:
                if line[0].x >= (i - 5) * MAP_SPLIT_SIZE and\
                    line[1].x <= (i + 6) * MAP_SPLIT_SIZE:
                    lines.append(line)
            self.splited_lines.append(lines)


    def get_lines(self) -> list:
        return self.lines


    def get_split_lines(self, x: float) -> list:
        return self.splited_lines[int(x / MAP_SPLIT_SIZE)]


    def draw(self, window):
        for line in self.lines:
            pg.draw.line(window, MAP_COLOR, line[0], line[1])
            # mid = (line[0] - line[1]) / 2 + line[1]
            # nmid = mid + line[2] * 10
            # pg.draw.line(window, (255, 0, 0), mid, nmid)


