import pygame as pg
import random
import math
import time
import sys

from pygame.math import Vector2 as vec2

from define import  WIN_W, WIN_H, NB_WATER, WATER_BEGIN_SPACE,\
                    WATER_RADIUS, WATER_COLOR, WATER_SMOOTHING_RADIUS,\
                    WATER_RGB_FAST1, WATER_RGB_FAST2,\
                    MOUSE_RADIUS, MOUSE_RADIUS2, MOUSE_FORCE, GRAVITY
from terrain import Terrain
from water import   updateWater, calculateDensity, calculatePressureForce,\
                    calculateViscosityForce

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

        # self.gravity = True
        self.gravity = False

        # self.simulate = True
        self.simulate = False
        self.simulationSpeed = 10

        self.nbWater = NB_WATER
        self.waterPositions: list[vec2] = []
        self.waterPredictionPos: list[vec2] = []
        self.waterVelocities: list[vec2] = []
        self.waterDensities: list[float] = []

        if NB_WATER > 0:
            waterPerRow = int(math.sqrt(NB_WATER))
            waterPerCol = (NB_WATER // waterPerRow) + 1
            width = waterPerRow * WATER_RADIUS * 2 +\
                    (waterPerRow - 1) * WATER_BEGIN_SPACE
            heigh = waterPerCol * WATER_RADIUS * 2 +\
                    (waterPerCol - 1) * WATER_BEGIN_SPACE
            startX = WIN_W / 2 - width / 2
            # startY = WATER_RADIUS + WATER_BEGIN_SPACE
            startY =  WIN_H / 2 - heigh / 2
            for i in range(NB_WATER):
                x = startX + (i % waterPerRow) *\
                        (WATER_BEGIN_SPACE + WATER_RADIUS * 2)
                y = startY + (i // waterPerRow) *\
                        (WATER_BEGIN_SPACE + WATER_RADIUS * 2)

                pos = vec2(x, y)
                vel = vec2(0, 0)

                pos, vel = updateWater(pos, vel,
                                       self.terrain.get_split_lines(pos.x), 0.16)

                self.waterPositions.append(pos)
                self.waterPredictionPos.append(pos)
                self.waterVelocities.append(vel)
                self.waterDensities.append(0)

            # for _ in range(NB_WATER):
            #     x = random.randint(WATER_RADIUS, WIN_W - WATER_RADIUS - 1)
            #     y = random.randint(WATER_RADIUS, WIN_H - WATER_RADIUS - 1)

            #     self.waterPositions.append(vec2(x, y))
            #     self.waterPredictionPos.append(vec2(x, y))
            #     self.waterVelocities.append(vec2(0, 0))
            #     self.waterDensities.append(0)

        self.waterGrid: list[list[list[int]]] = []
        self.waterGridW = WIN_W // WATER_SMOOTHING_RADIUS + 1
        self.waterGridH = WIN_H // WATER_SMOOTHING_RADIUS + 1
        for y in range(self.waterGridH):
                waterLine = []
                for x in range(self.waterGridW):
                    waterLine.append([])
                self.waterGrid.append(waterLine)

        for i in range(self.nbWater):
                gx = int(self.waterPositions[i].x // WATER_SMOOTHING_RADIUS)
                gy = int(self.waterPositions[i].y // WATER_SMOOTHING_RADIUS)
                self.waterGrid[gy][gx].append(i)
                for uy in [-1, 0, 1]:
                    for ux in [-1, 0, 1]:
                        if ux == 0 and uy == 0: continue
                        nx = gx + ux
                        ny = gy + uy

                        if nx < 0 or nx >= self.waterGridW\
                            or ny < 0 or ny >= self.waterGridH:
                            continue
                        self.waterGrid[ny][nx].append(i)


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

        self.terrain.draw(self.win)

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
        if self.waitInput > 0:
            self.waitInput = max(0, self.waitInput - delta)
        else:
            if self.keyboardState[pg.K_UP] or self.keyboardState[pg.K_DOWN]:
                self.waterPositions.append(vec2(self.mousePos))
                self.waterPredictionPos.append(vec2(self.mousePos))
                self.waterVelocities.append(vec2(0, 0))
                self.waterDensities.append(0)
                self.nbWater += 1
                if self.keyboardState[pg.K_UP]:
                    self.waitInput = 0.2
            if self.keyboardState[pg.K_SPACE]:
                self.simulate = not self.simulate
                self.waitInput = 0.2
                tkt = ["off", "on"]
                print(f"simulation {tkt[self.simulate]}")
            if self.keyboardState[pg.K_g]:
                self.gravity = not self.gravity
                self.waitInput = 0.2
                tkt = ["off", "on"]
                print(f"gravity {tkt[self.gravity]}")
            if self.keyboardState[pg.K_LEFT]:
                self.simulationSpeed = max(0.5, self.simulationSpeed - 0.5)
                self.waitInput = 0.2
                print(f"simulation speed : {self.simulationSpeed}")
            if self.keyboardState[pg.K_RIGHT]:
                self.simulationSpeed = min(20, self.simulationSpeed + 0.5)
                self.waitInput = 0.2
                print(f"simulation speed : {self.simulationSpeed}")

        if self.keyboardState[pg.K_RSHIFT]:
            delta *= 2

        # =========================================================
        # Simulate and draw water
        # =========================================================
        delta *= self.simulationSpeed # To speed up the simulation

        if self.simulate:

            # Compute water grid
            for y in range(self.waterGridH):
                for x in range(self.waterGridW):
                    self.waterGrid[y][x].clear()

            # Put waterid in grid
            for i in range(self.nbWater):
                gx = int(self.waterPositions[i].x // WATER_SMOOTHING_RADIUS)
                gy = int(self.waterPositions[i].y // WATER_SMOOTHING_RADIUS)
                self.waterGrid[gy][gx].append(i)
                for uy in [-1, 0, 1]:
                    for ux in [-1, 0, 1]:
                        if ux == 0 and uy == 0: continue
                        nx = gx + ux
                        ny = gy + uy

                        if nx < 0 or nx >= self.waterGridW\
                            or ny < 0 or ny >= self.waterGridH:
                            continue
                        self.waterGrid[ny][nx].append(i)

            # Compute predicted pos
            for i in range(self.nbWater):
                pos = self.waterPositions[i]
                velocity = self.waterVelocities[i]
                if self.gravity:
                    self.waterVelocities[i] += vec2(0, 1) * GRAVITY * delta

                if self.mouseState[0] or self.mouseState[2]:
                    dir = pos - vec2(self.mousePos)
                    dst = dir.length()
                    if dst == 0:
                        dir = vec2(1, 0)
                    else:
                        dir /= dst

                    if dst <= MOUSE_RADIUS:
                        strength = 1 - (dst**2 / MOUSE_RADIUS2)

                        if self.mouseState[0]: # Push water away
                            self.waterVelocities[i] += dir * strength * MOUSE_FORCE
                        else: # Pull water to the mouse
                            self.waterVelocities[i] -= dir * strength * MOUSE_FORCE

                pos, velocity = updateWater(pos.copy(),
                                            velocity.copy(),
                                            self.terrain.get_split_lines(pos.x),
                                            delta)
                self.waterPredictionPos[i] = pos

            # Apply gravity and compute densities
            for i in range(self.nbWater):
                self.waterDensities[i] = calculateDensity(self.waterPredictionPos[i],
                                                        self.waterPredictionPos,
                                                        self.waterGrid)

            # Calculate and apply pressure
            for i in range(self.nbWater):
                if self.waterDensities[i] == 0:
                    self.waterVelocities[i] = vec2(0, 0)
                    continue
                pressureForce = calculatePressureForce(i, self.waterPredictionPos,
                                                    self.waterDensities,
                                                    self.waterGrid)
                pressureAcceleration = pressureForce / self.waterDensities[i]
                self.waterVelocities[i] += pressureAcceleration * delta

                viscosityForce = calculateViscosityForce(i, self.waterPredictionPos,
                                                         self.waterVelocities,
                                                         self.waterGrid)
                self.waterVelocities[i] += viscosityForce * delta

            # Update positions with screen collision
            for i in range(self.nbWater):
                pos = self.waterPositions[i]
                velocity = self.waterVelocities[i]

                pos, velocity = updateWater(pos, velocity,
                                            self.terrain.get_split_lines(pos.x),
                                            delta)

                self.waterPositions[i] = pos
                self.waterVelocities[i] = velocity

                speed = velocity.length()

                if speed <= 10:
                    fastRatio = min(1, speed / 10)
                    slowRatio = 1 - fastRatio

                    r = WATER_COLOR[0] * slowRatio + WATER_RGB_FAST1[0] * fastRatio
                    g = WATER_COLOR[1] * slowRatio + WATER_RGB_FAST1[1] * fastRatio
                    b = WATER_COLOR[2] * slowRatio + WATER_RGB_FAST1[2] * fastRatio

                else:
                    fastRatio = min(1, (speed - 10) / 10)
                    slowRatio = 1 - fastRatio

                    r = WATER_RGB_FAST1[0] * slowRatio + WATER_RGB_FAST2[0] * fastRatio
                    g = WATER_RGB_FAST1[1] * slowRatio + WATER_RGB_FAST2[1] * fastRatio
                    b = WATER_RGB_FAST1[2] * slowRatio + WATER_RGB_FAST2[2] * fastRatio

                pg.draw.circle(self.win, (r, g, b), pos, WATER_RADIUS)

                p2 = pos + velocity
                pg.draw.line(self.win, (0, 255, 0), pos, p2)

        else:
            for i in range(self.nbWater):
                pos = self.waterPositions[i]
                speed = self.waterVelocities[i].length()

                if speed <= 10:
                    fastRatio = min(1, speed / 10)
                    slowRatio = 1 - fastRatio

                    r = WATER_COLOR[0] * slowRatio + WATER_RGB_FAST1[0] * fastRatio
                    g = WATER_COLOR[1] * slowRatio + WATER_RGB_FAST1[1] * fastRatio
                    b = WATER_COLOR[2] * slowRatio + WATER_RGB_FAST1[2] * fastRatio

                else:
                    fastRatio = min(1, (speed - 10) / 10)
                    slowRatio = 1 - fastRatio

                    r = WATER_RGB_FAST1[0] * slowRatio + WATER_RGB_FAST2[0] * fastRatio
                    g = WATER_RGB_FAST1[1] * slowRatio + WATER_RGB_FAST2[1] * fastRatio
                    b = WATER_RGB_FAST1[2] * slowRatio + WATER_RGB_FAST2[2] * fastRatio

                pg.draw.circle(self.win, (r, g, b), pos, WATER_RADIUS)

        if self.mouseState[0]:
            pg.draw.circle(self.win, (255, 0, 0), self.mousePos, MOUSE_RADIUS, 1)
        elif self.mouseState[2]:
            pg.draw.circle(self.win, (0, 255, 0), self.mousePos, MOUSE_RADIUS, 1)

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
