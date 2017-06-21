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