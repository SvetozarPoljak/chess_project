## Run:

```bash
sudo systemctl enable bluetooth
sudo systemctl start bluetooth
cd chess_project
mkdir build && cd build
make -j1
sudo ./chess -platform eglfs
