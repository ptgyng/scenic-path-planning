#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXNUM 40
#define INF 32767

typedef struct stCNode {
    int index;
    struct stCNode *next;
    int length;
} CNode;

typedef struct stHNode {
    char data[10];
    CNode *first;
} HNode, RoadList[MAXNUM];

typedef struct stGraph {
    RoadList roadlist;
    int nodenum;
    int edgenum;
} ALGraph;

#endif
