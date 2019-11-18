#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/lgf_reader.h>
#include <fstream>
#include <string>
#include <vector>
#include<bits/stdc++.h> 


using namespace lemon;
using std::vector;


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

/**
 * Aufgabe 3:  Laufzeit O(n^3)
 * 
 * Idee: Erzeuge einen neuen Graph mit den Knoten die verbunden werden sollen
 * und verbinde die Knoten, fuer die in Dijkstra ein Weg gefunden wurde.
 * Setze als Kosten der Kante die Kosten des Pfades aus Dijkstra.
 * 
 * Suche nun den mimalen Spannbaum auf diesem Graph. Ersetze die Kanten
 * des Spannbaums durch die Knoten und Kanten des zugehörigen Pfades aus Dijkstra.
 */
int heuristik(ListDigraph &g, ListDigraph::ArcMap<int> &weight, ListDigraph::NodeMap<int> &label,
    vector<int> includingNodes, ListDigraph::ArcMap<bool> &subGraphArcs,
    ListDigraph::NodeMap<bool> &subGraphNodes) {
    
    vector<int*> costFromTo(countNodes(g)+1);
    vector<int*> prevFromTo(countNodes(g)+1);
    for(int*& prev: prevFromTo)
        prev = new int[countNodes(g)+1];

    for(int &i : includingNodes) {
        costFromTo[i] = dijkstra(g, weight, label, i, prevFromTo[i]);
    }
        
    // Ausgabe der Kosten zwischen den Knoten
    for(int &i: includingNodes) {
        std::cout << i << ": ";
        for( int &j: includingNodes) {
            if(costFromTo[i][j] == INT_MAX)
                std::cout << "inf" << " ";
            else
                std::cout << costFromTo[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // TO-DO:    rekursiv Pfade aus Dijkstra bestimmen
    //         und neuen Graph erstellen. Darauf Spannbaum anwenden
    //         und in Graph mit original Knoten & Kanten tranformieren

    for(int* prev: prevFromTo)
        delete[] prev;
}

int main() {
    for (int i = 1; i <= 6; i++) {
        ListDigraph g;
        ListDigraph::ArcMap<int>    weight(g);
        ListDigraph::NodeMap<int>   label(g);
        ListDigraph::ArcMap<bool>   path(g,false);
        ListDigraph::ArcMap<bool>   subgraphArcs(g,false);
        ListDigraph::NodeMap<bool>  subgraphNodes(g,false);

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

        std::ifstream ifs;
        ifs.open("Data/Graph"+std::to_string(i)+"_Terminals.txt");        
        vector<int> points;
        for(std::string cur; std::getline(ifs, cur); points.push_back(std::stoi(cur)));
        ifs.close();

        if(i==1) {
            std::cout << "Ausgabe der min. Kosten zwischen gesuchten Knoten" << std::endl;
            heuristik(g, weight, label, points, subgraphArcs, subgraphNodes);
        }

    }
    
    return 1;
}