lines = open("bunny.obj").readlines()

vertices = []
normals = []
faces = []

maxx = float("-inf")
maxy = float("-inf")
maxz = float("-inf")
minx = float("inf")
miny = float("inf")
minz = float("inf")

numfaces = 0

scale = 2.0
yoffset = -0.25 #* scale
zoffset = -0.10 #* scale

for line in lines:
    line = line.strip()
    components = line.split()
    if not components:
        continue
    if (components[0] == 'v'):
        x = float(components[1]) * scale 
        y = float(components[2]) * scale + yoffset
        z = float(components[3]) * scale + zoffset
        maxx = max(maxx, x)
        maxy = max(maxy, y)
        maxz = max(maxz, z)
        minx = min(minx, x)
        miny = min(miny, y)
        minz = min(minz, z)
        vertices.append((x, y, z))
    if (components[0] == 'vn'):
        x = float(components[1])
        y = float(components[2])
        z = float(components[3])
        normals.append((x, y, z))
    if (components[0] == 'f'):
        vert0 = int(components[1].split('//')[0]) - 1
        vert1 = int(components[2].split('//')[0]) - 1
        vert2 = int(components[3].split('//')[0]) - 1
        normal = int(components[1].split('//')[1]) - 1
        faces.append((vert0, vert1, vert2, normal))

#print(len(vertices))
#print(len(normals))
#print(len(faces))
#print(maxx, maxy, maxz, minx, miny, minz)

index = 3
line_template = "list[%d] = new triangle(vec3(%f,%f,%f), vec3(%f,%f,%f), vec3(%f,%f,%f), vec3(%f,%f,%f));\n"
for face in faces:
    print(line_template % (index, vertices[face[0]][0], vertices[face[0]][1], vertices[face[0]][2], 
                                  vertices[face[1]][0], vertices[face[1]][1], vertices[face[1]][2],
                                  vertices[face[2]][0], vertices[face[2]][1], vertices[face[2]][2],
                                  normals[face[3]][0], normals[face[3]][1], normals[face[3]][2]))
    index = index + 1
