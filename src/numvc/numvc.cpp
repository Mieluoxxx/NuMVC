#include "numvc.h"
#include <cstring>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int V, E;
int cutoff;
int threshold;
float rho;

int conf_change[MAXV];
int dscore[MAXV];
int G[MAXV][MAXV] = {0};
int edge_weight[MAXE] = {0};
int timer_mod[MAXV];
int v_in_c[MAXV] = {0};
int bestv_in_c[MAXV] = {0};
int best_dscv = 0;
int tabu_remove = 0;
int average_weight = 0;
int delta_weight = 0;

Set C;
Set bestC;
Set uncovered;
Set v_to_e[MAXV];

Edge::Edge() {
    // 默认构造函数的实现
}

Edge::Edge(int aa, int bb) : a(aa), b(bb) {
    // 带参数的构造函数的实现
}


void cover(int e) {
    uncovered.erase(e);
}

void uncover(int e) {
    uncovered.insert(e);
}

void add(Edge edge[], int v) {
    C.insert(v);
    v_in_c[v] = 1;
    dscore[v] = -dscore[v];

    Set::iterator pi = v_to_e[v].begin();
    for (; pi != v_to_e[v].end(); pi++) {
        int edgidx = *pi;  // v's pi'th edge
        int neigh = edge[edgidx].a + edge[edgidx].b - v;

        if (!v_in_c[neigh]) {  // neighbor isn't in the cover
                               // then the edge should be added to covered.
            dscore[neigh] -= edge_weight[edgidx];
            conf_change[neigh] = 1;
            cover(edgidx);
        } else {
            // needn't set confchange, cuz it must be 1 for those in C
            dscore[neigh] += edge_weight[edgidx];
        }
    }
}

void remove(Edge edge[], int v) {
    C.erase(v);
    v_in_c[v] = 0;
    dscore[v] = -dscore[v];
    conf_change[v] = 0;

    Set::iterator pi = v_to_e[v].begin();
    for (; pi != v_to_e[v].end(); pi++) {
        int edgidx = *pi;  // v's pi'th edge
        int neigh = edge[edgidx].a + edge[edgidx].b - v;

        if (!v_in_c[neigh]) {  // neighbor isn't in the cover
                               // then the edge should be removed from the
                               // covered.
            dscore[neigh] += edge_weight[edgidx];
            conf_change[neigh] = 1;
            uncover(edgidx);
        } else {
            // needn't set confchange, cuz it must be 1 for those in C
            dscore[neigh] -= edge_weight[edgidx];
        }
    }
}

void update_bestC() {
    // bestC.clear();
    bestC.assign(C.begin(), C.end());
    for (int i = 1; i <= V; i++) {
        bestv_in_c[i] = v_in_c[i];
    }
}

void update_best_dscv() {
    Set::iterator pi = C.begin();
    best_dscv = *pi++;
    int tmpv;
    for (; pi != C.end(); pi++) {
        tmpv = *pi;
        if (tmpv == tabu_remove)
            continue;
        if (dscore[tmpv] < dscore[best_dscv])
            continue;
        else if (dscore[tmpv] > dscore[best_dscv])
            best_dscv = tmpv;
        // timer_mod less, means earlier modified.
        else if (timer_mod[tmpv] < timer_mod[best_dscv])
            best_dscv = tmpv;
    }
}

// weight forgetting mechanism
void forget_edge_weight(Edge edge[]){
    int sum_w = 0;
    memset(dscore, 0, sizeof(dscore));

    for (int i = 1; i <= E; i++) {
        edge_weight[i] = edge_weight[i] * rho;
        sum_w += edge_weight[i];
        
        // update dscore
        // if both vertexs of edge are in C, dscore won't be affected
        // edge's two vertexs are neither in C
        if (v_in_c[edge[i].a] + v_in_c[edge[i].b] == 0) {
            dscore[edge[i].a] += edge_weight[i];
            dscore[edge[i].b] += edge_weight[i];
        }
        // one vertex is in C
        else if (v_in_c[edge[i].a] + v_in_c[edge[i].b] == 1) {
            if (v_in_c[edge[i].a])
                dscore[edge[i].a] -= edge_weight[i];
            else
                dscore[edge[i].b] -= edge_weight[i];
        }
    }
    average_weight = sum_w / E;
}

