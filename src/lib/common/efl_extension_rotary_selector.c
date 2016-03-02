/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "efl_extension.h"
#include "efl_extension_private.h"
#include "circle/efl_extension_circle_private.h"
#include "common/efl_extension_common_private.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                                                         efl_extension_rotary_selector.c
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EEXT_ROTARY_SELECTOR_DATA_KEY "eext_rotary_selector_data"
#define _ROTARY_SELECTOR_SCREEN_WIDTH 360
#define _ROTARY_SELECTOR_SCREEN_HEIGHT 360
#define _ROTARY_SELECTOR_ITEM_WIDTH 56
#define _ROTARY_SELECTOR_ITEM_HEIGHT 56
#define _ROTARY_SELECTOR_ITEM_DIVISION_ANGLE 30
#define _ROTARY_SELECTOR_PAGE_ITEM_MAX 11
#define _ROTARY_SELECTOR_PAGE_ANIMATION_DURATION 0.35
#define _ROTARY_SELECTOR_NEXT_PAGE_ANIMATION_DELAY_TIME 0.1
#define _ROTARY_SELECTOR_END_EFFECT_ANIMATION_DURATION 0.6
#define _ROTARY_SELECTOR_DEFAULT_RADIUS 145.0
#define _ROTARY_SELECTOR_CUBIC_BEZIER_P1_X 0.33
#define _ROTARY_SELECTOR_CUBIC_BEZIER_P1_Y 0.0
#define _ROTARY_SELECTOR_CUBIC_BEZIER_P2_X 0.3
#define _ROTARY_SELECTOR_CUBIC_BEZIER_P2_Y 1.0

#define _ROTARY_SELECTOR_DEBUG_ENABLED 0

// ====================================== util ==================================== //
#define _DEGREES_TO_RADIANS(degrees) (degrees * M_PI / 180.0)
#define _RADIANS_TO_DEGREES(radians) (radians * 180.0 / M_PI)
#define ALMOST_ZERO_FLOAT   0.00001f

#define ROTARY_SELECTOR_COLOR_VALUE_SET(obj, part, state, r, g, b, a) \
                               do { \
                                  obj->part.state.r = r; \
                                  obj->part.state.g = g; \
                                  obj->part.state.b = b; \
                                  obj->part.state.a = a; \
                                  obj->part.state.set_flag = EINA_TRUE; \
                               } while (0)

#define ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, obj, part, state) \
                              do { \
                                 evas_object_color_set(content, \
                                       obj->part.state.r, \
                                       obj->part.state.g, \
                                       obj->part.state.b, \
                                       obj->part.state.a); \
                                       obj->part.state.set_flag = EINA_TRUE; \
                              } while (0)

#define ROTARY_SELECTOR_COLOR_VALUE_GET(obj, part, state, r, g, b, a) \
                              do { \
                                 if (r) *r = obj->part.state.r; \
                                 if (g) *g = obj->part.state.g; \
                                 if (b) *b = obj->part.state.b; \
                                 if (a) *a = obj->part.state.a; \
                              } while (0)

static double
_min(const double a, const double b)
{
   if (a < b)
     {
        return a;
     }
   return b;
}

static Eina_Bool
_doubleCompare(double p1, double p2)
{
   if ((p1 == 0.f) || (p2 == 0.f))
     {
        return (p1 == p2 || abs(p1 - p2) <= ALMOST_ZERO_FLOAT);
     }

   return (p1 == p2 || abs(p1 - p2) <= ALMOST_ZERO_FLOAT * _min(abs(p1), abs(p2)));
}

static void
_rgba_to_user_text_style_str(char *user_style, int r, int g, int b, int a)
{
   //for elm_entry_text_style_user_push string
   if (!user_style) return;

   if (r > 255) r = 255; if (r < 0) r = 0;
   if (g > 255) g = 255; if (g < 0) g = 0;
   if (b > 255) b = 255; if (b < 0) b = 0;
   if (a > 255) a = 255; if (a < 0) a = 0;

   sprintf(user_style, "DEFAULT='color=#%2.2x%2.2x%2.2x%2.2x'", r, g, b, a);
}

// ----------------------------------------- interpolator util --------------------------------------//

void
_circle_path_interpolator(double progress, const double startAngle, const double endAngle, const double startRadius, const double endRadius, double* pos_x, double* pos_y)
{
   const double center_x = _ROTARY_SELECTOR_SCREEN_WIDTH/2.0f;
   const double center_y = _ROTARY_SELECTOR_SCREEN_HEIGHT/2.0f;

   // 1. calculate angle
   double angle = 0.f;
   if (_doubleCompare(startAngle, endAngle))
     {
        angle = startAngle;
     }
   else if (startAngle < endAngle)
     {
        angle = startAngle + ((endAngle - startAngle) * progress);
     }
   else
     {
        angle = startAngle - ((startAngle - endAngle) * progress);
     }

   double radian = _DEGREES_TO_RADIANS(angle);
   double cosValue = cos(radian);
   double sinValue = sin(radian);

   // 2. calculate radius
   float radius = startRadius;
   if (endRadius >= 0)
     {
        if (startRadius <= endRadius)
          {
             radius = startRadius + ((endRadius-startRadius)*progress);
          }
        else
          {
             radius = startRadius - ((startRadius-endRadius)*progress);
          }
     }

   // 3. calculate target point
   *pos_x = center_x + (radius * (float)(cosValue)) - (_ROTARY_SELECTOR_ITEM_WIDTH / 2.f);
   *pos_y = center_y + (radius * (float)(sinValue)) - (_ROTARY_SELECTOR_ITEM_HEIGHT / 2.f);

   return;
}

// ----------------------------------------- internal header --------------------------------------//
static Eext_Rotary_Selector_Data *_eext_rotary_selector_data_init(Evas_Object *obj, Evas_Object *parent);
static Eext_Rotary_Selector_Data *_eext_rotary_selector_data_get(Evas_Object *rotary_selector);
static Eina_Bool _rotary_selector_rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info);
static void _rotary_selector_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _rotary_selector_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _rotary_selector_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

// ----------------------------------------- item API --------------------------------------//
static void _item_coords_update(Eext_Rotary_Selector_Data *rsd, Eext_Rotary_Selector_Item *item);
static void _items_transformation_update(Eext_Rotary_Selector_Data *rsd);
static void _items_invalidate(Eext_Rotary_Selector_Data *rsd);
static void _layer_items_invalidate(Eext_Rotary_Selector_Data *rsd);
static void _item_rearrange(Eext_Rotary_Selector_Data *rsd, int selected_index);
static void _item_select(Eext_Rotary_Selector_Data *rsd, int index);
static void _item_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Eext_Rotary_Selector_Item *_item_create(Eext_Rotary_Selector_Data *rsd);
static void _item_selected_signal_send(Eext_Rotary_Selector_Data *rsd, int previous_item_index, int current_item_index);
static void _item_clicked_cb(void *data, Evas_Object *obj, void *event_info);
static void _item_pressed_cb(void *data, Evas_Object *obj, void *event_info);
static Eext_Rotary_Selector_Item * _item_get(Eext_Rotary_Selector_Data *rsd, int index);
static void _item_text_copy(char **target, const char* source);

// ----------------------------------------- animation API --------------------------------------//
static Eina_Bool _previous_page_animator_cb(void *data);
static Eina_Bool _item_update_animator_cb(void *data);
static void _page_animation_run(Eext_Rotary_Selector_Data *rsd, Eina_Bool direction);
static void _end_effect_animation_run(Eext_Rotary_Selector_Data *rsd, Eina_Bool direction);
static void _item_update_animation_run(Eext_Rotary_Selector_Data *rsd);
static Eina_Bool _next_page_animator_cb(void *data);
static Eina_Bool _next_page_timer_cb(void *data);
static Eina_Bool _end_effect_animator_cb(void *data);

// ----------------------------------------- selector API --------------------------------------//
static void _selector_update(Eext_Rotary_Selector_Data *rsd, Eina_Bool page_changed);
static void _selector_content_update(Eext_Rotary_Selector_Data *rsd);
static void _selector_clicked_signal(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _selector_pressed_signal(void *data, Evas_Object *obj, const char *emission, const char *source);

// ----------------------------------------- internal header --------------------------------------//
static Eext_Rotary_Selector_Data *
_eext_rotary_selector_data_init(Evas_Object *obj, Evas_Object *parent)
{
   Eext_Rotary_Selector_Data *rsd = NULL;

   rsd = (Eext_Rotary_Selector_Data *)calloc(1, sizeof(Eext_Rotary_Selector_Data));
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }
   rsd->parent = parent;
   rsd->rotary_selector = obj;
   rsd->selector_bg = NULL;
   rsd->item_list = NULL;
   rsd->item_count = 0;
   rsd->selected_index = 0;
   rsd->current_page = 0;
   rsd->animation_direction = EINA_TRUE;
   rsd->previous_page_animator = NULL;
   rsd->next_page_animator = NULL;
   rsd->previous_page_animator_start_time = 0.0f;
   rsd->next_page_animator_start_time = 0.0f;
   rsd->next_page_animation_started = EINA_FALSE;
   rsd->previous_page_animation_started = EINA_FALSE;
   rsd->next_page_timer = NULL;
   rsd->end_effect_animation_started = EINA_FALSE;
   rsd->end_effect_animator_start_time = 0.0f;
   rsd->end_effect_animator = NULL;
   rsd->item_update_animator = NULL;
   rsd->selector_bg_image_color.normal_color.set_flag = EINA_FALSE;
   rsd->selector_bg_image_color.pressed_color.set_flag = EINA_FALSE;

   return rsd;
}

