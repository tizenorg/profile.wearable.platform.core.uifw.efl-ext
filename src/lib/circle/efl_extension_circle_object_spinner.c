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

#define EEXT_CIRCLE_OBJECT_SPINNER_TYPE "Eext_Circle_Object_Spinner"
#define SPINNER_ITEM_NAME "default"

#define EEXT_CIRCLE_OBJECT_SPINNER_DATA_GET(circle_obj, data) \
   Eext_Circle_Object_Spinner_Data *data = NULL; \
   if (circle_obj && circle_obj->widget_data && \
       circle_obj->widget_type && !strcmp(circle_obj->widget_type, EEXT_CIRCLE_OBJECT_SPINNER_TYPE)) \
      data = (Eext_Circle_Object_Spinner_Data *)circle_obj->widget_data; \
   if (!data)

static const double MARK_WIDTH = 6.0;
static const double MARK_HEIGHT = 23.0;
static const double RADIUS = 168.5; //BG_HEIGHT / 2 - MARK_HEIGHT / 2;
static const double MARK_RADIUS = 2.000317;
// chord = 2 * radius * sin(x/2), x = arcsin(sin(x)), arcsin(x) = 1 / sqrt(1 - pow(x, 2))

static const double END_EFFECT_ANGLE = 15.0;

static const double ANIMATION_DURATION = 0.3;

static void
_eext_circle_object_spinner_mark_angle_offset_set(Eext_Circle_Object *obj, double angle)
{
   Eext_Circle_Object_Item *item;

   item = _eext_circle_object_item_get(obj, SPINNER_ITEM_NAME);

   _eext_circle_object_item_angle_offset_set(item, angle - (MARK_RADIUS / 2));
   _eext_circle_object_item_angle_set(item, MARK_RADIUS);
}

static Eina_Bool
_end_effect_expired_timer_cb(void *data)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)circle_obj->widget_data;
   Eext_Circle_Object_Item *item;

   item = _eext_circle_object_item_get(circle_obj, SPINNER_ITEM_NAME);

   _eext_circle_object_item_angle_transit_set(item, ANIMATION_DURATION,
                                         MARK_RADIUS, widget_data->to_angle_offset - (MARK_RADIUS / 2),
                                         EINA_TRUE,
                                         0.25, 0.46,
                                         0.45, 1.0);

   widget_data->end_effect_expired_timer = NULL;

   return ECORE_CALLBACK_CANCEL;
}

static void
_eext_circle_object_spinner_mark_update(Eext_Circle_Object *obj, Eina_Bool animate)
{
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)obj->widget_data;

   if (evas_object_visible_get(obj->widget_object) && animate)
     {
        Eext_Circle_Object_Item *item;

        item = _eext_circle_object_item_get(obj, SPINNER_ITEM_NAME);

        if (widget_data->end_effect_angle)
          {
             if (widget_data->end_effect_expired_timer) return;

             _eext_circle_object_item_angle_transit_set(item, ANIMATION_DURATION,
                                                   MARK_RADIUS, widget_data->to_angle_offset + widget_data->end_effect_angle - (MARK_RADIUS / 2),
                                                   EINA_TRUE,
                                                   0.25, 0.46,
                                                   0.45, 1.0);

             widget_data->end_effect_expired_timer = ecore_timer_add(ANIMATION_DURATION, _end_effect_expired_timer_cb, obj);
          }
        else
          {
             if (widget_data->end_effect_expired_timer)
               {
                  ecore_timer_del(widget_data->end_effect_expired_timer);
                  widget_data->end_effect_expired_timer = NULL;
               }

             _eext_circle_object_item_angle_transit_set(item, ANIMATION_DURATION,
                                                   MARK_RADIUS, widget_data->to_angle_offset - (MARK_RADIUS / 2),
                                                   EINA_TRUE,
                                                   0.25, 0.46,
                                                   0.45, 1.0);
          }
     }
   else
     _eext_circle_object_spinner_mark_angle_offset_set(obj, widget_data->to_angle_offset);
}

static double
_eext_circle_object_spinner_angle_get(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)obj->widget_data;
   double min, max, angle;

   if (widget_data->angle) return widget_data->angle;

   elm_spinner_min_max_get(obj->widget_object, &min, &max);

   if (elm_spinner_wrap_get(obj->widget_object))
     angle = 360 / (max - min + 1);
   else
     {
        if (min == max)
          angle = 0.0;
        else
          angle = 360 / (max - min);
     }

   return angle;
}

