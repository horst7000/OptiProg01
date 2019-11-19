#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/lgf_reader.h>
#include <fstream>
#include <string>
#include <vector>
#include<bits/stdc++.h> // fuer INT_MAX
#include <chrono>  // fuer Laufzeiterfassung


using namespace std::chrono; 
using namespace lemon;
using std::vector;

struct Node {
    int label, id;
};


/**
 * Aufgabe1:  Laufzeit O(n^3)
 * Gibt Gewicht des Spannbaums zurueck. Uebergebene ArcMap "SpanningTree" enthaelt nach
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
        // oder v in X und u nicht in X
        for(ListDigraph::ArcIt a(g); a!=INVALID; ++a) {
            if(
                weight[a] < minCost &&
                ( isNodeInX[g.source(a)] && !isNodeInX[g.target(a)] ||
                isNodeInX[g.target(a)] && !isNodeInX[g.source(a)] )
            ) {
                minWeightArc = a;
                minCost = weight[minWeightArc];
            }
        }

        spanningTree[minWeightArc] = true;
        totalWeight += weight[minWeightArc];
        
        // fuege v bzw. u zu X hinzu
        if(isNodeInX[g.source(minWeightArc)])
            isNodeInX[g.target(minWeightArc)] = true;
        else
            isNodeInX[g.source(minWeightArc)] = true;
        nodeCountX++;
    }

    return totalWeight;
}

/**
 * Aufgabe2:  Laufzeit O(n^3)
 * Gibt minimale Kosten von start zu allen Knoten zurueck.
 * Mittels Array "prev" koennen alle Wege rekonstruiert werden.
 */
int* dijkstra(ListDigraph &g, bool directed, ListDigraph::ArcMap<int> &weight, ListDigraph::NodeMap<int> &label,
    int startId, int* prev) {
    // initialisiere T
    ListDigraph::NodeMap<bool>  isNodeInT(g, true);
    int nodeCountT  = countNodes(g);

    // initialisiere d: Kosten von start
    int* d = new int[countNodes(g)];
    std::fill_n(d,countNodes(g),INT_MAX);
    d[startId] = 0;

    while(nodeCountT > 0) {
        ListDigraph::Node minWeightNode;
        int minCost = INT_MAX;

        // finde guenstigsten Ausgangspunkt in T
        for(ListDigraph::NodeIt n(g); n!=INVALID; ++n) {
            if(d[g.id(n)] < minCost &&
                isNodeInT[n]) {
                minWeightNode   = n;
                minCost         = d[g.id(minWeightNode)];
            }
        }

        isNodeInT[minWeightNode] = false;
        nodeCountT--;

        for(ListDigraph::ArcIt a(g); a!=INVALID; ++a) {
            if(g.source(a) == minWeightNode && // Kanten ausgehend von Ausgangspunkt
                d[g.id(g.source(a))] + weight[a] < d[g.id(g.target(a))]) {
                d[g.id(g.target(a))]    = d[g.id(g.source(a))] + weight[a];
                prev[g.id(g.target(a))] = g.id(g.source(a));
            } // Falls Graph als ungerichtet betrachtet wird:
            else if(!directed && g.target(a) == minWeightNode && 
                d[g.id(g.target(a))] + weight[a] < d[g.id(g.source(a))]) {
                d[g.id(g.source(a))]    = d[g.id(g.target(a))] + weight[a];
                prev[g.id(g.source(a))] = g.id(g.target(a));
            }          
        }
    }
    
    return d;
}

/**
 * Aufgabe 3:  Laufzeit O(n^3 * |T|)  wenn T die Menge der gesuchten Knoten ist
 * 
 * Idee: Erzeuge einen neuen Graph mit den Knoten die verbunden werden sollen
 * und verbinde die Knoten, fuer die in Dijkstra ein Weg gefunden wurde.
 * Setze als Kosten der Kante die Kosten des Pfades aus Dijkstra.
 * 
 * Suche nun den mimalen Spannbaum auf diesem Graph. Ersetze die Kanten
 * des Spannbaums durch die Knoten und Kanten des zugehörigen Pfades aus Dijkstra.
 */
