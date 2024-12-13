import pygame as pg
import random
import math
import time
import sys

from pygame.math import Vector2 as vec2

from define import  WIN_W, WIN_H, NB_WATER, WATER_BEGIN_SPACE,\
                    WATER_RADIUS, WATER_COLOR, WATER_INFLUENCE_RADIUS
# from terrain import Terrain
from water import updateWater, computeDensity, computePropertyGradient

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

        # self.terrain = Terrain()

        self.nbWater = NB_WATER
        self.waterPosition = []
        self.waterVelocity = []
        self.waterDensity = []

        if NB_WATER > 0:
            # waterPerRow = int(math.sqrt(NB_WATER))
            # waterPerCol = (NB_WATER - 1) / waterPerRow + 1
            # startX = WIN_W / 2 - (-waterPerRow + WATER_RADIUS * 2 * WATER_BEGIN_SPACE) / 1.35
            # for i in range(NB_WATER):
            #     x = startX + (i % waterPerRow - waterPerRow + WATER_RADIUS * 2) * WATER_BEGIN_SPACE
            #     y = (i // waterPerRow - waterPerCol + WATER_RADIUS * 2) * WATER_BEGIN_SPACE

            #     self.waterPosition.append(vec2(x, y))
            #     self.waterVelocity.append(vec2(0, 0))
            #     self.waterDensity.append(0)

            for i in range(NB_WATER):
                x = random.randint(WATER_RADIUS, WIN_W - WATER_RADIUS - 1)
                y = random.randint(WATER_RADIUS, WIN_H - WATER_RADIUS - 1)

                self.waterPosition.append(vec2(x, y))
                self.waterVelocity.append(vec2(0, 0))
                self.waterDensity.append(0)


    def run(self):
        """
        This method is the main loop of the game
        """
        # Game loop
        while self.runMainLoop:
            self.input()
            self.update()
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


    def update(self):
        # We clean our screen with one color
        self.win.fill((0, 0, 0))

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

        for i in range(self.nbWater):
            self.waterDensity[i] = computeDensity(i, self.waterPosition, self.nbWater)


        # Simulate and draw water
        for i in range(self.nbWater):
            pos = self.waterPosition[i]
            velocity = self.waterVelocity[i]

            gradient = computePropertyGradient(i, self.waterPosition, self.waterDensity, self.nbWater)
            pos, velocity = updateWater(pos, velocity, delta)

            pg.draw.circle(self.win, WATER_COLOR, pos, WATER_RADIUS)

            self.waterPosition[i] = pos
            self.waterVelocity[i] = velocity

            p1 = pos
            p2 = p1 + gradient.normalize() * 20
            pg.draw.line(self.win, (255, 0, 0), p1, p2)

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
