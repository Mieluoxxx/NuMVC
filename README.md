本项目为Nuist-2021随机算法大作业

复现NuMVC代码

[个人论文阅读笔记](https://lab.moguw.top/post/NuMVC的复现/)

项目运行：
先配置xmake环境

```bash
xmake -r numvc && xmake run numvc
```

优点:
* 项目在小数据集上效果不错，用时较少，与正确答案比较接近
* 用xmake打包具有不错的跨平台运行特性

缺点:
* 在路径设置上没有做到全自动，仍然需要部分手动设置，main.cpp和convert.cpp
* 当节点和边达到一定规模时，例如p_hat1500-1.clq数据集，会发生越界访问的情况，本人Cpp经验不足无法解决


参考资料

NuMVC: An efficient local search algorithm for minimum vertex cover. S Cai, K Su, C Luo, A Sattar. Journal of Artificial Intelligence Research 46, 687-716, 2013.

主要参考项目
* [NuMVC复现1](https://github.com/Zachary-ZS/MaxClique/tree/master)
* [NuMVC复现2-Java](https://github.com/csayogesh/NuMVC)
