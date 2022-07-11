#pragma once

#include "search.h"
#include "path.h"

namespace LTPuyo
{

struct DecisionFactor
{
    int trigger_point = 18;
    int trigger_point_enemy = 18;
    int trigger_point_harass = 36;
    int max_harass_chain = 2;
};

class Decision
{
public:
    static void decide(Field field, SearchInfo& search_info, SearchCandidate& result);
    static void classify_candidate(SearchInfo& search_info, avec<SearchCandidate, 22>& have_chain, avec<SearchCandidate, 22>& have_nchain, avec<SearchCandidate, 22>& have_neval);
public:
    static void action_build(avec<SearchCandidate, 22>& have_chain, avec<SearchCandidate, 22>& have_nchain, avec<SearchCandidate, 22>& have_neval, SearchCandidate& result);
    static void action_execute_biggest_chain(avec<SearchCandidate, 22>& have_chain, avec<SearchCandidate, 22>& have_nchain, avec<SearchCandidate, 22>& have_neval, SearchCandidate& result);
};

static void bench_search(int iter)
{
    using namespace std;

    int64_t node_count = 0;
    int64_t time = 0;

    vector<pair<Puyo, Puyo>> queue;
    for (int i = 0; i < iter; ++i) {
        queue.push_back({ Puyo(rand() % 4), Puyo(rand() % 4) });
    }

    Field field;

    double chain_total = 0;
    double chain_count = 0;

    for (int i = 0; i < iter; ++i)
    {
        vector<pair<Puyo, Puyo>> tqueue;
        tqueue.push_back(queue[(i + 0) % iter]);
        tqueue.push_back(queue[(i + 1) % iter]);
        tqueue.push_back(queue[(i + 2) % iter]);

        SearchInfo sinfo;

        auto time_1 = chrono::high_resolution_clock::now();
        Search::search(field, tqueue, sinfo, 18, 0);
        auto time_2 = chrono::high_resolution_clock::now();

        node_count += sinfo.node;
        time += chrono::duration_cast<chrono::milliseconds>(time_2 - time_1).count();

        SearchCandidate scan;

        Decision::decide(field, sinfo, scan);

        Puyo pair[2] = { tqueue[0].first, tqueue[0].second };
        field.drop_pair(scan.placement.x, pair, scan.placement.rotation);

        Chain chain;
        field.pop(chain);

        if (chain.count > 0) {
            chain_total += chain.count;
            chain_count += 1;
        }
    }

    cout << "nps:   " << (node_count / time) << " knode/s" << endl;
    cout << "atime: " << (time / iter) << " ms" << endl;
    if (chain_count > 0) {
        cout << "chain: " << (chain_total / chain_count) << endl;
    }
};

};