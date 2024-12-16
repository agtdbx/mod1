import random
import math

from numba import njit
from pygame.math import Vector2 as vec2
from define import  COLLISION_ENERGY_KEEP, WATER_RADIUS, WATER_RADIUS2,\
                    WATER_MIN_X, WATER_MAX_X, WATER_MIN_Y, WATER_MAX_Y,\
                    WATER_SMOOTHING_RADIUS, WATER_SMOOTHING_RADIUS2, WATER_MASS,\
                    WATER_SMOOTHING_VOLUME, TARGET_DENSITY, PRESSURE_MULTIPLIER,\
                    WATER_SMOOTHING_SCALE, VISCOSITY_FORCE

def updateWater(pos: vec2,
                velocity: vec2,
                lines: list[vec2, vec2, vec2],
                delta: float) -> tuple[vec2, vec2]:
    lastPos = pos.copy()
    pos += velocity * delta

    # Collison with the terrain
    if velocity.x != 0 or velocity.y != 0:
        dirLen = velocity.length()
        dir = velocity / dirLen
        dirLen += WATER_RADIUS * 2
        endWaterLine = lastPos + dir * dirLen - dir * WATER_RADIUS
        for line in lines:
            p1: vec2 = line[0]
            p2: vec2 = line[1]
            n: vec2 = line[2]
            v: vec2 = line[3]

            # if not line_circle_collision(p1.x, p1.y, p2.x, p2.y, pos.x, pos.y,
            #                                 WATER_RADIUS, WATER_RADIUS2):
            #     continue
            if not direction_line_collision(lastPos, endWaterLine, p1, p2,):
                continue

            # Reflection
            if n.dot(dir) >= 0:
                n = n.copy() * -1

            divider = dir.dot(dir)
            projDir = n * (n.dot(dir) / divider) * 2
            dir = (dir - projDir).normalize()
            pos = lastPos
            velocity = dir * dirLen * COLLISION_ENERGY_KEEP
            break

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


def direction_line_collision(p0: vec2,
                             p1: vec2,
                             p2: vec2,
                             p3: vec2) -> bool:
    s1 = p1 - p0
    s2 = p3 - p2

    s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y)
    t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y)

    if s >= 0 and s <= 1 and t >= 0 and t <= 1:
        return True
    return False


# def direction_line_collision(waterPos: vec2,
#                              waterDir: vec2,
#                              waterDirLen: float,
#                              segNormal: vec2,
#                              segDir: vec2,
#                              segP1: vec2) -> bool:
#     if 0 <= waterDir.dot(segNormal):
#         return False

#     v1 = waterPos - segP1
#     v2 = segDir
#     v3 = vec2(-waterDir.y, waterDir.x)

#     subpart = v2.dot(v2)
#     if subpart == 0:
#         return False

#     t1 = v2.cross(v1) / subpart
#     if (t1 < 0):
#         return False

#     t2 = v1.dot(v3) / subpart
#     if (t2 < 0 or 1 < t2):
#         return False

#     return t1 <= waterDirLen


@njit(fastmath=True)
def line_circle_collision(x1: float, y1: float,
                          x2: float, y2: float,
                          cx: float, cy: float,
                          r: float, r2: float) -> bool:
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
        return False

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
            return False
        return True

    if minDist <= r:
        return True
    return False


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


@njit(fastmath=True)
def viscositySmoothingKernel(dst: float) -> float:
    if dst > WATER_SMOOTHING_RADIUS:
        return 0
    value = dst**2 / WATER_SMOOTHING_RADIUS2
    return value**3


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

    pos = positions[waterId]
    gx = int(pos.x // WATER_SMOOTHING_RADIUS)
    gy = int(pos.y // WATER_SMOOTHING_RADIUS)

    density = densities[waterId]

    for otherId in grid[gy][gx]:
        if waterId == otherId: continue

        offset = positions[otherId] - pos
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
        pressureForce += sharedPressure * dir * slope * WATER_MASS / density

    return pressureForce


def calculateViscosityForce(waterId: int,
                            positions: list[vec2],
                            velocities: list[vec2],
                            grid: list[list[list[int]]]) -> vec2:
    viscosityForce = vec2(0, 0)

    pos = positions[waterId]
    vel = velocities[waterId]
    gx = int(pos.x // WATER_SMOOTHING_RADIUS)
    gy = int(pos.y // WATER_SMOOTHING_RADIUS)

    for otherId in grid[gy][gx]:
        if waterId == otherId: continue

        velDir = (velocities[otherId] - vel)
        velLen = velDir.length()
        if velLen == 0:
            continue
        velDir /= velLen
        dst = (pos - positions[otherId]).length()
        influence = viscositySmoothingKernel(dst)
        viscosityForce += velDir * influence

    return viscosityForce * VISCOSITY_FORCE

