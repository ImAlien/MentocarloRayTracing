# Monte Carlo Path Trace
### 编译环境

Visual Studio 2017以上

c++11


### 运行

点击sln文件，项目编译时需要选 x86模式

##### 参数修改：

main.h 中的SCENE_NAME自定义，注意要跟scenes文件夹中的场景文件夹名相一致，例如”cornell-box”, “bedroom”, “veach-mis”，

在main.h中更改SPP的值，可以更改Sample per pixel的值。

##### 结果生成：

主文件夹下的image.png会展示所渲染得到的图形。