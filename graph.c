#define _CRT_SECURE_NO_WARNINGS 1
#include "graph.h"

double **parray;
int visited[MAXNUM];
char *guidePath[2 * MAXNUM];
extern char path[MAXNUM][10];

void createGraph(ALGraph *graph)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    char e1[10] = "";
    char e2[10] = "";
    CNode *node1;
    CNode *node2;

    printf("请输入顶点数和边数：\n");
    scanf("%d", &graph->nodenum);
    scanf("%d", &graph->edgenum);

    printf("\n      请输入各顶点的信息:\n");
    printf("请输入各顶点的名字：\n");
    for (i = 0; i < graph->nodenum; i++)
    {
        scanf("%s", graph->roadlist[i].data);
        graph->roadlist[i].first = NULL;
    }

    for (k = 0; k < graph->edgenum; k++)
    {
        printf("请输入第%d条边的两个顶点和边的权值：", k + 1);
        scanf("%s", e1);
        scanf("%s", e2);
        scanf("%d", &l);

        i = locate(*graph, e1);
        j = locate(*graph, e2);

        node1 = (CNode *)malloc(sizeof(CNode));
        node2 = (CNode *)malloc(sizeof(CNode));

        node1->index = j;
        node1->length = l;
        node1->next = graph->roadlist[i].first;
        graph->roadlist[i].first = node1;

        node2->index = i;
        node2->length = l;
        node2->next = graph->roadlist[j].first;
        graph->roadlist[j].first = node2;
    }
}

void printGraph(ALGraph *graph)
{
    int i = 0;
    int j = 0;
    CNode *P = (CNode *)malloc(sizeof(CNode));

    parray = (double **)malloc(sizeof(double *) * graph->nodenum);
    for (i = 0; i < graph->nodenum; i++)
    {
        parray[i] = (double *)malloc(sizeof(double) * graph->nodenum);
    }

    for (i = 0; i < graph->nodenum; i++)
    {
        for (j = 0; j < graph->nodenum; j++)
        {
            if (i == j)
                parray[i][j] = 0;
            else
                parray[i][j] = INF;
        }
    }

    for (i = 0; i < graph->nodenum; i++)
    {
        for (P = graph->roadlist[i].first; P != NULL; P = P->next)
        {
            j = P->index;
            parray[i][j] = P->length;
        }
    }

    for (i = 0; i < graph->nodenum; i++)
    {
        printf("\t%s", graph->roadlist[i].data);
    }
    printf("\n");

    for (i = 0; i < graph->nodenum; i++)
    {
        printf("%s\t", graph->roadlist[i].data);
        for (j = 0; j < graph->nodenum; j++)
        {
            printf("%.0f\t", parray[i][j]);
        }
        printf("\n");
    }

    free(P);
}

void createGuideGraph(ALGraph *graph, ALGraph *guideGraph)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int n = 0;
    int flag = 0;
    int edgenum = 0;
    int m = 0;
    CNode *node1;
    CNode *node2;

    DFSTraverse(*graph);

    for (i = 0; i < graph->nodenum - 1; i++)
    {
        k = 0;
        flag = 1;
        while (flag)
        {
            guidePath[n++] = path[i + k];
            if (isEdge(*graph, path[i + k], path[i + 1]))
                flag = 0;
            else
                k--;
        }
    }
    guidePath[n] = path[i];

    printf("导游路线为：\n");
    for (i = 0; i <= n; i++)
        printf("%s-", guidePath[i]);
    printf("\n");

    for (i = 0; i <= n; i++)
        visited[i] = 0;

    for (i = 0; i < graph->nodenum; i++)
    {
        strcpy(guideGraph->roadlist[i].data, graph->roadlist[i].data);
        guideGraph->roadlist[i].first = NULL;
    }

    for (k = 0; k < n; k++)
    {
        i = locate(*graph, guidePath[k]);
        j = locate(*graph, guidePath[k + 1]);

        if (visited[j])
        {
            m = k + 2;
            if (m <= n)
            {
                while (visited[locate(*graph, guidePath[m])])
                {
                    if (isEdge(*graph, guidePath[k], guidePath[m]))
                    {
                        j = locate(*graph, guidePath[m]);
                        node1 = (CNode *)malloc(sizeof(CNode));
                        node2 = (CNode *)malloc(sizeof(CNode));

                        node1->index = j;
                        node1->length = parray[i][j];

                        node2 = guideGraph->roadlist[i].first;
                        guideGraph->roadlist[i].first = node1;
                        node1->next = node2;

                        edgenum++;
                    }
                    if (++m >= n)
                        break;
                }
            }
        }
        else
        {
            visited[i] = 1;
            visited[j] = 1;

            node1 = (CNode *)malloc(sizeof(CNode));
            node2 = (CNode *)malloc(sizeof(CNode));

            node1->index = j;
            node1->length = parray[i][j];

            node2 = guideGraph->roadlist[i].first;
            guideGraph->roadlist[i].first = node1;
            node1->next = node2;

            edgenum++;
        }
    }

    guideGraph->nodenum = graph->nodenum;
    guideGraph->edgenum = edgenum;
}