int heuristik(ListDigraph &g, ListDigraph::ArcMap<int> &weight, ListDigraph::NodeMap<int> &label,
    vector<Node> nodesToConnect, ListDigraph::ArcMap<bool> &subGraphArcs,
    ListDigraph::NodeMap<bool> &subGraphNodes) {
        
    // initialisiere Kosten und Pfade fuer Dijkstra
    vector<int*> costFromTo(nodesToConnect.size());  // Indizes:  [index in nodesToConnect][id Zielknoten]
    vector<int*> prevFromTo(nodesToConnect.size());  // also Bsp.: costFromTo[3,4] entspricht Kosten von
                                                         // nodesToConnect[3].label  nach Knoten mit id 4
    for(int*& prev: prevFromTo)
        prev = new int[countNodes(g)]();

    // Wende Dijkstra auf gesuchte Knoten an
    std::cout << "... wende Dijsktra "<< nodesToConnect.size() << " mal auf gesuchte Knoten an ... " << std::endl;
    for (size_t i = 0; i < nodesToConnect.size(); i++) {
        int start = nodesToConnect[i].id;
        // std::cout << "dikstra " << i+1 << " / " << nodesToConnect.size() << std::endl;
        costFromTo[i] = dijkstra(g, false, weight, label, start, prevFromTo[i]);
    }
    
    /*
    // Ausgabe der Kosten zwischen den Knoten
    for(size_t i = 0; i < nodesToConnect.size(); i++) {
        std::cout << nodesToConnect[i].label << ": ";
        for( size_t j = 0; j < nodesToConnect.size(); j++) {
            if(costFromTo[i][nodesToConnect[j].id] == INT_MAX)
                std::cout << "inf" << " ";
            else
                std::cout << costFromTo[i][nodesToConnect[j].id] << " ";
        }
        std::cout << std::endl;
    }
    // */
    
    // erstelle neuen Graph
    ListDigraph h;
    ListDigraph::NodeMap<int> labelH(h);
    ListDigraph::ArcMap<int> weightH(h);

    // fuege alle Knoten hinzu
    for (size_t i = 0; i < nodesToConnect.size(); i++) {
        ListDigraph::Node u = h.addNode();
        labelH[u] = nodesToConnect[i].label;
    }
    // fuege alle Kanten hinzu
    for (size_t i = 0; i < nodesToConnect.size(); i++) {
        ListDigraph::Node u = mapFind(h,labelH,nodesToConnect[i].label);
        for (Node &v : nodesToConnect) {
            if(costFromTo[i][v.id] != INT_MAX && labelH[u] != v.label) {
                ListDigraph::Node vInH = mapFind(h,labelH,v.label);
                ListDigraph::Arc arc = h.addArc(u, vInH);
                weightH[arc] = costFromTo[i][v.id];
            }
        }
    }
    

    ListDigraph::ArcMap<bool> spanningTree(h,false);
    int heuristikWeight = prim(h, weightH, spanningTree);

    /*
    std::cout << std::endl << "Spannbaum von h:" << std::endl;
    for (ListDigraph::ArcIt a(h); a!=INVALID; ++a) {
        if(spanningTree[a])
            std::cout << labelH[h.source(a)] << " " << labelH[h.target(a)] << std::endl;
    }
    // */

    // fuelle  ArcMap<bool> subGraphArcs,   NodeMap<bool> subGraphNodes

    for (ListDigraph::ArcIt a(h); a!=INVALID; ++a) {
        if(spanningTree[a]) {
            int sId = g.id(mapFind(g,label,labelH[h.source(a)]));
            int tId = g.id(mapFind(g,label,labelH[h.target(a)]));
            
            int sIdInNodesToCon = 0;
            int tIdInNodesToCon = 0;
            // prevFromTo[][] benoetigt Index der Knoten in nodesToConnect Vector
            for (size_t i = 0; i < nodesToConnect.size(); i++) {
                if(nodesToConnect[i].id == sId)
                    sIdInNodesToCon = i;
                if(nodesToConnect[i].id == tId)
                    tIdInNodesToCon = i;
            }
            
            int v = tId;
            int vOld = v;
            subGraphNodes[g.nodeFromId(v)] = true;
            while(prevFromTo[sIdInNodesToCon][v] != 0) {
                // Knoten entlang des Dijkstra-Pfades zu subGraphNodes hinzufuegen
                vOld = v;
                v    = prevFromTo[sIdInNodesToCon][v];
                subGraphNodes[g.nodeFromId(v)] = true;

                // Kanten entlang des Dijkstra-Pfades zu subGraphNodes hinzufuegen
                ListDigraph::Arc arc = findArc(g,g.nodeFromId(vOld), g.nodeFromId(v));
                if(arc == INVALID) 
                                 arc = findArc(g,g.nodeFromId(v), g.nodeFromId(vOld));

                subGraphArcs[arc]    = true;
            }
        }
    }
    

    for(int*& prev: prevFromTo)
        delete[] prev;
    for(int*& costs: costFromTo)
        delete[] costs;

    return heuristikWeight;
}

