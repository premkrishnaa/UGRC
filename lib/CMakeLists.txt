cmake_minimum_required(VERSION 2.8)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(graph
                MatchingAlgorithm.cc StableMarriage.cc Popular.cc
                SReduction.cc RHeuristicHRLQ.cc HHeuristicHRLQ.cc
                BipartiteGraph.cc PartnerList.cc PreferenceList.cc Vertex.cc
                GraphReader.cc Utils.cc OneSidedReduction.cc)
