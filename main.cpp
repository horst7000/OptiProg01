#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/lgf_reader.h>
#include<bits/stdc++.h> 


using namespace lemon;



/**
 * Aufgabe1:  Laufzeit O(n^3)
 * Gibt Gewicht des Spannbaums zurueck. Uebergebene ArcMap SpanningTree enthaelt nach
 * durchlauf im Spannbaum enthaltene Kanten.
 */
int prim(ListDigraph &g, ListDigraph::ArcMap<int> &weight, ListDigraph::ArcMap<bool> &spanningTree) {
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

        spanningTree[minWeightArc] = true;
        totalWeight += weight[minWeightArc];
        
        // fuege v zu X hinzu
        isNodeInX[g.target(minWeightArc)] = true;
        nodeCountX++;
        // g.erase(minWeightArc);
    }

    return totalWeight;
}

/**
 * Aufgabe2:  Laufzeit O(n^3)
 * Gibt minimale Kosten von start zu allen Knoten zurueck.
 * Mittels Prev koennen alle Wege rekonstruiert werden.
 */
int* dijkstra(ListDigraph &g, ListDigraph::ArcMap<int> &weight, ListDigraph::NodeMap<int> &label,
    int start, int* prev) {
    // initialisiere T
    ListDigraph::NodeMap<bool>  isNodeInT(g, true);
    int nodeCountT  = countNodes(g);

    // initialisiere d: Kosten von start
    int* d = new int[countNodes(g)+1];  // +1 damit max. index = label[maxNode]
    std::fill_n(d,countNodes(g)+1,INT_MAX);
    d[start] = 0;

    while(nodeCountT > 0) {
        ListDigraph::Node minWeightNode;
        int minCost = INT_MAX;

        // finde guenstigsten Ausgangspunkt in T
        for(ListDigraph::NodeIt n(g); n!=INVALID; ++n) {
            if(d[label[n]] < minCost &&
                isNodeInT[n]) {
                minWeightNode   = n;
                minCost         = d[label[minWeightNode]];
            }
        }

        isNodeInT[minWeightNode] = false;
        nodeCountT--;

        for(ListDigraph::ArcIt a(g); a!=INVALID; ++a) {
            if(g.source(a) == minWeightNode && // Kanten ausgehend von Ausgangspunkt
                d[label[g.source(a)]] + weight[a] < d[label[g.target(a)]]) {
                d[label[g.target(a)]]    = d[label[g.source(a)]] + weight[a];
                prev[label[g.target(a)]] = label[g.source(a)];
            }            
        }
    }
    
    return d;
}

int main() {
    for (int i = 1; i <= 6; i++) {
        ListDigraph g;
        ListDigraph::ArcMap<int>    weight(g);
        ListDigraph::NodeMap<int>   label(g);
        ListDigraph::ArcMap<bool>   path(g,false);

        digraphReader(g, "Data/Graph"+std::to_string(i)+".lgf"). // read the directed graph into g
            arcMap("weight", weight).
            nodeMap("label", label).
            run();

        std::cout << "lade Graph" << i << ".lgf" << std::endl;

        // init fuer prim
        ListDigraph::ArcMap<bool>   spanningTree(g,false);
        int totalWeight = prim(g,weight,spanningTree);
        std::cout << "Anzahl Knoten: " << countNodes(g) << "   Gew. des Spannbaums: " << totalWeight << std::endl;


        // init fuer dijkstra
        int prev[countNodes(g)+1];
        int start = 21;        
        int end = 6;
        
        int* shortestPathCosts = dijkstra(g, weight, label, start, prev);
        // std::cout << "Kosten von " << start << " nach " << end
        //     << ": " << shortestPathCosts[end] << std::endl << std::endl;
        delete[] shortestPathCosts;
    }
    
    return 1;
}