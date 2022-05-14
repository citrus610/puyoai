#include <iostream>
#include "../bot/decision.h"
#include "render.h"
#include "data.h"

Color pchar_to_color(char c)
{
    switch (c)
    {
    case 'R':
        return COLOR_FG_DARK_RED;
    case 'Y':
        return COLOR_FG_DARK_YELLOW;
    case 'G':
        return COLOR_FG_DARK_GREEN;
    case 'B':
        return COLOR_FG_DARK_BLUE;
    case '#':
        return COLOR_FG_GREY;
    default:
        return COLOR_FG_BLACK;
    }
    return COLOR_FG_BLACK;
};

std::vector<LTPuyo::Pair> create_queue()
{
    using namespace LTPuyo;
    using namespace std;

    vector<Puyo> bag;
    bag.reserve(256);

    for (int i = 0; i < 64; ++i) {
        for (uint8_t p = 0; p < Puyo::COUNT - 1; ++p) {
            bag.push_back(Puyo(p));
        }
    }

    for (int t = 0; t < 4; ++t) {
        for (int i = 0; i < 256; ++i) {
            int k = rand() % 256;
            Puyo value = bag[i];
            bag[i] = bag[k];
            bag[k] = value;
        }
    }

    vector<Pair> queue;
    queue.reserve(128);

    for (int i = 0; i < 128; ++i) {
        queue.push_back({ bag[i * 2], bag[i * 2 + 1] });
    }

    return queue;
};

void render_field(LTPuyo::Field field)
{
    using namespace LTPuyo;
    using namespace std;

    draw_rectangle(0, 0, 8, 14, PIXEL_SOLID, COLOR_FG_WHITE);
    draw_rectangle(1, 1, 6, 12, PIXEL_SOLID, COLOR_FG_BLACK);

    for (int y = 11; y >= 0; --y) {
        for (int x = 0; x < 6; ++x) {
            char c = convert_puyo_char(field.get_puyo(x, y));
            Color color = pchar_to_color(c);
            draw(x + 1, 11 - y + 1, PIXEL_CIRCLE, color);
        }
    }
};

void render_field(LTPuyo::Field field, LTPuyo::FieldMono mask)
{
    using namespace LTPuyo;
    using namespace std;

    draw_rectangle(0, 0, 8, 14, PIXEL_SOLID, COLOR_FG_WHITE);
    draw_rectangle(1, 1, 6, 12, PIXEL_SOLID, COLOR_FG_BLACK);

    for (int y = 11; y >= 0; --y) {
        for (int x = 0; x < 6; ++x) {
            if (((mask.column[x] >> y) & 1) > 0) {
                continue;
            }

            char c = convert_puyo_char(field.get_puyo(x, y));
            Color color = pchar_to_color(c);
            draw(x + 1, 11 - y + 1, PIXEL_CIRCLE, color);
        }
    }
};

void render_queue(std::pair<LTPuyo::Puyo, LTPuyo::Puyo> p1, std::pair<LTPuyo::Puyo, LTPuyo::Puyo> p2)
{
    using namespace LTPuyo;
    using namespace std;

    draw_rectangle(8, 0, 3, 7, PIXEL_SOLID, COLOR_FG_WHITE);
    draw_rectangle(9, 1, 1, 2, PIXEL_SOLID, COLOR_FG_BLACK);
    draw_rectangle(9, 4, 1, 2, PIXEL_SOLID, COLOR_FG_BLACK);

    Color color;

    color = pchar_to_color(convert_puyo_char(p1.first));
    draw(9, 1, PIXEL_CIRCLE, color);
    color = pchar_to_color(convert_puyo_char(p1.second));
    draw(9, 2, PIXEL_CIRCLE, color);

    color = pchar_to_color(convert_puyo_char(p2.first));
    draw(9, 4, PIXEL_CIRCLE, color);
    color = pchar_to_color(convert_puyo_char(p2.second));
    draw(9, 5, PIXEL_CIRCLE, color);
};

