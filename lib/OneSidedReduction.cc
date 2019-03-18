#include "OneSidedReduction.h"
#include "Vertex.h"
#include "TDefs.h"
#include "Utils.h"
#include "PartnerList.h"
#include <vector>
#include <iostream>
#include <map>
#include <iterator>

std::unique_ptr<BipartiteGraph> one_sided_reduce_graph(
        const std::unique_ptr<BipartiteGraph>& G)
{
    BipartiteGraph::ContainerType A, B;

    // first add vertices from partition B
    for (auto it : G->get_B_partition()) {
        // vertex in partition B and their capacities remain unchanged
        auto v = it.second;
        auto u_id = v->get_id();
        auto u = std::make_shared<Vertex>(u_id,
                        v->get_lower_quota(), v->get_upper_quota());

        // add this vertex to partition B
        B.emplace(u_id, u);
    }

    // add vertices from partition A
    for (auto it : G->get_A_partition()) {
        // vertex in partition A and their capacities remain unchanged
        auto v = it.second;
        auto& v_pref_list = v->get_preference_list();
        auto u_id = v->get_id();
        auto u = std::make_shared<Vertex>(u_id,
                        v->get_lower_quota(), v->get_upper_quota());

        // add this vertex to partition A
        A.emplace(u_id, u);

        // Each vertex a in A has a dummy d_a in B
        const auto& dummy_id = get_dummy_id_one_side_ties(u_id);
        auto dummy = std::make_shared<Vertex>(dummy_id, u_id, 0, 1, true);

        // add this dummy to partition B
        B.emplace(dummy_id, dummy);

        auto& u_pref_list = u->get_preference_list();

        // add this clone to partition A with pref list a's pref list followed by d_a
        for (auto i = v_pref_list.all_begin(), e = v_pref_list.all_end();
             i != e; ++i)
        {
            auto neighbour = v_pref_list.get_vertex(*i);
            u_pref_list.emplace_back(B.at(neighbour->get_id()));
        }
        u_pref_list.emplace_back(dummy);

        // create a clone (a_*) for each a in A
        const auto& clone_id = get_clone_id_one_side_ties(u_id);
        auto clone = std::make_shared<Vertex>(clone_id, u_id, 0, 1, true);

        // add this clone to partition A with pref list d_a followed by a's pref list
        A.emplace(clone_id, clone);
        auto& clone_pref_list = clone->get_preference_list();
        clone_pref_list.emplace_back(dummy);
        for (auto i = v_pref_list.all_begin(), e = v_pref_list.all_end();
             i != e; ++i)
        {
            auto neighbour = v_pref_list.get_vertex(*i);
            clone_pref_list.emplace_back(B.at(neighbour->get_id()));
        }

        // add the preference list of d_a as a, a_*
        auto& dummy_pref_list = dummy->get_preference_list();
        dummy_pref_list.emplace_back(u);
        dummy_pref_list.emplace_back(clone);
    }

    // create the preferences for the vertices in partition B
    for(auto it : G->get_B_partition()){
        auto v = it.second;
        auto& v_pref_list = v->get_preference_list();

        auto vertex = B.at(v->get_id());
        auto& pref_list = vertex->get_preference_list();
        std::vector<VertexPtr> temp;
        for(auto i=v_pref_list.all_begin(), e=v_pref_list.all_end();
            i!=e; ++i){
            auto u = v_pref_list.get_vertex(*i);
            const auto& u_id = u->get_id();
            const auto& u_clone_id = get_clone_id_one_side_ties(u_id);
            pref_list.emplace_back(A.at(u_clone_id));
            if(i+1 != e){
                auto u_next = v_pref_list.get_vertex(*(i+1));
                if(v_pref_list.get_rank(*i) != v_pref_list.get_rank(*(i+1))){
                    for(auto temp_it = temp.begin(); temp_it != temp.end(); ++temp_it){
                        pref_list.emplace_back(*temp_it);
                    }
                    pref_list.emplace_back(A.at(u_id));
                    temp.clear();
                }
                else{
                    temp.push_back(A.at(u_id));
                }
            }
            else{
                for(auto temp_it = temp.begin(); temp_it != temp.end(); ++temp_it){
                    pref_list.emplace_back(*temp_it);
                }
                pref_list.emplace_back(A.at(u_id));
                temp.clear();                
            }
        }
    }

    return std::make_unique<BipartiteGraph>(A, B);
}

void map_back(const std::unique_ptr<BipartiteGraph>& G1, MatchedPairListType& M1,
                    const std::unique_ptr<BipartiteGraph>& G, MatchedPairListType& M)
{
    // print_graph(std::cout,G);
    // print_graph(std::cout,G1);
    // std::cout << "\n";
    // for(auto iter : M1){
    //     auto& v = iter.first;
    //     std::cout << v->get_id() << "\n";
    // }
    // print_matching(G1, M1, std::cout);
    // std::cout << "\n";
    auto& G_A = G->get_A_partition();
    auto& G_B = G->get_B_partition();
    for(auto a : G_A){
        auto& u = a.second;
        PartnerList temp;
        M.insert(std::pair<VertexPtr, PartnerList>(u,temp));
    }
    for(auto b : G_B){
        auto& u = b.second;
        PartnerList temp;
        M.insert(std::pair<VertexPtr, PartnerList>(u,temp));
    }
    // for(auto iter : M){
    //     auto& v = iter.first;
    //     std::cout << v->get_id() << "\n";
    // }
    // print_matching(G, M, std::cout);
    // std::cout << M.size() << "\n";
    for (auto it : G1->get_A_partition()) {
        auto u = it.second;
        auto M1_u = M1.find(u);     // vertex
        const auto& M1_u_id = it.first;  // vertex id
        IdType M1_u_id_orig;

        if (M1_u != M1.end()) {
            // std::cout << M1_u_id << " " << (M1_u->first)->get_id() << "\n";
            auto& M1_u_partners = M1_u->second;
            if(M1_u_partners.size() != 1){
                // std::cout << "Error! " << M1_u_partners.size() << "\n";
                continue;
            }
            if(is_clone(M1_u_id)){
                // std::cout << "clone\n";
                M1_u_id_orig = get_original(M1_u_id);
            }
            else{
                // std::cout << "not clone\n";
                M1_u_id_orig = M1_u_id;                
            }
            auto p1 = M1_u_partners.begin();
            auto v_p1 = M1_u_partners.get_vertex(p1);
            // std::cout << v_p1->get_id() << "\n";
            if(not is_dummy(v_p1->get_id())){
                // std::cout << "Not dummy\n" << M1_u_id_orig << "\n";
                auto x = G_A.at(M1_u_id_orig);
                auto y = G_B.at(v_p1->get_id());
                auto& x_partner_list = M[x];
                auto& y_partner_list = M[y];
                auto& x_pref_list = x->get_preference_list();
                auto& y_pref_list = y->get_preference_list();
                auto x_rank = y_pref_list.get_rank(y_pref_list.find(x));
                auto y_rank = x_pref_list.get_rank(x_pref_list.find(y));
                x_partner_list.add_partner(std::make_pair(y_rank, y));
                y_partner_list.add_partner(std::make_pair(x_rank, x));
            }
        }
    }
}