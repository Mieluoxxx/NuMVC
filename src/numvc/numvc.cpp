#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <ctime>
#include <cmath>
#include "numvc.h"

using namespace std;

// 全局变量声明
int V, E;                 // 顶点数和边数
int cutoff;               // 运行截止步数
int threshold;            // 权重平均值的阈值
float rho;                // 权重遗忘系数

int conf_change[MAXV];    // 记录节点是否发生了变化
int d_score[MAXV];        // 节点的分数
int G[MAXV][MAXV] = {0};  // 图的邻接矩阵
int edge_weight[MAXE] = {0};  // 边的权重数组
int timer_mod[MAXV];       // 计时器数组
int v_in_c[MAXV] = {0};    // 记录节点是否在当前解中
int bestv_in_c[MAXV] = {0};  // 记录最优解中节点的情况
int best_dscv = 0;         // 最优解的分数
int tabu_remove = 0;       // 禁忌移除的节点
int average_weight = 0;    // 边权重的平均值
int delta_weight = 0;      // 边权重变化量

Set C;                      // 当前解的节点集合
Set bestC;                  // 最优解的节点集合
Set uncovered;              // 未覆盖的边的集合
Set v_to_e[MAXV];           // 节点到边的映射
Edge edge[MAXE];            // 边的结构体定义

Edge::Edge() {
    // 默认构造函数的实现
}

Edge::Edge(int aa, int bb) : a(aa), b(bb) {
    // 带参数的构造函数的实现
}

// 将边标记为已覆盖
void cover(int e) {
    uncovered.erase(e);
}

// 将边标记为未覆盖
void uncover(int e) {
    uncovered.insert(e);
}

// 向当前解中添加节点
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

// 从当前解中移除节点
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

// 将当前解更新为最优解
void update_bestC() {
    bestC = C;  // 使用 std::list 的赋值操作符进行复制
    copy(begin(v_in_c), end(v_in_c), begin(bestv_in_c));
}

// 更新最优解的分数
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

// 遗忘边的权重
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

// 更新未覆盖边的权重
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

    // 如果平均权重超过阈值，则遗忘边的权重
    if (average_weight >= threshold) {
        forget_edge_weight();
    }
}

// 初始化图数据并贪心地找到原始C
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
        // 最初，所有边都未覆盖
        uncovered.insert(i);
    }

    // 贪心地找到一个覆盖集合
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
                // 存储所有具有最高 d_score 的索引
                best_idxs[best_cnt++] = v;
            }
        }
        if (best_cnt > 0) {
            vertex_add(best_idxs[rand() % best_cnt]);
        }
    }

    update_bestC();
}

// NuMVC算法
void solve() {
    init_();
    int step = 0;
    while (step < cutoff) {
        if (uncovered.empty()) {
            if (C.size() < bestC.size())
                update_bestC();
            // 选择C中d_score最高的顶点并将其从C中移除
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
            // 随机打破平局
            if (best_cnt > 0) {
                vertex_remove(best_idxs[rand() % best_cnt]);
            }
            continue;
        }

        // 选择C中d_score最高的u，更喜欢较老的
        update_best_dscv();
        vertex_remove(best_dscv);
        // 随机选择一个未覆盖的边
        int randidx = rand() % (uncovered.size());
        Set::iterator pi = uncovered.begin();
        for (int i = 0; i < randidx; i++)
            pi++;
        int choose_edge = *pi;

        // 随机选择choose_edge的一个顶点添加到C中
        int v1 = edge[choose_edge].a, v2 = edge[choose_edge].b;
        int best_vertex_addv = 0;
        // 因为choose_edge未覆盖，其两个顶点中必有一个confchanged为1
        if (conf_change[v1] == 0)
            best_vertex_addv = v2;
        else if (conf_change[v2] == 0)
            best_vertex_addv = v1;
        // 两者都confchanged为1，选择d_score较高且较老的
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

// 检查解是否有效
bool check_valid() {
    for (int i = 1; i <= E; ++i) {
        if (bestv_in_c[edge[i].a] == 0 && bestv_in_c[edge[i].b] == 0) {
            return false;
        }
    }
    return true;
}

// 打印结果
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

// 清空工作空间，重置数据结构和数组
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