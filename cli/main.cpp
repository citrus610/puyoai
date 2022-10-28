#include <iostream>
#include "../ai/ai.h"
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

std::vector<Cell::Pair> create_queue()
{
    using namespace std;

    vector<Cell::Type> bag;
    bag.reserve(256);

    for (int i = 0; i < 64; ++i) {
        for (uint8_t p = 0; p < Cell::COUNT - 1; ++p) {
            bag.push_back(Cell::Type(p));
        }
    }

    for (int t = 0; t < 4; ++t) {
        for (int i = 0; i < 256; ++i) {
            int k = rand() % 256;
            Cell::Type value = bag[i];
            bag[i] = bag[k];
            bag[k] = value;
        }
    }

    vector<Cell::Pair> queue;
    queue.reserve(128);

    for (int i = 0; i < 128; ++i) {
        queue.push_back({ bag[i * 2], bag[i * 2 + 1] });
    }

    return queue;
};

void render_field(Field field)
{
    using namespace std;

    draw_rectangle(0, 0, 8, 14, PIXEL_SOLID, COLOR_FG_WHITE);
    draw_rectangle(1, 1, 6, 12, PIXEL_SOLID, COLOR_FG_BLACK);

    for (i8 y = 11; y >= 0; --y) {
        for (i8 x = 0; x < 6; ++x) {
            char c = Cell::to_char(field.get_cell(x, y));
            Color color = pchar_to_color(c);
            draw(x + 1, 11 - y + 1, PIXEL_CIRCLE, color);
        }
    }
};

void render_field(Field field, FieldBit mask)
{
    using namespace std;

    draw_rectangle(0, 0, 8, 14, PIXEL_SOLID, COLOR_FG_WHITE);
    draw_rectangle(1, 1, 6, 12, PIXEL_SOLID, COLOR_FG_BLACK);

    for (int y = 11; y >= 0; --y) {
        for (int x = 0; x < 6; ++x) {
            if (((mask.cols[x] >> y) & 1) > 0) {
                continue;
            }

            char c = Cell::to_char(field.get_cell(x, y));
            Color color = pchar_to_color(c);
            draw(x + 1, 11 - y + 1, PIXEL_CIRCLE, color);
        }
    }
};

void render_plan(Field field, Field plan)
{
    for (i8 y = 11; y >= 0; --y) {
        for (i8 x = 0; x < 6; ++x) {
            if (field.get_cell(x, y) != Cell::Type::NONE) {
                continue;
            }
            char c = Cell::to_char(plan.get_cell(x, y));
            Color color = pchar_to_color(c);
            draw(x + 1, 11 - y + 1, PixelType('+'), color);
        }
    }
};

void render_queue(Cell::Pair p1, Cell::Pair p2)
{
    using namespace std;

    draw_rectangle(7, 0, 3, 7, PIXEL_SOLID, COLOR_FG_WHITE);
    draw_rectangle(8, 1, 1, 2, PIXEL_SOLID, COLOR_FG_BLACK);
    draw_rectangle(8, 4, 1, 2, PIXEL_SOLID, COLOR_FG_BLACK);

    Color color;

    color = pchar_to_color(Cell::to_char(p1.first));
    draw(8, 1, PIXEL_CIRCLE, color);
    color = pchar_to_color(Cell::to_char(p1.second));
    draw(8, 2, PIXEL_CIRCLE, color);

    color = pchar_to_color(Cell::to_char(p2.first));
    draw(8, 4, PIXEL_CIRCLE, color);
    color = pchar_to_color(Cell::to_char(p2.second));
    draw(8, 5, PIXEL_CIRCLE, color);
};

void load_json_heuristic(Weight& h)
{
    std::ifstream file;
    file.open("config.json");
    json js;
    file >> js;
    file.close();
    from_json(js, h);
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
    to_json(js, DEFAULT_WEIGHT);
    o << std::setw(4) << js << std::endl;
    o.close();
};

int main()
{
    using namespace std;

    create_window(15, 18, 50);

    srand(uint32_t(time(NULL)));

    Weight heuristic;
    save_json_heuristic();
    load_json_heuristic(heuristic);

    cin.get();

    int time_wait = 500;

    vector<Cell::Pair> queue = create_queue();

    const char c[13][7] = {
       "......",
       "......",
       "......",
       "......",
       "......",
       "......",
       "......",
       ".....G",
       "R....G",
       "BBBGGR",
       "RGYBBR",
       "RRGYYB",
       "GGYRRB",
    };

    Field field;
    field.from(c);

    render_field(field);
    this_thread::sleep_for(chrono::milliseconds(time_wait));

    int i = 0;
    while (true) 
    {
        if (field.get_height(2) > 11) {
            return 0;
        }

        vector<Cell::Pair> tqueue;
        tqueue.push_back(queue[(i + 0) % 128]);
        tqueue.push_back(queue[(i + 1) % 128]);
        tqueue.push_back(queue[(i + 2) % 128]);

        ++i;

        auto time_1 = chrono::high_resolution_clock::now();

        AI::Result ai_result = AI::think_1p(field, tqueue, 32, 1000, heuristic);

        auto time_2 = chrono::high_resolution_clock::now();
        int64_t time = chrono::duration_cast<chrono::microseconds>(time_2 - time_1).count();
        draw_text(0, 15, std::wstring(L"time: ") + std::to_wstring(int(std::round(double(time) / 1000.0))) + std::wstring(L" ms"), COLOR_FG_WHITE);
        draw_text(0, 16, std::wstring(L"eval: ") + std::to_wstring(ai_result.eval), COLOR_FG_WHITE);

        Cell::Pair pair = { tqueue[0].first, tqueue[0].second };

        // field.drop_pair(scan.placement.x, pair, scan.placement.rotation);
        field.drop_pair(ai_result.placement.x, ai_result.placement.direction, pair);

        render_field(field);
        render_queue(tqueue[1], tqueue[2]);
        render_plan(field, ai_result.plan);
        render();
        this_thread::sleep_for(chrono::milliseconds(time_wait));
        clear();

        FieldBit pop_mask = field.get_pop_mask();
        int chain = 1;

        while (pop_mask.get_count() > 0)
        {
            draw_text(8, 8, std::to_wstring(chain), COLOR_FG_WHITE);
            render_field(field, pop_mask);
            render_queue(tqueue[1], tqueue[2]);
            render();
            this_thread::sleep_for(chrono::milliseconds(time_wait));
            clear();

            for (uint8_t puyo = 0; puyo < Cell::COUNT; ++puyo) {
                for (int x = 0; x < 6; ++x) {
                    field.data[puyo].cols[x] = pext16(field.data[puyo].cols[x], ~pop_mask.cols[x]);
                }
            }

            draw_text(8, 8, std::to_wstring(chain), COLOR_FG_WHITE);
            render_field(field);
            render_queue(tqueue[1], tqueue[2]);
            render();
            this_thread::sleep_for(chrono::milliseconds(time_wait));
            clear();

            memset(pop_mask.cols, 0, sizeof(pop_mask));
            pop_mask = field.get_pop_mask();
            chain += 1;
        }
    }

    return 0;
};