import pygame as pg

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
            pass

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


def line_circle_collision(x1, y1, x2, y2, cx, cy, r):
# // is either end INSIDE the circle?
# // if so, return true immediately
# boolean inside1 = pointCircle(x1,y1, cx,cy,r);
# boolean inside2 = pointCircle(x2,y2, cx,cy,r);
# if (inside1 || inside2) return true;

# // get length of the line
# float distX = x1 - x2;
# float distY = y1 - y2;
# float len = sqrt( (distX*distX) + (distY*distY) );

# // get dot product of the line and circle
# float dot = ( ((cx-x1)*(x2-x1)) + ((cy-y1)*(y2-y1)) ) / pow(len,2);

# // find the closest point on the line
# float closestX = x1 + (dot * (x2-x1));
# float closestY = y1 + (dot * (y2-y1));

# // is this point actually on the line segment?
# // if so keep going, but if not, return false
# boolean onSegment = linePoint(x1,y1,x2,y2, closestX,closestY);
# if (!onSegment) return false;

# // optionally, draw a circle at the closest
# // point on the line
# fill(255,0,0);
# noStroke();
# ellipse(closestX, closestY, 20, 20);

# // get distance to closest point
# distX = closestX - cx;
# distY = closestY - cy;
# float distance = sqrt( (distX*distX) + (distY*distY) );

# if (distance <= r) {
# return true;
# }
# return false;
