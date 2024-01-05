#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "numvc.h"
#include "utils.h"
using namespace std;


int main(int argc, char* argv[]) {
    string rootPath = "C:/Users/weiyi/Github/NuMVC/";
    string filename = "C1000.9.txt";

    string rawPath = rootPath + "data/raw/" + filename;
    string processedPath = rootPath + "data/processed/" + filename;
    string resultsPath = rootPath + "data/results/" + filename;

    freopen(processedPath.c_str(), "r", stdin);
    freopen(resultsPath.c_str(), "w", stdout);
    Edge edge[MAXE];
    srand(unsigned(time(NULL)));
    while (scanf("%d%d", &V, &E) != EOF) {
        // Build the graph
        int aa, bb;
        memset(G, 0, sizeof(G));
        memset(edge, 0, sizeof(edge));
        memset(conf_change, 0, sizeof(conf_change));
        C.clear();
        uncovered.clear();
        bestC.clear();
        for (int i = 1; i <= V; i++)
            v_to_e[i].clear();

        for (int i = 0; i < E; i++) {
            scanf("%d%d", &aa, &bb);
            G[aa][bb] = 1;
            G[bb][aa] = 1;
        }
        int numedge = 1;
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
        // ---Set the parameters here.------------
        cutoff = V * 300;
        threshold = (int)(0.5 * V);
        rho = 0.3;
        printf("----------------Parameters:--------------\n");
        printf("      V: %d    E: %d\n    \\rho: %f", V, E, rho);
        printf("    cutoff: %d    threshold: %d\n", cutoff, threshold);
        printf("\n----------------Results:--------------\n");

        // Search using NuMVC
        solve(edge);
        // Check if the res is valid
        if (is_VC(edge)) {
            printf("The found clique size is %d.\n", V - bestC.size());
            printf("The found clique:\n");
            print_res();
        }
        else{
            printf("0\n0");
        }
    }
    
    return 0;
}