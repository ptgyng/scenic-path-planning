/* =====================================================================
   graph.h — 景区景点图的数据结构与函数接口声明
   数据结构: ALGraph (Adjacency List Graph, 邻接表存储的图)
   说明: 所有 .c 实现文件 include 本头文件即可拿到结构体与函数原型
   ===================================================================== */

#ifndef GRAPH_H
#define GRAPH_H

#include "global.h"   /* 引入 INF、MAX_VERTEX、MAX_NAME */

/* =====================================================================
   1. 边节点 (ArcNode) —— 邻接表链表的一个节点
   ===================================================================== */
typedef struct ArcNode {
    int adjvex;             /* 该边指向的顶点下标 (0 ~ vertex_count-1) */
    int weight;             /* 该边的权值 (景点距离) */
    struct ArcNode *nextarc;/* 指向下一条邻接边 (形成链表) */
} ArcNode;

/* =====================================================================
   2. 顶点节点 (VNode) —— 邻接表的一个"表头"
   ===================================================================== */
typedef struct VNode {
    char data[MAX_NAME];    /* 顶点信息: 景点名称 (如 "大门") */
    ArcNode *firstarc;      /* 指向第一条依附该顶点的边 */
} VNode;

/* =====================================================================
   3. 图结构 (ALGraph) —— Adjacency List Graph
   景区景点图 = 顶点数组 (VNode[]) + 顶点数 + 边数
   ===================================================================== */
typedef struct {
    VNode vertices[MAX_VERTEX]; /* 顶点数组, 即邻接表的表头 */
    int vertex_count;           /* 当前实际顶点数 */
    int edge_count;             /* 当前实际边数 */
} ALGraph;

/* =====================================================================
   4. 函数接口声明
   按功能模块划分: 图构建 → 输出 → 遍历 → 回路检测 → 最短路径 → MST → 文件IO
   ===================================================================== */

/* -------- 模块 A: 图构建与销毁 -------- */
/* 根据交互输入创建景区景点图 */
void CreateGraph(ALGraph *G);

/* 释放图中所有边节点 (防止内存泄漏) */
void DestroyGraph(ALGraph *G);

/* 根据顶点名查找下标, 找不到返回 -1 */
int LocateVertex(ALGraph *G, const char *name);

/* 向图中添加一条无向边 (v1_name, v2_name, weight) */
void AddEdge(ALGraph *G, const char *v1_name, const char *v2_name, int weight);

/* -------- 模块 B: 图展示 -------- */
/* 以邻接矩阵形式输出景区景点分布图 */
void ShowGraph(ALGraph *G);

/* -------- 模块 C: DFS 遍历与导游线路 -------- */
/* DFS 递归遍历, 返回生成的导游线路 (存于 route), 长度返回 */
void DFSTraverse(ALGraph *G, int start, int route[], int *route_len);

/* 输出完整导游线路 (内部调用 DFSTraverse) */
void ShowTourRoute(ALGraph *G);

/* -------- 模块 D: 回路检测 -------- */
/* 检测导游线路中是否存在回路, 若有则输出回路路径, 返回 1; 无则返回 0 */
int DetectCycle(ALGraph *G);

/* -------- 模块 E: 最短路径 (Dijkstra + Floyd) -------- */
/* 计算 start → end 的最短路径及距离, 结果存于 path[], 长度返回 */
void Dijkstra(ALGraph *G, int start, int end,
              int path[], int *path_len, int *distance);

/* 展示某两点之间的最短路径结果 (交互调用 Dijkstra) */
void ShowShortestPath(ALGraph *G);

/* Floyd 全源最短路径, 结果存于 dist[][] 二维数组 */
void Floyd(ALGraph *G, int dist[MAX_VERTEX][MAX_VERTEX]);

/* 展示 Floyd 全源最短路径表 */
void ShowFloyd(ALGraph *G);

/* -------- 模块 F: 最小生成树 (Prim + Kruskal) -------- */
/* Prim 算法求最小生成树, 输出每一条加入的边及其权值 */
void Prim(ALGraph *G);

/* Kruskal 算法求最小生成树, 需要用到并查集辅助 */
void Kruskal(ALGraph *G);

/* -------- 模块 G: 文件读写 -------- */
/* 把当前图数据保存到文件, 成功返回 1, 失败返回 0 */
int SaveGraph(ALGraph *G, const char *filename);

/* 从文件中读取景点图, 成功返回 1, 失败返回 0 */
int LoadGraph(ALGraph *G, const char *filename);

#endif /* GRAPH_H */
