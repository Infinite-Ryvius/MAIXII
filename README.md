# MAIXII
#### 1.介绍
该工程主要用于测试MAIXII-Dock相关的媒体API接口  
由于MAIXII使用的平台为v831，因此主要参考的是全志的API。  
文档参考手册: http://files.lindeni.org/lindenis-v5/documents/soft_design/MPP/
#### 2.编译
编译该工程需要安装交叉编译链，详见 https://github.com/sipeed/libmaix   
直接在源路径使用 `Make` 即可编译
##### 2.1 编译tool工具
在tool/* 下可以直接用 `make` 编译相关的静态库，并需要把静态库放到./lib/static/ 下
#### 3.测试
- 交叉编译后需要将lib库下的动态库上传到/usr/lib/下
- 还需将awlib的动态库文件夹上传到/usr/下，并为awlib添加相应的环境变量
- 为awlib添加环境变量，可在/etc/profile文件下添加 `export LD_LIBRARY_PATH=/usr/awlib:${LD_LIBRARY_PATH}`
