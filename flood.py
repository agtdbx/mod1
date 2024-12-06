import math
import random

MAP_SIZE = 100
MAX_HEIGHT = 100
# POINTS = [(50, 50, 10)]
POINTS = [(50, 50, 42), (0, 0, 10), (30, 40, 25)]

# Create map
heighmap = [[(0.0, 0.0)] * MAP_SIZE for _ in range(MAP_SIZE)]


# Set point in map
next_points = set()

for x, y, z in POINTS:
	if x < 0 or x >= MAP_SIZE:
		print(f"x must be between 0 and {MAP_SIZE - 1}")
		exit(1)
	if y < 0 or y >= MAP_SIZE:
		print(f"y must be between 0 and {MAP_SIZE - 1}")
		exit(1)
	if z < 0 or z >= MAX_HEIGHT:
		print(f"z must be between 0 and {MAX_HEIGHT}")
		exit(1)
	x = int(x)
	y = int(y)
	heighmap[y][x] = (float(z), (x, y, z))
	next_points.add((x, y))


# Flood
next_change = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, -1), (-1, 0)]

while len(next_points) > 0:
	new_points = set()

	for point in next_points:
		# get value
		x, y = point
		value, origin = heighmap[y][x]

		ox, oy, oz = origin

		for change in next_change:
			nx = x + change[0]
			ny = y + change[1]

			if nx < 0 or nx >= MAP_SIZE or ny < 0 or ny >= MAP_SIZE:
				continue

			dist = math.sqrt((ox - nx)**2 + (oy - ny)**2)
			if dist == 0:
				continue
			val = max(0, oz**2 - dist**2)
			val /= oz**2
			val = val**3
			new_val = val * oz

			last_val = heighmap[ny][nx][0]
			if new_val <= last_val:
				continue

			heighmap[ny][nx] = (new_val, origin)
			if (nx, ny) not in new_points:
				new_points.add((nx, ny))

	next_points = new_points.copy()


# draw heighmap
for y in range(MAP_SIZE):
	for x in range(MAP_SIZE):
		print(f"{int(heighmap[y][x][0] * 1000)/100:2} ", end='')
	print()
