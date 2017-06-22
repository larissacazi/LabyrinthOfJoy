#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

//Way==========================================================================
typedef struct Way{
	int *vector;
	int numOfPoints;
	float totalDistance;
	BOOL finalized;
	int *fill;
} Way;

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

Way *createWay(int np) {
	int i = 0;
	Way *way = NULL;

	way = (Way*)calloc(1, sizeof(Way));
	way->vector = malloc(sizeof(int) * np);
	way->fill = malloc(sizeof(int) * np);
	for(i=0; i<np; i++) {
		way->vector[i] = -1;
		way->fill[i] = 0;
	}
	way->numOfPoints = 0;

	return way;
}

void copyFillFromWay(Way way, int np, int **fill) {
	int i = 0;

	for(i=0; i<np; i++) {
		(*fill)[i] = way.fill[i];
	}
}

void copyFillToWay(Way *way, int np, int *fill) {
	int i = 0;

	//printf("copyFillToWay::numOfPoints = %d\n", np);
	for(i=0; i<np; i++) {
		//printf("fill[%d] = %d\n", i, fill[i]);
		//printf("way.fill[%d] = %d\n", i, (*way).fill[i]);
		(*way).fill[i] = fill[i];
	}
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

BOOL hasChildren(Edge **graph, int idx, int np, int *fill) {
	int j = 0;
	for(j=0; j<np; j++)	{
		if(j == idx) continue;
		if(fill[j] == 1) continue;
		if(graph[idx][j].dist >= 0) return TRUE;
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

	while(vector[i] != -1) {
		printf("%d ", vector[i]);
		i++;
	}
	printf("%.2f\n", sum);
}

int getNextChildren(Edge **graph, int np, int idx, int ab, int *fill) {
	int i = 0;

	if(idx >= np) return -1;

	for(i=ab+1; i<np; i++) {
		if(fill[i] == 1) continue;
		if(graph[idx][i].dist >= 0) {
			return i;
		}
	}

	return -1;
}

int getNumberOfChildren(Edge **graph, int np, int idx, int *fill) {
	int i = 0, count = 0;

	for(i=0; i<np; i++) {
		if(i == idx) continue;
		if(fill[i] == 1) continue;
		if(graph[idx][i].dist >= 0) {
			count++;
		}
	}

	return count;
}

void reallocWay(Way** ways, int *nWays, int np, int count) {
	int i = 0, j = 0;

	(*ways) = (Way*)realloc((*ways), sizeof(Way)*((*nWays) + count));
	
	for(j=0; j<count; j++) {
		(*ways)[(*nWays)+j].vector = malloc(sizeof(int) * np);
		(*ways)[(*nWays)+j].fill = malloc(sizeof(int) * np);
		for(i=0; i<np; i++) {
			(*ways)[(*nWays)+j].vector[i] = -1;
			(*ways)[(*nWays)+j].fill[i] = 0;
		}
		
		(*ways)[(*nWays)+j].numOfPoints = 0;
		(*ways)[(*nWays)+j].totalDistance = 0;
		(*ways)[(*nWays)+j].finalized =  FALSE;
	}

	(*nWays) = (*nWays) + count;
}

void insertPointInWay(Way *way, Edge **graph, int from, int to) {
	(*way).vector[(*way).numOfPoints] = to;
	(*way).totalDistance = (*way).totalDistance + graph[from][to].dist;
	(*way).finalized = graph[from][to].isPortal;
	(*way).numOfPoints++;
}

BOOL finalizedWay(Way way) {
	return way.finalized;
}

void printWays(Way* ways, int n) {
	int i = 0, j = 0;

	for(i=0; i<n; i++) {
		printf("---- Way[%d]:\n", i);
		printf("\tFinalized =\t[%d]\n", ways[i].finalized);
		printf("\tNumOfPoints =\t[%d]\n", ways[i].numOfPoints);
		printf("\tTotalDist =\t[%.2f]\n", ways[i].totalDistance);
		for(j=0; j<ways[i].numOfPoints; j++) printf("%d\t", ways[i].vector[j]);
		printf("\n------------------\n");
	}

}

int returnFather(Way way, int from) {
	int j = 0;

	for(j=0; j<way.numOfPoints; j++) {
		if(way.vector[j] == from) return  way.vector[j-1];
	}

	return -1;
}

void copyLastway(Way *current, Way last) {
	int i = 0;

	(*current).totalDistance = last.totalDistance;
	(*current).finalized = last.finalized;
	(*current).numOfPoints = last.numOfPoints;

	for(i=0; i<last.numOfPoints; i++) {
		(*current).vector[i] = last.vector[i];
		(*current).fill[i] = last.fill[i];
	}
}

/*
void getWays(Edge **graph, int np, Way **ways, int *nways, int *i, int from, int to) {
	int nextChildren = 0, children = from, num = 0;

	insertPointInWay(&ways[i], graph, from, to);

	if(getNextChildren(graph, np, from, to) == -1) return;
	if((*ways)[i].finalized == TRUE) return;

	num = getNumberOfChildren(graph, np, from);
	(*nways) = (*nways) + num;
	reallocWay(&ways, &nWays, np);

	while((nextChildren = getNextChildren(graph, np, from, children)) != -1) {
		i++;
		copyLastway(&ways[i], ways[i-1]);
		getWays(graph, np, ways, nways, i, from, to);
		children = nextChildren;
	}
}*/

int getIdFromWaysNotFinalized(Edge **graph, int np, Way *ways, int nWays, int *fill) {
	int i = 0, from = 0;

	for(i=0; i<nWays; i++) {
		from = ways[i].vector[ways[i].numOfPoints-1];
		printf("getIdFromWaysNotFinalized::from %d\n", from);
		if(ways[i].finalized == TRUE && hasChildren(graph, from, np, fill) == FALSE) continue;
		if(ways[i].finalized == FALSE && hasChildren(graph, from, np, fill) == FALSE) continue;
		return i;
	}

	return -1;
}

Way *getPossibleWays(Edge **graph, int np, int startIdx, int *n) {
	Way* ways = NULL;
	int nWays = 0, i = 0, j = 0, old = 0;
	int from = startIdx, to = startIdx, nextChildren = 0, num = 0;
	int *fill = NULL;

	fill = (int*)calloc(np, sizeof(int));

	ways = createWay(np); nWays++; i = nWays - 1;
	printf("Indexes: from[%d] to[%d]\n", from, to);
	insertPointInWay(&ways[i], graph, from, to);
	fill[to] = 1;
	copyFillToWay(&ways[i], np, fill);
	printWays(ways, nWays);

	while(i < nWays && i != -1) {
		getchar();
		printf("\t I = %d\n", i);
		copyFillFromWay(ways[i], np, &fill);

		if(ways[i].finalized == TRUE && hasChildren(graph, ways[i].vector[ways[i].numOfPoints-1], np, fill) == TRUE) {
			printf("finalized!\n");
			reallocWay(&ways, &nWays, np, 1); i++;
			copyLastway(&ways[nWays-1], ways[i-1]);
			ways[nWays-1].finalized = FALSE;
			from = ways[nWays-1].vector[ways[i].numOfPoints-1];
			printWays(ways, nWays);
			printf("0000000000 \t finalized! I = %d nWays = %d\n", i, nWays);
		}

		num = getNumberOfChildren(graph, np, from, fill);
		printf("from [%d] - num = %d\n", from, num);
		if(num > 1) {
			num--;
			old = nWays;
			printf("Before nWays = %d i = %d\n", nWays, i);
			reallocWay(&ways, &nWays, np, num);
			printf("After nWays = %d i = %d\n", nWays, i);

			for(j=old; j<nWays; j++) {
				copyLastway(&ways[j], ways[i]);
			}
			printf("Ways: nWays[%d] i[%d]----------\n", nWays, i);
			printWays(ways, nWays);
			printf("----------------\n");

			to = 0;
			for(j=i; j<nWays; j++) {
				printf("Ok j [%d] from[%d] to[%d]\n", j, from, to);
				printf("nextChildren :");
				nextChildren = getNextChildren(graph, np, from, to, fill);
				printf("nextChildren = [%d]\n", nextChildren);
				to = nextChildren;
				printf("From[%d] To[%d]\n", from, to);
				insertPointInWay(&ways[j], graph, from, to);
				fill[to] = 1;
				getchar();
			}

			for(j=i; j<nWays; j++) copyFillToWay(&ways[j], np, fill);

			from = to;
		}
		else if(num == 1) {
			to = 0;
			printf("num==1, from[%d] to[%d]\n", from, to);
			nextChildren = getNextChildren(graph, np, from, to, fill);
			to = nextChildren;
			printf("From[%d] To[%d]\n", from, to);
			insertPointInWay(&ways[i], graph, from, to);
			fill[to] = 1;
			copyFillToWay(&ways[i], np, fill);
			from = to;
		}
		else if(num == 0) {
			i++;
		}
		printWays(ways, nWays);

		/*if(hasChildren(graph, from, np) == FALSE) {
			insertPointInWay(&ways[j], graph, from, to);
		}*/

		printf("Inserted!\n");

		//i = getIdFromWaysNotFinalized(graph, np, ways, nWays, fill);
		if(i < nWays) from = ways[i].vector[ways[i].numOfPoints-1];
		printf("I = %d FROM = %d ---- Enter...\n", i, from);
	}

	(*n) = nWays;
	return ways;
}

Way *workingWays(Way *ways, int nWays, Chamber *chambers, int nc, int np, int *nw) {
	Way *correct = NULL;
	int i, j, n = 0;

	for(i=0; i<nWays; i++) {
		printf("---- \t Way[%d]\n", i);
		printf("\t Last element [%d]\n", ways[i].vector[ways[i].numOfPoints-1]);
		for(j=0; j<nc; j++) {
			printf("chamber[%d].v = %d\n", j, chambers[j].v);
			if((ways[i].vector[ways[i].numOfPoints-1] + 1) == chambers[j].v) {
				printf("Entrou:: chamber[%d].o = %d\n", j, chambers[j].o);
				if(chambers[j].o == 1) {
					reallocWay(&correct, &n, np, 1);
					copyLastway(&correct[n-1], ways[i]);
				}
			}
		}
		printf("-------------\n");
	}

	(*nw) = n;
	return correct;
}

void printSolution(Way *ways, int nWays) {
	int i = 0, j = 0;

	for(i=0; i<nWays; i++) {
		printf("%d ", ways[i].numOfPoints);
		for(j=0; j<ways[i].numOfPoints; j++) {
			printf("%d ", ways[i].vector[j]+1);
		}
		printf("%d\n", (int)ways[i].totalDistance);
	}
}

void swapWays(Way first, Way second, int np) {
	Way *aux = createWay(np);
	copyLastway(aux, first);
	copyLastway(&first, second);
	copyLastway(&second, *aux);
}

void sortWays(Way *ways, int nWays, int np) {
	int i, j;
	for(i=0; i<nWays-1; i++) {
			if(ways[i].totalDistance > ways[i+1].totalDistance) {
				swapWays(ways[i], ways[i+1], np);
			}
			else if(ways[i].totalDistance == ways[i+1].totalDistance) {
					if(ways[i].numOfPoints > ways[i+1].numOfPoints) {
						swapWays(ways[i], ways[i+1], np);
					}
					else if(ways[i].numOfPoints == ways[i+1].numOfPoints) {
							for(j=0; j<ways[i].numOfPoints; j++) {
									if(ways[i].vector[j] > ways[i+1].vector[j]) {
										swapWays(ways[i], ways[i+1], np);
									}
							}
					}
			}
	}

	printSolution(ways, nWays);
}

//MAIN FUNCTION================================================================
int main() {
	int np; //Number of points
	int nc; //Number of chambers
	int ns; //Number of segs
	int nw = 0; //Number of ways
	int nportals = 0, nWays = 0;
	int i = 0;
	Point *points = NULL;
	Chamber *chambers = NULL;
	int startIdx;
	Segment *segs = NULL;
	Edge **graph = NULL;
	Way *ways = NULL, *correctWays = NULL;

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
	scanf("%d", &startIdx); startIdx = startIdx - 1;

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
	printf("Printing GRAPH...\n");
	printGraph(graph, np);

	//Working in this data
	printf("Getting Possible Ways...startIdx[%d]\n", startIdx);
	ways = getPossibleWays(graph, np, startIdx, &nWays);
	printf("Printing WAYS...\n");
	printWays(ways, nWays);

	correctWays = correctWays = workingWays(ways, nWays, chambers, nc, np, &nw);
	printf("\n ------ \t Printing CORRECT WAYS...\n");
	//printWays(correctWays, nw);

	sortWays(correctWays, nw, np);

	//Free Everything
	freeGraph(graph, np);

	return 0;
}