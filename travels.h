#ifndef TRAVELS_H
#define TRAVELS_H

#include "global.h"

int locate(ALGraph graph, char *e);
void DFSTraverse(ALGraph graph);
void DFS(ALGraph graph, int i);

int isEdge(ALGraph graph, char *e1, char *e2);

void shortPath(ALGraph graph, int path[][MAXNUM], double shortpath[][MAXNUM]);
void printShortPath(ALGraph graph, int path[][MAXNUM], double shortpath[][MAXNUM], int i, int j);

int loopTest(ALGraph graph);
void minDistance(ALGraph graph, int path[][MAXNUM], double shortpath[][MAXNUM]);

#endif