static double
_eext_circle_object_spinner_angle_offset_get(Eext_Circle_Object *obj, double val)
{
   double min, angle_offset, real_part;

   elm_spinner_min_max_get(obj->widget_object, &min, NULL);

   angle_offset = (val - min) * _eext_circle_object_spinner_angle_get(obj);
   real_part = angle_offset - (int)angle_offset;
   angle_offset = (int)angle_offset % 360 + real_part;

   return angle_offset;
}

static void
_eext_circle_object_spinner_update(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)obj->widget_data;
   double val;

   val = elm_spinner_value_get(obj->widget_object);

   widget_data->to_angle_offset = _eext_circle_object_spinner_angle_offset_get(obj, val);

   _eext_circle_object_spinner_mark_update(obj, EINA_FALSE);
}

static void
_eext_circle_object_spinner_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)circle_obj->widget_data;

   if (widget_data->rotary_event)
     _eext_circle_object_spinner_mark_update(circle_obj, EINA_TRUE);
   else
     _eext_circle_object_spinner_update(circle_obj);
}

static double
_looping_angle_distance_get(double from_angle_offset, double to_angle_offset, Eina_Bool value_up)
{
   double angle_distance = 0;

   if (value_up)
     {
        if (from_angle_offset > to_angle_offset)
          angle_distance = 360 - from_angle_offset + to_angle_offset;
        else if  (from_angle_offset < to_angle_offset)
          angle_distance = to_angle_offset - from_angle_offset;
     }
   else
     {
        if (from_angle_offset > to_angle_offset)
          angle_distance = from_angle_offset - to_angle_offset;
        else if  (from_angle_offset < to_angle_offset)
          angle_distance = from_angle_offset + 360 - to_angle_offset;
     }

   return angle_distance;
}

static void
_eext_circle_object_spinner_value_update(Eext_Circle_Object *obj, Eina_Bool value_up)
{
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)obj->widget_data;
   double val, step, min, max;

   val = elm_spinner_value_get(obj->widget_object);

   step = elm_spinner_step_get(obj->widget_object);
   elm_spinner_min_max_get(obj->widget_object, &min, &max);

   widget_data->end_effect_angle = 0.0;

   if (elm_spinner_wrap_get(obj->widget_object))
     {
        if (value_up)
          {
             if (val + step > max)
               {
                  widget_data->to_angle_offset += _looping_angle_distance_get(
                    _eext_circle_object_spinner_angle_offset_get(obj, max),
                    _eext_circle_object_spinner_angle_offset_get(obj, min),
                    value_up);
                  val = min;
               }
             else
               {
                  widget_data->to_angle_offset += _eext_circle_object_spinner_angle_get(obj) * step;
                  val += step;
               }
          }
        else
          {
             if (val - step < min)
               {
                  widget_data->to_angle_offset -= _looping_angle_distance_get(
                    _eext_circle_object_spinner_angle_offset_get(obj, min),
                    _eext_circle_object_spinner_angle_offset_get(obj, max),
                    value_up);
                  val = max;
               }
             else
               {
                  widget_data->to_angle_offset -= _eext_circle_object_spinner_angle_get(obj) * step;
                  val -= step;
               }
          }
     }
   else
     {
        if (value_up)
          {
             if (val + step > max)
               {
                  if (val == max)
                    widget_data->end_effect_angle = END_EFFECT_ANGLE;
                  else
                    widget_data->to_angle_offset += widget_data->angle * (max - val);
               }
             else
               widget_data->to_angle_offset += _eext_circle_object_spinner_angle_get(obj) * step;

             val += step;
          }
        else
          {
             if (val - step < min)
               {
                  if (val == min)
                    widget_data->end_effect_angle = -END_EFFECT_ANGLE;
                  else
                    widget_data->to_angle_offset -= widget_data->angle * (val - min);
               }
             else
               widget_data->to_angle_offset -= _eext_circle_object_spinner_angle_get(obj) * step;

             val -= step;
          }
     }

   widget_data->rotary_event = EINA_TRUE;
   elm_spinner_value_set(obj->widget_object, val);
   widget_data->rotary_event = EINA_FALSE;
}

