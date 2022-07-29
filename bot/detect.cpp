#include "detect.h"

DetectorScore Detector::detect(Field& field)
{
    DetectorScore result = { 0, 0 };

    int height[6];
    field.get_height(height);

    // Find min x and max x for scanning
    int min_x = 0;
    int max_x = 5;

    for (int i = 2; i >= 0; --i) {
        if (height[i] > 11) {
            min_x = i;
            break;
        }
    }

    for (int i = 3; i < 6; ++i) {
        if (height[i] > 11) {
            max_x = i;
            break;
        }
    }

    // Find normalized heights
    // Since we don't want to detect "ugly" chain, we will have to normalize the heights array
    // Ugly chain = chain triggered in a well
    int height_normalize[6];
    Detector::normalize_height(height, height_normalize, min_x, max_x);

    // Find min y for each column
    int min_y[6];
    Detector::min_y(height_normalize, min_y, min_x, max_x);

    // Detect chain
    bool visit[13][6] = { false };

    for (int x = min_x; x <= max_x; ++x) {
        for (int y = min_y[x]; y < height[x]; ++y) {
            if (visit[x][y] || (field.puyo[static_cast<uint8_t>(Puyo::GARBAGE)].column[x] & (1 << y))) {
                continue;
            }

            Puyo puyo_fill = field.get_puyo(x, y);

            // Remove ugly chain trigger
            if ((x > 1 && height[x - 1] <= y && (field.puyo[static_cast<uint8_t>(puyo_fill)].column[x - 2] & (1 << y))) ||
                (x < 4 && height[x + 1] <= y && (field.puyo[static_cast<uint8_t>(puyo_fill)].column[x + 2] & (1 << y))) ||
                (y > 0 && x > 0 && height[x - 1] <= y && (field.puyo[static_cast<uint8_t>(puyo_fill)].column[x - 1] & (1 << (y - 1)))) ||
                (y > 0 && x < 5 && height[x + 1] <= y && (field.puyo[static_cast<uint8_t>(puyo_fill)].column[x + 1] & (1 << (y - 1)))) ||
                (y == height[x] - 1 && x > 0 && (field.puyo[static_cast<uint8_t>(puyo_fill)].column[x - 1] & (1 << (y + 1)))) ||
                (y == height[x] - 1 && x < 5 && (field.puyo[static_cast<uint8_t>(puyo_fill)].column[x + 1] & (1 << (y + 1))))) {
                continue;
            }

            Field copy = field;
            Chain chain;
            FieldMono mask = FieldMono();

            Detector::flood_fill(field, height, visit, x, y, puyo_fill, mask);

            for (uint8_t puyo = 0; puyo < static_cast<uint8_t>(Puyo::COUNT); ++puyo) {
                for (int i = 0; i < 6; ++i) {
                    copy.puyo[puyo].column[i] = pext16(copy.puyo[puyo].column[i], ~mask.column[i]);
                }
            }

            copy.pop(chain);

            result.chain_count = std::max(result.chain_count, chain.count);
            result.chain_score = std::max(result.chain_score, Field::calculate_point(chain));
        }
    }

    return result;
};

void Detector::flood_fill(Field& field, int height[6], bool visit[13][6], int x, int y, Puyo puyo, FieldMono& mask)
{
    mask.column[x] |= 1 << y;
    visit[y][x] = true;

    for (int i = 0; i < 4; ++i) {
        if (x + ROTATION_OFFSET[i][0] < 0 ||
            x + ROTATION_OFFSET[i][0] > 5 ||
            y + ROTATION_OFFSET[i][1] < 0 ||
            y + ROTATION_OFFSET[i][1] > 12 ||
            visit[y + ROTATION_OFFSET[i][1]][x + ROTATION_OFFSET[i][0]] ||
            y + ROTATION_OFFSET[i][1] >= height[x + ROTATION_OFFSET[i][0]]) {
            continue;
        }

        if (field.puyo[static_cast<uint8_t>(puyo)].column[x + ROTATION_OFFSET[i][0]] & (1 << (y + ROTATION_OFFSET[i][1]))) {
            Detector::flood_fill(field, height, visit, x + ROTATION_OFFSET[i][0], y + ROTATION_OFFSET[i][1], puyo, mask);
        }
    }
};

void Detector::normalize_height(int height[6], int height_normalize[6], int min_x, int max_x)
{
    // height_normalize[min_x] = std::max(height[min_x], height[min_x + 1] - 1);
    // height_normalize[max_x] = std::max(height[max_x], height[max_x - 1] - 1);
    height_normalize[min_x] = std::max(height[min_x], height[min_x + 1]);
    height_normalize[max_x] = std::max(height[max_x], height[max_x - 1]);

    if (max_x - min_x < 2) {
        return;
    }

    for (int i = min_x + 1; i < max_x; ++i) {
        height_normalize[i] = height[i];
        if (height[i - 1] > height[i] && height[i + 1] > height[i]) {
            height_normalize[i] = std::min(height[i - 1], height[i + 1]);
        }
    }
};

void Detector::min_y(int height_normalize[6], int min_y[6], int min_x, int max_x)
{
    min_y[min_x] = std::min(height_normalize[min_x] - 1, height_normalize[min_x + 1]);
    min_y[max_x] = std::min(height_normalize[max_x] - 1, height_normalize[max_x - 1]);

    for (int i = min_x + 1; i < max_x; ++i) {
        min_y[i] = std::min(height_normalize[i] - 1, std::min(height_normalize[i - 1], height_normalize[i + 1]));
        min_y[i] = std::max(min_y[i], height_normalize[i] - 2);
    }
};