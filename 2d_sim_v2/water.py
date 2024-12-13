import math

from pygame.math import Vector2 as vec2
from define import  GRAVITY, COLLISION_ENERGY_KEEP, WATER_RADIUS2, WATER_MASS,\
                    WATER_MIN_X, WATER_MAX_X, WATER_MIN_Y, WATER_MAX_Y,\
                    WATER_INFLUENCE_RADIUS, WATER_INFLUENCE_RADIUS2,\
                    WATER_INFLUENCE_VOLUME, WATER_INFLUENCE_SCALE

def updateWater(pos: vec2, velocity: vec2, delta: float) -> tuple[vec2, vec2]:
    # velocity += vec2(0, 1) * GRAVITY * delta
    pos += velocity * delta

    # Collision with the screen
    collisionWithWindow(pos, velocity)

    return (pos, velocity)


def collisionWithWindow(pos: vec2, velocity: vec2):
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


def smoothInfluence(dst2: float) -> float:
    value = max(0, WATER_INFLUENCE_RADIUS2 - dst2)
    return value * value * value / WATER_INFLUENCE_VOLUME


def smoothInfluenceDerivate(dst: float) -> float:
    if dst > WATER_INFLUENCE_RADIUS:
        return 0

    f = WATER_INFLUENCE_RADIUS2 - dst**2
    return WATER_INFLUENCE_SCALE * dst * f**2


def computeDensity(id: int, waterPositions: list[vec2], nbWater: int) -> float:
    density = 0

    pos = waterPositions[id]

    for j in range(nbWater):
        if j == id:
            continue
        waterPos = waterPositions[j]
        dst2 = (waterPos - pos).length_squared()
        influence = smoothInfluence(dst2)
        density += influence * WATER_MASS

    return density


def computePropertyGradient(id: int, waterPositions: list[vec2], waterDensity: list[float], nbWater: int) -> vec2:
    propertyGradient = vec2(0, 0)

    pos = waterPositions[id]

    for j in range(nbWater):
        if j == id:
            continue
        waterPos = waterPositions[j]
        dir = waterPos - pos
        dst = dir.length()
        if dst > 0:
            dir /= dst
        else:
            dir = vec2(1, 0)

        derivation = smoothInfluenceDerivate(dst)
        if derivation == 0:
            continue
        # density = computeDensity(id, waterPositions, nbWater)
        density = waterDensity[j]
        if density == 0:
            continue

        propertyGradient += dir * derivation * WATER_MASS / density

    return propertyGradient