static Eext_Rotary_Selector_Data *
_eext_rotary_selector_data_get(Evas_Object *rotary_selector)
{
   if (!rotary_selector)
     {
        LOGE("rotary_selector is NULL!!");
        return NULL;
     }

   return evas_object_data_get(rotary_selector, EEXT_ROTARY_SELECTOR_DATA_KEY);
}

static Eina_Bool
_rotary_selector_rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data*)data;

   if (elm_object_disabled_get(rsd->rotary_selector))
     {
        return ECORE_CALLBACK_PASS_ON;
     }

   if (rsd->previous_page_animator || rsd->next_page_animator || rsd->end_effect_animator)
     {
        return ECORE_CALLBACK_PASS_ON;
     }

   int previous_item_index = rsd->selected_index;
   int first_index = rsd->current_page*_ROTARY_SELECTOR_PAGE_ITEM_MAX;
   int last_index = 0;
   if (rsd->current_page == 0)
     {
        last_index = _ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }
   else
     {
        last_index = (rsd->current_page + 1)*_ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }

   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE)
     {
        if (rsd->selected_index < rsd->item_count - 1)
          {
             rsd->selected_index += 1;
             _item_selected_signal_send(rsd, previous_item_index, rsd->selected_index);
             if (last_index < rsd->selected_index)
               {
                  _page_animation_run(rsd, EINA_TRUE);
                  _selector_update(rsd, EINA_TRUE);
               }
             else
               {
                  _selector_update(rsd, EINA_FALSE);
               }
             evas_object_smart_callback_call(rsd->rotary_selector, "item,selected", (void*)_item_get(rsd, rsd->selected_index));
          }
        else
          {
             _end_effect_animation_run(rsd, EINA_TRUE);
          }
     }
   else
     {
        if (rsd->selected_index > 0)
          {
             rsd->selected_index -= 1;
             _item_selected_signal_send(rsd, previous_item_index, rsd->selected_index);
             if (first_index > rsd->selected_index)
               {
                  _page_animation_run(rsd, EINA_FALSE);
                  _selector_update(rsd, EINA_TRUE);
               }
             else
               {
                  _selector_update(rsd, EINA_FALSE);
               }
             evas_object_smart_callback_call(rsd->rotary_selector, "item,selected", (void*)_item_get(rsd, rsd->selected_index));
          }
        else
          {
             _end_effect_animation_run(rsd, EINA_FALSE);
          }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static void
_rotary_selector_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   _item_select(rsd, rsd->selected_index);
}

static void
_rotary_selector_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = data;
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   _item_select(rsd, rsd->selected_index);
}

static void
_rotary_selector_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   if (rsd->item_list)
     {
        Eext_Rotary_Selector_Item *item = NULL;
        EINA_LIST_FREE(rsd->item_list, item)
          evas_object_del(item->base.obj);
        rsd->item_list = NULL;
     }
   if (rsd->previous_page_animator)
     {
        ecore_animator_del(rsd->previous_page_animator);
        rsd->previous_page_animator = NULL;
     }
   if (rsd->next_page_animator)
     {
        ecore_animator_del(rsd->next_page_animator);
        rsd->next_page_animator = NULL;
     }
   if (rsd->next_page_timer)
     {
        ecore_timer_del(rsd->next_page_timer);
        rsd->next_page_timer = NULL;
     }
   if (rsd->end_effect_animator)
     {
        ecore_animator_del(rsd->end_effect_animator);
        rsd->end_effect_animator = NULL;
     }
   if (rsd->item_update_animator)
     {
        ecore_animator_del(rsd->item_update_animator);
        rsd->item_update_animator = NULL;
     }

   edje_object_signal_callback_del(elm_layout_edje_get(rsd->rotary_selector), "eext,clicked", "", _selector_clicked_signal);
   edje_object_signal_callback_del(elm_layout_edje_get(rsd->rotary_selector), "eext,clicked", "", _selector_pressed_signal);
   eext_rotary_object_event_callback_del(obj, _rotary_selector_rotary_cb);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOVE, _rotary_selector_move_cb);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_SHOW, _rotary_selector_show_cb);

   evas_object_data_del(obj, EEXT_ROTARY_SELECTOR_DATA_KEY);

   free(rsd);
}

// ----------------------------------------- item API --------------------------------------//
static void
_item_coords_update(Eext_Rotary_Selector_Data *rsd, Eext_Rotary_Selector_Item *item)
{
   const float endAngle[] = { 300.f, 330.f, 0.f, 30.f, 60.f, 90.f, 120.f, 150.f, 180.f, 210.f, 240.f };

   Eext_Rotary_Selector_Item_Coords *coords = &item->coords;
   _circle_path_interpolator(1.0f, 0, endAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], 0, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coords->x, &coords->y);

#if _ROTARY_SELECTOR_DEBUG_ENABLED
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(item->base.obj, &x, &y, &w, &h);

   LOGI("item(0x%x), index(%d), x(%f), y(%f)", item, item->index, coords->x, coords->y);
#endif
}

static void
_items_transformation_update(Eext_Rotary_Selector_Data *rsd)
{
   Eina_List *l = NULL;
   Eext_Rotary_Selector_Item *item = NULL;
   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        _item_coords_update(rsd, item);
     }
}

static void
_items_invalidate(Eext_Rotary_Selector_Data *rsd)
{
#if _ROTARY_SELECTOR_DEBUG_ENABLED
   double start_time = ecore_time_unix_get();
#endif
   if (rsd->item_count <= 0)
     {
        return;
     }
   int i = 0;
   Eina_List *l = NULL;
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(rsd->rotary_selector, &x, &y, &w, &h);
   Eext_Rotary_Selector_Item *item = NULL;

   int first_index = rsd->current_page*_ROTARY_SELECTOR_PAGE_ITEM_MAX;
   int last_index = 0;
   if (rsd->current_page == 0)
     {
        last_index = _ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }
   else
     {
        last_index = (rsd->current_page + 1)*_ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }

   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        Evas_Object *button = item->base.obj;

        if (i >= first_index && i <= last_index)
          {
             evas_object_color_set(button, 255, 255, 255, 255);
             evas_object_move(button, x + item->coords.x, y + item->coords.y);
             evas_object_raise(button);
             evas_object_show(button);
#if _ROTARY_SELECTOR_DEBUG_ENABLED
             LOGI("obj : 0x%x show index : %d x:%f y:%f", item->base.obj, item->index, x + item->coords.x, y + item->coords.y);
#endif
          }
        else
          {
             evas_object_hide(button);
#if _ROTARY_SELECTOR_DEBUG_ENABLED
             LOGI("obj : 0x%x hide index : %d", item->base.obj, item->index);
#endif
          }
        i++;
     }

   _layer_items_invalidate(rsd);

#if _ROTARY_SELECTOR_DEBUG_ENABLED
   LOGI("elapsed time(%f)", ecore_time_unix_get() - start_time);
#endif
}

