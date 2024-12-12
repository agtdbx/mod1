import pygame as pg
import math

from pygame.math import Vector2 as vec2
from define import WATER_RADIUS, WATER_COLOR, WIN_W, WIN_H, AIR_FRICTION_RATIO

class Water:
    def __init__(self, x: int, y: int):
        self.pos = vec2(x, y)
        self.dir = vec2(0, 0)
        self.speed = 0


    def applyForce(self, dir: vec2, strengh: float):
        self.dir = self.dir * self.speed + dir * strengh
        self.speed = self.dir.length()
        if self.speed != 0:
            self.dir /= self.speed


    def tick(self, delta:float, lines: list):
        if self.speed == 0:
            return

        newpos = self.pos + self.dir * self.speed * delta

        for line in lines:
            p1, p2, n = line
            if not line_circle_collision(p1.x, p1.y, p2.x, p2.y,
                                         self.pos.x, self.pos.y, WATER_RADIUS):
                continue

            newpos = self.pos
            self.dir = vec2(0, 0)
            self.speed = 0
            break

        # Collision with window
        newpos.x = max(WATER_RADIUS, min(WIN_W - WATER_RADIUS, newpos.x))
        newpos.y = max(WATER_RADIUS, min(WIN_H - WATER_RADIUS, newpos.y))

        self.pos = newpos
        self.speed *= AIR_FRICTION_RATIO
        if self.speed < 0.1:
            self.speed = 0
            self.dir = vec2(0, 0)


    def draw(self, win: pg.Surface):
        pg.draw.circle(win, WATER_COLOR, self.pos, WATER_RADIUS)


def line_circle_collision(x1, y1, x2, y2, cx, cy, r) -> bool:
    OPx = x1 - cx
    OPy = y1 - cy
    distOP = math.sqrt(OPx**2 + OPy**2)

    OQx = x2 - cx
    OQy = y2 - cy
    distOQ = math.sqrt(OQx**2 + OQy**2)

    if distOP > distOQ:
        maxDist = distOP
        minDist = distOQ
    else:
        maxDist = distOQ
        minDist = distOP

    if maxDist < r:
        return False

    PQx = x2 - x1
    PQy = y2 - y1
    distPQ = math.sqrt(PQx**2 + PQy**2)

    QPx = x1 - x2
    QPy = y1 - y2

    OPdotQP = OPx * QPx + OPy * QPy

    if OPdotQP > 0:
        OQdotPQ = OQx * PQx + OQy * PQy
        if OQdotPQ > 0:
            tri_area = abs(cx * QPy + x1 * OQy + x2 * (cy - y1)) * 0.5
            minDist = (2 * tri_area) / distPQ

    if minDist <= r and maxDist >= r:
        return True

    return False
