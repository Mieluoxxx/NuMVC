#ifndef NUMVC_H
#define NUMVC_H

#include <list>
#include <set>

#define MAXV 1200   // 最大节点数量
#define MAXE 200079 //  最大边数量

typedef std::list<int> Set; // 用于存储节点的集合

#define RANDOMSEED 23   // 随机种子
#define insert push_back    // 为了兼容list和set
#define erase remove        // 为了兼容list和set

extern int V, E;        // 节点数和边数
extern int cutoff;      // 限制
extern int threshold;   // 阈值
extern float rho;   // rho

extern int conf_change[MAXV];   // 置信度变化
extern int dscore[MAXV];        // 节点的置信度
extern int G[MAXV][MAXV];       // 邻接矩阵
extern int edge_weight[MAXE];   // 边的权重
extern int timer_mod[MAXV];     // 计时器
extern int v_in_c[MAXV];        // 节点是否在C中
extern int bestv_in_c[MAXV];    // 节点是否在bestC中
extern int best_dscv;           // 最优置信度   
extern int tabu_remove;         // 禁忌移除
extern int average_weight;      // 平均权重
extern int delta_weight;        // 变化的权重

struct Edge {   // 边
    int a, b;
    Edge();
    Edge(int aa, int bb);
};

extern Set C;   
extern Set bestC;
extern Set uncovered;
extern Set v_to_e[MAXV];
extern Edge edge[MAXE];

void cover(int e);
void uncover(int e);
void add(int v);
void remove(int v);
void update_bestC();
void update_best_dscv();
void forget_edge_weight();
void update_edge_weight();
void init_();
void solve();
bool is_VC();
void print_res();

#endif // NUMVC_H

