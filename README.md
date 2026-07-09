# 景区路径规划系统 · Scenic Path Planning

基于 **图论** 算法的景区路径规划系统, 支持 **导游线路生成 (DFS)**、**回路检测**、**最短路径查询 (Dijkstra / Floyd)** 和 **最小生成树 (Prim / Kruskal)**。

---

## ✨ 功能一览

| 编号 | 功能 | 核心算法 |
|------|------|---------|
| 1 | 创建景区景点图 | 邻接表存储 |
| 2 | 展示景区景点分布图 | 邻接矩阵输出 |
| 3 | 导游线路生成 | **深度优先遍历 (DFS)** |
| 4 | 导游线路中的回路检测 | DFS + 回边判断 + 并查集思想 |
| 5 | 两景点间最短路径和距离 | **Dijkstra 贪心算法** |
| 6 | 全源最短路径表 | **Floyd 动态规划** |
| 7 | 最小生成树 | **Prim 算法** |
| 8 | 最小生成树 | **Kruskal 算法 + 并查集** |
| 9 | 保存景区景点图到文件 | 文本文件 I/O |
| 10 | 从文件加载景区景点图 | 文本文件 I/O |

---

## 📁 项目结构

```
实验/
├── global.h        # 全局常量 (INF, MAX_VERTEX, MAX_NAME)
├── graph.h         # 数据结构与函数声明 (ALGraph, ArcNode, VNode)
├── graph.c         # 图构建 / 销毁 / 邻接矩阵 / DFS / 回路检测
├── shortest_path.c # Dijkstra 单源最短路 + Floyd 全源最短路
├── mst.c           # Prim + Kruskal + 并查集 (Union-Find)
├── file_io.c       # 景点数据读写 (文本格式)
├── main.c          # 主菜单与交互逻辑
├── build.bat       # Windows 一键编译脚本
└── README.md       # 项目说明 (本文件)
```

---

## 🛠️ 编译与运行

### Windows (推荐)

```bat
REM 方式 1: GCC (MinGW)
build.bat
scenic.exe

REM 方式 2: Visual Studio
REM 打开 "Visual Studio 开发人员命令提示符", 然后:
build.bat cl
scenic.exe
```

### 手动编译

```bash
# GCC
gcc -O2 -o scenic.exe graph.c shortest_path.c mst.c file_io.c main.c

# Clang
clang -O2 -o scenic.exe graph.c shortest_path.c mst.c file_io.c main.c

# MSVC
cl /O2 /Fe:scenic.exe graph.c shortest_path.c mst.c file_io.c main.c
```

---

## 🎯 快速上手 · 示例数据

启动程序后, 输入 `1` 创建景区景点图, 按以下格式输入:

```
8 9
大门 碧玉潭 红叶林 赏花园 飞云石 九曲桥 白虎岩 观月阁
大门 碧玉潭 9
大门 红叶林 8
碧玉潭 赏花园 7
碧玉潭 飞云石 6
飞云石 观月阁 3
赏花园 观月阁 11
红叶林 九曲桥 4
红叶林 白虎岩 5
九曲桥 白虎岩 7
```

预期输出:

| 功能 | 预期结果 |
|------|---------|
| 邻接矩阵 | 大门行: `0 9 8 32767 32767 32767 32767 32767` |
| 导游线路 | `大门-红叶林-白虎岩-九曲桥-红叶林-大门-碧玉潭-飞云石-观月阁-赏花园-` |
| 回路检测 | `红叶林-白虎岩-九曲桥-红叶林` |
| 大门→观月阁最短路径 | `大门 → 碧玉潭 → 飞云石 → 观月阁`, 距离 = **18** |

---

## 📐 算法简介

### Dijkstra 算法
- **思想**: 贪心, 每次从"未确定集合"选一个距起点最近的顶点, 加入"已确定集合", 然后松弛其所有邻接边.
- **数据结构**: `dist[]` 距离数组 + `prev[]` 前驱数组 + `set[]` 标记数组.
- **复杂度**: O(n²) (本实现) / O((n+e) log n) (若用二叉堆)

### Floyd 算法
- **思想**: 动态规划, `dist[k][i][j]` 表示 i→j 且中间顶点只能用 {0..k} 的最短距离.
- **递推式**: `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])`
- **复杂度**: O(n³), 一次计算得到任意两点的最短距离

### Prim 算法
- **思想**: 从任意顶点开始, 每次选"连接已选集合和未选集合"的最小权边.
- **数据结构**: `lowcost[]` + `adjvex[]`
- **复杂度**: O(n²) — 适合稠密图

### Kruskal 算法
- **思想**: 对所有边按权升序排序, 依次选"不构成回路"的最小权边.
- **核心工具**: 并查集 (Union-Find) — 路径压缩 + 按秩合并, 几乎 O(1)
- **复杂度**: O(e log e) — 适合稀疏图

### DFS 导游线路与回路检测
- **线路**: 标准 DFS, 邻居按顶点下标降序. 当从子递归返回时, 若当前节点仍有未访问邻居, 则在返回路径中记录"回到当前节点".
- **回路**: 发现回边 `u → v` (v 是 u 的祖先且仍在递归栈中), 沿 `parent[]` 从 u 回溯到 v, 反转后首尾用 v 闭合.

---

## 📄 文件格式

保存/加载使用的文本格式:

```
n_vertexes n_edges
vertex1 vertex2 vertex3 ...
vertexA vertexB weight
vertexC vertexD weight
...
```

例如:
```
8 9
大门 碧玉潭 红叶林 赏花园 飞云石 九曲桥 白虎岩 观月阁
大门 碧玉潭 9
大门 红叶林 8
...
```

---

## 🔑 全局常量

在 `global.h` 中定义:

| 宏 | 默认值 | 说明 |
|---|-------|------|
| `INF` | 32767 | 无穷大 / 无连通路径 |
| `MAX_VERTEX` | 100 | 最多容纳的景点数 |
| `MAX_NAME` | 64 | 单个景点名的最大字符数 |

---

## 🧠 设计要点

1. **邻接表存储**: 用单链表组织每个顶点的邻接边, 稀疏图下空间效率优于邻接矩阵.
2. **头文件保护**: `#ifndef GRAPH_H / #define GRAPH_H / #endif`, 防止多重包含.
3. **模块划分**: 图操作 / 最短路 / MST / 文件 I/O / 主菜单 各成一个 `.c`, 便于维护和测试.
4. **中文支持**: 源文件使用 UTF-8 编码, Windows 控制台若用 GBK, 建议用 `build.bat` 的默认 GCC 编译模式, 或自行设置控制台编码 `chcp 65001`.

---

## 📝 开发记录

- 语言: C (C99 兼容)
- 依赖: 仅标准库 (`stdio.h`, `stdlib.h`, `string.h`)
- 内存安全: `DestroyGraph()` 遍历并 free 所有链表节点, 无内存泄漏
- 输入容错: `CreateGraph()` 对非法输入有提示; `ShowShortestPath()` 检查起点/终点是否存在

