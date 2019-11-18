#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/lgf_reader.h>
#include<bits/stdc++.h> 



using namespace lemon;


ListDigraph::Arc findMinArcWithoutCircle(
    ListDigraph &g,
    ListDigraph::ArcMap<int> &weight,
    ListDigraph::NodeMap<bool> &isNodeReached
    ) {
    ListDigraph::ArcIt minWeightArc(g);
    for (ListDigraph::ArcIt a(g); a!=INVALID; ++a) {
        if(weight[a] <= weight[minWeightArc] && // search minimum weight and
            (!isNodeReached[g.source(a)] || !isNodeReached[g.target(a)])) { // avoid circles                
            minWeightArc = a;
        }
    }
    return minWeightArc;
}

int prim(ListDigraph &g, ListDigraph::ArcMap<int> &weight) {
    // initialisiere X
    ListDigraph::NodeMap<bool>  isNodeInX(g, false);
    isNodeInX[g.nodeFromId(0)] = true;

    int nodeCountX  = 1;
    int totalWeight = 0;

    while(nodeCountX < countNodes(g)) {
        ListDigraph::Arc minWeightArc;
        int minCost = INT_MAX;

        // finde guenstigste Kante (u,v) mit u in X und v nicht in X
        for(ListDigraph::ArcIt a(g); a!=INVALID; ++a) {
            if(weight[a] < minCost &&
                isNodeInX[g.source(a)] &&
                !isNodeInX[g.target(a)]) {
                minWeightArc = a;
                minCost = weight[minWeightArc];
            }
        }

        totalWeight += weight[minWeightArc];
        
        // fuege v zu X hinzu
        isNodeInX[g.target(minWeightArc)] = true;
        nodeCountX++;
        // g.erase(minWeightArc);
    }

    return totalWeight;
}

int kruskal(ListDigraph &g, ListDigraph::ArcMap<int> &weight) {    
    ListDigraph::NodeMap<bool>  isNodeReached(g, false);
    ListDigraph::Arc minWeightArc;
    int nodeCount   = 0;
    int totalWeight = 0;

    int arcCount = 0;

    while(nodeCount < countNodes(g)) {
        minWeightArc = findMinArcWithoutCircle(g,weight,isNodeReached);
        if(!isNodeReached[g.source(minWeightArc)]) {
            isNodeReached[g.source(minWeightArc)] = true;
            nodeCount++;
        }
        if(!isNodeReached[g.target(minWeightArc)]) {
            isNodeReached[g.target(minWeightArc)] = true;
            nodeCount++;
        }
        // int nodeCountG = countNodes(g);
        totalWeight += weight[minWeightArc];
        arcCount++;
        // std::cout << nodeCount << " (+" << weight[minWeightArc]
        //     << ") " << totalWeight << std::endl;
        std::cout << g.id(g.source(minWeightArc)) << " " << g.id(g.target(minWeightArc)) << std::endl;    
        g.erase(minWeightArc);
    }
    std::cout << arcCount << std::endl;
    return totalWeight;
}


int main() {
    for (int i = 1; i <= 6; i++) {
        ListDigraph g;
        ListDigraph::ArcMap<int>    weight(g);
        ListDigraph::NodeMap<int>   label(g);

        digraphReader(g, "Data/Graph"+std::to_string(i)+".lgf"). // read the directed graph into g
            arcMap("weight", weight).
            nodeMap("label", label).
            run();

        std::cout << "lade Graph" << i << ".lgf" << std::endl;

        // int totalWeightK = kruskal(g,weight);
        int totalWeight = prim(g,weight);
        std::cout << countNodes(g) << " " << totalWeight
            << std::endl << std::endl;
    }
    
    return 1;
}