#include "PreferenceList.h"
#include "Vertex.h"
#include <algorithm>
#include <sstream>
#include <iostream>

PreferenceList::PreferenceList()
    : start_iter_(0), end_iter_(0), cur_rank_(0), start_iter_orig_(0)
{}

PreferenceList::PreferenceList(const PreferenceList& that) {
    cur_rank_ = 0;//that.cur_rank_;
    pref_list_ = that.pref_list_;
    start_iter_ = 0;
    start_iter_orig_ = 0;
    end_iter_ = pref_list_.size();
}

PreferenceList::~PreferenceList() {
    clear();
}

PreferenceList::SizeType PreferenceList::begin() {
    return start_iter_;
}

PreferenceList::SizeType PreferenceList::end() {
    return end_iter_;
}

PreferenceList::Iterator PreferenceList::all_begin() {
    return pref_list_.begin();
}

PreferenceList::Iterator PreferenceList::all_end() {
    return pref_list_.end();
}

/// clear the list
void PreferenceList::clear() {
    pref_list_.clear();
    start_iter_ = 0;
    end_iter_ = 0;
}

/// is the preference list empty
bool PreferenceList::empty() {
    return size() == 0;
}

/// is the preference list empty
PreferenceList::SizeType PreferenceList::size() {
    return end() - begin();
}

/// insert element at end
void PreferenceList::emplace_back_ties(VertexPtr v, bool same_rank) {
    if(not same_rank){
        ++cur_rank_;
    }
    pref_list_.emplace_back(cur_rank_, v);

    // end_iter_ always points one past the end of preference list
    end_iter_ = pref_list_.size();
}

void PreferenceList::emplace_back(VertexPtr v) {
    pref_list_.emplace_back(++cur_rank_, v);

    // end_iter_ always points one past the end of preference list
    end_iter_ = pref_list_.size();
}
/// find the vertex in the container
PreferenceList::SizeType PreferenceList::find(VertexPtr v) {
    for (SizeType i = start_iter_orig_, e = end(); i != e; ++i) {
        if (get_vertex(i) == v) {
            return i;
        }
    }

    return end();
}

VertexPtr PreferenceList::get_vertex(const ElementType& e) {
    return e.second;
}

RankType PreferenceList::get_rank(const ElementType& e) {
    return e.first;
}

VertexPtr PreferenceList::get_vertex(SizeType index) {
    return get_vertex(pref_list_.at(index));
}

RankType PreferenceList::get_rank(SizeType index) {
    return (index < end()) ? get_rank(pref_list_.at(index)) : RANK_INFINITY ;
}

/// return the index of the vertex that this vertex will now propose to
PreferenceList::SizeType PreferenceList::get_proposal_index() {
    return begin();
}

/// does this vertex prefer a to b
bool PreferenceList::is_ranked_better(VertexPtr a, VertexPtr b) {
    return get_rank(find(a)) < get_rank(find(b));
}

bool PreferenceList::is_ranked_better_ties(VertexPtr a, VertexPtr b) {
    // RankType ra = get_rank(find(a));
    // RankType rb = get_rank(find(b));
    // std::cout << ra << " " << rb << "\n";
    if(get_rank(find(a)) > get_rank(find(b)))
        return false;
    return true;    
}


/// restrict the preference list [begin(), pref_list_[v]]
void PreferenceList::restrict_preferences(VertexPtr v) {
    auto index = find(v);
    end_iter_ = (index == end()) ? index : (index + 1);
}

/// remove the first vertex from the preference list
void PreferenceList::move_proposal_index() {
     if (begin() < end()) {
        ++start_iter_;
     }
}

std::ostream& operator<<(std::ostream& out, PreferenceList& pl) {
    return out << &pl;
}

std::ostream& operator<<(std::ostream& out, PreferenceList* pl) {
    std::stringstream stmp;
    int flag = 0;

    for (PreferenceList::SizeType i = pl->begin(), e = pl->end();
         i != e; ++i)
    {   //stmp << pl->get_rank(pl->pref_list_.at(i)) << " ";
        if(i+1 != e){            
            if(pl->get_rank(pl->pref_list_.at(i)) == pl->get_rank(pl->pref_list_.at(i+1))){
                if(flag == 0){
                    stmp << "[";
                    flag = 1;                    
                }
                stmp << pl->get_vertex(pl->pref_list_.at(i))->get_id();
            }
            else{
                stmp << pl->get_vertex(pl->pref_list_.at(i))->get_id();
                if(flag == 1){
                    stmp << "]";
                    flag = 0;
                }
            }
        }

        if (i+1 == e) {
           stmp << pl->get_vertex(pl->pref_list_.at(i))->get_id();
           if(flag == 1){stmp << "]";}
           stmp << ";";
        } else {
            stmp << ", ";
        }
    }

    return out << stmp.str();
}

