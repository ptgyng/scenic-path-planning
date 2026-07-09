/* =====================================================================
   graph.c — 景区景点图的核心实现
   模块: 图构建 (CreateGraph / DestroyGraph / AddEdge / LocateVertex)
         图展示 (ShowGraph)
         DFS 导游线路 (DFSTraverse / ShowTourRoute)
         回路检测 (DetectCycle)
   ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* =====================================================================
   模块 A: 图构建与销毁
   ===================================================================== */

/* ---- 根据顶点名查找顶点下标, 找不到返回 -1 ---- */
int LocateVertex(ALGraph *G, const char *name)
{
    int i;
    for (i = 0; i < G->vertex_count; i++) {
        if (strcmp(G->vertices[i].data, name) == 0)
            return i;
    }
    return -1;
}

/* ---- 向图中添加一条无向边 (v1 — v2, 权值 weight) ----
   无向图需要双向插入: v1 的邻接链表插入 v2, v2 的邻接链表插入 v1
*/
void AddEdge(ALGraph *G, const char *v1_name, const char *v2_name, int weight)
{
    int i = LocateVertex(G, v1_name);
    int j = LocateVertex(G, v2_name);
    ArcNode *p;

    if (i == -1 || j == -1) {
        printf("  警告: 未找到顶点 [%s] 或 [%s], 跳过此边\n", v1_name, v2_name);
        return;
    }

    /* v1 → v2 */
    p = (ArcNode *)malloc(sizeof(ArcNode));
    p->adjvex = j;
    p->weight = weight;
    p->nextarc = G->vertices[i].firstarc;
    G->vertices[i].firstarc = p;

    /* v2 → v1 */
    p = (ArcNode *)malloc(sizeof(ArcNode));
    p->adjvex = i;
    p->weight = weight;
    p->nextarc = G->vertices[j].firstarc;
    G->vertices[j].firstarc = p;

    G->edge_count++;
}

/* ---- 交互式创建景区景点图 ----
   输入:
     顶点数 边数
     顶点1 顶点2 ... 顶点N
     边1顶点A 边1顶点B 边1权值
     ...
*/
void CreateGraph(ALGraph *G)
{
    int i, n_edges;
    char v1[MAX_NAME], v2[MAX_NAME];
    int w;

    /* 1. 先把旧图清空 (防止重复调用) */
    DestroyGraph(G);

    /* 2. 读取顶点数和边数 (边数先暂存到局部变量) */
    if (scanf("%d %d", &G->vertex_count, &n_edges) != 2) {
        printf("  输入格式错误!\n");
        G->vertex_count = 0;
        return;
    }

    /* 3. 初始化: 读取每个顶点的名称, 同时清空邻接表头 */
    for (i = 0; i < G->vertex_count; i++) {
        scanf("%s", G->vertices[i].data);
        G->vertices[i].firstarc = NULL;
    }

    /* 4. 读取 n_edges 条边, 每次 AddEdge (AddEdge 内部 G->edge_count 自增) */
    G->edge_count = 0;
    for (i = 0; i < n_edges; i++) {
        scanf("%s %s %d", v1, v2, &w);
        AddEdge(G, v1, v2, w);
    }

    printf("景区景点图创建成功！\n");
}

/* ---- 销毁图: 释放所有边节点, 重置计数 ---- */
void DestroyGraph(ALGraph *G)
{
    int i;
    ArcNode *p, *q;
    for (i = 0; i < G->vertex_count; i++) {
        p = G->vertices[i].firstarc;
        while (p != NULL) {
            q = p->nextarc;
            free(p);
            p = q;
        }
        G->vertices[i].firstarc = NULL;
    }
    G->vertex_count = 0;
    G->edge_count = 0;
}

/* =====================================================================
   模块 B: 图展示 (邻接矩阵)
   ===================================================================== */

void ShowGraph(ALGraph *G)
{
    int i, j;
    int mat[MAX_VERTEX][MAX_VERTEX];
    ArcNode *p;

    /* 1. 初始化: 对角线 0, 其余 INF */
    for (i = 0; i < G->vertex_count; i++) {
        for (j = 0; j < G->vertex_count; j++)
            mat[i][j] = INF;
        mat[i][i] = 0;
    }

    /* 2. 遍历邻接表填矩阵 */
    for (i = 0; i < G->vertex_count; i++) {
        p = G->vertices[i].firstarc;
        while (p != NULL) {
            mat[i][p->adjvex] = p->weight;
            p = p->nextarc;
        }
    }

    /* 3. 打印 */
    printf("\n景区景点分布图（邻接矩阵）：\n");
    printf(" ");
    for (j = 0; j < G->vertex_count; j++)
        printf(" %s", G->vertices[j].data);
    printf("\n");
    for (i = 0; i < G->vertex_count; i++) {
        printf("%s", G->vertices[i].data);
        for (j = 0; j < G->vertex_count; j++)
            printf(" %d", mat[i][j]);
        printf("\n");
    }
}

/* =====================================================================
   模块 C: DFS 遍历与导游线路
   遍历顺序: 每个顶点的邻居按"邻接顶点下标降序"处理
   早停机制: visited_count == vertex_count 时立即停止
   ===================================================================== */

