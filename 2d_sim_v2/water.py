import random
import math

from numba import njit
from pygame.math import Vector2 as vec2
from define import  COLLISION_ENERGY_KEEP,\
                    WATER_MIN_X, WATER_MAX_X, WATER_MIN_Y, WATER_MAX_Y,\
                    WATER_SMOOTHING_RADIUS, WATER_SMOOTHING_RADIUS2, WATER_MASS,\
                    WATER_SMOOTHING_VOLUME, TARGET_DENSITY, PRESSURE_MULTIPLIER,\
                    WATER_SMOOTHING_SCALE

def updateWater(pos: vec2, velocity: vec2, delta: float) -> tuple[vec2, vec2]:
    pos += velocity * delta

    # Collision with the screen
    if pos.x < WATER_MIN_X:
        pos.x = WATER_MIN_X
        velocity.x *= -1 * COLLISION_ENERGY_KEEP
    if pos.x > WATER_MAX_X:
        pos.x = WATER_MAX_X
        velocity.x *= -1 * COLLISION_ENERGY_KEEP
    if pos.y < WATER_MIN_Y:
        pos.y = WATER_MIN_Y
        velocity.y *= -1 * COLLISION_ENERGY_KEEP
    if pos.y > WATER_MAX_Y:
        pos.y = WATER_MAX_Y
        velocity.y *= -1 * COLLISION_ENERGY_KEEP

    return (pos, velocity)


@njit(fastmath=True)
def smoothingKernel(dst: float) -> float:
    if dst > WATER_SMOOTHING_RADIUS:
        return 0
    return (WATER_SMOOTHING_RADIUS - dst)**2 / WATER_SMOOTHING_VOLUME


@njit(fastmath=True)
def smoothingKernelDerivate(dst: float) -> float:
    if dst >= WATER_SMOOTHING_RADIUS:
        return 0

    return (dst - WATER_SMOOTHING_RADIUS) * WATER_SMOOTHING_SCALE


def calculateDensity(point: vec2,
                     positions: list[vec2],
                     grid: list[list[list[int]]]) -> float:
    density = 0

    gx = int(point.x // WATER_SMOOTHING_RADIUS)
    gy = int(point.y // WATER_SMOOTHING_RADIUS)

    for i in grid[gy][gx]:
        dst = (positions[i] - point).magnitude()
        influence = smoothingKernel(dst)
        density += WATER_MASS * influence

    return density


# def calculateProperty(point: vec2, positions: list[vec2]) -> float:
#     property = 0

#     for i in range(len(positions)):
#         dst = (positions[i] - point).magnitude()
#         influence = smoothingKernel(dst)
#         density = calculateDensity(point, positions)
#         property += influence * WATER_MASS / density

#     return property


@njit(fastmath=True)
def convertDensityToPressure(density: float) -> float:
    densityError = density - TARGET_DENSITY
    pressure = densityError * PRESSURE_MULTIPLIER
    return pressure


@njit(fastmath=True)
def calculateSharedPressure(densityA: float, densityB: float) -> float:
    pressureA = convertDensityToPressure(densityA)
    pressureB = convertDensityToPressure(densityB)
    return (pressureA + pressureB) / 2


def calculatePressureForce(waterId: int,
                           positions: list[vec2],
                           densities: list[vec2],
                           grid: list[list[list[int]]]) -> vec2:
    pressureForce = vec2(0, 0)

    gx = int(positions[waterId].x // WATER_SMOOTHING_RADIUS)
    gy = int(positions[waterId].y // WATER_SMOOTHING_RADIUS)

    density = densities[waterId]

    for otherId in grid[gy][gx]:
        if waterId == otherId: continue

        offset = positions[otherId] - positions[waterId]
        dst = offset.length()
        if dst != 0:
            dir = offset / dst
        else:
            x = random.random()
            y = random.random()
            while x == 0 and y == 0:
                y = random.random()
            dir = vec2(x, y).normalize()
        slope = smoothingKernelDerivate(dst)
        sharedPressure = calculateSharedPressure(density, densities[otherId])
        pressureForce += -sharedPressure * dir * slope * WATER_MASS / density

    return pressureForce
