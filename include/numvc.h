#ifndef NUMVC_H
#define NUMVC_H

#include <list>
#include <set>

#define MAXV 1600
#define MAXE 500079

typedef std::list<int> Set;

#define RANDOMSEED 23
#define insert push_back
#define erase remove

extern int V, E;
extern int cutoff;
extern int threshold;
extern float rho;

extern int conf_change[MAXV];
extern int dscore[MAXV];
extern int G[MAXV][MAXV];
extern int edge_weight[MAXE];
extern int timer_mod[MAXV];
extern int v_in_c[MAXV];
extern int bestv_in_c[MAXV];
extern int best_dscv;
extern int tabu_remove;
extern int average_weight;
extern int delta_weight;

struct Edge {
    int a, b;
    Edge();
    Edge(int aa, int bb);
};

extern Set C;
extern Set bestC;
extern Set uncovered;
extern Set v_to_e[MAXV];

void cover(int e);
void uncover(int e);
void add(Edge edge[],int v);
void remove(Edge edge[],int v);
void update_bestC();
void update_best_dscv();
void forget_edge_weight(Edge edge[]);
void update_edge_weight(Edge edge[]);
void init_(Edge edge[]);
void solve(Edge edge[]);
bool is_VC(Edge edge[]);
void print_res();

#endif // NUMVC_H

