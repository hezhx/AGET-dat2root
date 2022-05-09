# AGET-dat2root
The Optimized code to convert the `.dat` binary data to `.root` and `.txt` file.

[主文件](./Dat2Root.C)为Dat2Root.C。
```
root -l
[0] .L Dat2Root.C
[1] Dat2Root("test.dat")
```
Wave_Display.C可以画出相邻四个事件的波形。\
get_Energy_Spectrum.C给出点火pad的能谱，将一个触发内的所有点火pad的信号幅度叠加，作为一个时间（触发）的能量值。\
WaveFit.C拟合一个波形。
