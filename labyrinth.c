#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct edge EDGE;

typedef enum {
	FALSE,
	TRUE
} BOOL;

typedef struct Chamber {
	int v;	//Index - defines the chamber's position inside the labyrinth
	int n;	//Number of Portals
	int o;	//Logic Value - if a Portal for the outside world exists
} Chamber;

typedef struct Segment {
	int b;	//First point of Segment
	int e;	//Second point of Segment
	float d;	//Distance between the two points of Segment
} Segment;

typedef struct Point {
	float x;
	float y;
} Point;

//Graph========================================================================
typedef struct Edge {
	float dist;
	BOOL isPortal;
} Edge;

//Point Functions==============================================================
void printPoints(Point *points, int np) {
	int i = 0;

	for(i=0; i<np; i++) {
		printf("Point[%d] = (x=%.2f, y=%.2f)\n", i, points[i].x, points[i].y);
	}
}
//Chamber Functions============================================================
int getChamberIndexById(Chamber *chambers, int nc, int startIdx) {
	int i = 0;
	for(i=0; i<nc; i++) {
		if(chambers[i].v == startIdx) return i;
	}
	return -1;
}

void printChambers(Chamber *chambers, int nc) {
	int i = 0;

	printf("Printing Chambers:: nc[%d]\n", nc);
	for(i=0; i<nc; i++) {
		printf("Chamber[%d] = (v=%d, n=%d, o=%d)\n", i, chambers[i].v, chambers[i].n, chambers[i].o);
	}
}

//Segment Functions============================================================
void calculateDistance(Segment *segs, int idx, Point *points) {
	printf("b = [%d] - Points[%d] = (%.2f, %.2f)\n", segs[idx].b, abs(segs[idx].b)-1, points[abs(segs[idx].b)-1].x, points[abs(segs[idx].b)-1].y);
	printf("e = [%d] - Points[%d] = (%.2f, %.2f)\n", segs[idx].e, abs(segs[idx].e)-1, points[abs(segs[idx].e)-1].x, points[abs(segs[idx].e)-1].y);
	segs[idx].d = sqrt(pow(points[abs(segs[idx].b)-1].x - points[abs(segs[idx].e)-1].x, 2) 
		+ pow(points[abs(segs[idx].b)-1].y - points[abs(segs[idx].e)-1].y, 2));
	if(segs[idx].d == 0) exit(0);

	printf("dist = %.2f\n", segs[idx].d);
	printf("------------------\n");
}

int getSegmentIndexByIdB(Segment *segs, int ns, int startIdx, int id) {
	int i = 0;

	for(i=startIdx; i<ns; i++) {
		if(segs[i].b < 0 || segs[i].e < 0) continue;
		if(abs(segs[i].b) == id) return i;
	}

	return -1;
}

int getSegmentIndexByIdE(Segment *segs, int ns, int startIdx, int id) {
	int i = 0;

	for(i=startIdx; i<ns; i++) {
		if(segs[i].b < 0 || segs[i].e < 0) continue;
		if(segs[i].e == id) return i;
	}

	return -1;
}

void printSegments(Segment *segs, int ns) {
	int i = 0;

	printf("Printing Segments:: ns[%d]\n", ns);
	for(i=0; i<ns; i++) {
		printf("Segment[%d] = (b=%d, e=%d, dist=%.2f)\n", i, segs[i].b, segs[i].e, segs[i].d);
	}
}

//Graph Functions==============================================================
void printGraph(Edge **graph, int ng) {
	int i = 0, j = 0;

	for(i=0; i<ng; i++) {
		printf("---- Linha %d:\n", i);
		for(j=0; j<ng; j++) {
			if(graph[i][j].dist >= 0) {
				printf("node[%d]=(%.2f, %d) ", j, graph[i][j].dist, graph[i][j].isPortal);
			}
		}
		printf("\n");	
	}

}

Edge **createGraph(int np) {
	int i = 0, j = 0;
	Edge **graph = NULL;

	graph = (Edge**)calloc(np, sizeof(Edge*));
	for(i=0; i<np; i++) {
		graph[i] = (Edge*)calloc(np, sizeof(Edge));
		for(j=0; j<np; j++) {
			graph[i][j].dist = -1;
		}
	}

	return graph;
}

Edge **copyGraph(Edge **graph, int ng) {
	Edge **aux = NULL;
	int i = 0, j = 0;
	aux = createGraph(ng);

	for(i=0; i<ng; i++)
	{
		for(j=0; j<ng; j++)
		{
			aux[i][j].dist = graph[i][j].dist;
			aux[i][j].isPortal = graph[i][j].isPortal;
		}
	}

	return aux;
}

void freeGraph(Edge **graph, int ng) {
	int i = 0;

	for(i=0; i<ng; i++)
	{
		free(graph[i]);
	}
	free(graph);
}