static Eina_Bool
_rotary_timer_cb(void *data)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)circle_obj->widget_data;
   Eext_Circle_Object_Item *item;
   double angle_offset_diff;

   item = _eext_circle_object_item_get(circle_obj, SPINNER_ITEM_NAME);
   if (!item) return ECORE_CALLBACK_CANCEL;

   angle_offset_diff = fabs(_eext_circle_object_item_angle_offset_get(item)
                       - item->draw.angle_offset);

   if (angle_offset_diff <= 1.0e-8)
     elm_layout_signal_emit(elm_object_part_content_get(circle_obj->widget_object, "elm.swallow.text_button"),
         "elm,state,rotary,inactive", "eext");

   widget_data->rotary_timer = NULL;

   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_rotary_changed_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)circle_obj->widget_data;

   elm_layout_signal_emit(elm_object_part_content_get(circle_obj->widget_object, "elm.swallow.text_button"),
       "elm,state,rotary,active", "eext");

   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE)
     _eext_circle_object_spinner_value_update(circle_obj, EINA_TRUE);
   else
     _eext_circle_object_spinner_value_update(circle_obj, EINA_FALSE);

   if (widget_data->rotary_timer)
     {
        ecore_timer_del(widget_data->rotary_timer);
        widget_data->rotary_timer = NULL;
     }

   widget_data->rotary_timer = ecore_timer_add(1.0, _rotary_timer_cb, circle_obj);

   return EINA_TRUE;
}

static void
_eext_circle_object_spinner_del_cb(Eext_Circle_Object *obj)
{
   if (!obj) return;

   if (obj->widget_data)
     {
        Eext_Circle_Object_Spinner_Data *widget_data = (Eext_Circle_Object_Spinner_Data *)obj->widget_data;
        if (widget_data->end_effect_expired_timer)
          {
             ecore_timer_del(widget_data->end_effect_expired_timer);
             widget_data->end_effect_expired_timer = NULL;
          }

        if (widget_data->rotary_timer)
          {
             ecore_timer_del(widget_data->rotary_timer);
             widget_data->rotary_timer = NULL;
          }

        free(obj->widget_data);
        obj->widget_data = NULL;
     }
}

static void
_eext_circle_object_spinner_disabled_cb(Eext_Circle_Object *obj)
{
   if (obj->disabled)
     eext_rotary_object_event_callback_del(obj->widget_object, _rotary_changed_cb);
   else
     eext_rotary_object_event_callback_add(obj->widget_object, _rotary_changed_cb, obj);
}

static void
_eext_circle_object_spinner_init(Eext_Circle_Object *obj, Evas_Object *spinner)
{
   Eext_Circle_Object_Spinner_Data *data;
   Eext_Circle_Object_Item *item;

   obj->widget_object = spinner;
   obj->widget_type = EEXT_CIRCLE_OBJECT_SPINNER_TYPE;
   obj->del_func = _eext_circle_object_spinner_del_cb;
   obj->disabled_func = _eext_circle_object_spinner_disabled_cb;

   data = (Eext_Circle_Object_Spinner_Data *)calloc(1, sizeof(Eext_Circle_Object_Spinner_Data));
   obj->widget_data = (void *)data;

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, SPINNER_ITEM_NAME);
   _eext_circle_object_item_color_set(item, 250, 125, 47, 255);
   _eext_circle_object_item_radius_set(item, RADIUS);
   _eext_circle_object_item_line_width_set(item, MARK_HEIGHT);
   _eext_circle_object_item_angle_offset_set(item, item->angle_offset - (MARK_RADIUS / 2));
   _eext_circle_object_item_angle_set(item, MARK_RADIUS);
   _eext_circle_object_item_append(obj, item);

   elm_spinner_editable_set(spinner, EINA_FALSE);

   evas_object_smart_callback_add(spinner, "changed", _eext_circle_object_spinner_changed_cb, obj);

   eext_rotary_object_event_callback_add(obj->widget_object, _rotary_changed_cb, obj);
}

EAPI Evas_Object *
eext_circle_object_spinner_add(Evas_Object *spinner, Eext_Circle_Surface *surface)
{
   Evas_Object *obj;

   if (!spinner) return NULL;

   obj = elm_image_add(spinner);
   _eext_circle_object_init(obj, spinner, surface);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   _eext_circle_object_spinner_init(circle_obj, spinner);

   return obj;
}

EAPI void
eext_circle_object_spinner_angle_set(Evas_Object *obj, double angle)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_CIRCLE_OBJECT_SPINNER_DATA_GET(circle_obj, widget_data) return;

   if ((angle <= 0) || (widget_data->angle == angle)) return;

   widget_data->angle = angle;
}
