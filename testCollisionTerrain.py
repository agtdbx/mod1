import pygame as pg
import math
import time
import sys

from pygame.math import Vector2 as vec2

LINE_COLOR = (0, 200, 0)
LINE_AXIS_COLOR = (200, 0, 0)
LINE_AXIS_SIZE = 10
WATER_COLOR = (0, 0, 200)
WATER_RADIUS = 10
WATER_GRAVITY = vec2(0, 100)
COLLISION_ENERGY_KEEP = 0.7

def waterCollidLine(waterPosition: vec2,
					linePos: vec2, lineXaxis: vec2) -> bool:

	posFromLine = waterPosition - linePos

	# bbox collide
	if posFromLine.x < -WATER_RADIUS or posFromLine.x > lineXaxis.x + WATER_RADIUS \
		or posFromLine.y < -WATER_RADIUS or posFromLine.y > lineXaxis.y + WATER_RADIUS:
		return False

	# ix = px + dx * a
	# ix - px = dx * a
	# (ix - px) / dx = a

	if lineXaxis.x != 0:
		a = posFromLine.x / lineXaxis.x
		interPos = lineXaxis * a

	elif lineXaxis.y != 0:
		a = posFromLine.y / lineXaxis.y
		interPos = lineXaxis * a

	else:
		return False

	dst = (posFromLine - interPos).length()

	if dst > WATER_RADIUS:
		return False

	return True



class Game:
	def __init__(self):
		"""
		This method define all variables needed by the program
		"""
		# Start of pygame
		pg.init()

		# We remove the toolbar of the window's height
		self.winSize = ((1600, 900))
		# We create the window
		self.win = pg.display.set_mode(self.winSize, pg.RESIZABLE)

		self.clock = pg.time.Clock() # The clock be used to limit our fps
		self.fps = 60

		self.last = time.time()

		self.runMainLoop = True

		self.watersPositions = []
		self.watersVelocities = []
		self.lines = []

		xOffset = self.winSize[0] / 100
		for i in range(100):
			self.addWater(vec2(xOffset * (i + 0.5), WATER_RADIUS))
		self.addLine(vec2(100, 300), vec2(700, 700))


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

		# Make water fall
		for i in range(len(self.watersPositions)):
			self.watersVelocities[i] += WATER_GRAVITY * delta
			testPos = self.watersPositions[i] + self.watersVelocities[i] * delta

			if testPos.x < WATER_RADIUS:
				testPos.x = WATER_RADIUS
				self.watersVelocities[i].x *= -1
				self.watersVelocities[i] *= COLLISION_ENERGY_KEEP
			if testPos.x > self.winSize[0] - WATER_RADIUS:
				testPos.x = self.winSize[0] - WATER_RADIUS
				self.watersVelocities[i].x *= -1
				self.watersVelocities[i] *= COLLISION_ENERGY_KEEP

			if testPos.y < WATER_RADIUS:
				testPos.y = WATER_RADIUS
				self.watersVelocities[i].y *= -1
				self.watersVelocities[i] *= COLLISION_ENERGY_KEEP
			if testPos.y > self.winSize[1] - WATER_RADIUS:
				testPos.y = self.winSize[1] - WATER_RADIUS
				self.watersVelocities[i].y *= -1
				self.watersVelocities[i] *= COLLISION_ENERGY_KEEP

			for line in self.lines:
				pos = line[0]
				direction = line[2]
				normal = line[3]
				if waterCollidLine(testPos, pos, direction):

					velLen = self.watersVelocities[i].length()
					if (velLen == 0):
						velDir = vec2(0, 1)
					else:
						velDir = self.watersVelocities[i] / velLen

					divider = velDir.dot(velDir)
					projDir = normal * (normal.dot(velDir) / divider) * 2
					velDir = (velDir - projDir).normalize()

					self.watersVelocities[i] = velDir * velLen * COLLISION_ENERGY_KEEP
					testPos = self.watersPositions[i] + self.watersVelocities[i] * delta
					break

			self.watersPositions[i] = testPos

		pg.display.set_caption(str(self.clock.get_fps()))


	def render(self):
		"""
		This is the method where all graphic update will be done
		"""
		# We clean our screen with one color
		self.win.fill((0, 0, 0))

		# Draw water
		for waterPos in self.watersPositions:
			pg.draw.circle(self.win, WATER_COLOR, waterPos, WATER_RADIUS)

		# Draw line
		for line in self.lines:
			p1 = line[0]
			p2 = line[1]
			x_axis = line[2]
			y_axis = line[3]

			pg.draw.line(self.win, LINE_COLOR, p1, p2)

			pm = p1 + x_axis * 0.5
			py = pm + y_axis * LINE_AXIS_SIZE
			pg.draw.line(self.win, LINE_AXIS_COLOR, pm, py)


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


	def addWater(self, pos: vec2):
		self.watersPositions.append(pos)
		self.watersVelocities.append(vec2(0, 0))


	def addLine(self, p1: vec2, p2: vec2):
		dir = p2 - p1
		y_axis = vec2(dir.y, -dir.x).normalize()

		self.lines.append([p1, p2, dir, y_axis])

Game().run() # Start game
