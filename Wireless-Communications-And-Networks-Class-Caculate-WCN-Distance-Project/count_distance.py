import math


def d(r, a, n):
    return (10 ** ((-r - a) / 10 / n)) * 100


"""
def getDistance(lonA, latA, lonB, latB):
        ra = 6378140  # radius of equator: meter
        rb = 6356755  # radius of polar: meter
        ho=(latA-latB)/360*ra*math.cos((lonA+lonB)/2)
        ve=abs(lonA-lonB)/360*rb
        return math.sqrt(ho**2+ve**2)
"""


def getDistance(lonA, latA, lonB, latB):
    ra = 6378140  # radius of equator: meter
    rb = 6356755  # radius of polar: meter
    flatten = (ra - rb) / ra  # Partial rate of the earth
    # change angle to radians
    radLatA = math.radians(latA)
    radLonA = math.radians(lonA)
    radLatB = math.radians(latB)
    radLonB = math.radians(lonB)

    pA = math.atan(rb / ra * math.tan(radLatA))
    pB = math.atan(rb / ra * math.tan(radLatB))
    x = math.acos(
        math.sin(pA) * math.sin(pB)
        + math.cos(pA) * math.cos(pB) * math.cos(radLonA - radLonB)
    )
    c1 = (math.sin(x) - x) * (math.sin(pA) + math.sin(pB)) ** 2 / math.cos(x / 2) ** 2
    c2 = (math.sin(x) + x) * (math.sin(pA) - math.sin(pB)) ** 2 / math.sin(x / 2) ** 2
    dr = flatten / 8 * (c1 - c2)
    distance = ra * (x + dr)
    return distance


def ce(a, b):
    if a - b > 0.001 or a - b < -0.001:
        return False
    return True


rssi = [-108, -99, -89]


# print(getDistance(24.96715, 121.18766, 24.96758, 121.18908))
# print(getDistance(24.96822, 121.19437, 24.96758, 121.18908))
# print(getDistance(24.97154, 121.19268, 24.96758, 121.18908))

point = [[0, 0], [0, 0], [0, 0]]
print(d(rssi[0], 90, 3), d(rssi[1], 86, 1.3), d(rssi[2], 83, 1.7))
i = 24.96716
while i < 24.97154:
    j = 121.18767
    while j < 121.19437:
        if ce(getDistance(i, j, 24.96715, 121.18766), d(rssi[0], 90, 3)) and ce(
            getDistance(i, j, 24.96822, 121.19437), d(rssi[1], 86, 1.3)
        ):
            point[0][0] = i
            point[0][1] = j
            print("point 1:")
            print(i, j)
        if ce(getDistance(i, j, 24.96715, 121.18766), d(rssi[0], 90, 3)) and ce(
            getDistance(i, j, 24.97154, 121.19268), d(rssi[2], 83, 1.7)
        ):
            point[1][0] = i
            point[1][1] = j
            print("point 2:")
            print(i, j)
        if ce(getDistance(i, j, 24.96822, 121.19437), d(rssi[1], 86, 1.3)) and ce(
            getDistance(i, j, 24.97154, 121.19268), d(rssi[2], 83, 1.7)
        ):
            point[2][0] = i
            point[2][1] = j
            print("point 3:")
            print(i, j)
        j = j + 0.00001
    i = i + 0.00001

print(
    (point[0][0] + point[1][0] + point[2][0]) / 3,
    (point[0][1] + point[1][1] + point[2][1]) / 3,
)
