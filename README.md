# RobotArmSimulation

### Installation (On ArchLinux)
#### Note: replace 'profile' with your terminal profile

Install miniconda for easy package management
```
curl -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh -b -u
echo $'\nexport PATH=~/miniconda3/bin:"${PATH}"\n' >> ~/.profile  # Add Conda to PATH.
source ~/.profile
conda init
```

Activate environment
```
cd ~/RobotArmSimulation
conda create --name ras python=3.7 -y
conda activate ras
```

Install base development packages
```
sudo pacman -Syu --needed base-devel
pip install -r requirements.txt
python setup.py install --user
```

Install GPU acceleration for Tensorflow
```
yay -S cuda-10.1
tar -xvzf cudnn-10.1-linux-x64-v7.6.5.32.tgz
sudo cp -P cuda/include/cudnn.h /opt/cuda-10.1/include/
sudo cp -P cuda/lib64/libcudnn* /opt/cuda-10.1/lib64/
sudo chmod a+r /opt/cuda-10.1/include/cudnn.h /opt/cuda-10.1/lib64/libcudnn*
export CUDA_HOME=/opt/cuda-10.1
export PATH=$CUDA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH
source ~/.profile
```