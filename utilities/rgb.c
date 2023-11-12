#include "rgb.h"

// 21 steps is about 0.35 seconds
// Formula is time period us / 16666us (60hz)
#define RGB_FADE_STEPS 21

uint8_t _rgb_anim_steps = 0;
bool _rgb_out_dirty = false;

rgb_s _rgb_next[ADAPTER_RGB_COUNT]     = {0};
rgb_s _rgb_current[ADAPTER_RGB_COUNT]  = {0};
rgb_s _rgb_last[ADAPTER_RGB_COUNT]     = {0};

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void _rgb_update_all()
{
    rgb_itf_update(_rgb_current);
}

uint32_t _rgb_blend(rgb_s *original, rgb_s *new, float blend)
{
    float or = (float) original->r;
    float og = (float) original->g;
    float ob = (float) original->b;
    float nr = (float) new->r;
    float ng = (float) new->g;
    float nb = (float) new->b;
    float outr = or + ((nr-or)*blend);
    float outg = og + ((ng-og)*blend);
    float outb = ob + ((nb-ob)*blend);
    rgb_s col = {
        .r = (uint8_t) outr,
        .g = (uint8_t) outg,
        .b = (uint8_t) outb
    };
    return col.color;
}

void _rgb_animate_step()
{
    static uint8_t steps = RGB_FADE_STEPS;
    const float blend_step = 1.0f/RGB_FADE_STEPS;
    bool done = true;

    if (_rgb_out_dirty)
    {
        memcpy(_rgb_last, _rgb_current, sizeof(_rgb_last));
        steps = 0;
        _rgb_out_dirty = false;
        done = false;
    }

    if (steps <= RGB_FADE_STEPS)
    {
        float blender = blend_step * (float) steps;
        // Blend between old and next colors appropriately
        for(uint8_t i = 0; i < ADAPTER_RGB_COUNT; i++)
        {
            _rgb_current[i].color = _rgb_blend(&_rgb_last[i], &_rgb_next[i], blender);
        }
        steps++;
        _rgb_update_all();
    }
    else if (!done)
    {
        memcpy(_rgb_current, _rgb_next, sizeof(_rgb_next));
        _rgb_update_all();
        done = true;
    }
}

void _rgb_set_sequential(rgb_s *colors, uint8_t len, uint32_t color)
{
    for(uint8_t i = 0; i < len; i++)
    {
        colors[i].color = color;
    }
}

// Enable the RGB transition to the next color
void rgb_set_dirty()
{

    _rgb_out_dirty = true;

}

void rgb_set_instant(uint32_t color)
{

    for(uint8_t i = 0; i < ADAPTER_RGB_COUNT; i++)
    {
        _rgb_last[i].color = color;
        _rgb_current[i].color = color;
        _rgb_next[i].color = color;
    }
    _rgb_update_all();

}

// Set all RGBs to one color
void rgb_set_all(uint32_t color)
{
    for(uint8_t i = 0; i < ADAPTER_RGB_COUNT; i++)
    {
        _rgb_next[i].color = color;
    }
}

void rgb_set_single(uint32_t color, uint8_t idx)
{
    _rgb_next[idx].color = color;
}

void rgb_init()
{
    rgb_itf_init();
}

uint32_t _rgb_interval = 16666;

// One tick of RGB logic
// only performs actions if necessary
void rgb_task(uint32_t timestamp)
{
    static interval_s _i_state = {.last_time = 0, .this_time = 0};

    if(interval_run(timestamp, _rgb_interval, &_i_state))
    {
        _rgb_animate_step();
    }

}
