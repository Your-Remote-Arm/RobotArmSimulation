# camera.py
import numpy as np
import pybullet as p

class LinkCamera:
    """
    A virtual camera rigidly mounted to a specific robot link.

    Each time you call `render()`, it:
      1. Queries the link’s world pose via getLinkState.
      2. Applies a fixed offset in the link’s local frame.
      3. Builds an OpenGL‐style view matrix looking forward.
      4. Builds a projection matrix (perspective FOV).
      5. Returns (rgb, depth, segm) from p.getCameraImage.

    Args:
      robot_id     (int):    bodyUniqueId of the robot (e.g. your UR5).
      link_index   (int):    The link index to attach the camera to.
      offset       (3‐tuple): Offset in meters in the link’s local frame (x, y, z).
      fov          (float):  Vertical field of view, in degrees.
      width        (int):    Image width (pixels).
      height       (int):    Image height (pixels).
      z_near       (float):  Near clipping plane (m).
      z_far        (float):  Far clipping plane (m).
    """
    def __init__(self, robot_id, link_index,
                 offset=(0.0, 0.0, 0.10),
                 fov=60.0,
                 width=640, height=480,
                 z_near=0.01, z_far=5.0):
        self.robot_id   = robot_id
        self.link_index = link_index
        self.offset     = np.array(offset, dtype=np.float32)
        self.fov        = fov
        self.width      = width
        self.height     = height
        self.aspect     = width / height
        self.z_near     = z_near
        self.z_far      = z_far

    def _compute_view_proj(self):
        # 1) Query link’s world‐frame (pos,quat):
        link_state = p.getLinkState(
            self.robot_id,
            self.link_index,
            computeForwardKinematics=True
        )
        link_pos, link_orn = link_state[0], link_state[1]

        # 2) Quaternion → rotation matrix (3×3):
        rot = p.getMatrixFromQuaternion(link_orn)
        rot = np.array(rot).reshape(3, 3)
        # rot[:,0]= link’s +X in world, rot[:,1]= +Y, rot[:,2]= +Z

        # 3) Apply local‐frame offset → world coords:
        world_offset = rot.dot(self.offset)
        eye = np.array(link_pos) + world_offset

        # 4) Define forward = link’s +Z axis, up = link’s +Y axis:
        forward = rot[:, 2]
        up      = rot[:, 1]
        target  = eye + forward

        # 5) Build view matrix:
        view = p.computeViewMatrix(
            cameraEyePosition    = eye.tolist(),
            cameraTargetPosition = target.tolist(),
            cameraUpVector       = up.tolist()
        )

        # 6) Build projection matrix:
        proj = p.computeProjectionMatrixFOV(
            fov     = self.fov,
            aspect  = self.aspect,
            nearVal = self.z_near,
            farVal  = self.z_far
        )

        return view, proj

    def render(self):
        """
        Renders an RGB-D (and segmentation) image from this link‐mounted camera.
        Returns:
          rgb   : (H×W×3) uint8 array
          depth : (H×W) float32 array (in meters)
          segm  : (H×W) int32 array of segmentation IDs
        """
        view_mat, proj_mat = self._compute_view_proj()

        _, _, rgba, depth_buf, segm_buf = p.getCameraImage(
            width            = self.width,
            height           = self.height,
            viewMatrix       = view_mat,
            projectionMatrix = proj_mat,
            flags            = p.ER_BULLET_HARDWARE_OPENGL
        )

        # Convert RGBA → RGB
        rgba = np.array(rgba, dtype=np.uint8).reshape(
            (self.height, self.width, 4)
        )
        rgb = rgba[:, :, :3]

        # Convert depth buffer → linear depth in meters
        zbuf = np.array(depth_buf).reshape((self.height, self.width)).astype(np.float32)
        z_ndc = 2.0 * zbuf - 1.0
        depth = (2.0 * self.z_near * self.z_far) / (
            self.z_far + self.z_near - z_ndc * (self.z_far - self.z_near)
        )

        segm = np.array(segm_buf, dtype=np.int32).reshape((self.height, self.width))

        return rgb, depth, segm
