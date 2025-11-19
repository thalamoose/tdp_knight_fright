#include "kftypes.h"
#include "lerp.h"
#include "utilities.h"

#define FP_ONE I_TO_F(1)
#define FP_MUL(a, b) fp_mul_f88(a, b)

// Blend one coordinate using Catmull–Rom spline
static s16 catmull_blend(s16 p0, s16 p1, s16 p2, s16 p3, s16 t)
{
    s16 t2 = FP_MUL(t, t);
    s16 t3 = FP_MUL(t2 / 100, t);
    // This is to attempt to preserve FP accuracy.
    // We assume t=F(0) to F(100)
    s16 half = FP_ONE >> 1;

    return FP_MUL(half,
                  (2 * p1) +
                      FP_MUL((-p0 + p2), t) +
                      FP_MUL((2 * p0 - 5 * p1 + 4 * p2 - p3), t2) +
                      FP_MUL((-p0 + 3 * p1 - 3 * p2 + p3), t3));
}

// Compute one pos16 on spline segment
static coord catmull_point(coord p0, coord p1, coord p2, coord p3, s16 t)
{
    coord r;
    r.x = catmull_blend(p0.x, p1.x, p2.x, p3.x, t);
    r.y = catmull_blend(p0.y, p1.y, p2.y, p3.y, t);
    return r;
}

// Generate spline points for all segments
void catmull_rom_spline(coord *ctrl, s16 count, s16 steps)
{
    if (count < 2)
        return;

    for (s16 i = 0; i < count - 1; i++)
    {
        coord p0 = (i == 0) ? ctrl[i] : ctrl[i - 1];
        coord p1 = ctrl[i];
        coord p2 = ctrl[i + 1];
        coord p3 = (i + 2 < count) ? ctrl[i + 2] : ctrl[i + 1];

        for (s16 s = 0; s <= steps; s++)
        {
            s16 t = (s * FP_ONE) / steps;
            coord p = catmull_point(p0, p1, p2, p3, t);
        }
    }
}
//
// This doesn't work right. Fixed point accuracy issues.
//
coord SplineBlend(coord *p, s16 percentage)
{
    coord result = catmull_point(p[0], p[1], p[2], p[3], percentage);
    return result;
}
#if 0
s16 main(void) {
    // Control points (10.6 fixed-point)
    coord ctrl[] = {
        { I_TO_F(50),  I_TO_F(50)  },
        { I_TO_F(100), I_TO_F(100) },
        { I_TO_F(150), I_TO_F(0)   },
        { I_TO_F(200), I_TO_F(100) }
    };

    s16 num_points = sizeof(ctrl)/sizeof(ctrl[0]);
    s16 steps = 20; // samples per segment

    printf("Catmull–Rom spline (10.6 fixed-point):\n");
    catmull_rom_spline(ctrl, num_points, steps);
    return 0;
}
#endif