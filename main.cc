#include "GraphReader.h"
#include "BipartiteGraph.h"
#include "PartnerList.h"
#include "MatchingAlgorithm.h"
#include "StableMarriage.h"
#include "Popular.h"
#include "OneSidedReduction.h"
#include "RHeuristicHRLQ.h"
#include "HHeuristicHRLQ.h"
#include "Utils.h"
#include "Vertex.h"
#include <stdexcept>
#include <iostream>
#include <unistd.h>

template<typename T>
void compute_matching(bool A_proposing, const char* input_file, const char* output_file, bool st, bool dt) {
    GraphReader reader(input_file);
    std::unique_ptr<BipartiteGraph> G = reader.read_graph();
    // std::cout << "Original Graph: \n\n";
    // print_graph(std::cout,G);
    if(st == true){
        std::unique_ptr<BipartiteGraph> G1 = one_sided_reduce_graph(G);
        // std::cout << "\n";
        // std::cout << "Reduced Graph: \n\n";
        // print_graph(std::cout,G1);
        T alg(G1, A_proposing);
        if (alg.compute_matching()) {
            auto& M1 = alg.get_matched_pairs();
            // for(auto iter : M1){
            //     auto& v = iter.first;
            //     std::cout << v->get_id() << "\n";
            // }
            std::ofstream out(output_file);
            // print_matching(G1, M1, out);
            T alg1(G, A_proposing);
            // std::cout << "\nReduced Matching:\n\n";
            // print_matching(G1, M1, std::cout);
            auto &M = alg1.get_matched_pairs();
            // std::cout << M1.size() << " " << M.size() << "\n";
            // std::cout << "\nOriginal Matching:\n\n";
            map_back(G1,M1,G,M);
            // std::cout << M1.size() << " " << M.size() << "\n";
            print_matching(G, M, out);
            exchange_stability(M,G,true);
            exchange_stability(M,G,false);
            bool flg = consistency_check(M, G);
            if(flg == true)
                std::cout << "Unstable";
        } else {
            throw std::runtime_error("unable to compute matching.");
        }
    }
    else if(dt == true){
        std::cout << "Double Sided Ties\n";
    }
    else{
        T alg(G, A_proposing);
        if (alg.compute_matching()) {
            auto& M = alg.get_matched_pairs();
            std::ofstream out(output_file);
            print_matching(G, M, out);
        } else {
            throw std::runtime_error("unable to compute matching.");
        }        
    }
}

int main(int argc, char* argv[]) {
    int c = 0;
    bool compute_stable = false;
    bool compute_popular = false;
    bool compute_max_card = false;
    bool compute_rhrlq = false;
    bool compute_hhrlq = false;
    bool A_proposing = true;
    bool single_ties = false;
    bool double_ties = false;
    const char* input_file = nullptr;
    const char* output_file = nullptr;

    opterr = 0;
    // choose the proposing partition using -A and -B
    // -s, -p, and -m flags compute the stable, max-card popular and pop among
    // max-card matchings respectively
    // -r and -h compute the resident and hospital heuristic for an HRLQ instance
    // -i is the path to the input graph, -o is the path where the matching
    // computed should be stored
    while ((c = getopt(argc, argv, "ABspmrhtTi:o:")) != -1) {
        switch (c) {
            case 'A': A_proposing = true; break;
            case 'B': A_proposing = false; break;
            case 's': compute_stable = true; break;
            case 'p': compute_popular = true; break;
            case 'm': compute_max_card = true; break;
            case 'r': compute_rhrlq = true; break;
            case 'h': compute_hhrlq = true; break;
            case 't': single_ties = true; break;
            case 'T': double_ties = true; break;
            case 'i': input_file = optarg; break;
            case 'o': output_file = optarg; break;
            case '?':
                if (optopt == 'i') {
                    std::cerr << "Option -i requires an argument.\n";
                } else if (optopt == 'o') {
                    std::cerr << "Option -o requires an argument.\n";
                } else {
                    std::cerr << "Unknown option: " << optopt << '\n';
                }
                break;
            default: break;
        }
    }

    // std::cout << "Main Program\n";

    if (not input_file or not output_file) {
        // do not proceed if file names are not valid
    } else if (compute_stable) {
        compute_matching<StableMarriage>(A_proposing, input_file, output_file, single_ties, double_ties);
    } else if (compute_popular) {
        compute_matching<MaxCardPopular>(A_proposing, input_file, output_file, single_ties, double_ties);
    } else if (compute_max_card) {
        compute_matching<PopularAmongMaxCard>(A_proposing, input_file, output_file, single_ties, double_ties);
    } else if (compute_rhrlq) {
        compute_matching<RHeuristicHRLQ>(A_proposing, input_file, output_file, single_ties, double_ties);
    } else if (compute_hhrlq) {
        compute_matching<HHeuristicHRLQ>(A_proposing, input_file, output_file, single_ties, double_ties);
    }

    return 0;
}
