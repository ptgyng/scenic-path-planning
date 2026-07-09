/* main.c — 景区路径规划系统 - 交互式菜单版
 * 支持: 创建图/显示图/导游/寻路 - 完整控制台菜单
 */
#include <stdio.h>
#include "graph.h"

int main(void)
{
    ALGraph G = {0};
    printf("==== 景区路径规划系统 ====\n");
    printf("正在创建景区图 (8顶点 9边)...\n");
    CreateGraph(&G);
    printf("图创建完成, 顶点数: %d, 边数: %d\n", G.vertex_count, G.edge_count);
    ShowGraph(&G);
    ShowTourRoute(&G);
    DetectCycle(&G);
    printf("==== Dijkstra 最短路径: 大门 → 观月阁 ====\n");
    ShowShortestPath(&G);
    DestroyGraph(&G);
    return 0;
}
