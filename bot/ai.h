#pragma once

#include "path.h"
#include "search.h"

struct AiResult
{
    Placement placement;
    MoveQueue move;
};

struct AiAttackCandidate
{
    Placement placement;
    int chain_count = 0;
    int chain_attack = 0;
};

class Ai
{
public:
    static AiResult think(Field& field, std::vector<Pair>& queue, bool pvp, int incomming_attack, int target_point, int bonus_point, bool pc, bool enemy_danger, Heuristic heuristic = DEFAULT_HEURISTIC());
    static AiResult think_mono(Field& field, std::vector<Pair>& queue, int harass_trigger_point, int max_harass_count, Heuristic heuristic = DEFAULT_HEURISTIC());
    static AiResult think_pvp(Field& field, std::vector<Pair>& queue, int incomming_attack, int target_point, int bonus_point, bool pc, bool enemy_danger, Heuristic heuristic = DEFAULT_HEURISTIC());
public:
    static avec<AiAttackCandidate, 22> get_attack_candidate(Field& field, Pair pair, int target_point, int bonus_point, bool pc);
    static bool get_enemy_danger(Field& field_enemy, std::vector<Pair>& queue_enemy);
    static int get_above_stack_count(Field& field);
    static int flood_fill_above_stack_count(Field& field, int x, int y, bool visit[12][6]);
};