#include <iostream>
#include "../bot/decision.h"
#include "render.h"

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

int main()
{
    using namespace LTPuyo;
    using namespace std;

    // bench_search(1000);

    create_window(16, 18, 50);

    srand(uint32_t(time(NULL)));

    cin.get();

    int time_wait = 500;

    vector<pair<Puyo, Puyo>> queue;
    for (int i = 0; i < 1000; ++i) {
        queue.push_back({ Puyo(rand() % 4), Puyo(rand() % 4) });
    }

    Field field;
    render_field(field);
    this_thread::sleep_for(chrono::milliseconds(time_wait));

    int i = 0;
    while (true) 
    {
        vector<pair<Puyo, Puyo>> tqueue;
        tqueue.push_back(queue[(i + 0) % 1000]);
        tqueue.push_back(queue[(i + 1) % 1000]);
        tqueue.push_back(queue[(i + 2) % 1000]);

        ++i;

        SearchInfo sinfo;

        Search::search(field, tqueue, sinfo);

        draw_text(1, 16, std::wstring(L"node: ") + std::to_wstring(sinfo.node), COLOR_FG_WHITE);

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