static void
_layer_items_invalidate(Eext_Rotary_Selector_Data *rsd)
{
   if (rsd->item_count <= 0)
     {
        return;
     }
   int i = 0;
   Eina_List *l = NULL;
   double coord_x, coord_y;
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(rsd->rotary_selector, &x, &y, &w, &h);
   Eext_Rotary_Selector_Item *item = NULL;

   int first_index = rsd->current_page*_ROTARY_SELECTOR_PAGE_ITEM_MAX;
   int last_index = 0;
   if (rsd->current_page == 0)
     {
        last_index = _ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }
   else
     {
        last_index = (rsd->current_page + 1)*_ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }

   Evas_Object *current_first_item = NULL;
   Evas_Object *second_layer_item = NULL;

   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        Evas_Object *button = item->base.obj;
        if (i == first_index)
          {
             current_first_item = button;
          }

        if(rsd->current_page > 0 && i == first_index-1) //Second layer Right
          {
             second_layer_item = button;
          }
        else if (last_index < (rsd->item_count - 1) && i == last_index + 1) //Second layer Left
          {
             second_layer_item = button;
          }
        i++;
     }

   l = NULL;
   item = NULL;
   i = 0;
   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        Evas_Object *button = item->base.obj;
        if (rsd->current_page > 0 && i == first_index-1) //Second layer Right
          {
             _circle_path_interpolator(1, 0, -65, _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coord_x, &coord_y);
             evas_object_move(button, x + coord_x, y + coord_y);
             evas_object_stack_below(button, current_first_item);
             evas_object_color_set(button, 128, 128, 128, 128);
             evas_object_show(button);
          }
        else if (rsd->current_page > 0 && i == first_index-2) //third layer Right
          {
             _circle_path_interpolator(1, 0, -69, _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coord_x, &coord_y);
             evas_object_move(button, x + coord_x, y + coord_y);
             evas_object_stack_below(button, second_layer_item);
             evas_object_color_set(button, 102, 102, 102, 102);
             evas_object_show(button);
          }
        else if (last_index < (rsd->item_count - 1) && i == last_index + 1) //Second layer Left
          {
             _circle_path_interpolator(1, 0, -115, _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coord_x, &coord_y);
             evas_object_move(button, x + coord_x, y + coord_y);
             evas_object_stack_below(button, current_first_item);
             evas_object_color_set(button, 128, 128, 128, 128);
             evas_object_show(button);
          }
        else if (last_index < (rsd->item_count - 1) && i == last_index + 2) //third layer Left
          {
             _circle_path_interpolator(1, 0, -111, _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coord_x, &coord_y);
             evas_object_move(button, x + coord_x, y + coord_y);
             evas_object_stack_below(button, second_layer_item);
             evas_object_color_set(button, 102, 102, 102, 102);
             evas_object_show(button);
          }
        i++;
     }
}

static void
_item_rearrange(Eext_Rotary_Selector_Data *rsd, int selected_index)
{
   Eina_List *l = NULL;
   int i = 0;
   Eext_Rotary_Selector_Item *item = NULL;

   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        item->index = i;
        i++;
     }
   rsd->item_count = i;
   rsd->selected_index = selected_index;

#if _ROTARY_SELECTOR_DEBUG_ENABLED
   LOGI("item is rearranged!! : item_count(%d)", rsd->item_count);
#endif
}

static void
_item_select(Eext_Rotary_Selector_Data *rsd, int index)
{
#if _ROTARY_SELECTOR_DEBUG_ENABLED
   LOGI("item is selected!! : index(%d)", index);
#endif
   {
      _item_selected_signal_send(rsd, rsd->selected_index, index);
      rsd->selected_index = index;
      _selector_update(rsd, EINA_FALSE);
      _items_transformation_update(rsd);
      _items_invalidate(rsd);
   }
}

static void
_item_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Rotary_Selector_Item *item = data;

   evas_object_smart_callback_del(obj, "clicked", _item_clicked_cb);
   evas_object_smart_callback_del(obj, "pressed", _item_pressed_cb);

   if (!item)
      return;

   if (item->text1)
     {
        free(item->text1);
        item->text1 = NULL;
     }
   if (item->text2)
     {
        free(item->text2);
        item->text2 = NULL;
     }
   if (item->domain)
     {
        free(item->domain);
        item->domain = NULL;
     }

   if (item)
     {
        free(item);
     }
}

static Eext_Rotary_Selector_Item *
_item_create(Eext_Rotary_Selector_Data *rsd)
{
   Eext_Rotary_Selector_Item *item = NULL;
   Evas_Object *button = NULL;
   Evas_Object *rotary_selector = rsd->rotary_selector;

   button = elm_button_add(rotary_selector);
   if (!button)
    {
       LOGE("button is NULL!!");
       return NULL;
    }

   evas_object_size_hint_weight_set(button,EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_style_set(button, "rotary_selector_item");
   evas_object_resize(button, _ROTARY_SELECTOR_ITEM_WIDTH, _ROTARY_SELECTOR_ITEM_HEIGHT);
   evas_object_smart_member_add(button, rotary_selector);

   Evas_Object *action_bg = elm_layout_add(button);
   elm_layout_theme_set(action_bg, "layout/rotary_selector", "item", "bg_image");
   elm_object_part_content_set(button, "item,bg_image", action_bg);

   item = (Eext_Rotary_Selector_Item *)calloc(1, sizeof(Eext_Rotary_Selector_Item));
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   item->base.obj = button;
   item->index = -1;
   item->parent = rsd->parent;
   item->rotary_selector = rotary_selector;
   item->coords.x = 0.0f;
   item->coords.y = 0.0f;
   item->domain = NULL;
   item->text1 = NULL;
   item->text2 = NULL;
   item->selector_icon_color.normal_color.set_flag = EINA_FALSE;
   item->selector_icon_color.pressed_color.set_flag = EINA_FALSE;
   item->item_bg_image_color.normal_color.set_flag = EINA_FALSE;
   item->item_bg_image_color.pressed_color.set_flag = EINA_FALSE;
   item->item_icon_color.normal_color.set_flag = EINA_FALSE;
   item->item_icon_color.pressed_color.set_flag = EINA_FALSE;
   item->selector_icon_color.normal_color.set_flag = EINA_FALSE;
   item->selector_icon_color.pressed_color.set_flag = EINA_FALSE;
   item->custom_content_color.normal_color.set_flag = EINA_FALSE;
   item->custom_content_color.pressed_color.set_flag = EINA_FALSE;
   item->selector_main_text_color.normal_color.set_flag = EINA_FALSE;
   item->selector_main_text_color.pressed_color.set_flag = EINA_FALSE;
   item->selector_sub_text_color.normal_color.set_flag = EINA_FALSE;
   item->selector_sub_text_color.pressed_color.set_flag = EINA_FALSE;

   item->item_bg_image.normal = action_bg;
   item->item_bg_image.pressed = action_bg;

   evas_object_smart_callback_add(button, "clicked", _item_clicked_cb, item);
   evas_object_smart_callback_add(button, "pressed", _item_pressed_cb, item);
   evas_object_event_callback_add(button, EVAS_CALLBACK_DEL, _item_del_cb, item);

   return item;
}


static void
_item_selected_signal_send(Eext_Rotary_Selector_Data *rsd, int previous_item_index, int current_item_index)
{
   Eina_List *l = NULL;
   int i = 0;
   Eext_Rotary_Selector_Item *item = NULL;

   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        if (previous_item_index != -1 && i == previous_item_index)
          {
             elm_object_signal_emit(item->base.obj, "elm,action,button,unselected", "");
#if _ROTARY_SELECTOR_DEBUG_ENABLED
             LOGI("item is unselected!! : index(%d)", previous_item_index);
#endif
          }
        if (current_item_index !=-1 && i == current_item_index)
          {
             elm_object_signal_emit(item->base.obj, "elm,action,button,selected", "");
#if _ROTARY_SELECTOR_DEBUG_ENABLED
             LOGI("item is selected!! : index(%d)", current_item_index);
#endif
          }
        i++;
     }
}

static void
_item_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)data;
   Eext_Rotary_Selector_Data *rsd = NULL;

   rsd = _eext_rotary_selector_data_get(rotary_selector_item->rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   rotary_selector_item->state = EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL;
   _item_selected_signal_send(rsd, rsd->selected_index, rotary_selector_item->index);

   if (rsd->selected_index == rotary_selector_item->index)
     {
        evas_object_smart_callback_call(rsd->rotary_selector, "item,clicked", (void*)_item_get(rsd, rsd->selected_index));
     }
   else
     {
        rsd->selected_index = rotary_selector_item->index;
        evas_object_smart_callback_call(rsd->rotary_selector, "item,selected", (void*)_item_get(rsd, rotary_selector_item->index));
     }

   _selector_update(rsd, EINA_FALSE);
}

static void
_item_pressed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)data;
   Eext_Rotary_Selector_Data *rsd = NULL;

   rsd = _eext_rotary_selector_data_get(rotary_selector_item->rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   rotary_selector_item->state = EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED;

   _selector_update(rsd, EINA_FALSE);
}

static Eext_Rotary_Selector_Item *
_item_get(Eext_Rotary_Selector_Data *rsd, int index)
{
   int i = 0;
   Eina_List *l = NULL;
   Eext_Rotary_Selector_Item *item = NULL;
   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        if (index == i)
          {
             return item;
          }
        i++;
     }

   return NULL;
}

static void
_item_text_copy(char **target, const char* source)
{
   int size = 0;

   if (*target)
     {
        free(*target);
        *target = NULL;
     }
   size = strlen(source) + 1;
   *target = malloc(sizeof(char)*size);
   if (!(*target))
   {
      LOGE("allocation is failed!!");
      return;
   }
   memset(*target, 0, size);
   strncpy(*target, source, size);
}

