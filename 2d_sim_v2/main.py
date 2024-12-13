import pygame as pg
import random
import math
import time
import sys

from pygame.math import Vector2 as vec2

from define import  WIN_W, WIN_H, NB_WATER, WATER_BEGIN_SPACE,\
                    WATER_RADIUS, WATER_COLOR, WATER_SMOOTHING_RADIUS, GRAVITY
# from terrain import Terrain
from water import updateWater, calculateDensity, calculatePressureForce

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
        self.waterPositions = []
        self.waterVelocities = []
        self.waterDensities = []

        if NB_WATER > 0:
            # waterPerRow = int(math.sqrt(NB_WATER))
            # width = waterPerRow * WATER_RADIUS * 2 +\
            #         (waterPerRow - 1) * WATER_BEGIN_SPACE
            # startX = WIN_W / 2 - width / 2
            # startY = WATER_RADIUS + WATER_BEGIN_SPACE
            # for i in range(NB_WATER):
            #     x = startX + (i % waterPerRow) *\
            #             (WATER_BEGIN_SPACE + WATER_RADIUS * 2)
            #     y = startY + (i // waterPerRow) *\
            #             (WATER_BEGIN_SPACE + WATER_RADIUS * 2)

            #     self.waterPositions.append(vec2(x, y))
            #     self.waterVelocities.append(vec2(0, 0))
            #     self.waterDensities.append(0)

            for _ in range(NB_WATER):
                x = random.randint(WATER_RADIUS, WIN_W - WATER_RADIUS - 1)
                y = random.randint(WATER_RADIUS, WIN_H - WATER_RADIUS - 1)

                self.waterPositions.append(vec2(x, y))
                self.waterVelocities.append(vec2(0, 0))
                self.waterDensities.append(0)


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

        # if self.waitInput > 0:
        #     self.waitInput = max(0, self.waitInput - delta)
        # else:
        #     if self.mouseState[0]:
        #         density = calculateDensity(vec2(self.mousePos),
        #                                    self.waterPositions)
        #         print(f"density : {density}")
        #         self.waitInput = 0.2

        # =========================================================
        # Simulate and draw water
        # =========================================================
        delta *= 10 # To speed up the simulation

        # Apply gravity and compute densities
        for i in range(self.nbWater):
            pos = self.waterPositions[i]
            # self.waterVelocities[i] += vec2(0, 1) * GRAVITY * delta
            self.waterDensities[i] = calculateDensity(pos, self.waterPositions)

        # Calculate and apply pressure
        for i in range(self.nbWater):
            pos = self.waterPositions[i]
            pressureForce = calculatePressureForce(i, self.waterPositions,
                                                   self.waterDensities)
            pressureAcceleration = pressureForce / self.waterDensities[i]
            self.waterVelocities[i] = pressureAcceleration * delta

        # Update positions with screen collision
        for i in range(self.nbWater):
            pos = self.waterPositions[i]
            velocity = self.waterVelocities[i]

            pos, velocity = updateWater(pos, velocity, delta)

            pg.draw.circle(self.win, WATER_COLOR, pos, WATER_RADIUS)

        # pg.draw.circle(self.win, (255, 0, 0), self.mousePos,
        #                WATER_SMOOTHING_RADIUS, 1)

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
