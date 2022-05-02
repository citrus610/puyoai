#include <iostream>
#include "../bot/decision.h"

int main()
{
    using namespace LTPuyo;
    using namespace std;

    srand(uint32_t(time(NULL)));

    vector<pair<Puyo, Puyo>> queue;
    for (int i = 0; i < 1000; ++i) {
        queue.push_back({ Puyo(rand() % 4), Puyo(rand() % 4) });
    }

    Field field;
    field.print();
    cin.get();
    system("cls");

    for (int i = 0; i + 2 < 1000; ++i) {
        vector<pair<Puyo, Puyo>> tqueue;
        tqueue.push_back(queue[i]);
        tqueue.push_back(queue[i + 1]);
        tqueue.push_back(queue[i + 2]);

        SearchInfo sinfo;

        Search::search(field, tqueue, sinfo);

        cout << "search done" << endl;
        cout << "node: " << sinfo.node << endl;

        SearchCandidate scan;

        Decision::decide(field, sinfo, scan);

        cout << "decide done" << endl;

        Puyo pair[2] = { tqueue[0].first, tqueue[0].second };

        field.drop_pair(scan.placement.x, pair, scan.placement.rotation);

        field.print();
        cin.get();
        system("cls");

        Chain chain;
        field.pop(chain);
        if (chain.count > 0) {
            field.print();
            cin.get();
            system("cls");
        }
    }

    return 0;
};