// ----------------------------------------- animation API --------------------------------------//
static Eina_Bool
_previous_page_animator_cb(void *data)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;
   Eina_List *l = NULL;
   Eext_Rotary_Selector_Item *item = NULL;

   int i = 0;
   int last_index = 0;
   double p = 0.0f;
   double v[4] = {_ROTARY_SELECTOR_CUBIC_BEZIER_P1_X, _ROTARY_SELECTOR_CUBIC_BEZIER_P1_Y, _ROTARY_SELECTOR_CUBIC_BEZIER_P2_X, _ROTARY_SELECTOR_CUBIC_BEZIER_P2_Y};

   int first_index = rsd->current_page*_ROTARY_SELECTOR_PAGE_ITEM_MAX;

   if (rsd->current_page == 0)
     {
        last_index = _ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }
   else
     {
        last_index = (rsd->current_page + 1)*_ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }

   if (rsd->previous_page_animation_started)
     {
        rsd->previous_page_animation_started = EINA_FALSE;
        rsd->previous_page_animator_start_time = ecore_time_unix_get();
     }

   double dt = ecore_time_unix_get() - rsd->previous_page_animator_start_time;
   p = ecore_animator_pos_map_n(dt /_ROTARY_SELECTOR_PAGE_ANIMATION_DURATION, ECORE_POS_MAP_CUBIC_BEZIER, 4, v);

   if (p <= 1.0f)
     {
        const float startAngle[] = { 300.f, 330.f, 0.f, 30.f, 60.f, 90.f, 120.f, 150.f, 180.f, 210.f, 240.f };

        EINA_LIST_FOREACH(rsd->item_list, l, item)
          {
             Evas_Coord new_a;
             Evas_Object *button = item->base.obj;
             Eext_Rotary_Selector_Item_Coords *coords = &item->coords;
             evas_object_raise(button);

             //current page
             if (first_index <= i && i <= last_index)
               {
                  //Forward
                  if (rsd->animation_direction)
                    {
                       // page 0 --> 1 : 0page
                       const float endAngle[] = { 660.f, 660.f, 300.f, 300.f, 300.f, 300.f, 300.f, 300.f, 300.f, 300.f, 300.f };
                       _circle_path_interpolator(p, startAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], endAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coords->x, &coords->y);
                    }
                  else
                    {
                       // page 1 --> 0 : 1page
                       const float endAngle[] = { 240.f, 240.f, -120.f, -120.f, -120.f, -120.f, -120.f, -120.f, -120.f, -120.f, -120.f };
                       _circle_path_interpolator(p, startAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], endAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coords->x, &coords->y);
                    }
                  new_a = 255*(1-p);
                  evas_object_move(button, item->coords.x, item->coords.y);
                  evas_object_color_set(button, new_a, new_a, new_a, new_a);
               }
             i++;
          }
     }
   if (p >= 1.0f)
     {
        rsd->previous_page_animator_start_time = 0.0f;
        rsd->previous_page_animator = NULL;

        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_item_update_animator_cb(void *data)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;

   _item_rearrange(rsd, rsd->selected_index);
   _item_select(rsd, rsd->selected_index);

   rsd->item_update_animator = NULL;

#if _ROTARY_SELECTOR_DEBUG_ENABLED
   LOGI("item_list(0x%d), count(%d)", rsd->item_list, rsd->item_count);
#endif

   return ECORE_CALLBACK_CANCEL;
}

static void
_page_animation_run(Eext_Rotary_Selector_Data *rsd, Eina_Bool direction)
{
   if (direction) //Forward
     {
        rsd->animation_direction = EINA_TRUE;
     }
   else //Backward
     {
        rsd->animation_direction = EINA_FALSE;
     }

   Edje_Message_Float_Set *msg = malloc(sizeof(Edje_Message_Float_Set));
   if (!msg) return;

   msg->count = 1;
   msg->val[0] = 1;
   edje_object_message_send(elm_layout_edje_get(rsd->rotary_selector), EDJE_MESSAGE_FLOAT_SET, 0, msg);

   rsd->previous_page_animator_start_time = ecore_loop_time_get();
   rsd->previous_page_animation_started = EINA_TRUE;
   rsd->previous_page_animator = ecore_animator_add(_previous_page_animator_cb, rsd);

   rsd->next_page_timer = ecore_timer_add(_ROTARY_SELECTOR_NEXT_PAGE_ANIMATION_DELAY_TIME, _next_page_timer_cb, rsd);
}

static void
_end_effect_animation_run(Eext_Rotary_Selector_Data *rsd, Eina_Bool direction)
{
   if (direction) //Forward
     {
        rsd->animation_direction = EINA_TRUE;
     }
   else //Backward
     {
        rsd->animation_direction = EINA_FALSE;
     }
   rsd->end_effect_animation_started = EINA_TRUE;
   rsd->end_effect_animator = ecore_animator_add(_end_effect_animator_cb, rsd);
}

static void
_item_update_animation_run(Eext_Rotary_Selector_Data *rsd)
{
   if (rsd->item_update_animator)
     {
#if _ROTARY_SELECTOR_DEBUG_ENABLED
        LOGI("item update animator is already running!!");
#endif
        return;
     }

   rsd->item_update_animator = ecore_animator_add(_item_update_animator_cb, rsd);
}

static Eina_Bool
_next_page_animator_cb(void *data)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;
   Eina_List *l = NULL;
   Eext_Rotary_Selector_Item *item = NULL;

   int first_index = rsd->current_page*_ROTARY_SELECTOR_PAGE_ITEM_MAX;
   int i = 0;
   Evas_Coord new_a;
   Eext_Rotary_Selector_Item_Coords *coords = NULL;
   Evas_Object *button = NULL;
   int last_index = 0;
   if (rsd->current_page == 0)
     {
        last_index = _ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }
   else
     {
        last_index = (rsd->current_page + 1)*_ROTARY_SELECTOR_PAGE_ITEM_MAX - 1;
     }

   double p = 0.0f;
   double v[4] = {_ROTARY_SELECTOR_CUBIC_BEZIER_P1_X, _ROTARY_SELECTOR_CUBIC_BEZIER_P1_Y, _ROTARY_SELECTOR_CUBIC_BEZIER_P2_X, _ROTARY_SELECTOR_CUBIC_BEZIER_P2_Y};

   if (rsd->next_page_animation_started)
     {
        rsd->next_page_animation_started = EINA_FALSE;
        rsd->next_page_animator_start_time = ecore_time_unix_get();
     }

   double dt = ecore_time_unix_get() - rsd->next_page_animator_start_time;
   p = ecore_animator_pos_map_n(dt /_ROTARY_SELECTOR_PAGE_ANIMATION_DURATION, ECORE_POS_MAP_CUBIC_BEZIER, 4, v);

   if (p <= 1.0f)
     {
        EINA_LIST_FOREACH(rsd->item_list, l, item)
          {
             button = item->base.obj;
             coords = &item->coords;
             evas_object_raise(button);

             if (rsd->animation_direction) //Forward
               {
                  if (first_index + _ROTARY_SELECTOR_PAGE_ITEM_MAX <= i && i<= last_index + _ROTARY_SELECTOR_PAGE_ITEM_MAX)    //current page+1
                    {
                       // page 0 --> 1 : 1page
                       const float endAngle[] = { 300.f, 330.f, 360.f, 390.f, 420.f, 450.f, 480.f, 510.f, 540.f, 570.f, 600.f };
                       _circle_path_interpolator(p, 300, endAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coords->x, &coords->y);
                       new_a = 255*p;
                       evas_object_move(button, item->coords.x, item->coords.y);
                       evas_object_color_set(button, new_a, new_a, new_a, new_a);
                       evas_object_show(button);
                    }
               }
             else //Backward
               {
                  if (first_index - _ROTARY_SELECTOR_PAGE_ITEM_MAX <= i && i<= last_index - _ROTARY_SELECTOR_PAGE_ITEM_MAX)    //current page-1
                    {
                       // page 1 --> 0 : 0page
                       const float endAngle[] = { -60.f, -30.f, 0.f, 30.f, 60.f, 90.f, 120.f, 150.f, 180.f, 210.f, 240.f };
                       _circle_path_interpolator(p, 240, endAngle[item->index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], _ROTARY_SELECTOR_DEFAULT_RADIUS, _ROTARY_SELECTOR_DEFAULT_RADIUS, &coords->x, &coords->y);
                       new_a = 255*p;
                       evas_object_move(button, item->coords.x, item->coords.y);
                       evas_object_color_set(button, new_a, new_a, new_a, new_a);
                       evas_object_show(button);
                    }
               }
             i++;
          }
     }

   if (p >= 1.0f)
     {
        //Forward
        if (rsd->animation_direction)
          {
             rsd->current_page += 1;
             _items_invalidate(rsd);
          }
        else //Backward
          {
             rsd->current_page -= 1;
             _items_invalidate(rsd);
          }

        rsd->next_page_animator_start_time = 0.0f;
        rsd->next_page_animator = NULL;

        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_next_page_timer_cb(void *data)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;

   rsd->next_page_timer = NULL;

   rsd->next_page_animator_start_time = ecore_loop_time_get();
   rsd->next_page_animation_started = EINA_TRUE;
   rsd->next_page_animator = ecore_animator_add(_next_page_animator_cb, rsd);

   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_end_effect_animator_cb(void *data)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;

   double p = 0.0f;
   double v[4] = {_ROTARY_SELECTOR_CUBIC_BEZIER_P1_X, _ROTARY_SELECTOR_CUBIC_BEZIER_P1_Y, _ROTARY_SELECTOR_CUBIC_BEZIER_P2_X, _ROTARY_SELECTOR_CUBIC_BEZIER_P2_Y};

   if (rsd->end_effect_animation_started)
     {
        rsd->end_effect_animation_started = EINA_FALSE;
        rsd->end_effect_animator_start_time = ecore_time_unix_get();
     }

   double dt = ecore_time_unix_get() - rsd->end_effect_animator_start_time;
   p = ecore_animator_pos_map_n(dt /_ROTARY_SELECTOR_END_EFFECT_ANIMATION_DURATION, ECORE_POS_MAP_CUBIC_BEZIER, 4, v);

   Evas_Object *selector = elm_object_part_content_get(rsd->rotary_selector, "selector");
   if (!selector)
     {
        LOGE("selector is NULL!!");
        return ECORE_CALLBACK_CANCEL;
     }

   Evas_Map *map = NULL;
   map = evas_map_new(4);
   if (!map)
     {
        LOGE("map is NULL!!");
        return ECORE_CALLBACK_CANCEL;
     }

   double selector_angle = ((rsd->selected_index + 1)%_ROTARY_SELECTOR_PAGE_ITEM_MAX)*_ROTARY_SELECTOR_ITEM_DIVISION_ANGLE;
   double effect_angle = rsd->animation_direction ? _ROTARY_SELECTOR_ITEM_DIVISION_ANGLE/2 : -_ROTARY_SELECTOR_ITEM_DIVISION_ANGLE/2;

   evas_map_util_points_populate_from_object(map, selector);

   if (p <= 0.5f)
     {
        evas_map_util_rotate(map, selector_angle + effect_angle*p/0.5, _ROTARY_SELECTOR_SCREEN_WIDTH/2, _ROTARY_SELECTOR_SCREEN_HEIGHT/2);
        evas_object_map_enable_set(selector, EINA_TRUE);
        evas_object_map_set(selector, map);
     }
   else if (p <= 1.0f)
     {
        evas_map_util_rotate(map, selector_angle + effect_angle - effect_angle*(p - 0.5)/0.5, _ROTARY_SELECTOR_SCREEN_WIDTH/2, _ROTARY_SELECTOR_SCREEN_HEIGHT/2);
        evas_object_map_enable_set(selector, EINA_TRUE);
        evas_object_map_set(selector, map);
     }

   if (p >= 1.0f)
     {
        rsd->animation_direction = EINA_TRUE;
        rsd->end_effect_animator_start_time = 0.0f;
        rsd->end_effect_animator = NULL;
        if (map) evas_map_free(map);

        return ECORE_CALLBACK_CANCEL;
     }

   if (map) evas_map_free(map);

   return ECORE_CALLBACK_RENEW;
}

