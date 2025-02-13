/*
 * lv_draw_eve_arc.c
 *
 *  Created on: 11 dic 2023
 *      Author: juanj
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve.h"
#if LV_USE_DRAW_EVE
#include "lv_eve.h"

/*********************
 *      DEFINES
 *********************/

#ifndef M_PI
    #define M_PI 3.1415926f
#endif


/**********************
 * STATIC PROTOTYPES
 **********************/

static void draw_eve_arc(lv_draw_eve_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
static bool is_same_cuadrant(int16_t startAngle, int16_t endAngle) ;
static void draw_rounded_end(lv_point_t center, int32_t radius, int32_t angle, int32_t width);
static void lv_draw_eve_mask_angle(const lv_draw_arc_dsc_t * dsc, int32_t vertex_x, int32_t vertex_y,
                                   int32_t start_angle, int32_t end_angle);
static PrimitiveDraw get_mask_direction(int16_t angle);
static int32_t chord_length(int16_t radius, int16_t angleDegrees);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve_arc(lv_draw_eve_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    //if(is_same_cuadrant(dsc->start_angle, dsc->end_angle)){ /* this method drawing two arc  */

    //lv_draw_arc_dsc_t p = *dsc;

    // p.end_angle =  (p.start_angle + 180) % 360;
    // draw_eve_arc(draw_unit, &p, coords); // draw mid arc

    // p.start_angle = p.end_angle;
    // p.end_angle = dsc->end_angle;
    // draw_eve_arc(draw_unit, &p, coords); // drwa mid arc

    //}else {
    draw_eve_arc(draw_unit, dsc, coords);
    //}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static int32_t chord_length(int16_t radius, int16_t angleDegrees)
{
    angleDegrees %= 360;
    if(angleDegrees < 0) angleDegrees += 360;
    int32_t sin_value = lv_trigo_sin(angleDegrees / 2);
    int64_t chordLength = 2 * radius * sin_value / 32768.0;
    return (int32_t)chordLength ;
}


static PrimitiveDraw get_mask_direction(int16_t angle)
{
    if(angle >= 315 || angle <  45) {
        return EDGE_STRIP_R;
    }
    if(angle >=  45 && angle < 135) {
        return EDGE_STRIP_B;
    }
    if(angle >= 135 && angle < 225) {
        return EDGE_STRIP_L;
    }
    if(angle >= 225 && angle < 315) {
        return EDGE_STRIP_A;
    }
    return 0;
}


static void draw_rounded_end(lv_point_t center, int32_t radius, int32_t angle, int32_t width)
{
    int32_t rounded_y  = center.y + ((lv_trigo_sin(angle) * radius) >> LV_TRIGO_SHIFT);
    int32_t rounded_x  = center.x + ((lv_trigo_cos(angle) * radius) >> LV_TRIGO_SHIFT);
    eve_draw_circle_simple(rounded_x, rounded_y, width);
}



static bool is_same_cuadrant(int16_t startAngle, int16_t endAngle)
{
    if(startAngle > endAngle) {
        if((startAngle >= 0 && startAngle < 90) && (endAngle >= 0 && endAngle < 90)) {
            return true;
        }
        else if((startAngle >= 90 && startAngle < 180) && (endAngle >= 90 && endAngle < 180)) {
            return true;
        }
        else if((startAngle >= 180 && startAngle < 270) && (endAngle >= 180 && endAngle < 270)) {
            return true;
        }
        else if((startAngle >= 270 && startAngle < 360) && (endAngle >= 270 && endAngle < 360)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}



static void lv_draw_eve_mask_angle(const lv_draw_arc_dsc_t * dsc, int32_t vertex_x, int32_t vertex_y,
                                   int32_t start_angle, int32_t end_angle)
{

    /*Constrain the input angles*/


    if(start_angle < 0)
        start_angle = 0;
    else if(start_angle > 359)
        start_angle = 359;

    if(end_angle < 0)
        end_angle = 0;
    else if(end_angle > 359)
        end_angle = 359;

    LV_ASSERT_MSG(start_angle >= 0 && start_angle <= 360, "Unexpected start angle");

    int32_t mid_angle_op;
    int32_t angle_range;
    int32_t mask_dir_start;
    int32_t mask_dir_end;
    lv_point_t start;
    lv_point_t end;
    lv_point_t angle_range_op;

    if(end_angle > start_angle) {
        angle_range = LV_ABS(end_angle - start_angle);
    }
    else {
        angle_range = 360 - start_angle + end_angle;
    }

    mid_angle_op = (angle_range / 2) + start_angle + 180;
    mid_angle_op = mid_angle_op % 360;

    mask_dir_end = LV_ABS(((360 - angle_range) / 4) + end_angle);
    mask_dir_start = LV_ABS(((360 - angle_range) / 4) + mid_angle_op);

    mask_dir_start = mask_dir_start % 360;
    mask_dir_end = mask_dir_end % 360;

    start.y = (lv_trigo_sin(start_angle) >> 5) + vertex_y;
    start.x = (lv_trigo_cos(start_angle) >> 5) + vertex_x;

    end.y = (lv_trigo_sin(end_angle) >> 5) + vertex_y;
    end.x = (lv_trigo_cos(end_angle) >> 5) + vertex_x;

    angle_range_op.y = (lv_trigo_sin(mid_angle_op) >> 5) + vertex_y;
    angle_range_op.x = (lv_trigo_cos(mid_angle_op) >> 5) + vertex_x;

    if(angle_range <= 180) {
        /* Two sides mask and 6 vertex points */

        /* Masking end angle */
        PrimitiveDraw edge = get_mask_direction(mask_dir_end);
        eve_primitive(edge); /* Side one */
        eve_vertex_2f(angle_range_op.x, angle_range_op.y);
        eve_vertex_2f(vertex_x, vertex_y);
        eve_vertex_2f(end.x, end.y);

        /* Masking start angle */
        edge = get_mask_direction(mask_dir_start);
        eve_primitive(edge); /* Side two */
        eve_vertex_2f(angle_range_op.x, angle_range_op.y);
        eve_vertex_2f(vertex_x, vertex_y);
        eve_vertex_2f(start.x, start.y);

    }

    else {

        if(is_same_cuadrant(start_angle,
                            end_angle)) { /* "It is not an optimal implementation for the case where both angles (start and end) are in the same quadrant */
            /* todo */
            lv_point_t end_line_cntr;
            lv_point_t start_line_cntr;

            lv_point_t end_line_brd;
            lv_point_t start_line_brd;

            int16_t chord = chord_length(dsc->radius, 360 - angle_range);
            int16_t w = ((chord / 4) < 1) ? 1 : chord / 4;
            int16_t r_width = w;

            end_line_brd.y = vertex_y + ((lv_trigo_sin(end_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
            end_line_brd.x = vertex_x + ((lv_trigo_cos(end_angle) * dsc->radius) >> LV_TRIGO_SHIFT);

            start_line_brd.y = vertex_y + ((lv_trigo_sin(start_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
            start_line_brd.x = vertex_x + ((lv_trigo_cos(start_angle) * dsc->radius) >> LV_TRIGO_SHIFT);

            eve_draw_rect_simple(start_line_brd.x, start_line_brd.y, end_line_brd.x, end_line_brd.y, 0);

            start_line_brd.y = start_line_brd.y + ((lv_trigo_sin(start_angle - 90) * r_width) >> LV_TRIGO_SHIFT);
            start_line_brd.x = start_line_brd.x + ((lv_trigo_cos(start_angle - 90) * r_width) >> LV_TRIGO_SHIFT);

            end_line_brd.y = end_line_brd.y + ((lv_trigo_sin(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
            end_line_brd.x = end_line_brd.x + ((lv_trigo_cos(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);

            end_line_cntr.y = vertex_y + ((lv_trigo_sin(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
            end_line_cntr.x = vertex_x + ((lv_trigo_cos(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);

            start_line_cntr.y = vertex_y + ((lv_trigo_sin(start_angle + 270) * r_width) >> LV_TRIGO_SHIFT);
            start_line_cntr.x = vertex_x + ((lv_trigo_cos(start_angle + 270) * r_width) >> LV_TRIGO_SHIFT);

            eve_primitive(LINE_STRIP);
            eve_line_width(r_width * 16);
            eve_vertex_2f(start_line_cntr.x, start_line_cntr.y);
            eve_vertex_2f(start_line_brd.x, start_line_brd.y);
            eve_vertex_2f(end_line_brd.x, end_line_brd.y);
            eve_vertex_2f(end_line_cntr.x, end_line_cntr.y);

        }
        else { /* One side mask and 3 vertex points */
            /* Masking end and start angles */
            PrimitiveDraw edge = get_mask_direction(mid_angle_op);
            eve_primitive(edge);
            eve_vertex_2f(end.x, end.y);
            eve_vertex_2f(vertex_x, vertex_y);
            eve_vertex_2f(start.x, start.y);
        }
    }

}



static void draw_eve_arc(lv_draw_eve_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{

    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(dsc->start_angle == dsc->end_angle)
        return;

    lv_color_t color = dsc->color;
    lv_opa_t opa = dsc->opa;
    lv_point_t center = dsc->center;
    int32_t width = dsc->width;
    uint16_t radius_out = dsc->radius;
    uint16_t radius_in = dsc->radius - dsc->width;
    int32_t start_angle = (int32_t) dsc->start_angle;
    int32_t end_angle = (int32_t) dsc->end_angle;

    if(width > radius_out)
        width = radius_out;

    while(start_angle >= 360)
        start_angle -= 360;
    while(end_angle >= 360)
        end_angle -= 360;

    eve_scissor(draw_unit->base_unit.clip_area->x1, draw_unit->base_unit.clip_area->y1, draw_unit->base_unit.clip_area->x2, draw_unit->base_unit.clip_area->y2);

    eve_save_context();

    eve_color(color);
    eve_color_opa(opa);

    eve_color_mask(0, 0, 0, 1);
    eve_stencil_func(EVE_ALWAYS, 0, 1);
    eve_stencil_op(EVE_REPLACE, EVE_REPLACE);
    eve_draw_circle_simple(center.x, center.y, radius_out); // radius_out

    eve_blend_func(EVE_ONE, EVE_ZERO);
    eve_draw_circle_simple(center.x, center.y, radius_in + 2); // radius_in

    eve_stencil_func(EVE_ALWAYS, 1, 1);
    eve_stencil_op(EVE_REPLACE, EVE_REPLACE);
    eve_blend_func(EVE_ZERO, EVE_ONE_MINUS_SRC_ALPHA);
    eve_color_opa(0XFF);

    /* Start masking arc */

    lv_draw_eve_mask_angle(dsc, center.x, center.y, start_angle, end_angle);

    /* End masking arc */

    eve_draw_circle_simple(center.x, center.y, radius_in); // radius_in

    eve_color_mask(1, 1, 1, 1);
    eve_blend_func(EVE_DST_ALPHA, EVE_ONE_MINUS_DST_ALPHA);
    eve_draw_circle_simple(center.x, center.y, radius_in); // radius_in

    eve_stencil_func(EVE_NOTEQUAL, 1, 0XFF);
    eve_blend_func(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA);
    //EVE_cmd_dl_burst(COLOR_A(opa));

    eve_color_opa(opa);
    eve_draw_circle_simple(center.x, center.y, radius_out); // radius_out

    eve_restore_context();

    if(dsc->rounded) {
        eve_save_context();
        eve_color_opa(opa);
        eve_color(color);
        int32_t half_width = width / 2;
        int32_t adjusted_radius = radius_out - half_width;
        draw_rounded_end(center, adjusted_radius, end_angle, half_width);
        draw_rounded_end(center, adjusted_radius, start_angle, half_width);
        eve_restore_context();
    }

    EVE_end_cmd_burst();
    EVE_execute_cmd();
    EVE_start_cmd_burst();
}



#endif /*LV_USE_DRAW_EVE*/
