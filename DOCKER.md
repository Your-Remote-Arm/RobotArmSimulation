Docker: Build, Run, Share
================================

Why Docker for this repo
- Consistent ROS2 Humble environment across all machines
- Keep your host clean; avoid installing heavy system deps
- Fast onboarding: build once, reuse the same image everywhere
- GUI support (RViz, tools) via X11 without local installs
- Host networking simplifies ROS graph discovery across devices
- Share preconfigured images on Docker Hub for collaboration
- Checkpoint working setups per feature/commit by committing containers

Quick start
- Build base image: `docker build -t ras:dev .`
- Run interactive dev shell (with host networking + X11):
  `docker run --rm -it --name roverdev --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw ras:dev`
- Inside container, run setup: `bash setup_scripts/setup_everything_common.sh`


Local development
- Build image: Creates a clean Ubuntu 22.04 base with only core tools. The ROS/SDK bits are installed by scripts you run inside the container.
  - `docker build -t ras:dev .`
- Run container: Host networking helps ROS; X11 mount enables GUIs like RViz.
  - `docker run --rm -it --name roverdev --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw ras:dev`
- Initialize environment inside: Installs ROS 2 Humble and common deps.
  - `bash setup_scripts/setup_everything_common.sh`


Persisting work to a new image
- Commit the configured container to an image (so teammates can reuse):
  - `docker commit roverdev <dockerhub_user>/ras:humble-<date>`
- Test the new image locally:
  - `docker run --rm -it --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw <dockerhub_user>/ras:humble-<date>`


Using Docker Hub (collaboration)
- Login: `docker login`
- Tag (optional rename): `docker tag ras:dev <dockerhub_user>/ras:base`
- Push: `docker push <dockerhub_user>/ras:humble-<date>`
- Pull (on another machine): `docker pull <dockerhub_user>/ras:humble-<date>`
- Run the pulled image:
  - `docker run --rm -it --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw <dockerhub_user>/ras:humble-<date>`


Recommended tagging
- Immutable build tag: `humble-YYYYMMDD` (e.g., `humble-2025-09-24`).
- Moving tag for latest good build: `humble-latest`.
- Optional branch tag for feature work: `feature-<shortname>`.


Mounting your workspace (optional)
- If you prefer editing the repo on the host and using the container only for tools, mount the repo instead of copying it:
  - `docker run --rm -it --name roverdev --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw -v $(pwd):/root/ras ras:dev`
- Then run setup scripts inside the container as usual.


Updating your image
- Rebuild base (after Dockerfile changes): `docker build -t ras:dev .`
- Re-provision a new container, rerun setup script, and re-commit a fresh image/tag.


Clean-up
- Stop/remove container: `docker rm -f roverdev` (if still running)
- Remove unused images: `docker image prune -a`
- Remove dangling volumes (if created): `docker volume prune`


Troubleshooting
- GUI doesn’t show: Ensure X11 is allowed: on host, `xhost +local:`. Then rerun container with `-e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw`.
- Submodules empty: Inside container or host, run `git submodule update --init --recursive`.
- Missing ROS package at build time: Inside container, after sourcing ROS, run `rosdep update && rosdep install --from-paths src -y --ignore-src`.
