/* =====================================================================
   file_io.c — 景区景点图的文件读写
   文件格式:
     第 1 行: n_vertexes n_edges     (顶点数 和 边数)
     第 2 行: vertex1 vertex2 ... vertexN   (N 个顶点名, 空格分隔)
     第 3..2+N_edges 行: vertexA vertexB weight   (每条无向边)
   示例:
     8 9
     大门 碧玉潭 红叶林 赏花园 飞云石 九曲桥 白虎岩 观月阁
     大门 碧玉潭 9
     大门 红叶林 8
     ...
   ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* ---- 把当前图保存到文本文件 ---- */
int SaveGraph(ALGraph *G, const char *filename)
{
    FILE *fp;
    int i;
    int edges_saved;
    ArcNode *p;

    if (G->vertex_count == 0) {
        printf("  图为空, 无法保存!\n");
        return 0;
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("  无法打开文件 [%s] 写入!\n", filename);
        return 0;
    }

    /* 1. 写入顶点数 和 边数 */
    fprintf(fp, "%d %d\n", G->vertex_count, G->edge_count);

    /* 2. 写入所有顶点名 */
    for (i = 0; i < G->vertex_count; i++) {
        if (i > 0) fprintf(fp, " ");
        fprintf(fp, "%s", G->vertices[i].data);
    }
    fprintf(fp, "\n");

    /* 3. 写入所有边 (无向图每条边出现两次, 只保存 i < p->adjvex 那份) */
    edges_saved = 0;
    for (i = 0; i < G->vertex_count; i++) {
        p = G->vertices[i].firstarc;
        while (p != NULL) {
            if (i < p->adjvex) {
                fprintf(fp, "%s %s %d\n",
                        G->vertices[i].data,
                        G->vertices[p->adjvex].data,
                        p->weight);
                edges_saved++;
            }
            p = p->nextarc;
        }
    }

    fclose(fp);
    printf("  已保存 %d 个顶点, %d 条边 到 [%s]\n",
           G->vertex_count, edges_saved, filename);
    return 1;
}

/* ---- 从文本文件读取景点图 ---- */
int LoadGraph(ALGraph *G, const char *filename)
{
    FILE *fp;
    int i, n_edges;
    char v1[MAX_NAME], v2[MAX_NAME];
    int w;

    /* 1. 先清空旧图 (如果有) */
    DestroyGraph(G);

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("  无法打开文件 [%s] 读取!\n", filename);
        return 0;
    }

    /* 2. 读取 顶点数 和 边数 */
    if (fscanf(fp, "%d %d", &G->vertex_count, &n_edges) != 2) {
        printf("  文件格式错误: 第 1 行必须是 '顶点数 边数'\n");
        fclose(fp);
        return 0;
    }
    if (G->vertex_count <= 0 || G->vertex_count > MAX_VERTEX) {
        printf("  顶点数 %d 超出有效范围 (1 ~ %d)!\n", G->vertex_count, MAX_VERTEX);
        fclose(fp);
        G->vertex_count = 0;
        return 0;
    }

    /* 3. 读取 N 个顶点名, 同时初始化邻接表头 */
    for (i = 0; i < G->vertex_count; i++) {
        if (fscanf(fp, "%s", G->vertices[i].data) != 1) {
            printf("  文件格式错误: 第 2 行顶点名不足!\n");
            fclose(fp);
            G->vertex_count = 0;
            return 0;
        }
        G->vertices[i].firstarc = NULL;
    }

    /* 4. 读取 n_edges 条边 */
    G->edge_count = 0;
    for (i = 0; i < n_edges; i++) {
        if (fscanf(fp, "%s %s %d", v1, v2, &w) != 3) {
            printf("  警告: 第 %d 条边读取失败, 跳过\n", i + 1);
            continue;
        }
        if (w <= 0) {
            printf("  警告: 边 [%s-%s] 的权值 %d 非法, 跳过\n", v1, v2, w);
            continue;
        }
        AddEdge(G, v1, v2, w);
    }

    fclose(fp);
    printf("  已从 [%s] 加载 %d 个顶点, %d 条边\n",
           filename, G->vertex_count, G->edge_count);
    return 1;
}
