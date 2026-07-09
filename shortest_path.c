/* =====================================================================
   shortest_path.c — 最短路径算法实现
   - Dijkstra: 单源最短路径 (指定起点 start → 终点 end)
   - Floyd:  全源最短路径 (所有顶点对之间的最短距离)
   ===================================================================== */

#include <stdio.h>
#include <string.h>
#include "graph.h"

/* ---- 辅助: 把图转换成邻接矩阵 (O(n + e)) ---- */
static void BuildMatrix(ALGraph *G, int mat[MAX_VERTEX][MAX_VERTEX])
{
    int i, j;
    ArcNode *p;
    for (i = 0; i < G->vertex_count; i++) {
        for (j = 0; j < G->vertex_count; j++)
            mat[i][j] = INF;
        mat[i][i] = 0;
    }
    for (i = 0; i < G->vertex_count; i++) {
        p = G->vertices[i].firstarc;
        while (p != NULL) {
            mat[i][p->adjvex] = p->weight;
            p = p->nextarc;
        }
    }
}

/* =====================================================================
   Dijkstra 算法
   思想: 贪心
     - dist[]  从起点到各点的当前最短距离
     - prev[]  最短路径上当前点的前驱 (用于最后回溯重建路径)
     - set[]   是否已确定最短路径 (在集合 S 内)
   步骤:
     1. 初始化: dist[start]=0, 其余=INF, prev 全=-1, set 全=0
     2. 循环 n 次:
        a. 从"未确定集合"中选出 dist 最小的 u
        b. 标记 set[u]=1 (加入已确定集合)
        c. 对 u 的每个邻居 v 做松弛: 若 dist[u] + w(u,v) < dist[v] 则更新
   3. 最后从 end 沿 prev 回溯得到路径, 再反向输出
   ===================================================================== */

void Dijkstra(ALGraph *G, int start, int end,
              int path[], int *path_len, int *distance)
{
    int dist[MAX_VERTEX];   /* 起点到各点的当前最短距离 */
    int prev[MAX_VERTEX];   /* 最短路径上的前驱节点 */
    int set[MAX_VERTEX];    /* 是否已确定 (加入集合 S) */
    int i, k, u, min;
    int tmp[MAX_VERTEX];    /* 临时存反向路径 (从 end 回溯到 start) */
    int cnt;

    /* -- 1. 初始化 -- */
    for (i = 0; i < G->vertex_count; i++) {
        dist[i] = INF;
        prev[i] = -1;
        set[i] = 0;
    }
    dist[start] = 0;

    /* -- 2. 循环 n 次, 每次确定一个点的最短路径 -- */
    for (k = 0; k < G->vertex_count; k++) {

        /* 2a. 从"未确定"集合中选 dist 最小的 u */
        u = -1;
        min = INF;
        for (i = 0; i < G->vertex_count; i++) {
            if (!set[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }
        if (u == -1) break;     /* 剩余点全部不可达 */
        if (u == end) break;    /* 终点已确定, 可提前退出 */

        /* 2b. 标记 u 为已确定 */
        set[u] = 1;

        /* 2c. 遍历 u 的所有邻接点, 松弛 */
        {
            ArcNode *p = G->vertices[u].firstarc;
            while (p != NULL) {
                int v = p->adjvex;
                int w = p->weight;
                if (!set[v] && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    prev[v] = u;
                }
                p = p->nextarc;
            }
        }
    }

    /* -- 3. 从 end 沿 prev 回溯重建路径 -- */
    cnt = 0;
    u = end;
    while (u != -1) {
        tmp[cnt++] = u;
        u = prev[u];
    }
    /* 反向: 从 start → end 顺序 */
    for (i = 0; i < cnt; i++)
        path[i] = tmp[cnt - 1 - i];
    *path_len = cnt;
    *distance = dist[end];
}

/* ---- 交互式查询两点间最短路径 ---- */
void ShowShortestPath(ALGraph *G)
{
    char s_name[MAX_NAME], e_name[MAX_NAME];
    int start, end;
    int path[MAX_VERTEX], path_len, distance;
    int i;

    printf("请输入起点景点名称：");
    scanf("%s", s_name);
    start = LocateVertex(G, s_name);
    if (start == -1) { printf("  未找到起点 [%s]！\n", s_name); return; }

    printf("请输入终点景点名称：");
    scanf("%s", e_name);
    end = LocateVertex(G, e_name);
    if (end == -1) { printf("  未找到终点 [%s]！\n", e_name); return; }

    Dijkstra(G, start, end, path, &path_len, &distance);

    if (distance == INF || path_len <= 1) {
        printf("\n从【%s】到【%s】不可达\n",
               G->vertices[start].data, G->vertices[end].data);
        return;
    }

    printf("\n从【%s】到【%s】的最短路径为：\n",
           G->vertices[start].data, G->vertices[end].data);
    for (i = 0; i < path_len; i++) {
        if (i > 0) printf(" → ");
        printf("%s", G->vertices[path[i]].data);
    }
    printf("\n最短距离为：%d 。\n", distance);
}

/* =====================================================================
   Floyd 算法 (Warshall-Floyd)
   思想: 动态规划
     dist_k[i][j] = 从 i 到 j, 中间节点仅允许使用 {0..k} 的最短距离
     递推式: dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])
   复杂度: O(n^3) — 三重循环
   优点: 一次计算给出任意两点间最短距离, 代码极其简洁
   缺点: 不能处理"存在负权环"的图 (本题权值都是距离, 保证非负)
   ===================================================================== */

void Floyd(ALGraph *G, int dist[MAX_VERTEX][MAX_VERTEX])
{
    int i, j, k, n;

    /* -- 1. 初始化: 直接用邻接矩阵作为初始距离 -- */
    BuildMatrix(G, dist);
    n = G->vertex_count;

    /* -- 2. 三重循环: k 是"中间节点集合的上限" --
         关键: k 必须是最外层循环 (依次把 0,1,2,...n-1 纳入中间集合)
    */
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if (dist[i][k] < INF && dist[k][j] < INF &&
                    dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}

/* ---- 展示 Floyd 全源最短路径表 ---- */
void ShowFloyd(ALGraph *G)
{
    int dist[MAX_VERTEX][MAX_VERTEX];
    int i, j;

    Floyd(G, dist);

    printf("\nFloyd 全源最短路径表（任意两点间最短距离）：\n");
    printf(" ");
    for (j = 0; j < G->vertex_count; j++)
        printf(" %s", G->vertices[j].data);
    printf("\n");
    for (i = 0; i < G->vertex_count; i++) {
        printf("%s", G->vertices[i].data);
        for (j = 0; j < G->vertex_count; j++) {
            if (dist[i][j] >= INF) printf(" INF");
            else printf(" %d", dist[i][j]);
        }
        printf("\n");
    }
}
