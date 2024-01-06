#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <ctime>
#include <cmath>
#include "numvc.h"

using namespace std;

int V, E;
int cutoff;
int threshold;
float rho;

int conf_change[MAXV];
int d_score[MAXV];
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
Edge edge[MAXE];

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

void vertex_add(int v) {
    C.insert(v);
    v_in_c[v] = 1;
    d_score[v] = -d_score[v];

    for (int edgidx : v_to_e[v]) {
        int neigh = edge[edgidx].a + edge[edgidx].b - v;

        if (!v_in_c[neigh]) {
            d_score[neigh] -= edge_weight[edgidx];
            conf_change[neigh] = 1;
            cover(edgidx);
        } else {
            d_score[neigh] += edge_weight[edgidx];
        }
    }
}

// vertex_remove v from C
void vertex_remove(int v) {
    C.erase(v);
    v_in_c[v] = 0;
    d_score[v] = -d_score[v];
    conf_change[v] = 0;

    for (int edgidx : v_to_e[v]) {
        int neigh = edge[edgidx].a + edge[edgidx].b - v;

        if (!v_in_c[neigh]) {
            d_score[neigh] += edge_weight[edgidx];
            conf_change[neigh] = 1;
            uncover(edgidx);
        } else {
            d_score[neigh] -= edge_weight[edgidx];
        }
    }
}

// Update bestC as C
void update_bestC() {
    bestC = C;  // 使用 std::list 的赋值操作符进行复制
    copy(begin(v_in_c), end(v_in_c), begin(bestv_in_c));
}

void update_best_dscv() {
    best_dscv = *C.begin(); // 初始化为第一个节点
    for (int tmpv : C) {
        if (tmpv == tabu_remove)
            continue;
        if (d_score[tmpv] < d_score[best_dscv])
            continue;
        else if (d_score[tmpv] > d_score[best_dscv] || timer_mod[tmpv] < timer_mod[best_dscv])
            best_dscv = tmpv;
    }
}

void forget_edge_weight() {
    int sum_w = 0;
    fill(begin(d_score), end(d_score), 0);

    for (int i = 1; i <= E; i++) {
        edge_weight[i] *= rho;
        sum_w += edge_weight[i];

        // update d_score
        int a_in_C = v_in_c[edge[i].a];
        int b_in_C = v_in_c[edge[i].b];

        if (a_in_C + b_in_C == 0) {
            d_score[edge[i].a] += edge_weight[i];
            d_score[edge[i].b] += edge_weight[i];
        } else if (a_in_C + b_in_C == 1) {
            d_score[a_in_C ? edge[i].a : edge[i].b] -= edge_weight[i];
        }
    }

    average_weight = sum_w / E;
}

void update_edge_weight() {
    for (int tmpe : uncovered) {
        edge_weight[tmpe]++;
        d_score[edge[tmpe].a]++;
        d_score[edge[tmpe].b]++;
    }

    delta_weight += uncovered.size();
    
    if (delta_weight >= E) {
        average_weight++;
        delta_weight -= E;
    }

    // Forget weights if ave_w reaches threshold
    if (average_weight >= threshold) {
        forget_edge_weight();
    }
}

// init the datas for the gragh and find the oroginal C greedily
void init_() {
    average_weight = 1;
    delta_weight = 0;
    for (int i = 1; i <= V; i++) {
        conf_change[i] = 1;
        d_score[i] = 0;
        timer_mod[i] = 0;
        v_in_c[i] = 0;
        bestv_in_c[i] = 0;
    }
    for (int i = 1; i <= E; i++) {
        edge_weight[i] = 1;
        d_score[edge[i].a] += edge_weight[i];
        d_score[edge[i].b] += edge_weight[i];
        // At first, all the edges are uncovered
        uncovered.insert(i);
    }

    // Find a cover greedily
    while (!uncovered.empty()) {
        int best_d_score = 0;
        int best_cnt = 0;
        int best_idxs[MAXV] = {0};
        for (int v = 1; v <= V; v++) {
            if (v_in_c[v])
                continue;
            if (d_score[v] > best_d_score) {
                best_d_score = d_score[v];
                best_idxs[0] = v;
                best_cnt = 1;
            } else if (d_score[v] == best_d_score) {
                // store all the idxs with the highest d_score
                best_idxs[best_cnt++] = v;
            }
        }
        if (best_cnt > 0) {
            vertex_add(best_idxs[rand() % best_cnt]);
        }
    }

    update_bestC();
}

// NuMVC algorithm
void solve() {
    init_();
    int step = 0;
    while (step < cutoff) {
        if (uncovered.empty()) {
            if (C.size() < bestC.size())
                update_bestC();
            // choose a vertex in C with the highest d_score and vertex_remove it
            Set::iterator pi = C.begin();
            int best_d_score = -10000000;
            int best_cnt = 0;
            int best_idxs[MAXV] = {0};
            for (; pi != C.end(); pi++) {
                int tmpv = *pi;
                if (d_score[tmpv] > best_d_score) {
                    best_d_score = d_score[tmpv];
                    best_idxs[0] = tmpv;
                    best_cnt = 1;
                } else if (d_score[tmpv] == best_d_score) {
                    best_idxs[best_cnt++] = tmpv;
                }
            }
            // breaking ties randomly
            if (best_cnt > 0) {
                vertex_remove(best_idxs[rand() % best_cnt]);
            }
            continue;
        }

        // choose a u \in C with the highest d_score, in favor of the oldest
        update_best_dscv();
        vertex_remove(best_dscv);
        // choose an uncovered edge randomly
        int randidx = rand() % (uncovered.size());
        Set::iterator pi = uncovered.begin();
        for (int i = 0; i < randidx; i++)
            pi++;
        int choose_edge = *pi;

        // choose a vertex of choose_edge to be vertex_added to C
        int v1 = edge[choose_edge].a, v2 = edge[choose_edge].b;
        int best_vertex_addv = 0;
        // cuz choose_edge is uncovered, one of its 2 vertexs must be
        // Confchanged 1.
        if (conf_change[v1] == 0)
            best_vertex_addv = v2;
        else if (conf_change[v2] == 0)
            best_vertex_addv = v1;
        // Both confchanged 1, choose the one whose d_score's higher which is older
        else {
            if (d_score[v1] > d_score[v2] || (d_score[v1] == d_score[v2] && timer_mod[v1] < timer_mod[v2]))
                best_vertex_addv = v1;
            else
                best_vertex_addv = v2;
        }
        vertex_add(best_vertex_addv);
        timer_mod[best_vertex_addv] = timer_mod[best_dscv] = step;
        tabu_remove = best_vertex_addv;
        update_edge_weight();
        step++;
    }
}

bool check_valid() {
    for (int i = 1; i <= E; ++i) {
        if (bestv_in_c[edge[i].a] == 0 && bestv_in_c[edge[i].b] == 0) {
            return false;
        }
    }
    return true;
}


void print_res() {
    cout << V - bestC.size() << '\n';

    bool firstone = true;
    for (int i = 1; i <= V; ++i) {
        if (!bestv_in_c[i]) {
            cout << (firstone ? "" : " ") << i;
            firstone = false;
        }
    }

    cout << '\n';
}

void clear_workspace(){
    memset(G, 0, sizeof(G));
    memset(edge, 0, sizeof(edge));
    memset(conf_change, 0, sizeof(conf_change));
    C.clear();
    uncovered.clear();
    bestC.clear();

    for (int i = 1; i <= V; i++)
    v_to_e[i].clear();
}