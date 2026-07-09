/* =====================================================================
   mst.c — 最小生成树 (Minimum Spanning Tree)
   - Prim 算法: 从某顶点出发, 贪心选"连接已选集合和未选集合的最小权边"
   - Kruskal 算法: 对所有边按权排序, 贪心选"不构成回路"的最小权边
   - 并查集 (Union-Find): 辅助 Kruskal 高效判断两个顶点是否已连通
   ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

/* =====================================================================
   并查集 (Disjoint Set Union / Union-Find)
   - parent[x]: x 的父节点, 若 x 是根则 parent[x] == x
   - rank[x]:  以 x 为根的树"高度/秩"  (用于按秩合并, 保持树矮)
   操作:
     - find(x): 找 x 的根, 路径压缩 (让 x 直接指向根, 下次查找 O(1))
     - union_sets(x,y): 合并 x 与 y 所在集合, 按秩合并
   时间复杂度: 近似 O(alpha(n)) ≈ 几乎常数
   ===================================================================== */

static int uf_parent[MAX_VERTEX];
static int uf_rank[MAX_VERTEX];

static void UF_Init(int n)
{
    int i;
    for (i = 0; i < n; i++) {
        uf_parent[i] = i;     /* 每个节点初始是自己的根 */
        uf_rank[i] = 0;
    }
}

static int UF_Find(int x)
{
    /* 路径压缩: 递归使 x 直接指向根 */
    if (uf_parent[x] != x)
        uf_parent[x] = UF_Find(uf_parent[x]);
    return uf_parent[x];
}

static int UF_Union(int x, int y)
{
    int xr = UF_Find(x);
    int yr = UF_Find(y);
    if (xr == yr) return 0;    /* 已在同一集合, 无需合并 */
    /* 按秩合并: 矮树挂到高树下面, 保持整体尽可能矮 */
    if (uf_rank[xr] < uf_rank[yr]) {
        uf_parent[xr] = yr;
    } else if (uf_rank[xr] > uf_rank[yr]) {
        uf_parent[yr] = xr;
    } else {
        uf_parent[yr] = xr;
        uf_rank[xr]++;
    }
    return 1;
}

/* =====================================================================
   Prim 算法
   思想: 从顶点 0 开始, 每一步选"一端在已选集合 S, 一端在未选集合 V-S"
         的最小权边, 把它加入 MST. 共选 n-1 条边.
   数据结构:
     - lowcost[v]: 从 v 到"已选集合 S" 的当前最小边权
     - adjvex[v]:  上面那条最小边在 S 中的端点
   复杂度: O(n^2) (邻接矩阵实现, 适合稠密图)
   ===================================================================== */

void Prim(ALGraph *G)
{
    int n = G->vertex_count;
    int lowcost[MAX_VERTEX];   /* 各未选点到已选集合的最小权 */
    int adjvex[MAX_VERTEX];    /* 对应在已选集合中的端点 */
    int i, j, k, min, total;
    int mat[MAX_VERTEX][MAX_VERTEX];
    ArcNode *p;

    if (n < 2) { printf("  顶点数太少, 无法生成 MST\n"); return; }

    /* 1. 建邻接矩阵 */
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) mat[i][j] = INF;
        mat[i][i] = 0;
        p = G->vertices[i].firstarc;
        while (p != NULL) {
            mat[i][p->adjvex] = p->weight;
            p = p->nextarc;
        }
    }

    /* 2. 初始化: 从顶点 0 开始 */
    for (i = 1; i < n; i++) {
        lowcost[i] = mat[0][i];
        adjvex[i] = 0;
    }
    lowcost[0] = 0;   /* 0 已加入 S */

    printf("\nPrim 最小生成树:\n");
    total = 0;

    /* 3. 选 n-1 条边 (加入 n-1 个新顶点到 S) */
    for (i = 1; i < n; i++) {

        /* 3a. 在"未选集合"中找 lowcost 最小的顶点 k */
        min = INF;
        k = -1;
        for (j = 1; j < n; j++) {
            if (lowcost[j] != 0 && lowcost[j] < min) {
                min = lowcost[j];
                k = j;
            }
        }
        if (k == -1) {
            printf("  图不连通, 无法生成 MST!\n");
            return;
        }

        /* 3b. 输出这条边并累加总权 */
        printf("  边: %s -- %s, 权值: %d\n",
               G->vertices[adjvex[k]].data,
               G->vertices[k].data, min);
        total += min;

        /* 3c. k 加入已选集合, 更新 lowcost / adjvex */
        lowcost[k] = 0;     /* 0 表示"已在 S 内" */
        for (j = 1; j < n; j++) {
            if (lowcost[j] != 0 && mat[k][j] < lowcost[j]) {
                lowcost[j] = mat[k][j];
                adjvex[j] = k;
            }
        }
    }

    printf("  最小生成树总权值: %d\n", total);
}

/* =====================================================================
   Kruskal 算法
   思想: 对所有边按权从小到大排序, 依次检查每条边 (u,v):
         若 u 和 v 尚未连通 (并查集 find(u) != find(v)),
         则选此边加入 MST, 并执行 union(u,v).
   复杂度: O(E log E)  (主要来自排序, 适合稀疏图)
   ===================================================================== */

/* --- 边结构体: 两个端点 + 权 --- */
typedef struct {
    int u, v, w;
} Edge;

static int cmp_edge(const void *a, const void *b)
{
    return ((Edge *)a)->w - ((Edge *)b)->w;
}

void Kruskal(ALGraph *G)
{
    int n = G->vertex_count;
    int i, e_total, mst_edges, total;
    Edge edges[MAX_VERTEX * MAX_VERTEX];
    ArcNode *p;

    if (n < 2) { printf("  顶点数太少, 无法生成 MST\n"); return; }

    /* 1. 收集所有边 (无向图中每条边会出现两次: u→v 和 v→u)
          为避免重复, 只记录 u < v 的那一条 */
    e_total = 0;
    for (i = 0; i < n; i++) {
        p = G->vertices[i].firstarc;
        while (p != NULL) {
            if (i < p->adjvex) {   /* 只保留一份 */
                edges[e_total].u = i;
                edges[e_total].v = p->adjvex;
                edges[e_total].w = p->weight;
                e_total++;
            }
            p = p->nextarc;
        }
    }

    /* 2. 排序: 按权升序 */
    qsort(edges, (size_t)e_total, sizeof(Edge), cmp_edge);

    /* 3. 初始化并查集 */
    UF_Init(n);

    /* 4. 依次选边, 直到 n-1 条 (或者边用完) */
    printf("\nKruskal 最小生成树:\n");
    total = 0;
    mst_edges = 0;

    for (i = 0; i < e_total && mst_edges < n - 1; i++) {
        int u = edges[i].u, v = edges[i].v, w = edges[i].w;
        if (UF_Union(u, v)) {   /* 不构成回路, 加入 MST */
            printf("  边: %s -- %s, 权值: %d\n",
                   G->vertices[u].data, G->vertices[v].data, w);
            total += w;
            mst_edges++;
        }
    }

    if (mst_edges < n - 1) {
        printf("  图不连通, 无法生成完整 MST!\n");
    } else {
        printf("  最小生成树总权值: %d\n", total);
    }
}