// ----------------------------------------- new API --------------------------------------//
static void
_selector_update(Eext_Rotary_Selector_Data *rsd, Eina_Bool page_changed)
{
   Evas_Object *edje = elm_layout_edje_get(rsd->rotary_selector);
   Evas_Object *content;
   if (!edje)
     {
        LOGE("edje is NULL!!");
        return;
     }

   const char* signals[] = {"eext,rotate,0", "eext,rotate,30", "eext,rotate,60", "eext,rotate,90", "eext,rotate,120",
                            "eext,rotate,150", "eext,rotate,180", "eext,rotate,210", "eext,rotate,240", "eext,rotate,270", "eext,rotate,300",};

   if (page_changed)
     {
        if (rsd->selected_index%_ROTARY_SELECTOR_PAGE_ITEM_MAX == 0)
          {
             edje_object_signal_emit(edje, "eext,rotate,360", "eext");
          }
        else
          {
             edje_object_signal_emit(edje, "eext,rotate,-60", "eext");
          }
     }
   else
     {
        edje_object_signal_emit(edje, signals[rsd->selected_index%_ROTARY_SELECTOR_PAGE_ITEM_MAX], "eext");
     }

   content = elm_object_part_content_unset(elm_object_part_content_get(rsd->rotary_selector, "selector"), "selector,bg_image");
   if (content)
     {
        evas_object_hide(content);
     }

   if ((rsd->state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL) && (rsd->custom_content.normal))
     {
        elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "selector"), "selector,bg_image", rsd->custom_content.normal);
     }
   else if ((rsd->state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED) && (rsd->custom_content.pressed))
     {
        elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "selector"), "selector,bg_image", rsd->custom_content.pressed);
     }
   else
     {
        elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "selector"), "selector,bg_image", rsd->selector_bg);
     }

   if ((rsd->state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL) && rsd->selector_bg_image_color.normal_color.set_flag)
     {
        if (rsd->custom_content.normal)
          {
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(rsd->custom_content.normal, rsd, selector_bg_image_color, normal_color);
          }
     }
   else if ((rsd->state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED) && rsd->selector_bg_image_color.pressed_color.set_flag)
     {
        if (rsd->custom_content.pressed)
          {
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(rsd->custom_content.pressed, rsd, selector_bg_image_color, pressed_color);
          }
     }

   _selector_content_update(rsd);
}

