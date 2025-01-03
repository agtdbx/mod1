data = [
    [[0, 1], [2, 3]],
    [[4], [5, 6, 7]],
]

gridW = 2
gridH = 2

currentVector = []
for line in data:
    for d in line:
        currentVector.append(d)

print()
print(f"original data : {currentVector}")

def printData(x, y):
    id = x + y * gridW
    print(f"data at ({x}, {y}) : {currentVector[id]}")

print()
printData(0, 0)
printData(1, 0)
printData(0, 1)
printData(1, 1)

flatData = []
offsets = []
currentOffset = 0
for vec in currentVector:
    for d in vec:
        flatData.append(d)
    offsets.append(currentOffset)
    currentOffset += len(vec)

print()
print(f"flat data : {flatData}")
print(f"offsets   : {offsets}")


def printDataFromFlat(x, y):
    idOffset = x + y * gridW
    startId = offsets[idOffset]

    # Case that data isn't at the end of flat data
    if idOffset + 1 < len(offsets):
        endId = offsets[idOffset + 1]
    # Case that data is at the end of flat data
    else:
        endId = len(flatData)

    print(f"data for ({x}, {y}) : ", end='')
    for i in range(startId, endId):
        print(f"{flatData[i]} ", end='')
    print()

print()
printDataFromFlat(0, 0)
printDataFromFlat(1, 0)
printDataFromFlat(0, 1)
printDataFromFlat(1, 1)
