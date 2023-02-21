# CG_project
Monte Carlo Path Trace
ZJU CG homework

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

在vs中查看<glm/glm.hpp>是否能够正确导入

查看<opencv2/core/core.hpp>能否正确导入
