#pragma once
#include "ParamTable.h"

namespace vf
{
// Plain snapshot of all parameters. Every member is a float named exactly like its parameter ID
// (booleans: > 0.5, choices: index).
struct Params
{
#define VF_P_FLOAT(id, name, mn, mx, def, ctr, unit, dec) float id = (float) (def);
#define VF_P_BOOL(id, name, def) float id = (float) (def);
#define VF_P_CHOICE(id, name, def, items) float id = (float) (def);
    VF_PARAM_LIST (VF_P_FLOAT, VF_P_BOOL, VF_P_CHOICE)
#undef VF_P_FLOAT
#undef VF_P_BOOL
#undef VF_P_CHOICE
};
} // namespace vf
