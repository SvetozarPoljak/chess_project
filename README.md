## Run:

```bash
## if wifi and bluetooth share same antenna
## do: sudo rfkill block wifi, then later 
## undo with: sudo rfkill unblock wifi 
## when finished running application
sudo systemctl enable bluetooth
sudo systemctl start bluetooth
cd chess_project
mkdir build && cd build
make -j1
sudo ./chess -platform eglfs
