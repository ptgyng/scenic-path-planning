/* main.c — DFS 导游线路测试驱动 (feature/dfs 分支开发)
 * 功能: 创建图 → 输出导游线路 → 检测回路
 */
#include <stdio.h>
#include "graph.h"

int main(void)
{
    ALGraph G = {0};
    printf("==== DFS 导游线路测试 ====\n");
    printf("正在创建景区图 (8顶点 9边)...\n");
    CreateGraph(&G);
    printf("图创建完成, 顶点数: %d, 边数: %d\n", G.vertex_count, G.edge_count);
    ShowTourRoute(&G);
    DetectCycle(&G);
    DestroyGraph(&G);
    return 0;
}
