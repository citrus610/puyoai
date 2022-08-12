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
            min_x = i + 1;
            break;
        }
    }

    for (int i = 3; i < 6; ++i) {
        if (height[i] > 11) {
            max_x = i - 1;
            break;
        }
    }

    // For every x position from min_x to max_y
    for (int x = min_x; x <= max_x; ++x) {
        // If there is a well at column x, then ignore
        if ((x == 0 && height[0] < height[1] - 1) ||
            (x == 5 && height[5] < height[4] - 1) ||
            (height[x] < height[x - 1] - 1 && height[x] < height[x + 1] - 1)) {
            continue;
        }

        int max_puyo_add = std::min(3, 12 - height[x]);

        // For every puyo color
        for (int p = 0; p < static_cast<int>(Puyo::COUNT) - 1; ++p) {
            Field field_copy = field;

            for (int i = 0; i < max_puyo_add; ++i) {
                field_copy.drop_puyo(x, Puyo(p));
                
                if (field_copy.count_group(x, field_copy.get_height(x) - 1) < 4) {
                    continue;
                }

                Chain chain;
                field_copy.pop(chain);
                result = std::max(result, DetectorScore { .chain_count = chain.count, .puyo_add_count = i + 1 });
                break;
            }
        }
    }

    return result;
};