#include "detect.h"

Detect::Score Detect::detect(Field& field)
{
    Detect::Score result = Detect::Score();

    u8 height[6];
    field.get_height(height);

    i8 min_x = 0;
    for (i8 i = 2; i >= 0; --i) {
        if (height[i] > 11) {
            min_x = i + 1;
            break;
        }
    }

    for (i8 x = min_x; x < 6; ++x) {
        if (height[x] > 11) {
            break;
        }

        u8 max_puyo_add = std::min(3, 12 - height[x]);

        for (u8 p = 0; p < Cell::COUNT - 1; ++p) {
            Field field_copy = field;

            for (u8 i = 0; i < max_puyo_add; ++i) {
                field_copy.drop_puyo(x, Cell::Type(p));

                if (field_copy.data[p].get_group_count(x, height[x]) < 4) {
                    continue;
                }

                Chain::Data chain = field_copy.pop();
                result = std::max(
                    result,
                    Detect::Score {
                        .score = Chain::get_score(chain),
                        .chain = u8(chain.links.get_size()),
                        .added = u8(i + 1),
                        // .height = u8(height[x] + i)
                        .height = height[x]
                    }
                );
                break;
            }
        }
    }

    return result;
};

void Detect::bench()
{
    using namespace std;

    const char c[13][7] = {
        "....B.",
        "....YB",
        "....YG",
        "...BBB",
        "...YYG",
        "...RYB",
        "R.GYRB",
        "BRYRGB",
        "RGGYRG",
        "RRGYRG",
        "BBYGBB",
        "BRRYGG",
        "RYYGBB",
    };

    Field field;
    field.from(c);
    field.print();

    i64 time = 0;
    const int iter = 2000000;

    u64 height_total = 0;

    for (int i = 0; i < iter; ++i) {
        auto time_1 = chrono::high_resolution_clock::now();
        Score result = detect(field);
        auto time_2 = chrono::high_resolution_clock::now();

        time += chrono::duration_cast<chrono::nanoseconds>(time_2 - time_1).count();

        height_total += result.height;
    }

    Score result = detect(field);

    cout << int(height_total) << endl;
    cout << "time: " << (time / iter) << " ns" << endl;
    cout << (int)result.score << " " << (int)result.chain << " " << (int)result.added << " " << (int)result.height << endl;
};