void load_json_heuristic(LTPuyo::Heuristic& h)
{
    std::ifstream file;
    file.open("config.json");
    json js;
    file >> js;
    file.close();
    LTPuyo::from_json(js, h);
};

void save_json_heuristic()
{
    std::ifstream f("config.json");
    if (f.good()) {
        return;
    };
    f.close();

    std::ofstream o("config.json");
    json js;
    LTPuyo::to_json(js, LTPuyo::DEFAULT_HEURISTIC());
    o << std::setw(4) << js << std::endl;
    o.close();
};

int main()
{
    using namespace LTPuyo;
    using namespace std;

    create_window(16, 18, 50);

    srand(uint32_t(time(NULL)));

    Heuristic heuristic;
    save_json_heuristic();
    load_json_heuristic(heuristic);

    cin.get();

    int time_wait = 500;

    vector<Pair> queue = create_queue();

    Field field;
    render_field(field);
    this_thread::sleep_for(chrono::milliseconds(time_wait));

    int i = 0;
    while (true) 
    {
        vector<Pair> tqueue;
        tqueue.push_back(queue[(i + 0) % 128]);
        tqueue.push_back(queue[(i + 1) % 128]);
        tqueue.push_back(queue[(i + 2) % 128]);

        ++i;

        SearchInfo sinfo;

        auto time_1 = chrono::high_resolution_clock::now();
        Search::search(field, tqueue, sinfo, heuristic);
        auto time_2 = chrono::high_resolution_clock::now();

        int64_t time = chrono::duration_cast<chrono::microseconds>(time_2 - time_1).count();

        draw_text(0, 14, std::wstring(L"node: ") + std::to_wstring(sinfo.node), COLOR_FG_WHITE);
        draw_text(0, 15, std::wstring(L"time: ") + std::to_wstring(int(std::round(double(time) / 1000.0))) + std::wstring(L" ms"), COLOR_FG_WHITE);
        if (time == 0) {
            draw_text(0, 16, std::wstring(L"nps:  ") + std::wstring(L"NULL") + std::wstring(L" kn/s"), COLOR_FG_WHITE);
        }
        else {
            draw_text(0, 16, std::wstring(L"nps:  ") + std::to_wstring(int(std::round(double(sinfo.node) / double(time) * 1000.0))) + std::wstring(L" kn/s"), COLOR_FG_WHITE);
        }

        if (sinfo.node == 0) {
            return -1;
        }

        SearchCandidate scan;

        Decision::decide(field, sinfo, scan);

        Puyo pair[2] = { tqueue[0].first, tqueue[0].second };

        field.drop_pair(scan.placement.x, pair, scan.placement.rotation);

        render_field(field);
        render_queue(tqueue[1], tqueue[2]);
        render();
        this_thread::sleep_for(chrono::milliseconds(time_wait));
        clear();

        FieldMono pop_mask = FieldMono();
        bool color[6];
        field.poppable_mask(pop_mask, color);
        int chain = 1;

        while (pop_mask.popcount() > 0)
        {
            draw_text(9, 8, std::to_wstring(chain), COLOR_FG_WHITE);
            render_field(field, pop_mask);
            render_queue(tqueue[1], tqueue[2]);
            render();
            this_thread::sleep_for(chrono::milliseconds(time_wait));
            clear();

            for (uint8_t puyo = Puyo::RED; puyo < Puyo::COUNT; ++puyo) {
                for (int x = 0; x < 6; ++x) {
                    field.puyo[puyo].column[x] = pext16(field.puyo[puyo].column[x], ~pop_mask.column[x]);
                }
            }

            draw_text(9, 8, std::to_wstring(chain), COLOR_FG_WHITE);
            render_field(field);
            render_queue(tqueue[1], tqueue[2]);
            render();
            this_thread::sleep_for(chrono::milliseconds(time_wait));
            clear();

            memset(pop_mask.column, 0, sizeof(pop_mask));
            field.poppable_mask(pop_mask, color);
            chain += 1;
        }
    }

    return 0;
};