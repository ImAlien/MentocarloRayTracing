# CG_project
Monte Carlo Path Trace

### 编译环境

Visual Studio 2017以上

c++11

#### vcpkg配置

##### 安装vcpkg:

https://github.com/microsoft/vcpkg

下载后之后点击其中的bat命令

将目录放入环境变量，cmd中输入vcpkg能出现命令即可。

```
vcpkg integrate install
vcpkg install glm
vcpkg install opencv
```

##### 安装完成：

在vs中查看<glm/glm.hpp>是否能够正确导入(x86模式)

查看<opencv2/core/core.hpp>能否正确导入

### 运行

点击sln文件，项目编译时需要选 x86模式

##### 参数修改：

main.h 中的SCENE_NAME自定义，注意要跟scenes文件夹中的场景文件名相一致，例如”cornell-box”, “bedroom”, “veach-mis”，

在main.h中更改SPP的值，可以更改Sample per pixel的值。

##### 结果生成：

主文件夹下的image.png会展示所渲染得到的图形。