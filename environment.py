import pybullet as p
import time
import pybullet_data
import os

physicsClient = p.connect(p.GUI)
p.resetSimulation()
p.configureDebugVisualizer(p.COV_ENABLE_GUI, 0)
p.setGravity(0,0,-9.8)

script_dir = os.path.dirname(os.path.abspath(__file__))

p.setAdditionalSearchPath(os.path.join(script_dir, "data"))
p.setAdditionalSearchPath(pybullet_data.getDataPath())

planeId = p.loadURDF("custom_plane.urdf")
armId   = p.loadURDF("universal-robot-ur5/ur5robot.urdf",
                     [0,0,0],
                     p.getQuaternionFromEuler([0,0,0]),
                     useFixedBase=True)

# configuration
xmin, xmax = -2, 2
ymin, ymax = -2, 2
zmin, zmax =  0, 4

edges = [
    # bottom square
    ([xmin,ymin,zmin],[xmax,ymin,zmin]),
    ([xmax,ymin,zmin],[xmax,ymax,zmin]),
    ([xmax,ymax,zmin],[xmin,ymax,zmin]),
    ([xmin,ymax,zmin],[xmin,ymin,zmin]),
    # top square
    ([xmin,ymin,zmax],[xmax,ymin,zmax]),
    ([xmax,ymin,zmax],[xmax,ymax,zmax]),
    ([xmax,ymax,zmax],[xmin,ymax,zmax]),
    ([xmin,ymax,zmax],[xmin,ymin,zmax]),
    # vertical pillars
    ([xmin,ymin,zmin],[xmin,ymin,zmax]),
    ([xmax,ymin,zmin],[xmax,ymin,zmax]),
    ([xmax,ymax,zmin],[xmax,ymax,zmax]),
    ([xmin,ymax,zmin],[xmin,ymax,zmax]),
]

for (start, end) in edges:
    p.addUserDebugLine(start, end,
                       lineColorRGB=[0.5, 0.5, 0.5],
                       lineWidth=1,
                       lifeTime=0)

half_size = 2
spacing   = 1
z_level   = 0
color     = [0.5,0.5,0.5]
width     = 1

# draw the grid lines
for i in range(-half_size, half_size+1, spacing):
    # lines parallel to X
    p.addUserDebugLine([ -half_size, i, z_level],
                       [  half_size, i, z_level],
                       lineColorRGB=color,
                       lineWidth=width)
    # lines parallel to Y
    p.addUserDebugLine([ i, -half_size, z_level],
                       [ i,  half_size, z_level],
                       lineColorRGB=color,
                       lineWidth=width)

try:
    # Run forever (or until you Ctrl+C)
    while True:
        p.stepSimulation()
        time.sleep(1./240.)
except KeyboardInterrupt:
    # clean exit on Ctrl+C
    pass

p.disconnect()