static void
_selector_content_update(Eext_Rotary_Selector_Data *rsd)
{
   int i = 0;
   Eina_List *l = NULL;
   Eext_Rotary_Selector_Item *item = NULL;
   Evas_Object *content;

   EINA_LIST_FOREACH(rsd->item_list, l, item)
     {
        if (item->index == rsd->selected_index)
          {
             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && (item->selector_content.normal))
               {
                  content = elm_object_part_content_get(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content");
                  if (content)
                    {
                       if (content != item->selector_content.normal)
                         {
                            elm_object_part_content_unset(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content");
                            evas_object_hide(content);
                            elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content,show", "eext");
                            elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content", item->selector_content.normal);
                         }
                    }
                  else
                    {
                       elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content,show", "eext");
                       elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content", item->selector_content.normal);
                    }

               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && (item->selector_content.pressed))
               {
                  content = elm_object_part_content_get(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content");
                  if (content)
                    {
                       if (content != item->selector_content.pressed)
                         {
                            elm_object_part_content_unset(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content");
                            evas_object_hide(content);
                            elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content,show", "eext");
                            elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content", item->selector_content.pressed);
                         }
                    }
                  else
                    {
                       elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content,show", "eext");
                       elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "custom_content", item->selector_content.pressed);
                    }
               }
             else
               {
                  content = elm_object_part_content_get(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,icon");
                  if (content)
                    {
                       elm_object_part_content_unset(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,icon");
                       evas_object_hide(content);
                    }
                  elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "content,default", "eext");

                  if (((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && (item->selector_icon.normal)) ||
                      ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && (item->selector_icon.pressed)))
                    {
                       if (item->text1)
                         {
                            elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "text1,icon,show", "eext");
                         }
                       if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && (item->selector_icon.normal))
                         {
                            if (content != item->selector_icon.normal)
                               elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,icon", item->selector_icon.normal);
                         }
                       else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && (item->selector_icon.pressed))
                         {
                            if (content != item->selector_icon.pressed)
                               elm_object_part_content_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,icon", item->selector_icon.pressed);
                         }
                    }
                  else
                    {
                       if (item->text2)
                         {
                            elm_layout_signal_emit(elm_object_part_content_get(rsd->rotary_selector, "content"), "text1,text2,show", "eext");
                         }
                    }

                  if (item->domain)
                    {
                       elm_object_domain_translatable_part_text_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,main_text", item->domain, item->text1);
                       elm_object_domain_translatable_part_text_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,sub_text", item->domain, item->text2);
                    }
                  else
                    {
                       elm_object_part_text_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,main_text", item->text1);
                       elm_object_part_text_set(elm_object_part_content_get(rsd->rotary_selector, "content"), "selector,sub_text", item->text2);
                    }
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->item_bg_image.normal)
               {
                  content = elm_object_part_content_get(item->base.obj, "item,bg_image");
                  if (content)
                    {
                       if (content != item->item_bg_image.normal)
                         {
                            elm_object_part_content_unset(item->base.obj, "item,bg_image");
                            evas_object_hide(content);
                            elm_object_part_content_set(item->base.obj, "item,bg_image", item->item_bg_image.normal);
                         }
                    }
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->item_bg_image.pressed)
               {
                  content = elm_object_part_content_get(item->base.obj, "item,bg_image");
                  if (content)
                    {
                       if (content != item->item_bg_image.pressed)
                         {
                            elm_object_part_content_unset(item->base.obj, "item,bg_image");
                            evas_object_hide(content);
                            elm_object_part_content_set(item->base.obj, "item,bg_image", item->item_bg_image.pressed);
                         }
                    }
               }
             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->item_icon.normal)
               {
                  content = elm_object_part_content_get(item->base.obj, "item,icon");
                  if (content)
                    {
                       if (content != item->item_icon.normal)
                         {
                            elm_object_part_content_unset(item->base.obj, "item,icon");
                            evas_object_hide(content);
                            elm_object_part_content_set(item->base.obj, "item,icon", item->item_icon.normal);
                         }
                    }
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->item_icon.pressed)
               {
                  content = elm_object_part_content_get(item->base.obj, "item,icon");
                  if (content)
                    {
                       if (content != item->item_icon.pressed)
                         {
                            elm_object_part_content_unset(item->base.obj, "item,icon");
                            evas_object_hide(content);
                            elm_object_part_content_set(item->base.obj, "item,icon", item->item_icon.pressed);
                         }
                    }
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->item_bg_image_color.normal_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->item_bg_image.normal, item, item_bg_image_color, normal_color);
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->item_bg_image_color.pressed_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->item_bg_image.pressed, item, item_bg_image_color, pressed_color);
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->item_icon_color.normal_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->item_icon.normal, item, item_icon_color, normal_color);
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->item_icon_color.pressed_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->item_icon.pressed, item, item_icon_color, pressed_color);
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->selector_icon_color.normal_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->selector_icon.normal, item, selector_icon_color, normal_color);
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->selector_icon_color.pressed_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->selector_icon.pressed, item, selector_icon_color, pressed_color);
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->custom_content_color.normal_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->selector_content.normal, item, custom_content_color, normal_color);
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->custom_content_color.pressed_color.set_flag)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(item->selector_content.pressed, item, custom_content_color, pressed_color);
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->selector_main_text_color.normal_color.set_flag)
               {
                  char user_style[255] = {0};
                  _rgba_to_user_text_style_str(user_style,
                                               item->selector_main_text_color.normal_color.r,
                                               item->selector_main_text_color.normal_color.g,
                                               item->selector_main_text_color.normal_color.b,
                                               item->selector_main_text_color.normal_color.a);
                  content = elm_object_part_content_get(rsd->rotary_selector, "content");
                  edje_object_part_text_style_user_push(elm_layout_edje_get(content), "selector,main_text", user_style);
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->selector_main_text_color.pressed_color.set_flag)
               {
                  char user_style[255] = {0};
                  _rgba_to_user_text_style_str(user_style,
                                               item->selector_main_text_color.pressed_color.r,
                                               item->selector_main_text_color.pressed_color.g,
                                               item->selector_main_text_color.pressed_color.b,
                                               item->selector_main_text_color.pressed_color.a);
                  content = elm_object_part_content_get(rsd->rotary_selector, "content");
                  edje_object_part_text_style_user_push(elm_layout_edje_get(content), "selector,main_text", user_style);
               }
              else
               {
                  content = elm_object_part_content_get(rsd->rotary_selector, "content");
                  edje_object_part_text_style_user_pop(elm_layout_edje_get(content), "selector,main_text");
               }

             if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL) && item->selector_sub_text_color.normal_color.set_flag)
               {
                  char user_style[255] = {0};
                  _rgba_to_user_text_style_str(user_style,
                                               item->selector_sub_text_color.normal_color.r,
                                               item->selector_sub_text_color.normal_color.g,
                                               item->selector_sub_text_color.normal_color.b,
                                               item->selector_sub_text_color.normal_color.a);
                  content = elm_object_part_content_get(rsd->rotary_selector, "content");
                  edje_object_part_text_style_user_push(elm_layout_edje_get(content), "selector,sub_text", user_style);
               }
             else if ((item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED) && item->selector_sub_text_color.pressed_color.set_flag)
               {
                  char user_style[255] = {0};
                  _rgba_to_user_text_style_str(user_style,
                                               item->selector_sub_text_color.pressed_color.r,
                                               item->selector_sub_text_color.pressed_color.g,
                                               item->selector_sub_text_color.pressed_color.b,
                                               item->selector_sub_text_color.pressed_color.a);
                  content = elm_object_part_content_get(rsd->rotary_selector, "content");
                  edje_object_part_text_style_user_push(elm_layout_edje_get(content), "selector,sub_text", user_style);
               }
              else
               {
                  content = elm_object_part_content_get(rsd->rotary_selector, "content");
                  edje_object_part_text_style_user_pop(elm_layout_edje_get(content), "selector,sub_text");
               }
          }
        i++;
     }
}

static void
_selector_clicked_signal(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;

   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   rsd->state = EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL;
   evas_object_smart_callback_call(rsd->rotary_selector, "item,clicked", (void*)_item_get(rsd, rsd->selected_index));
   _selector_update(rsd, EINA_FALSE);
}

static void
_selector_pressed_signal(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_Rotary_Selector_Data *rsd = (Eext_Rotary_Selector_Data *)data;

   rsd->state = EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED;
   _selector_update(rsd, EINA_FALSE);
}

// ----------------------------------------- public API implementation --------------------------------------//
EAPI Evas_Object *
eext_rotary_selector_add(Evas_Object *parent)
{
   if (!parent) return NULL;

   Eext_Rotary_Selector_Data *rsd = NULL;

   Evas_Object *rotary_selector = elm_layout_add(parent);
   if (!rotary_selector)
     {
        LOGE("rotary_selector is NULL!!");
        return NULL;
     }

   elm_layout_theme_set(rotary_selector, "layout", "rotary_selector", "default");
   evas_object_move(rotary_selector, 0, 0);
   evas_object_resize(rotary_selector, _ROTARY_SELECTOR_SCREEN_WIDTH, _ROTARY_SELECTOR_SCREEN_HEIGHT);

   rsd = _eext_rotary_selector_data_init(rotary_selector, parent);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   evas_object_data_set(rotary_selector, EEXT_ROTARY_SELECTOR_DATA_KEY, rsd);

   Evas_Object *selector = elm_layout_add(rotary_selector);
   if (!selector)
     {
        LOGE("selector is NULL!!");
        return NULL;
     }

   elm_layout_theme_set(selector, "layout/rotary_selector", "selector", "default");
   edje_object_signal_callback_add(elm_layout_edje_get(rsd->rotary_selector), "eext,clicked", "", _selector_clicked_signal, rsd);
   edje_object_signal_callback_add(elm_layout_edje_get(rsd->rotary_selector), "eext,pressed", "", _selector_pressed_signal, rsd);

   //default selector bg part
   rsd->selector_bg = elm_layout_add(rotary_selector);
   if (!rsd->selector_bg)
     {
        LOGE("selector_bg is NULL!!");
        return NULL;
     }

   elm_layout_theme_set(rsd->selector_bg, "layout/rotary_selector", "selector", "selector_bg");
   elm_object_part_content_set(selector, "selector,bg_image", rsd->selector_bg);

   elm_object_part_content_set(rotary_selector, "selector", selector);

   //default selector content part
   Evas_Object *content = elm_layout_add(rotary_selector);
   if (!content)
     {
        LOGE("content is NULL!!");
        return NULL;
     }

   elm_layout_theme_set(content, "layout/rotary_selector", "content", "default");
   elm_object_part_content_set(rotary_selector, "content", content);

   eext_rotary_object_event_callback_add(rotary_selector, _rotary_selector_rotary_cb, rsd);
   evas_object_event_callback_add(rotary_selector, EVAS_CALLBACK_MOVE, _rotary_selector_move_cb, rsd);
   evas_object_event_callback_add(rotary_selector, EVAS_CALLBACK_DEL, _rotary_selector_del_cb, rsd);
   evas_object_event_callback_add(rotary_selector, EVAS_CALLBACK_SHOW, _rotary_selector_show_cb, rsd);

   return rotary_selector;
}

