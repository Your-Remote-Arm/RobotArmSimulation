# RobotArmSimulation


def setup_environment():
    """Load plane, grid lines, and UR5 arm exactly once."""
    p.resetSimulation()
    p.setGravity(0, 0, -9.8)
    # base search paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    p.setAdditionalSearchPath(os.path.join(script_dir, "data"))
    p.setAdditionalSearchPath(pybullet_data.getDataPath())

    # load your custom plane
    planeId = p.loadURDF("custom_plane.urdf")

    # draw grid & cube frame (can be factored into another helper if you like)
    xmin, xmax = -2, 2
    ymin, ymax = -2, 2
    zmin, zmax =  0, 4

    edges = [
        ([xmin,ymin,zmin],[xmax,ymin,zmin]),
        ([xmax,ymin,zmin],[xmax,ymax,zmin]),
        ([xmax,ymax,zmin],[xmin,ymax,zmin]),
        ([xmin,ymax,zmin],[xmin,ymin,zmin]),
        ([xmin,ymin,zmax],[xmax,ymin,zmax]),
        ([xmax,ymin,zmax],[xmax,ymax,zmax]),
        ([xmax,ymax,zmax],[xmin,ymax,zmax]),
def setup_environment():
    """Load plane, grid lines, and UR5 arm exactly once."""
    p.resetSimulation()
    p.setGravity(0, 0, -9.8)
    # base search paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    p.setAdditionalSearchPath(os.path.join(script_dir, "data"))
    p.setAdditionalSearchPath(pybullet_data.getDataPath())

    # load your custom plane
    planeId = p.loadURDF("custom_plane.urdf")

    # draw grid & cube frame (can be factored into another helper if you like)
    xmin, xmax = -2, 2
    ymin, ymax = -2, 2
    zmin, zmax =  0, 4

    edges = [
        ([xmin,ymin,zmin],[xmax,ymin,zmin]),
        ([xmax,ymin,zmin],[xmax,ymax,zmin]),
        ([xmax,ymax,zmin],[xmin,ymax,zmin]),
        ([xmin,ymax,zmin],[xmin,ymin,zmin]),
        ([xmin,ymin,zmax],[xmax,ymin,zmax]),
        ([xmax,ymin,zmax],[xmax,ymax,zmax]),
        ([xmax,ymax,zmax],[xmin,ymax,zmax]),
        ([xmin,ymax,zmax],[xmin,ymin,zmax]),
        ([xmin,ymin,zmin],[xmin,ymin,zmax]),
        ([xmax,ymin,zmin],[xmax,ymin,zmax]),
        ([xmax,ymax,zmin],[xmax,ymax,zmax]),
        ([xmin,ymax,zmin],[xmin,ymax,zmax]),
    ]
    for start, end in edges:
        p.addUserDebugLine(start, end,
                           lineColorRGB=[0.5, 0.5, 0.5],
                           lineWidth=1,
                           lifeTime=0)

    half_size = 2
    spacing   = 1
    z_level   = 0
    color     = [0.5, 0.5, 0.5]
    width     = 1
    for i in range(-half_size, half_size+1, spacing):
        p.addUserDebugLine([-half_size, i, z_level],
                           [ half_size, i, z_level],
                           lineColorRGB=color,
                           lineWidth=width)
        p.addUserDebugLine([i, -half_size, z_level],
                           [i,  half_size, z_level],
                           lineColorRGB=color,
                           lineWidth=width)

    # load UR5
    armId = p.loadURDF("universal-robot-ur5/ur5robot.urdf",
                       [0, 0, 0],
                       p.getQuaternionFromEuler([0, 0, 0]),
                       useFixedBase=True)

    return planeId, armId


def print_active_joints(armId):
    """Print all non-fixed joints’ index, name, angle, and link-world position."""
    rows = []
    num_joints = p.getNumJoints(armId)

    for i in range(num_joints):
        info = p.getJointInfo(armId, i)
        joint_type = info[2]
        if joint_type != p.JOINT_FIXED:
            joint_name = info[1].decode('utf-8')
            joint_angle = p.getJointState(armId, i)[0]
            link_pos      = p.getLinkState(armId, i)[0]  # (x, y, z)

            # append a row for tabulate
            rows.append([
                i,
                joint_name,
                f"{joint_angle:.3f}",
                f"{link_pos[0]:.3f}",
                f"{link_pos[1]:.3f}",
                f"{link_pos[2]:.3f}"
            ])

    headers = ["Index", "Name", "Angle", "X", "Y", "Z"]
    print(tabulate(rows, headers=headers, tablefmt="github"))
        p.addUserDebugLine([i, -half_size, z_level],
                           [i,  half_size, z_level],
                           lineColorRGB=color,
                           lineWidth=width)

    # load UR5
    armId = p.loadURDF("universal-robot-ur5/ur5robot.urdf",
                       [0, 0, 0],
                       p.getQuaternionFromEuler([0, 0, 0]),
                       useFixedBase=True)

    return planeId, armId


def print_active_joints(armId):
    """Print all non-fixed joints’ index, name, angle, and link-world position."""
    rows = []
    num_joints = p.getNumJoints(armId)

    for i in range(num_joints):
        info = p.getJointInfo(armId, i)
        joint_type = info[2]
        if joint_type != p.JOINT_FIXED:
            joint_name = info[1].decode('utf-8')
            joint_angle = p.getJointState(armId, i)[0]
            link_pos      = p.getLinkState(armId, i)[0]  # (x, y, z)

            # append a row for tabulate
            rows.append([
                i,
                joint_name,
                f"{joint_angle:.3f}",
                f"{link_pos[0]:.3f}",
                f"{link_pos[1]:.3f}",
                f"{link_pos[2]:.3f}"
            ])

    headers = ["Index", "Name", "Angle", "X", "Y", "Z"]
    print(tabulate(rows, headers=headers, tablefmt="github"))