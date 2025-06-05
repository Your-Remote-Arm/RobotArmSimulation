import pybullet as p
import pybullet_data
import tkinter as tk
from tkinter import ttk
import time
import threading

class Tables:
    @staticmethod
    def update_table(tree, ur5_id, joint_indices):
        while True:
            for i, joint in enumerate(joint_indices):
                pos, vel, _, _ = p.getJointState(ur5_id, joint)
                tree.item(str(i), values=(joint, f"{pos:.3f}", f"{vel:.3f}"))
            time.sleep(0.1)

    @staticmethod
    def launch_gui(joint_indices, ur5_id):
        root = tk.Tk()
        root.title("Joint States")

        tree = ttk.Treeview(root, columns=("Joint", "Position", "Velocity"), show="headings")
        tree.heading("Joint", text="Joint Index")
        tree.heading("Position", text="Position (rad)")
        tree.heading("Velocity", text="Velocity (rad/s)")

        for i, j in enumerate(joint_indices):
            tree.insert("", "end", iid=str(i), values=(j, "-", "-"))

        tree.pack(fill="both", expand=True)

        # Start update thread
        thread = threading.Thread(
            target=Tables.update_table,
            args=(tree, ur5_id, joint_indices),
            daemon=True
        )
        thread.start()

        root.mainloop()