EAPI Eext_Object_Item *
eext_rotary_selector_item_append(Evas_Object *obj)
{
   Evas_Object* rotary_selector = obj;
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *item = NULL;

   rsd = _eext_rotary_selector_data_get(rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   item = _item_create(rsd);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   rsd->item_list = eina_list_append(rsd->item_list, item);

   _item_rearrange(rsd, rsd->selected_index);

   _item_update_animation_run(rsd);

   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item *
eext_rotary_selector_item_prepend(Evas_Object *obj)
{
   Evas_Object* rotary_selector = obj;
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *item = NULL;

   rsd = _eext_rotary_selector_data_get(rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   item = _item_create(rsd);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   rsd->item_list = eina_list_prepend(rsd->item_list, item);

   rsd->selected_index += 1;
   _item_rearrange(rsd, rsd->selected_index);

   _item_update_animation_run(rsd);

   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item *
eext_rotary_selector_item_insert_after(Evas_Object *obj, Eext_Object_Item *after)
{
   int selected_index = 0;
   Evas_Object* rotary_selector = obj;
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *item = NULL;
   Eext_Rotary_Selector_Item *after_item = (Eext_Rotary_Selector_Item *)after;

   rsd = _eext_rotary_selector_data_get(rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   item = _item_create(rsd);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   rsd->item_list = eina_list_append_relative(rsd->item_list, item, after_item);

   if (rsd->selected_index <= after_item->index)
     {
        selected_index = rsd->selected_index;
     }
   else if (rsd->selected_index > after_item->index)
     {
        selected_index = rsd->selected_index + 1;
     }

   rsd->selected_index = selected_index;
   _item_rearrange(rsd, rsd->selected_index);

   _item_update_animation_run(rsd);

   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item *
eext_rotary_selector_item_insert_before(Evas_Object *obj, Eext_Object_Item *before)
{
   int selected_index = 0;
   Evas_Object* rotary_selector = obj;
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *item = NULL;
   Eext_Rotary_Selector_Item *before_item = (Eext_Rotary_Selector_Item *)before;

   rsd = _eext_rotary_selector_data_get(rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   item = _item_create(rsd);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   rsd->item_list = eina_list_prepend_relative(rsd->item_list, item, before_item);

   if (rsd->selected_index >= before_item->index)
     {
        selected_index = rsd->selected_index + 1;
     }
   else if (rsd->selected_index < before_item->index)
     {
        selected_index = rsd->selected_index;
     }

   rsd->selected_index = selected_index;
   _item_rearrange(rsd, rsd->selected_index);

   _item_update_animation_run(rsd);

   return (Eext_Object_Item *)item;
}

EAPI void
eext_rotary_selector_item_del(Eext_Object_Item *item)
{
   int selected_index = 0;
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)item;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }

   rsd = _eext_rotary_selector_data_get(rotary_selector_item->rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   if (rsd->selected_index < rotary_selector_item->index)
     {
        selected_index = rsd->selected_index;
     }
   else if (rsd->selected_index > rotary_selector_item->index)
     {
        selected_index = rsd->selected_index - 1;
     }

   rsd->item_list = eina_list_remove(rsd->item_list, rotary_selector_item);
   evas_object_del(rotary_selector_item->base.obj);

   rsd->selected_index = selected_index;
   _item_rearrange(rsd, rsd->selected_index);

   _item_update_animation_run(rsd);

   return;
}

EAPI void
eext_rotary_selector_items_clear(Evas_Object *obj)
{
   Evas_Object* rotary_selector = obj;
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *item = NULL;

   if (!rotary_selector)
     {
        LOGE("rotary_selector is NULL!!");
        return;
     }

   rsd = _eext_rotary_selector_data_get(rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   EINA_LIST_FREE(rsd->item_list, item)
     evas_object_del(item->base.obj);

   rsd->selected_index = 0;

   _item_update_animation_run(rsd);

   return;
}

EAPI void
eext_rotary_selector_item_part_text_set(Eext_Object_Item *item, const char *part_name, const char *text)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)item;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }
   if (!text)
     {
        LOGE("text is NULL!!");
        return;
     }

   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get(rotary_selector_item->rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   if (!strcmp(part_name, "selector,main_text"))
     {
        _item_text_copy(&(rotary_selector_item->text1), text);
     }
   else if (!strcmp(part_name, "selector,sub_text"))
     {
        _item_text_copy(&(rotary_selector_item->text2), text);
     }
   else
     {
       LOGE("Not supported part name [%s]", part_name);
       return;
     }

   _item_update_animation_run(rsd);

   return;
}

EAPI const char *
eext_rotary_selector_item_part_text_get(const Eext_Object_Item *item, const char *part_name)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item*)item;

   if (!rotary_selector_item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return NULL;
     }

   if (!strcmp(part_name, "selector,main_text"))
     {
       return rotary_selector_item->text1;
     }
   else if (!strcmp(part_name, "selector,sub_text"))
     {
        return rotary_selector_item->text2;
     }

   return NULL;
}

EAPI void
eext_rotary_selector_item_domain_translatable_part_text_set(Eext_Object_Item *item, const char *part_name, const char *domain, const char *text)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)item;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }
   if (!domain)
     {
        LOGE("domain is NULL!!");
        return;
     }
   if (!text)
     {
        LOGE("text is NULL!!");
        return;
     }

   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get(rotary_selector_item->rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   if (!strcmp(part_name, "selector,main_text"))
     {
        _item_text_copy(&(rotary_selector_item->domain), domain);
        _item_text_copy(&(rotary_selector_item->text1), text);
     }
   else if (!strcmp(part_name, "selector,sub_text"))
     {
        _item_text_copy(&(rotary_selector_item->domain), domain);
        _item_text_copy(&(rotary_selector_item->text2), text);
     }
   else
     {
        LOGE("Not supported part name [%s]", part_name);
        return;
     }

   _item_update_animation_run(rsd);

   return;
}

EAPI void
eext_rotary_selector_item_part_content_set(Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state, Evas_Object *content)
{
   //Supported part names : "item,bg_image", "item,icon", "selector,icon", "selector,content"

   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)item;
   Evas_Object *temp_content;
   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }
   if (!content)
     {
        LOGE("content is NULL!!");
        return;
     }

   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get(rotary_selector_item->rotary_selector);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
     {
        if (!strcmp(part_name, "item,bg_image"))
          {
             if (rotary_selector_item->item_bg_image_color.normal_color.set_flag)
                ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, item_bg_image_color, normal_color);
             if (rotary_selector_item->state == state)
               {
                  temp_content = elm_object_part_content_unset(rotary_selector_item->base.obj, part_name);
                  if (temp_content)
                     evas_object_hide(temp_content);
                  elm_object_part_content_set(rotary_selector_item->base.obj, part_name, content);
               }

             rotary_selector_item->item_bg_image.normal = content;
          }
        else if (!strcmp(part_name, "item,icon"))
          {

             if (rotary_selector_item->item_icon_color.normal_color.set_flag)
                ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, item_icon_color, normal_color);
             if (rotary_selector_item->state == state)
               {
                  temp_content = elm_object_part_content_unset(rotary_selector_item->base.obj, part_name);
                  if (temp_content)
                     evas_object_hide(temp_content);
                  elm_object_part_content_set(rotary_selector_item->base.obj, part_name, content);
               }

             rotary_selector_item->item_icon.normal = content;
          }
        else if (!strcmp(part_name, "selector,icon"))
          {
             rotary_selector_item->selector_icon.normal = content;
          }
        else if (!strcmp(part_name, "selector,content"))
          {
             rotary_selector_item->selector_content.normal = content;
          }
        else
          {
             LOGE("Not supported part name [%s]", part_name);
             return;
          }
     }
   else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
     {
        if (!strcmp(part_name, "item,bg_image"))
          {
             if (rotary_selector_item->item_bg_image_color.pressed_color.set_flag)
                ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, item_bg_image_color, pressed_color);
             if (rotary_selector_item->state == state)
               {
                  temp_content = elm_object_part_content_unset(rotary_selector_item->base.obj, part_name);
                  if (temp_content)
                     evas_object_hide(temp_content);
                  elm_object_part_content_set(rotary_selector_item->base.obj, part_name, content);
               }

             rotary_selector_item->item_bg_image.pressed = content;
          }
        else if (!strcmp(part_name, "item,icon"))
          {
             if (rotary_selector_item->item_icon_color.pressed_color.set_flag)
                ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, item_icon_color, pressed_color);
             if (rotary_selector_item->state == state)
               {
                  temp_content = elm_object_part_content_unset(rotary_selector_item->base.obj, part_name);
                  if (temp_content)
                     evas_object_hide(temp_content);
                  elm_object_part_content_set(rotary_selector_item->base.obj, part_name, content);
               }

             rotary_selector_item->item_icon.pressed = content;
          }
        else if (!strcmp(part_name, "selector,icon"))
          {
             rotary_selector_item->selector_icon.pressed = content;
          }
        else if (!strcmp(part_name, "selector,content"))
          {
             rotary_selector_item->selector_content.pressed = content;
          }
        else
          {
             LOGE("Not supported part name [%s]", part_name);
             return;
          }
     }

   _item_update_animation_run(rsd);
}

