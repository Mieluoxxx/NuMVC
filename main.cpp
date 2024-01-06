#include <iostream>
#include <string.h>
#include "numvc.h"
using namespace std;


int main(int argc, char* argv[]) {
    string filename = "brock200_2.txt";
    // string filename = "brock400_2.txt";
    // string filename = "C125.9.txt";
    // string filename = "gen200_p0.9_44.txt";
    // string filename = "hamming8-4.txt";
    // string filename = "MANN_a27.txt";
    // string filename = "p_hat300-1.txt";
    // string filename = "p_hat1500-1.txt";    // 越界
    
    // string rootPath = "/Users/moguw/Github/NuMVC/";
    string rootPath = "C:/Users/weiyi/Github/NuMVC/";

    string rawPath = rootPath + "data/raw/" + filename;
    string processedPath = rootPath + "data/processed/" + filename;
    string resultsPath = rootPath + "data/results/" + filename;

    freopen(processedPath.c_str(), "r", stdin);
    freopen(resultsPath.c_str(), "w", stdout);


    srand(unsigned(time(NULL)));    // 
    while (cin >> V >> E) {
        // 从输入中读取节点数和边数，当无法再读取时终止循环
        int aa, bb;
        clear_workspace();  // 清理工作区，包括清空图的相关数据结构和集合

        // 构建图
        for (int i = 0; i < E; i++) {
            cin >> aa >> bb;
            G[aa][bb] = G[bb][aa] = 1;
        }

        int numedge = 1;
        // 遍历节点对，如果节点对之间没有边，则将边加入图中
        for (int i = 1; i <= V; i++)
            for (int j = i + 1; j <= V; j++)
                if (!G[i][j]) {
                    edge[numedge].a = i;
                    edge[numedge].b = j;
                    v_to_e[i].insert(numedge);
                    v_to_e[j].insert(numedge);
                    numedge++;
                }
        E = numedge - 1;

        // 设置参数
        cutoff = V * 300;
        threshold = static_cast<int>(0.5 * V);
        rho = 0.3;  //0.2 0.25 0.3 0.35 0.4

        // 借鉴项目输出构建方式
        printf("----------------Parameters:--------------\n");
        printf("      V: %d    E: %d\n    \\rho: %f", V, E, rho);
        printf("    cutoff: %d    threshold: %d\n", cutoff, threshold);
        printf("\n----------------Results:--------------\n");

        // 使用 NuMVC 进行搜索
        solve();

        // 检查结果是否为有效的顶点覆盖
        if (check_valid()) {
            printf("The found clique size is %d.\n", V - bestC.size());
            printf("The found clique:\n");
            print_res();
        } else {
            printf("0\n0");
        }
    }
    
    return 0;
}