#pragma once

#include "search.h"

namespace LTPuyo
{

class Decision
{
public:
    static void decide(Field field, SearchInfo& search_info, SearchCandidate& result);
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

    for (int i = 0; i < iter; ++i)
    {
        vector<pair<Puyo, Puyo>> tqueue;
        tqueue.push_back(queue[(i + 0) % iter]);
        tqueue.push_back(queue[(i + 1) % iter]);
        tqueue.push_back(queue[(i + 2) % iter]);

        ++i;

        SearchInfo sinfo;

        auto time_1 = chrono::high_resolution_clock::now();
        Search::search(field, tqueue, sinfo);
        auto time_2 = chrono::high_resolution_clock::now();

        node_count += sinfo.node;
        time += chrono::duration_cast<chrono::milliseconds>(time_2 - time_1).count();

        SearchCandidate scan;

        Decision::decide(field, sinfo, scan);

        Puyo pair[2] = { tqueue[0].first, tqueue[0].second };
        field.drop_pair(scan.placement.x, pair, scan.placement.rotation);

        Chain chain;
        field.pop(chain);
    }

    cout << "nps: " << (node_count / time) << "knode/s" << endl;
};

};