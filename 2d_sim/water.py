import pygame as pg
import math

from pygame.math import Vector2 as vec2
from numba import njit
from define import  WATER_RADIUS, WATER_RADIUS2, WATER_COLOR,\
                    WIN_W, WIN_H, AIR_FRICTION_RATIO, LINE_FRICTION_RATIO

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

        current_speed = self.speed * delta

        newpos = self.pos + self.dir * current_speed

        closest = None

        for line in lines:
            p1: vec2 = line[0]
            p2: vec2 = line[1]
            n: vec2 = line[2]

            collid, dist = line_circle_collision(p1.x, p1.y, p2.x, p2.y,
                                                 self.pos.x, self.pos.y,
                                                 WATER_RADIUS, WATER_RADIUS2)
            if not collid:
                continue

            if closest == None or closest[0] > dist:
                closest = (dist, n)

        if closest != None:
            dist, n = closest

            # Reflection
            if n.dot(self.dir) >= 0:
                n *= -1

            divider = self.dir.dot(self.dir)
            if divider == 0:
                newpos = self.pos
                self.dir = vec2(0, 0)
                self.speed = 0

            else:
                projDir = n * (n.dot(self.dir) / divider) * 2
                self.dir = (self.dir - projDir).normalize()
                newpos = self.pos + self.dir * current_speed
                self.speed *= LINE_FRICTION_RATIO

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


@njit(fastmath=True)
def line_circle_collision(x1: float, y1: float,
                          x2: float, y2: float,
                          cx: float, cy: float,
                          r: float, r2: float) -> tuple[bool, float]:
    OPx = x1 - cx
    OPy = y1 - cy
    distOP = OPx**2 + OPy**2

    OQx = x2 - cx
    OQy = y2 - cy
    distOQ = OQx**2 + OQy**2

    if distOP > distOQ:
        maxDist = distOP
        minDist = distOQ
    else:
        maxDist = distOQ
        minDist = distOP

    if maxDist < r2:
        return False, -1

    maxDist = math.sqrt(maxDist)

    QPx = x1 - x2
    QPy = y1 - y2

    OPdotQP = OPx * QPx + OPy * QPy

    needSqrt = True
    if OPdotQP > 0:
        PQx = x2 - x1
        PQy = y2 - y1
        OQdotPQ = OQx * PQx + OQy * PQy
        if OQdotPQ > 0:
            tri_area = abs(cx * QPy + x1 * OQy + x2 * (cy - y1))
            distPQ = math.sqrt(PQx**2 + PQy**2)
            minDist = tri_area / distPQ
            needSqrt = False

    if needSqrt:
        if minDist > r2:
            return False, -1
        return True, math.sqrt(minDist)

    if minDist <= r:
        return True, minDist
    return False, -1