vector<int> readTerminalsTxtFile(int graphNr, int terminalsNr = 0) {
    // lese Datei
    std::ifstream ifs;
    if(terminalsNr == 0)
        ifs.open("Data/Graph"+std::to_string(graphNr)+"_Terminals.txt");
    else
        ifs.open("Data/Graph"+std::to_string(graphNr)+"_Terminals"
            +std::to_string(terminalsNr)+".txt");
    vector<int> labelsToConnect;
    for(std::string cur; std::getline(ifs, cur); labelsToConnect.push_back(std::stoi(cur)));
    ifs.close();
    return labelsToConnect;
}

vector<Node> nodeVectorFromLabels(vector<int> labelsToConnect) {
    vector<Node> nodesToConnect;
    for(int &label : labelsToConnect) {
        Node n = {label, label-1}; // hier: id ist label-1
        nodesToConnect.push_back(n);
    }
    return nodesToConnect;
}

void outputHeuristikCostBound( ListDigraph &g, ListDigraph::ArcMap<int> &weight,
    ListDigraph::NodeMap<int> &label, vector<Node> nodesToConnect, ListDigraph::ArcMap<bool> &subGraphArcs,
    ListDigraph::NodeMap<bool> &subGraphNodes ) {
    
    auto start = high_resolution_clock::now(); 
    int heurWeight = heuristik(g, weight, label, nodesToConnect, subGraphArcs, subGraphNodes);
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start); 
    std::cout << "Kostenschranke fuer Teilgraph mit " << nodesToConnect.size() << " gesuchten Knoten: "
        << heurWeight << "  (" << duration.count() <<  "ms)" << std::endl;
}

void outputSmallSubgraph( ListDigraph &g, ListDigraph::ArcMap<bool> &subGraphArcs,
    ListDigraph::NodeMap<bool> &subGraphNodes ) {
    
    std::cout << std::endl << "Moeglichst kleiner Teilgraph von G mit gesuchten Knoten:" << std::endl;
    std::cout << "nodes" << std::endl;
    for (ListDigraph::NodeIt u(g); u!=INVALID; ++u) {
        if(subGraphNodes[u])
            std::cout << g.id(u)+1 << " " << std::endl;
    }
    std::cout << std::endl << "edges (ohne Gewicht)" << std::endl;
    for (ListDigraph::ArcIt a(g); a!=INVALID; ++a) {
        if(subGraphArcs[a])
            std::cout << g.id(g.source(a))+1 << " " << g.id(g.target(a))+1 << std::endl;
    }

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

        std::cout << ":::::::::: lade Graph" << i << ".lgf  ::::::::::" << std::endl;
        std::cout << "Anzahl Knoten: " << countNodes(g) << std::endl;

        // init fuer prim
        ListDigraph::ArcMap<bool>   spanningTree(g,false);
        auto start = high_resolution_clock::now();
        int totalWeight = prim(g,weight,spanningTree);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        std::cout << "Gew. des min. Spannbaums: " << totalWeight
            << "  (" << duration.count() <<  "ms)" << std::endl;


        // Berechnung und Ausgabe der Heuristik
        vector<int> labelsToConnect;
        vector<Node> nodesToConnect;
        if(i == 4 || i == 5) {
            labelsToConnect = readTerminalsTxtFile(i,1);
            nodesToConnect = nodeVectorFromLabels(labelsToConnect);
            outputHeuristikCostBound(g, weight, label, nodesToConnect, subgraphArcs, subgraphNodes);
            
            labelsToConnect = readTerminalsTxtFile(i,2);
            nodesToConnect = nodeVectorFromLabels(labelsToConnect);
            outputHeuristikCostBound(g, weight, label, nodesToConnect, subgraphArcs, subgraphNodes);
            
            labelsToConnect = readTerminalsTxtFile(i,3);
            nodesToConnect = nodeVectorFromLabels(labelsToConnect);
            outputHeuristikCostBound(g, weight, label, nodesToConnect, subgraphArcs, subgraphNodes);            
        } else {
            labelsToConnect = readTerminalsTxtFile(i);
            nodesToConnect = nodeVectorFromLabels(labelsToConnect);
            outputHeuristikCostBound(g, weight, label, nodesToConnect, subgraphArcs, subgraphNodes);
        }
        
        if(i == 4) {
            labelsToConnect = readTerminalsTxtFile(4,4);
            nodesToConnect = nodeVectorFromLabels(labelsToConnect);
            outputHeuristikCostBound(g, weight, label, nodesToConnect, subgraphArcs, subgraphNodes);
        }

        if(i==1) {
            outputSmallSubgraph(g, subgraphArcs, subgraphNodes);
        }

        std::cout << std::endl << std::endl;
    }
    

    return 1;
}