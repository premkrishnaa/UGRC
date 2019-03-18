#include "Utils.h"
#include "Vertex.h"
#include "PartnerList.h"
#include <set>
#include <iostream>
#include <sstream>
#include <stack>

// a new id is of the form id^k
IdType get_vertex_id(const IdType& id, int k) {
    std::stringstream stmp;
    stmp << id << '^' << k;
    return stmp.str();
}

int get_vertex_level(const IdType& id) {
    return std::atoi(id.substr(id.find('^')+1).c_str());
}

// a dummy id is of the form d^k_id
IdType get_dummy_id(const IdType& id, int k) {
    std::stringstream stmp;
    stmp << "d^" << k << '_' << id;
    return stmp.str();
}

IdType get_dummy_id_one_side_ties(const IdType& id) {
    std::stringstream stmp;
    stmp << "d" << '_' << id;
    return stmp.str();
}

IdType get_clone_id_one_side_ties(const IdType& id) {
    std::stringstream stmp;
    stmp << id << "_*";
    return stmp.str();
}

IdType get_original(const IdType& id) {
    return (id.substr(0,id.length()-2));
}

bool is_clone(const IdType& id) {
    return (id.substr(id.length()-2) == "_*");
}

bool is_dummy(const IdType& id) {
    return (id.substr(0,2) == "d_");
}

// return the dummy level from the given id
int get_dummy_level(const IdType& id) {
    auto caret_pos = id.find('^');
    auto underscore_pos = id.find('_');
    return std::atoi(id.substr(caret_pos+1, underscore_pos-caret_pos-1).c_str());
}

void print_matching(const std::unique_ptr<BipartiteGraph>& G,
                    MatchedPairListType& M, std::ostream& out)
{
    std::stringstream stmp;
    std::set<VertexPtr> printed;
    for (auto it : G->get_A_partition()) {
        auto u = it.second;
        auto M_u = M.find(u);

        if (M_u != M.end()) {
            auto& partners = M_u->second;

            for (auto pit = partners.cbegin(), pie = partners.cend(); pit != pie; ++pit) {
                auto v = partners.get_vertex(pit);
                printed.emplace(v);

                stmp << u->get_id() << ','
                     << v->get_id() << ','
                     << partners.get_rank(pit) << '\n';
            }
        }
    }

#if 0
    for (auto it : G->get_B_partition()) {
        auto u = it.second;
        auto M_u = M.find(u);

        if (M_u != M.end()) {
            auto& partners = M_u->second;

            for (auto pit = partners.cbegin(), pie = partners.cend(); pit != pie; ++pit) {
                auto v = partners.get_vertex(pit);
                printed.emplace(v);

                stmp << u->get_id() << ','
                     << v->get_id() << ','
                     << partners.get_rank(pit) << '\n';
            }
        }
    }
#endif

    out << stmp.str();
}


void exchange_stability(MatchedPairListType M_, const std::unique_ptr<BipartiteGraph>& G, bool A_proposing_) {
    std::cout << "Exchange Stability ";
    if(A_proposing_)
        std::cout << "Men\n";
    else
        std::cout << "Women\n";

    // const std::unique_ptr<BipartiteGraph>& G = get_graph();

    const auto& proposing_partition = A_proposing_ ? G->get_A_partition()
                                                   : G->get_B_partition();
    std::stack<VertexPtr> list1;
    std::stack<VertexPtr> list2;
    std::map<IdType,IdType> exchange_map;

    for (auto it : proposing_partition) {
        list1.push(it.second);
        list2.push(it.second);
    }

    while (not list1.empty()) {
        auto a1 = list1.top();
        auto& a1_pref_list = a1->get_preference_list();
        auto& a1_partner_list = M_[a1];
        list1.pop();
        while (not list2.empty()) {
            auto a2 = list2.top();
            auto& a2_pref_list = a2->get_preference_list();
            auto& a2_partner_list = M_[a2];
            list2.pop();
            // std::cout << "(" << a1->get_id() << "," << a2->get_id() << ")\n";
            if(a1_partner_list.size() != 1 || a2_partner_list.size() != 1){
                // std::cout << "Size Mismatch\n";
                continue;
            }
            auto p1 = a1_partner_list.get_vertex(a1_partner_list.cbegin());
            auto p2 = a2_partner_list.get_vertex(a2_partner_list.cbegin());
            if(a1->get_id() == a2->get_id())
                continue;
            if(a1_pref_list.find(p2) == a1_pref_list.end() || a2_pref_list.find(p1) == a2_pref_list.end()){
                // std::cout << "Element Mismatch\n";
            }
            else{
                if(a1_pref_list.is_ranked_better(p2,p1) && a2_pref_list.is_ranked_better(p1,p2)){
                    // std::cout << "Blocking: (" << a1->get_id() << "," << a2->get_id() << ")\n";
                    std::map<IdType,IdType>::iterator itr;
                    IdType a1_ID = a1->get_id(), a2_ID = a2->get_id();
                    itr = exchange_map.find(a2_ID);
                    if(itr == exchange_map.end()){
                        exchange_map.insert(std::pair<IdType,IdType>(a1_ID,a2_ID));
                    }
                    else if(itr->second != a1_ID){
                        exchange_map.insert(std::pair<IdType,IdType>(a1_ID,a2_ID));                  
                    }
                }
            }
        }

        for (auto it : proposing_partition) {
            list2.push(it.second);
        }

    }
    std::cout << "Total blocking pairs: " << exchange_map.size() << "\n";
    // for(auto it = exchange_map.begin(),et = exchange_map.end(); it!=et; it++){
    //     std::cout << "(" << it->first << "," << it->second << ")\n";
    // }
}

bool consistency_check(MatchedPairListType M_, const std::unique_ptr<BipartiteGraph>& G) {
    std::cout << "consistency_check \n";

    bool flag = false;
    const auto& proposing_partition =  G->get_A_partition();
                                                   
    std::stack<VertexPtr> list1;

    for (auto it : proposing_partition) {
        list1.push(it.second);
    }
    // std::cout << "size: " << list1.size() << "\n";
    while (not list1.empty()) {
        auto a1 = list1.top();
        auto& a1_pref_list = a1->get_preference_list();
        auto& a1_partner_list = M_[a1];
        list1.pop();
        if(a1_partner_list.size() == 0){
            for(auto b1 = a1_pref_list.all_begin(), b2 = a1_pref_list.all_end(); b1 != b2 ; b1++){
                auto b1_vertex = a1_pref_list.get_vertex(*b1);
                auto& b1_pref_list = b1_vertex->get_preference_list();
                auto& b1_partner_list = M_[b1_vertex];
                if(b1_partner_list.size() == 0)
                    flag = true;
                else if(b1_pref_list.is_ranked_better(a1,b1_partner_list.get_vertex(b1_partner_list.cbegin())))
                    flag = true;
            }
        }
        else{
            for(auto b1 = a1_pref_list.all_begin(), b2 = a1_pref_list.all_end(); b1 != b2 ; b1++){
                auto b1_vertex = a1_pref_list.get_vertex(*b1);
                auto& b1_pref_list = b1_vertex->get_preference_list();
                auto& b1_partner_list = M_[b1_vertex];
                if(a1_pref_list.is_ranked_better(b1_vertex,a1_partner_list.get_vertex(a1_partner_list.cbegin()))){
                    if(b1_partner_list.size() == 0)
                        flag = true;
                    else if(b1_pref_list.is_ranked_better(a1,b1_partner_list.get_vertex(b1_partner_list.cbegin())))
                        flag = true;
                }
            }
        }
    }

    return flag;
}