Edge **populateGraph(int np, Chamber *chambers, int nc, Segment *segs, int ns, int startIdx) {
	Edge **graph = createGraph(np);
	int i = 0, idxS = 0, idxC = 0;

	for(i=0; i<np; i++) {
		idxS = -1;
		if((idxC = getChamberIndexById(chambers, nc, i + 1)) != -1) {
				graph[i][i].dist = 0;
				graph[i][i].isPortal = chambers[idxC].o;
		}

		//Look for b side
		while((idxS = getSegmentIndexByIdB(segs, ns, idxS+1, i + 1)) != -1) {
			graph[i][segs[idxS].e-1].dist = segs[idxS].d;

			if((idxC = getChamberIndexById(chambers, nc, segs[idxS].e)) != -1) {
				graph[i][segs[idxS].e-1].isPortal = chambers[idxC].o;
			}
		}

		//Look for e side
		while((idxS = getSegmentIndexByIdE(segs, ns, idxS+1, i + 1)) != -1) {
			graph[i][segs[idxS].b-1].dist = segs[idxS].d;

			if((idxC = getChamberIndexById(chambers, nc, segs[idxS].b)) != -1) {
				graph[i][segs[idxS].b-1].isPortal = chambers[idxC].o;
			}
		}
	}

	return graph;
}

void clearVector(int *vector, int startIdx, int n) {
	for(int i=startIdx; i<n; i++)	{
		vector[i] = -1;
	}
}

void clearBoolVector(BOOL *vector, int startIdx, int n) {
	for(int i=startIdx; i<n; i++)	{
		vector[i] = FALSE;
	}
}

BOOL isAllVisited(BOOL *visited, int n) {
	for(int i=0; i<n; i++)	{
		if(visited[i] == FALSE) return FALSE;
	}

	return TRUE;
}

BOOL hasChildren(Edge **graph, int idx, int np) {
	for(j=0; j<np; j++)	{
		if(graph[idx][j].dist >= 0) retur TRUE;
	}
	return FALSE;
}

void fillParent(int *parent, int n) {
	for(int i=0; i<n; i++)	{
		parent[i] = i;
	}
}

void printWay(int *vector, float sum) {
	int i = 0;
	int len = strlen(vector);

	while(vector[i] != -1) {
		printf("%d ", vector[i]);
		i++;
	}
	printf("%.2f\n", sum);
}

int getNextChildren(Edge **graph, int np, int idx) {
	int i = 0;

	if(idx >= np) return -1;

	for(i=idx+1; i<np; i++) {
		if(graph[idx][i].dist >= 0) {
			return i;
		}
	}

	return -1;
}

void getPossibleWays(Edge **graph, int np, int startIdx) {
	int *vector = NULL;
	BOOL *visited = NULL;
	int *parent = NULL;
	float sum = 0;
	int aux = startIdx;
	int vectorIdx = 0;

	vector = (int*)calloc(sizeof(int)*np);
	clearVector(vector, 0, np);
	visited = (BOOL*)calloc(np, sizeof(BOOL));
	clearBoolVector(visited, 0, np);
	parent = (int*)malloc(sizeof(int) * np);
	fillParent(parent, np);

	while(isAllVisited(visited, np) == FALSE) {
		if(graph[aux][aux].isPortal == TRUE) {
			vector[vectorIdx] = aux;
			printWay(vector, sum);
		}

		if(hasChildren(graph, aux, np) == TRUE) {
			aux = getNextChildren(graph, np, aux);
		}

		while(hasChildren(graph, aux, np) != FALSE || graph[aux][aux].isPortal != TRUE) {

		}
	}

}

//MAIN FUNCTION================================================================
int main() {
	int np; //Number of points
	int nc; //Number of chambers
	int ns; //Number of segs
	int nportals = 0;
	int i = 0;
	Point *points = NULL;
	Chamber *chambers = NULL;
	int startIdx;
	Segment *segs = NULL;
	Edge **graph = NULL;

	//Getting number of points
	scanf("%d", &np);
	points = (Point*)malloc(sizeof(Point) * np);
	for(i=0; i<np; i++) {
		scanf("%f %f", &points[i].x, &points[i].y);
	}

	//Getting number of chambers
	scanf("%d", &nc);
	chambers = (Chamber*)malloc(sizeof(Chamber) * nc);
	for(i=0; i<nc; i++) {
		chambers[i].n = 0;
		scanf("%d %d", &chambers[i].v, &chambers[i].o);
		if(chambers[i].o == 1) nportals++;
		if(chambers[i].v < 0 || chambers[i].v > np) exit(0);
	}

	//Getting number of segs
	scanf("%d\n", &ns);
	segs = (Segment*)calloc(ns, sizeof(Segment));
	for(i=0; i<ns; i++) {
		scanf("%d %d", &segs[i].b, &segs[i].e);
		//printf("Segment[%d] = b[%d] e[%d]\n", segs[i].b, segs[i].e);
		if(abs(segs[i].b) > np) exit(0);
		if(abs(segs[i].e) > np) exit(0);
		if(abs(segs[i].b) == abs(segs[i].e)) exit(0);
		calculateDistance(segs, i, points);
	}

	//Reading Start Chamber
	scanf("%d", &startIdx);

	//Print Every Information read 
	//printf("Start point = %d\n", startIdx);
	//printf("np = %d\n", np);
	//printf("nc = %d\n", nc);
	//printf("ns = %d\n", ns);
	//printPoints(points, np);
	//printChambers(chambers, nc);
	//printSegments(segs, ns);

	//Converting in Graph
	//printf("Converting in Graph\n");
	graph = populateGraph(np, chambers, nc, segs, ns, startIdx);
	//printf("Printing GRAPH...\n");
	//printGraph(graph, np);

	//Working in this data
	getPossibleWays(graph, np, startIdx);


	//Free Everything
	freeGraph(graph, np);

	return 0;
}