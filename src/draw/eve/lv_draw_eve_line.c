/*
 * lv_draw_eve_line.c
 *
 *  Created on: 8 abr 2023
 *      Author: juanj
 */
/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve.h"
#if LV_USE_DRAW_EVE
#include "lv_eve.h"


void lv_draw_eve_line(lv_draw_eve_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc)
{

    if(dsc->width == 0)
        return;
    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y)
        return;



    uint32_t line_w = dsc->width * 8;
    eve_scissor(draw_unit->base_unit.clip_area->x1, draw_unit->base_unit.clip_area->y1, draw_unit->base_unit.clip_area->x2, draw_unit->base_unit.clip_area->y2);
    eve_save_context();
    eve_color_opa(dsc->opa);
    eve_color(dsc->color);

    if(dsc->dash_gap && dsc->dash_width) {
        /*TODO*/
    }
    /* Check if it's a vertical or horizontal line without rounding */
    bool is_vertical = (dsc->p1.x == dsc->p2.x);
    bool is_horizontal = (dsc->p1.y == dsc->p2.y);
    bool no_round = (!dsc->round_end || !dsc->round_start);

    if((is_vertical || is_horizontal) && no_round) {
        eve_primitive(RECTS);
        if(is_vertical) {
            /* Draw vertical line */
            eve_vertex_2f(dsc->p1.x, dsc->p1.y);
            eve_vertex_2f(dsc->p2.x, dsc->p2.y);
        }
        else {
            /* Draw horizontal line */
            eve_vertex_2f(dsc->p1.x, dsc->p1.y);
            eve_vertex_2f(dsc->p2.x, dsc->p2.y);
        }
    }
    else {
        /* Draw inclined line or line with rounding (not possible without rounding)*/
        eve_primitive(LINE_STRIP);
        eve_line_width(line_w);
        eve_vertex_2f(dsc->p1.x, dsc->p1.y);
        eve_vertex_2f(dsc->p2.x, dsc->p2.y);
    }

    eve_restore_context();
    EVE_end_cmd_burst();
    EVE_execute_cmd();
    EVE_start_cmd_burst();
}

#endif /*LV_USE_DRAW_EVE*/
