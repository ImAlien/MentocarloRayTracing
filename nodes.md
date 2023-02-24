

从二维平面上的像素做射线出发寻找三维空间中相交的物体的过程。



屏幕空间上的每个像素点，可以转移到归一化设备空间上，再计算ndc space的z值，即可确定ndc space的光线方向；

- 局部空间(Local Space，或者称为物体空间(Object Space))
- 世界空间(World Space)
- 观察空间(View Space，或者称为视觉空间(Eye Space))
- 裁剪空间(Clip Space)
- 屏幕空间(Screen Space)



光线追踪的优势：处理一些全局效果:软阴影,Glossy反射，间接光照