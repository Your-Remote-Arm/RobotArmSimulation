import pybullet as p
import numpy as np

class Gripper:
    def __init__(self):
        self.activated = False

    def step(self):
        return
    
    def activate(self, objects):
        del objects
        return

    def release(self):
        return

class Generic(Gripper):
    def __init__(self, robot_id, ee_link_index, obj_ids):
        super().__init__()

        # Load gripper and attach to end-effector
        gripper_pose = ((0.487, 0.109, 0.438), p.getQuaternionFromEuler((np.pi, 0, 0)))
        self.base = p.loadURDF('assets/ur5/gripper/robotiq_2f_85.urdf', *gripper_pose)

        p.createConstraint(
            parentBodyUniqueId=robot_id,
            parentLinkIndex=ee_link_index,
            childBodyUniqueId=self.base,
            childLinkIndex=-1,
            jointType=p.JOINT_FIXED,
            jointAxis=(0, 0, 0),
            parentFramePosition=(0, 0, 0),
            childFramePosition=(0, 0, 0.01)
        )

        # Parse joints
        self.joint_map = {}
        for i in range(p.getNumJoints(self.base)):
            name = p.getJointInfo(self.base, i)[1].decode()
            self.joint_map[name] = i

        # Identify main controllable joints (adjust names based on URDF)
        self.left_joint = self.joint_map.get("left_inner_finger_joint")
        self.right_joint = self.joint_map.get("right_inner_finger_joint")

        # Add gear constraint if mimic joints aren't defined in URDF
        # Comment this block out if URDF already handles it
        if self.left_joint is not None and self.right_joint is not None:
            self._add_mimic_gear_constraint(self.left_joint, self.right_joint)

        # Open/close position range
        self.open_pos = 0.8
        self.close_pos = 0.0
        self.force = 20

    def _add_mimic_gear_constraint(self, joint_a, joint_b):
        cid = p.createConstraint(
            parentBodyUniqueId=self.base,
            parentLinkIndex=joint_a,
            childBodyUniqueId=self.base,
            childLinkIndex=joint_b,
            jointType=p.JOINT_GEAR,
            jointAxis=[0, 0, 0],
            parentFramePosition=[0, 0, 0],
            childFramePosition=[0, 0, 0.01]
        )
        p.changeConstraint(cid, gearRatio=-1, maxForce=1000)
        print(f"[Gripper] Mimic constraint added between joints {joint_a} and {joint_b}")

    def open(self):
        if self.left_joint is not None:
            p.setJointMotorControl2(
                bodyUniqueId=self.base,
                jointIndex=self.left_joint,
                controlMode=p.POSITION_CONTROL,
                targetPosition=self.open_pos,
                force=self.force
            )

    def close(self):
        if self.left_joint is not None:
            p.setJointMotorControl2(
                bodyUniqueId=self.base,
                jointIndex=self.left_joint,
                controlMode=p.POSITION_CONTROL,
                targetPosition=self.close_pos,
                force=self.force
            )