#pragma once

#include "search.h"

namespace LTPuyo
{

class Decision
{
public:
    static void decide(Field field, SearchInfo& search_info, SearchCandidate& result);
};

};