#define _CRT_SECURE_NO_WARNINGS 1
#include "travels.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int visited[MAXNUM];
extern double **parray;
char path[MAXNUM][10];
int path_index = 0;
CNode *p;
typedef char str[10];

int locate(ALGraph graph, char *e)
{
    for (int i = 0; i < graph.nodenum; i++)
    {
        if (strcmp(e, graph.roadlist[i].data) == 0)
        {
            return i;
        }
    }
    return 0;
}

void DFS(ALGraph graph, int i)
{
    visited[i] = 1;
    strcpy(path[path_index++], graph.roadlist[i].data);
    for (p = graph.roadlist[i].first; p != NULL; p = p->next)
    {
        if (!visited[p->index])
        {
            DFS(graph, p->index);
            p = graph.roadlist[i].first;
        }
    }
}

void DFSTraverse(ALGraph graph)
{
    int i = 0;
    path_index = 0;
    for (i = 0; i < graph.nodenum; i++)
        visited[i] = 0;
    for (i = 0; i < graph.nodenum; i++)
        if (!visited[i])
            DFS(graph, i);
}

int isEdge(ALGraph graph, char *e1, char *e2)
{
    int i = locate(graph, e1);
    int j = locate(graph, e2);
    if (parray[i][j] < INF)
    {
        return 1;
    }
    return 0;
}

int loopTest(ALGraph graph)
{
    str *pnodes = (str *)malloc(sizeof(str) * graph.nodenum);
    int *indegree = (int *)malloc(sizeof(int) * graph.nodenum);
    int *degarr = (int *)malloc(sizeof(int) * graph.nodenum);

    int count = 0;

    CNode *pnode = (CNode *)malloc(sizeof(CNode));

    int i = 0;
    int j = 0;
    int top = 0;
    int base = 0;

    for (i = 0; i < graph.nodenum; i++)
        indegree[i] = 0;
    for (i = 0; i < graph.nodenum; i++)
    {
        for (pnode = graph.roadlist[i].first; pnode != NULL; pnode = pnode->next)
        {
            indegree[pnode->index]++;
        }
    }

    for (i = 0; i < graph.nodenum; i++)
        if (!indegree[i])
            degarr[top++] = i;

    while (top != base)
    {
        j = degarr[--top];
        strcpy(pnodes[count++], graph.roadlist[j].data);
        for (pnode = graph.roadlist[j].first; pnode != NULL; pnode = pnode->next)
        {
            if (!(--indegree[pnode->index]))
            {
                degarr[top++] = pnode->index;
            }
        }

    }
    if (count < graph.nodenum)
    {
        printf("图中有回路,回路是：\n");
        for (i = 0; i < graph.nodenum; i++)
        {
            for (j = 0; j < count; j++)
            {
                if (strcmp(graph.roadlist[i].data, pnodes[j]) == 0)
                {
                    break;
                }
            }
            if (j >= count)
                printf("%s ", graph.roadlist[i].data);
        }
        printf("\n");
        free(pnodes);
        free(indegree);
        free(degarr);
        free(pnode);
        return 0;
    }
    else
    {
        printf("图中没有回路。\n");
        free(pnodes);
        free(indegree);
        free(degarr);
        free(pnode);
        return 1;
    }

}

void shortPath(ALGraph graph, int path[][MAXNUM], double shortpath[][MAXNUM])
{
    int i = 0;
    int j = 0;
    int k = 0;
    for (i = 0; i < graph.nodenum; i++)
    {
        for (j = 0; j < graph.nodenum; j++)
        {
            shortpath[i][j] = parray[i][j];
            if (parray[i][j] < INF)
            {
                path[i][j] = i;
            }
        }
    }
    for (k = 0; k < graph.nodenum; k++)
    {
        for (i = 0; i < graph.nodenum; i++)
        {
            for (j = 0; j < graph.nodenum; j++)
            {
                if (shortpath[i][k] + shortpath[k][j] < shortpath[i][j])
                {
                    shortpath[i][j] = shortpath[i][k] + shortpath[k][j];
                    path[i][j] = k;
                }
            }
        }
    }
}

void printShortPath(ALGraph graph, int path[][MAXNUM], double shortpath[][MAXNUM], int i, int j)
{
    if (path[i][j] == i)
    {
        printf("%s--%s\n", graph.roadlist[i].data, graph.roadlist[j].data);
    }
    else {
        printShortPath(graph, path, shortpath, i, path[i][j]);
        printShortPath(graph, path, shortpath, path[i][j], j);
    }
}

void minDistance(ALGraph graph, int path[][MAXNUM], double shortpath[][MAXNUM])
{
    char start[10];
    char end[10];
    int i = 0;
    int j = 0;
    shortPath(graph, path, shortpath);
    printf("请依次输入要查询两处景点的名称:");
    scanf("%s", start);
    scanf("%s", end);
    i = locate(graph, start);
    j = locate(graph, end);
    printf("最短路径是:\n");
    printShortPath(graph, path, shortpath, i, j);
    printf("最短距离是:%.0f\n", shortpath[i][j]);
}
