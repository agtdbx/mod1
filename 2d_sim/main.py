import pygame as pg
import time
import sys

from pygame.math import Vector2 as vec2

from define import WIN_W, WIN_H, NB_WATER, WATER_RADIUS, GRAVITY
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
        self.fps = 60

        self.last = time.time()

        self.on_focus = True

        self.runMainLoop = True

        self.terrain = Terrain()

        self.waters = []
        for x in range(NB_WATER):
            self.waters.append(Water(WATER_RADIUS + (WATER_RADIUS + 5) * x,
                                     WATER_RADIUS))


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

        for i in range(NB_WATER):
            self.waters[i].applyForce(vec2(0, 1), GRAVITY)

        for water in self.waters:
            water.tick(delta, self.terrain.get_lines())

        pg.display.set_caption("fps : {:.2f}".format(self.clock.get_fps()))


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
