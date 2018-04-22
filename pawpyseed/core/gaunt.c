#include "gaunt.h"

double GAUNT_COEFF[4][4][4][7][4] = {{{{{0.28209479177387814, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}}},
{{{{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, -0.28209479177387814, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{-0.2185096861184158, 0.126156626101008, 0.0, 0.0},
{0.252313252202016, -0.2185096861184158, 0.0, 0.0},
{-0.2185096861184158, 0.30901936161855165, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}}},
{{{{0.28209479177387814, 0.0, 0.0, 0.0},
{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.30901936161855165, 0.0, 0.0},
{-0.2185096861184158, -0.2185096861184158, 0.0, 0.0},
{0.252313252202016, 0.126156626101008, 0.0, 0.0},
{-0.2185096861184158, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.1846743909223718, -0.08258889836115868, 0.0, 0.0},
{-0.2335966803276074, 0.14304816810266882, 0.0, 0.0},
{0.24776669508347607, -0.20230065940342062, 0.0, 0.0},
{-0.2335966803276074, 0.261169028265409, 0.0, 0.0},
{0.1846743909223718, -0.3198654279343846, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.28209479177387814, 0.0},
{0.0, -0.28209479177387814, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{-0.18022375157286857, 0.2207281154418226, -0.18022375157286857, 0.0},
{-0.09011187578643429, -0.09011187578643429, 0.2207281154418226, 0.0},
{0.18022375157286857, -0.09011187578643429, -0.18022375157286857, 0.0},
{-0.09011187578643429, 0.2207281154418226, 0.0, 0.0},
{-0.18022375157286857, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.15607834722743988, -0.09011187578643429, 0.04029925596769688, 0.0},
{-0.2207281154418226, 0.16119702387078752, -0.09011187578643429, 0.0},
{0.24179553580618127, -0.2207281154418226, 0.15607834722743988, 0.0},
{-0.2207281154418226, 0.2548748737361102, -0.23841361350444806, 0.0},
{0.15607834722743988, -0.23841361350444806, 0.3371677656723677, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}}},
{{{{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{-0.28209479177387814, 0.0, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{-0.28209479177387814, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, -0.3198654279343846, 0.0, 0.0},
{0.1846743909223718, 0.261169028265409, 0.0, 0.0},
{-0.2335966803276074, -0.20230065940342062, 0.0, 0.0},
{0.24776669508347607, 0.14304816810266882, 0.0, 0.0},
{-0.2335966803276074, -0.08258889836115868, 0.0, 0.0},
{0.1846743909223718, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{-0.16286750396763996, 0.06155813030745727, 0.0, 0.0},
{0.21324361862292307, -0.10662180931146154, 0.0, 0.0},
{-0.23841361350444806, 0.15078600877302686, 0.0, 0.0},
{0.24623252122982908, -0.19466390027300617, 0.0, 0.0},
{-0.23841361350444806, 0.23841361350444806, 0.0, 0.0},
{0.21324361862292307, -0.28209479177387814, 0.0, 0.0},
{-0.16286750396763996, 0.32573500793527993, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, -0.3198654279343846, 0.0},
{0.0, 0.261169028265409, 0.1846743909223718, 0.0},
{-0.20230065940342062, -0.2335966803276074, -0.08258889836115868, 0.0},
{0.24776669508347607, 0.14304816810266882, 0.0, 0.0},
{-0.20230065940342062, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.21026104350168, -0.21026104350168, 0.13298076013381088, 0.0},
{0.0, 0.16286750396763996, -0.18806319451591877, 0.0},
{-0.126156626101008, -0.059470803871759036, 0.2060129077457011, 0.0},
{0.168208834801344, -0.059470803871759036, -0.18806319451591877, 0.0},
{-0.126156626101008, 0.16286750396763996, 0.13298076013381088, 0.0},
{0.0, -0.21026104350168, 0.0, 0.0},
{0.21026104350168, 0.0, 0.0, 0.0}},
{{-0.12679217987703037, 0.06339608993851518, -0.023961469724456466, 0.0},
{0.19018826981554557, -0.11738674862413155, 0.05357947514468781, 0.0},
{-0.22731846124334895, 0.1694331772935932, -0.0928023731934731, 0.0},
{0.23961469724456466, -0.21431790057875125, 0.14175796661021042, 0.0},
{-0.22731846124334895, 0.2455320005465369, -0.20047603895459193, 0.0},
{0.19018826981554557, -0.25358435975406074, 0.26896683057741805, 0.0},
{-0.12679217987703037, 0.2196104975494288, -0.34723468516951067, 0.0}},
{{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}}},
{{{0.0, 0.0, 0.0, -0.28209479177387814},
{0.0, 0.0, 0.28209479177387814, 0.0},
{0.0, -0.28209479177387814, 0.0, 0.0},
{0.28209479177387814, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.0, 0.13298076013381088, -0.21026104350168, 0.21026104350168},
{-0.18806319451591877, 0.16286750396763996, 0.0, -0.21026104350168},
{-0.059470803871759036, -0.126156626101008, 0.16286750396763996, 0.13298076013381088},
{0.168208834801344, -0.059470803871759036, -0.18806319451591877, 0.0},
{-0.059470803871759036, 0.2060129077457011, 0.0, 0.0},
{-0.18806319451591877, 0.0, 0.0, 0.0},
{0.0, 0.0, 0.0, 0.0}},
{{0.20355072686733566, -0.18845135425709209, 0.14046334619025075, -0.07693494321105768},
{-0.04441841017299272, 0.14506992014597553, -0.17951486749246792, 0.14046334619025075},
{-0.09932258459927992, -0.025644981070352558, 0.14506992014597553, -0.18845135425709209},
{0.15386988642211535, -0.09932258459927992, -0.04441841017299272, 0.20355072686733566},
{-0.09932258459927992, 0.16219310146843374, -0.09595473285556255, -0.166198472532533},
{-0.04441841017299272, -0.09595473285556255, 0.21456130542787036, 0.0},
{0.20355072686733566, -0.166198472532533, 0.0, 0.0}},
{{-0.10864734032983336, 0.06272757118616618, -0.03136378559308309, 0.011854396693264041},
{0.17742036381239995, -0.1214714192760309, 0.07112638015958425, -0.03136378559308309},
{-0.22177545476549995, 0.17781595039896064, -0.1214714192760309, 0.06272757118616618},
{0.23708793386528085, -0.22177545476549995, 0.17742036381239995, -0.10864734032983336},
{-0.22177545476549995, 0.2429428385520618, -0.23047581331532352, 0.1717865285808715},
{0.17742036381239995, -0.23047581331532352, 0.26613054571859995, -0.25480059867297505},
{-0.10864734032983336, 0.1717865285808715, -0.25480059867297505, 0.36034246234410533}}}}};