EAPI Evas_Object *
eext_rotary_selector_item_part_content_get(const Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item*)item;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return NULL;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return NULL;
     }

   if (!strcmp(part_name, "item,bg_image"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          return rotary_selector_item->item_bg_image.normal;
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          return rotary_selector_item->item_bg_image.pressed;
     }
   else if (!strcmp(part_name, "item,icon"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          return rotary_selector_item->item_icon.normal;
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          return rotary_selector_item->item_icon.pressed;
     }
   else if (!strcmp(part_name, "selector,icon"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          return rotary_selector_item->selector_icon.normal;
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          return rotary_selector_item->selector_icon.pressed;
     }
   else if (!strcmp(part_name, "selector,content"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          return rotary_selector_item->selector_content.normal;
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          return rotary_selector_item->selector_content.pressed;
     }
   else
     {
       LOGE("Not supported part name [%s]", part_name);
       return NULL;
     }

   return NULL;
}

EAPI void
eext_rotary_selector_part_content_set(Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state, Evas_Object *content)
{
   //Supported part names : "selector,bg_image"

   if (!obj)
     {
        LOGE("obj is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }
   if (!content)
     {
        LOGE("content is NULL!!");
        return;
     }

   if (strcmp(part_name, "selector,bg_image"))
   {
      LOGE("Not supported part name!!!");
      return;
   }

   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get(obj);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }

   if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL)
     rsd->custom_content.normal = content;
   else if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED)
     rsd->custom_content.pressed = content;

   _item_update_animation_run(rsd);
}

EAPI Evas_Object *
eext_rotary_selector_part_content_get(const Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state)
{
   //Supported part names : "selector,bg_image"
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return NULL;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return NULL;
     }

   if (strcmp(part_name, "selector,bg_image"))
     {
        LOGE("Not supported part name!!!");
        return NULL;
     }

   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get((Evas_Object *)obj);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL)
     return rsd->custom_content.normal;
   else if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED)
     return rsd->custom_content.pressed;

   return NULL;
}

EAPI void
eext_rotary_selector_item_part_color_set(Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state, int r, int g, int b, int a)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item*)item;
   Evas_Object *content;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }

   if (!strcmp(part_name, "item,bg_image"))
     {

        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, item_bg_image_color, normal_color, r, g, b, a);
             if (rotary_selector_item->item_bg_image.normal)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(rotary_selector_item->item_bg_image.normal, rotary_selector_item, item_bg_image_color, normal_color);
               }

          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, item_bg_image_color, pressed_color, r, g, b, a);
             if (rotary_selector_item->item_bg_image.pressed)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(rotary_selector_item->item_bg_image.pressed, rotary_selector_item, item_bg_image_color, pressed_color);
               }
          }
     }
   else if (!strcmp(part_name, "item,icon"))
     {

        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, item_icon_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, item_icon_color, pressed_color, r, g, b, a);
          }

        if (rotary_selector_item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             content = eext_rotary_selector_item_part_content_get(item, "item,icon",EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL);
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, item_icon_color, normal_color);
          }
        else if (rotary_selector_item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             content = eext_rotary_selector_item_part_content_get(item, "item,icon",EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED);
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, item_icon_color, pressed_color);
          }
     }
   else if (!strcmp(part_name, "selector,icon"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, selector_icon_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, selector_icon_color, pressed_color, r, g, b, a);
          }

        if (rotary_selector_item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             content = eext_rotary_selector_item_part_content_get(item, "selector,icon",EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL);
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, selector_icon_color, normal_color);
          }
        else if (rotary_selector_item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             content = eext_rotary_selector_item_part_content_get(item, "selector,icon",EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED);
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, selector_icon_color, pressed_color);
          }
     }
   else if (!strcmp(part_name, "selector,content"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, custom_content_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, custom_content_color, pressed_color, r, g, b, a);
          }

        if (rotary_selector_item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             content = eext_rotary_selector_item_part_content_get(item, "selector,content",EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL);
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, custom_content_color, normal_color);
          }
        else if (rotary_selector_item->state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             content = eext_rotary_selector_item_part_content_get(item, "selector,content",EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED);
             ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(content, rotary_selector_item, custom_content_color, pressed_color);
          }
     }
   else if (!strcmp(part_name, "selector,main_text"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, selector_main_text_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, selector_main_text_color, pressed_color, r, g, b, a);
          }
        if (rotary_selector_item->state == state)
          {
             char user_style[255] = {0};
             _rgba_to_user_text_style_str(user_style, r, g, b, a);
             content = elm_object_part_content_get(rotary_selector_item->rotary_selector, "content");
             edje_object_part_text_style_user_push(elm_layout_edje_get(content), "selector,main_text", user_style);
          }
     }
   else if (!strcmp(part_name, "selector,sub_text"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, selector_sub_text_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rotary_selector_item, selector_sub_text_color, pressed_color, r, g, b, a);
          }
        if (rotary_selector_item->state == state)
          {
             char user_style[255] = {0};
             _rgba_to_user_text_style_str(user_style, r, g, b, a);
             content = elm_object_part_content_get(rotary_selector_item->rotary_selector, "content");
             edje_object_part_text_style_user_push(elm_layout_edje_get(content), "selector,sub_text", user_style);
          }
     }
   else
     {
       LOGE("Not supported part name [%s]", part_name);
       return;
     }

   return;
}

EAPI void
eext_rotary_selector_item_part_color_get(const Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state, int *r, int *g, int *b, int *a)
{
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item*)item;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }

   if (!strcmp(part_name, "item,bg_image") )
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, item_bg_image_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, item_bg_image_color, pressed_color, r, g, b, a);
          }
     }
   else if (!strcmp(part_name, "item,icon"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, item_icon_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, item_icon_color, pressed_color, r, g, b, a);
          }
     }
   else if (!strcmp(part_name, "selector,icon"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, selector_icon_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, selector_icon_color, pressed_color, r, g, b, a);
          }
     }
   else if (!strcmp(part_name, "selector,content"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, custom_content_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, custom_content_color, pressed_color, r, g, b, a);
          }
     }
   else if (!strcmp(part_name, "selector,main_text"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, selector_main_text_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, selector_main_text_color, pressed_color, r, g, b, a);
          }
     }
   else if (!strcmp(part_name, "selector,sub_text"))
     {
        if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, selector_sub_text_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rotary_selector_item, selector_sub_text_color, pressed_color, r, g, b, a);
          }
     }
   else
     {
       LOGE("Not supported part name [%s]", part_name);
       return;
     }
}

EAPI void
eext_rotary_selector_part_color_set(Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state, int r, int g, int b, int a)
{
   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get((Evas_Object *)obj);

   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }

   if (!strcmp(part_name, "item,icon") || !strcmp(part_name, "selector,bg_image"))
     {
        if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rsd, selector_bg_image_color, normal_color, r, g, b, a);
             if (rsd->custom_content.normal)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(rsd->custom_content.normal, rsd, selector_bg_image_color, normal_color);
               }
          }
        else if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_SET(rsd, selector_bg_image_color, pressed_color, r, g, b, a);
             if (rsd->custom_content.pressed)
               {
                  ROTARY_SELECTOR_EVAS_OBJECT_COLOR_SET(rsd->custom_content.pressed, rsd, selector_bg_image_color, pressed_color);
               }
          }

        return;
     }
   else
     {
       LOGE("Not supported part name [%s]", part_name);
       return;
     }

   return;
}

EAPI void
eext_rotary_selector_part_color_get(const Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state, int *r, int *g, int *b, int *a)
{
   Eext_Rotary_Selector_Data *rsd = NULL;
   rsd = _eext_rotary_selector_data_get((Evas_Object *)obj);

   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }
   if (!part_name)
     {
        LOGE("part_name is NULL!!");
        return;
     }

   if (!strcmp(part_name, "item,icon") || !strcmp(part_name, "selector,bg_image"))
     {
        if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rsd, selector_bg_image_color, normal_color, r, g, b, a);
          }
        else if (state == EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED)
          {
             ROTARY_SELECTOR_COLOR_VALUE_GET(rsd, selector_bg_image_color, pressed_color, r, g, b, a);
          }
        return;
     }
   else
     {
       LOGE("Not supported part name [%s]", part_name);
       return;
     }
}

EAPI void
eext_rotary_selector_selected_item_set(Evas_Object *obj, Eext_Object_Item *item)
{
   Eext_Rotary_Selector_Data *rsd = NULL;
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)item;

   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }

   rsd = _eext_rotary_selector_data_get(obj);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return;
     }
   if (rsd->item_update_animator)
     {
        return;
     }

   int previous_index = rsd->selected_index;
   rsd->selected_index = rotary_selector_item->index;

   _item_selected_signal_send(rsd, previous_index, rsd->selected_index);
   rsd->current_page = rsd->selected_index/_ROTARY_SELECTOR_PAGE_ITEM_MAX;
   _item_update_animation_run(rsd);
}

EAPI Eext_Object_Item*
eext_rotary_selector_selected_item_get(const Evas_Object *obj)
{
   Eext_Rotary_Selector_Data *rsd = _eext_rotary_selector_data_get((Evas_Object *)obj);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   return (Eext_Object_Item*)_item_get(rsd, rsd->selected_index);
}

EAPI const Eina_List*
eext_rotary_selector_items_get(const Evas_Object *obj)
{
   Eext_Rotary_Selector_Data *rsd = _eext_rotary_selector_data_get((Evas_Object *)obj);
   if (!rsd)
     {
        LOGE("rsd is NULL!!");
        return NULL;
     }

   return rsd->item_list;
}