static int visited[MAX_VERTEX];
static int in_stack[MAX_VERTEX];
static int parent[MAX_VERTEX];
static int route[MAX_VERTEX * 2];
static int route_len;
static int visited_count;
static int dfs_done;
static int cycle_found[MAX_VERTEX];
static int cycle_len;
static int has_cycle;

/* ---- 获取 u 的所有邻居并按下标降序排序 ---- */
static void GetSortedNeighbors(ALGraph *G, int u, int nb[], int wt[], int *cnt)
{
    ArcNode *p = G->vertices[u].firstarc;
    int i, j, tmp;
    *cnt = 0;
    while (p != NULL) {
        nb[*cnt] = p->adjvex;
        wt[*cnt] = p->weight;
        (*cnt)++;
        p = p->nextarc;
    }
    /* 选择排序: 按邻居下标降序 */
    for (i = 0; i < *cnt - 1; i++) {
        for (j = i + 1; j < *cnt; j++) {
            if (nb[j] > nb[i]) {
                tmp = nb[i]; nb[i] = nb[j]; nb[j] = tmp;
                tmp = wt[i]; wt[i] = wt[j]; wt[j] = tmp;
            }
        }
    }
}

/* ---- DFS 核心递归 ---- */
static void DFS(ALGraph *G, int u)
{
    int nb[MAX_VERTEX], wt[MAX_VERTEX], cnt, i;

    if (dfs_done) return;

    /* 1. 加入导游线路 */
    route[route_len++] = u;

    /* 2. 首次访问则计数 */
    if (!visited[u]) {
        visited[u] = 1;
        visited_count++;
        in_stack[u] = 1;
    } else {
        in_stack[u] = 1;
    }

    /* 3. 早停: 所有顶点都访问过一次 */
    if (visited_count == G->vertex_count) {
        dfs_done = 1;
        in_stack[u] = 0;
        return;
    }

    /* 4. 获取排序后的邻居 */
    GetSortedNeighbors(G, u, nb, wt, &cnt);

    /* 5. 逐个处理邻居 */
    for (i = 0; i < cnt; i++) {
        int v = nb[i];
        if (dfs_done) return;
        if (v == parent[u]) continue;   /* 不重复访问父节点 */

        if (!visited[v]) {
            parent[v] = u;
            DFS(G, v);
            if (dfs_done) return;
            /* 递归返回后: 若之后还有未访问的邻居, 则回溯记录当前节点 */
            {
                int j, has_more = 0;
                for (j = i + 1; j < cnt; j++) {
                    if (nb[j] != parent[u] && !visited[nb[j]]) { has_more = 1; break; }
                }
                if (has_more) route[route_len++] = u;
            }
        }
        else if (in_stack[v]) {
            /* 回边到祖先 → 存在回路. 记录到导游线路并记录回路 */
            route[route_len++] = v;

            if (!has_cycle) {
                int k;
                int cur = u;
                cycle_len = 0;
                while (cur != v && cur != -1) {
                    cycle_found[cycle_len++] = cur;
                    cur = parent[cur];
                }
                cycle_found[cycle_len++] = v;
                /* 反转: 使路径从 v 开始到 u */
                for (k = 0; k < cycle_len / 2; k++) {
                    int tmp = cycle_found[k];
                    cycle_found[k] = cycle_found[cycle_len - 1 - k];
                    cycle_found[cycle_len - 1 - k] = tmp;
                }
                cycle_found[cycle_len++] = v;  /* 闭合 */
                has_cycle = 1;
            }

            if (visited_count == G->vertex_count) {
                dfs_done = 1;
                in_stack[u] = 0;
                return;
            }
        }
    }

    in_stack[u] = 0;
}

/* ---- 对外接口: 执行 DFS, 输出导游线路 ---- */
void DFSTraverse(ALGraph *G, int start, int route_out[], int *route_len_out)
{
    int i;
    for (i = 0; i < G->vertex_count; i++) {
        visited[i] = 0;
        in_stack[i] = 0;
        parent[i] = -1;
    }
    route_len = 0;
    visited_count = 0;
    dfs_done = 0;
    has_cycle = 0;
    cycle_len = 0;

    if (start < 0 || start >= G->vertex_count) start = 0;
    DFS(G, start);

    for (i = 0; i < route_len; i++) route_out[i] = route[i];
    *route_len_out = route_len;
}

/* ---- 打印导游线路 ---- */
void ShowTourRoute(ALGraph *G)
{
    int i, r[MAX_VERTEX * 2], rlen;
    DFSTraverse(G, 0, r, &rlen);
    printf("\n导游路线为：\n");
    for (i = 0; i < rlen; i++) {
        if (i > 0) printf("-");
        printf("%s", G->vertices[r[i]].data);
    }
    printf("-\n");
}

/* =====================================================================
   模块 D: 回路检测 (利用 DFS 时记录的 cycle_found)
   ===================================================================== */

int DetectCycle(ALGraph *G)
{
    int i, r[MAX_VERTEX * 2], rlen;
    DFSTraverse(G, 0, r, &rlen);

    if (has_cycle) {
        printf("\n图中有回路，回路为：");
        for (i = 0; i < cycle_len; i++) {
            if (i > 0) printf("-");
            printf("%s", G->vertices[cycle_found[i]].data);
        }
        printf("\n");
        return 1;
    }
    printf("\n图中不存在回路\n");
    return 0;
}
