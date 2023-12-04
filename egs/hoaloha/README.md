# Create Windows Build for Kaldi
- Follow instructions here: https://github.com/HoalohaRobotics/kaldi/blob/master/windows/INSTALL.md
- I use the following options:
  - OpenFST
    - Clone the OpenFST repo from github to get OpenFST and build the sln (instead of CMake) 
  - Linear Algebra
    - use the "OpenBLAS" (not Intel MKL)
  - CUDA
    - none/ignore
 - Build Process should populate a directory called "kaldiwin_vs2019_OPENBLAS" in the root of the repo.
# (Re)Apply Git Changes
- Step 1 possibly overwrote my additions and changes, so make sure the kaldiwin_vs2019_OPENBLAS and repo directories matche my Git commits, specifically...
  - kaldiwin_vs2019_OPENBLAS\kaldiwin_vs2019.sln
  - kaldiwin_vs2019_OPENBLAS\kaldiwin\hoaloha-transcribe\*
  - egs\hoaloha\*
# Download Librispeech Model and Build the Graph in WSL
  - The model was too big to add to Git, so this one-time process will create it:
  - Need a WSL instance with docker installed.  (Replace path to hoaloha dir in egs with your local path).
    ```
    wsl  
    sudo dockerd &
    sudo docker run -it -v /mnt/c/Users/Gersh/Git/kaldi_hoaloha/egs/hoaloha:/opt/kaldi/egs/hoaloha kaldiasr/kaldi:latest bash 
  - In the running container
    ```
    ./setup.sh
# Build/Run in Windows
- Open kaldiwin_vs2019_OPENBLAS\kaldiwin_vs2019.sln
- Use "Debug" "x64" settings
- Find "hoaloha-transcribe" project and set to as startup project
- Project Properties make the Debugging->Working Directory point to your egs\hoaloha\s5 directory.
- Build the hoaloha-transcribe project (Ctrl-B)  [Avoid building the whole solution as that will take hours]