void update_edge_weight(Edge edge[]){
    Set::iterator pi = uncovered.begin();
    for (; pi != uncovered.end(); pi++) {
        int tmpe = *pi;
        edge_weight[tmpe]++;
        dscore[edge[tmpe].a]++;
        dscore[edge[tmpe].b]++;
        
    }
    delta_weight += uncovered.size();
    if (delta_weight >= E) {
        average_weight++;
        delta_weight -= E;
    }

    // Forget weights if ave_w reaches threshold
    if (average_weight >= threshold) {
        forget_edge_weight(edge);
    }
}

void init_(Edge edge[]) {
    average_weight = 1;
    delta_weight = 0;
    for (int i = 1; i <= V; i++) {
        conf_change[i] = 1;
        dscore[i] = 0;
        timer_mod[i] = 0;
        v_in_c[i] = 0;
        bestv_in_c[i] = 0;
    }
    for (int i = 1; i <= E; i++) {
        edge_weight[i] = 1;
        dscore[edge[i].a] += edge_weight[i];
        dscore[edge[i].b] += edge_weight[i];
        // At first, all the edges are uncovered
        uncovered.insert(i);
    }

    // Find a cover greedily
    while (!uncovered.empty()) {
        int best_dscore = 0;
        int best_cnt = 0;
        int best_idxs[MAXV] = {0};
        for (int v = 1; v <= V; v++) {
            if (v_in_c[v])
                continue;
            if (dscore[v] > best_dscore) {
                best_dscore = dscore[v];
                best_idxs[0] = v;
                best_cnt = 1;
            } else if (dscore[v] == best_dscore) {
                // store all the idxs with the highest dscore
                best_idxs[best_cnt++] = v;
            }
        }
        if (best_cnt > 0) {
            add(edge, best_idxs[rand() % best_cnt]);
        }
    }

    update_bestC();
}

// NuMVC algorithm
void solve(Edge edge[]) {
    init_(edge);
    int step = 0;
    while (step < cutoff) {
        // printf("Step %d \n", step);
        if (uncovered.empty()) {
            // whether to check the size before?-----[TO DO]
            if (C.size() < bestC.size())
                update_bestC();
            // choose a vertex in C with the highest dscore and remove it
            Set::iterator pi = C.begin();
            int best_dscore = -10000000;
            int best_cnt = 0;
            int best_idxs[MAXV] = {0};
            for (; pi != C.end(); pi++) {
                int tmpv = *pi;
                if (dscore[tmpv] > best_dscore) {
                    best_dscore = dscore[tmpv];
                    best_idxs[0] = tmpv;
                    best_cnt = 1;
                } else if (dscore[tmpv] == best_dscore) {
                    best_idxs[best_cnt++] = tmpv;
                }
            }
            // breaking ties randomly
            if (best_cnt > 0) {
                remove(edge, best_idxs[rand() % best_cnt]);
            }
            continue;
        }

        // choose a u \in C with the highest dscore, in favor of the oldest
        update_best_dscv();
        remove(edge, best_dscv);
        // choose an uncovered edge randomly
        int randidx = rand() % (uncovered.size());
        Set::iterator pi = uncovered.begin();
        for (int i = 0; i < randidx; i++)
            pi++;
        int choose_edge = *pi;

        // choose a vertex of choose_edge to be added to C
        int v1 = edge[choose_edge].a, v2 = edge[choose_edge].b;
        int best_addv = 0;
        // cuz choose_edge is uncovered, one of its 2 vertexs must be
        // Confchanged 1.
        if (conf_change[v1] == 0)
            best_addv = v2;
        else if (conf_change[v2] == 0)
            best_addv = v1;
        // Both confchanged 1, choose the one whose dscore's higher which is older
        else {
            if (dscore[v1] > dscore[v2] || (dscore[v1] == dscore[v2] && timer_mod[v1] < timer_mod[v2]))
                best_addv = v1;
            else
                best_addv = v2;
        }
        add(edge, best_addv);
        timer_mod[best_addv] = timer_mod[best_dscv] = step;
        tabu_remove = best_addv;
        update_edge_weight(edge);
        step++;
    }
}

// Check if the res is valid
bool is_VC(Edge edge[]){
    for (int i = 1; i <= E; i++) {
        if (bestv_in_c[edge[i].a] == 0 && bestv_in_c[edge[i].b] == 0) {
            //printf("Error!!! The Found C isn't a vertex cover.\n");
            return false;
        }
    }
    return true;
}

// print the result
void print_res(){
    printf("%d\n", V - bestC.size());
    bool firstone = true;
    for (int i = 1; i <= V; i++) {
        if (!bestv_in_c[i]) {
            if (firstone) {
                printf("%d", i);
                firstone = false;
            }
            else {
                printf(" %d", i);
            }
        }
    }
    printf("\n");
}
