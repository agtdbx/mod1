import pygame as pg
import math
import time
import sys

from pygame.math import Vector2 as vec2

from define import  WIN_W, WIN_H, GRAVITY, NB_WATER, WATER_RADIUS,\
                    WATER_EFFECT_RADIUS, WATER_EFFECT_RADIUS2,\
                    WATER_EFFECT_STRENGH, WATER_FRICTION_RATIO
from terrain import Terrain
from water import Water

class Game:
    def __init__(self):
        """
        This method define all variables needed by the program
        """
        # Start of pygame
        pg.init()

        # We remove the toolbar of the window's height
        self.winSize = vec2(WIN_W, WIN_H)
        # We create the window
        self.win = pg.display.set_mode(self.winSize, pg.RESIZABLE)

        self.clock = pg.time.Clock() # The clock be used to limit our fps
        self.fps = 0

        self.last = time.time()
        self.deltas = []
        self.fpsPrintTime = 0
        self.waitInput = 0

        self.on_focus = True

        self.runMainLoop = True

        self.terrain = Terrain()

        self.gravity = True
        # self.gravity = False

        self.nbWater = NB_WATER
        self.waters = []
        for i in range(NB_WATER):
            x = WATER_RADIUS + (WATER_RADIUS * 2) * i
            y = WATER_RADIUS
            while x >= WIN_W:
                x -= WIN_W
                y += WATER_RADIUS * 2
            self.waters.append(Water(x, y))
        self.waterTiles = []
        self.waterTilesW = WIN_W // WATER_EFFECT_RADIUS + 1
        self.waterTilesH = WIN_H // WATER_EFFECT_RADIUS + 1


    def run(self):
        """
        This method is the main loop of the game
        """
        # Game loop
        while self.runMainLoop:
            self.input()
            self.tick()
            self.render()
            self.clock.tick(self.fps)


    def input(self):
        """
        The method catch user's inputs, as key presse or a mouse click
        """
        # We check each event
        for event in pg.event.get():
            # If the event it a click on the top right cross, we quit the game
            if event.type == pg.QUIT:
                self.quit()

            if event.type == pg.WINDOWSIZECHANGED:
                self.winSize = vec2(self.win.get_size())

            if event.type == pg.WINDOWFOCUSGAINED:
                self.on_focus = True

            if event.type == pg.WINDOWFOCUSLOST:
                self.on_focus = False

        self.keyboardState = pg.key.get_pressed()
        self.mouseState = pg.mouse.get_pressed()
        self.mousePos = pg.mouse.get_pos()

        # Press espace to quit
        if self.keyboardState[pg.K_ESCAPE]:
            self.quit()


    def tick(self):
        """
        This is the method where all calculations will be done
        """
        tmp = time.time()
        delta = tmp - self.last
        self.last = tmp

        # Print fps
        self.deltas.append(delta)
        self.fpsPrintTime += delta
        if self.fpsPrintTime >= 1:
            self.fpsPrintTime -= 1
            avg = sum(self.deltas) / len(self.deltas)
            self.deltas.clear()
            print(f"fps : {1.0 / avg:7.2f} | water {self.nbWater:4}")

        # Add water
        self.waitInput = max(0, self.waitInput - delta)
        if self.mouseState[0] and self.waitInput == 0:
            self.waters.append(Water(self.mousePos[0], self.mousePos[1]))
            self.nbWater += 1
            self.waitInput = 0.2
        elif self.mouseState[1] and self.waitInput == 0:
            self.waters.append(Water(self.mousePos[0], self.mousePos[1]))
            self.nbWater += 1
        elif self.mouseState[2] and self.waitInput == 0:
            self.waters.append(Water(self.mousePos[0], self.mousePos[1]))
            self.nbWater += 1
            self.waitInput = 0.05
        elif self.keyboardState[pg.K_SPACE] and self.waitInput == 0:
            self.gravity = not self.gravity
            self.waitInput = 0.2

        # Generate tiles
        self.waterTiles.clear()
        for y in range(self.waterTilesH):
            lineTile = []
            for x in range(self.waterTilesW):
                lineTile.append([])
            self.waterTiles.append(lineTile)

        # Put every water in there tiles
        for i in range(self.nbWater):
            water: Water = self.waters[i]
            tx = int(water.pos.x // WATER_EFFECT_RADIUS)
            ty = int(water.pos.y // WATER_EFFECT_RADIUS)
            self.waterTiles[ty][tx].append((water, i))

            canLeft = (tx > 0)
            canRight = (tx < self.waterTilesW - 1)
            canUp = (ty > 0)
            canDown = (ty < self.waterTilesH - 1)

            if canLeft:
                self.waterTiles[ty][tx - 1].append((water, i))
            if canRight:
                self.waterTiles[ty][tx + 1].append((water, i))
            if canUp:
                self.waterTiles[ty - 1][tx].append((water, i))
            if canDown:
                self.waterTiles[ty + 1][tx].append((water, i))
            if canLeft and canUp:
                self.waterTiles[ty - 1][tx - 1].append((water, i))
            if canLeft and canDown:
                self.waterTiles[ty + 1][tx - 1].append((water, i))
            if canRight and canUp:
                self.waterTiles[ty - 1][tx + 1].append((water, i))
            if canRight and canDown:
                self.waterTiles[ty + 1][tx + 1].append((water, i))

        # Water simulation
        for i in range(self.nbWater):
            water: Water = self.waters[i]
            tx = int(water.pos.x // WATER_EFFECT_RADIUS)
            ty = int(water.pos.y // WATER_EFFECT_RADIUS)

            for otherWater, j in self.waterTiles[ty][tx]:
                if i == j:
                    continue

                dirX = otherWater.pos.x - water.pos.x
                dirY = otherWater.pos.y - water.pos.y

                dist = dirX**2 + dirY**2

                if dist > WATER_EFFECT_RADIUS2:
                    continue

                force = (1 - ((dist**2) / (WATER_EFFECT_RADIUS2**2))) * WATER_EFFECT_STRENGH
                if dist != 0:
                    dir = vec2(dirX, dirY) / math.sqrt(dist)
                else:
                    dir = vec2(1, 0)
                forcePerWater = force / 2
                water.speed *= WATER_FRICTION_RATIO

                otherWater.applyForce(dir, forcePerWater)
                water.applyForce(-dir, forcePerWater)

        # Tick waters
        if self.gravity:
            for water in self.waters:
                water.applyForce(vec2(0, 1), GRAVITY)
                water.tick(delta, self.terrain.get_split_lines(water.pos.x))
        else:
            for water in self.waters:
                water.tick(delta, self.terrain.get_split_lines(water.pos.x))


    def render(self):
        """
        This is the method where all graphic update will be done
        """
        # We clean our screen with one color
        self.win.fill((0, 0, 0))

        self.terrain.draw(self.win)

        for water in self.waters:
            water.draw(self.win)

        # We update the drawing.
        # Before the function call, any changes will be not visible
        pg.display.update()


    def quit(self):
        """
        This is the quit method
        """
        # Pygame quit
        pg.quit()
        sys.exit()


Game().run() # Start game
