#define _CRT_SECURE_NO_WARNINGS 1

#include "main.h"

int main()
{
    ALGraph graph;
    ALGraph guideGraph;
    double shortPath[MAXNUM][MAXNUM];
    int path[MAXNUM][MAXNUM];
    int opt = 0;
    int flag = 1;

    showMenu();
    while (1)
    {
        printf("请输入您的选择:\n");
        scanf("%d", &opt);
        while (flag && opt != 0 && opt != 1)
        {
            printf("输入错误，请重新选择：\n");
            scanf("%d", &opt);
        }
        flag = 0;
        while (opt < 0 || opt > 5)
        {
            printf("输入错误，请重新选择：\n");
            scanf("%d", &opt);
        }
        switch (opt)
        {
        case 0:
            return 0;
        case 1:
            createGraph(&graph);
            break;
        case 2:
            printGraph(&graph);
            break;
        case 3:
            createGuideGraph(&graph, &guideGraph);
            break;
        case 4:
            loopTest(guideGraph);
            break;
        case 5:
            minDistance(graph, path, shortPath);
            break;
        default:
            break;
        }
    }

    return 0;
}
