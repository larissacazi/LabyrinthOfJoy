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
	BOOL added;
} Chamber;

typedef struct Segment {
	int b;	//First point of Segment
	int e;	//Second point of Segment
	BOOL visited;
	float d;	//Distance between the two points of Segment
} Segment;

typedef struct Point {
	float x;
	float y;
} Point;

//Graph========================================================================
struct vertice {
	int idx;
	BOOL isPortal;
	int nedges;
	EDGE **edge;
};
typedef struct vertice Vertice;

struct edge {
	float dist;
	Vertice *pvertice;
};
typedef struct edge EDGE;

//=============================================================================
typedef struct Edge {
	int dist;
	BOOL isPortal
} Edge;
//Graph Functions==============================================================
Vertice *createVertice(int idx, BOOL isPortal) {
	Vertice *pvertice = NULL;

	pvertice = (Vertice*)calloc(1, sizeof(Vertice));

	pvertice->idx = idx;
	pvertice->isPortal = isPortal;

	return pvertice;
}

void addEdge(Vertice **pvertice, Edge *pedge) {
	(*pvertice)->edge[(*pvertice)->nedges] = pedge;
	(*pvertice)->nedges++;
}

Edge *createEdge(float dist) {
	Edge *pedge = NULL;
	pedge = (Edge*)calloc(1, sizeof(Edge));
	pedge->dist = dist;
}

void addNewEdge(Vertice **pvertice, int dist) {
	Edge *pedge = createEdge(dist);
	(*pvertice)->edge[(*pvertice)->nedges] = pedge;
	(*pvertice)->nedges++;
}
//Chamber Functions============================================================
int getChamberIndexById(Chamber *chambers, int nc, int startIdx) {
	int i = 0;

	for(i=0; i<nc; i++) {
		if(chambers[i].v == startIdx) return i;
	}

	return -1;
}

//Segment Functions============================================================
int getSegmentIndexById(Segment *segs, int ns, int id) {
	int i = 0;

	for(i=0; i<ns; i++) {
		if(segs[i].b < 0 || segs[i].e < 0) continue;
		if(segs[i].b == id) return i;
	}

	return -1;
}


//=============================================================================
void calculateDistance(Segment *segs, int idx, Point *points) {
	printf("b = [%d] - Points[%d] = (%.2f, %.2f)\n", segs[idx].b, abs(segs[idx].b)-1, points[abs(segs[idx].b)-1].x, points[abs(segs[idx].b)-1].y);
	printf("e = [%d] - Points[%d] = (%.2f, %.2f)\n", segs[idx].e, abs(segs[idx].e)-1, points[abs(segs[idx].e)-1].x, points[abs(segs[idx].e)-1].y);
	segs[idx].d = sqrt(pow(points[abs(segs[idx].b)-1].x - points[abs(segs[idx].e)-1].x, 2) 
		+ pow(points[abs(segs[idx].b)-1].y - points[abs(segs[idx].e)-1].y, 2));
	if(segs[idx].d == 0) exit(0);

	printf("dist = %.2f\n", segs[idx].d);
	printf("------------------\n");
}

void printPoints(Point *points, int np) {
	int i = 0;

	for(i=0; i<np; i++) {
		printf("Point[%d] = (x=%.2f, y=%.2f)\n", i, points[i].x, points[i].y);
	}
}

void printChambers(Chamber *chambers, int nc) {
	int i = 0;

	for(i=0; i<nc; i++) {
		printf("Chamber[%d] = (v=%d, n=%d, o=%d)\n", i, chambers[i].v, chambers[i].n, chambers[i].o);
	}
}

void printSegments(Segment *segs, int ns) {
	int i = 0;

	for(i=0; i<ns; i++) {
		printf("Segment[%d] = (b=%d, e=%d, dist=%.2f)\n", i, segs[i].b, segs[i].e, segs[i].d);
	}
}

Vertice *populateVerticeGraph(Chamber *chambers, int nc, Segment *segs, int ns, int startIdx) {
	Vertice *pvertice = NULL, *paux = NULL;
	int i = 0, idx = 0;

	//Add first vertice
	idx = getChamberIndexById(chambers, nc, startIdx);
	pvertice = createVertice(chambers[idx].v, chambers[idx].o);
	chambers[idx].added = TRUE;

	paux = pvertice;

	while(i < nc) {
		while((idx = getSegmentIndexById(segs, ns, id)) != -1) {

		}
		i++;
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
		chambers[i].added = FALSE;
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
	printf("Start point = %d\n", startIdx);
	printf("np = %d\n", np);
	printf("nc = %d\n", nc);
	printf("ns = %d\n", ns);
	printPoints(points, np);
	printChambers(chambers, nc);
	printSegments(segs, ns);

	//Converting in Graph

	return 0;
}