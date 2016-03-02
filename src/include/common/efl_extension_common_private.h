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

#ifndef __EFL_EXTENSION_COMMON_PRIVATE_H__
#define __EFL_EXTENSION_COMMON_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Eext_Index_Layout_Item Eext_Index_Layout_Item;
struct _Eext_Index_Layout_Item {
   EEXT_OBJECT_ITEM;
   Eext_Circle_Object_Item *item;

   const char *letter;
   int priority;
   Eina_Bool selected;

   Evas_Smart_Cb func;
};

/* Index Layout data */
typedef struct _Eext_Index_Layout_Data Eext_Index_Layout_Data;
struct _Eext_Index_Layout_Data {
   Evas_Object *main_ly;
   Evas_Object *panel;
   Evas_Object *index;
   Evas_Object *cue;
   Ecore_Timer *hide_timer;

   int max_index;
   int max_item;
   int highlighted_index;
   int index_priority;
   int *item_index;
   int default_item_count;

   Eina_List *current_list;
   Eina_List *item_list;
   Eina_List *omit_list;

   Eina_Bool panel_hide;
   Eina_Bool second_list_exist;
};

/* Rotary Selector data */
typedef struct _Eext_Rotary_Selector_Selector_Color Eext_Rotary_Selector_Selector_Color;
struct _Eext_Rotary_Selector_Selector_Color {
   int r;
   int g;
   int b;
   int a;
   Eina_Bool set_flag;
};

typedef struct _Eext_Rotary_Selector_State_Data Eext_Rotary_Selector_State_Data;
struct _Eext_Rotary_Selector_State_Data {
   Evas_Object *normal;
   Evas_Object *pressed;
};

typedef struct _Eext_Rotary_Selector_Selector_State_Color Eext_Rotary_Selector_Selector_State_Color;
struct _Eext_Rotary_Selector_Selector_State_Color {
   Eext_Rotary_Selector_Selector_Color normal_color;
   Eext_Rotary_Selector_Selector_Color pressed_color;
};

typedef struct _Eext_Rotary_Selector_Data Eext_Rotary_Selector_Data;
struct _Eext_Rotary_Selector_Data {
   Evas_Object *parent;
   Evas_Object *rotary_selector;
   Evas_Object *selector_bg;
   Eina_List *item_list;
   int item_count;
   int selected_index;
   int current_page;
   Eina_Bool animation_direction;
   Ecore_Animator *previous_page_animator;
   Ecore_Animator *next_page_animator;
   double previous_page_animator_start_time;
   double next_page_animator_start_time;
   Eina_Bool next_page_animation_started;
   Eina_Bool previous_page_animation_started;
   Ecore_Timer *next_page_timer;
   Eina_Bool end_effect_animation_started;
   double end_effect_animator_start_time;
   Ecore_Animator *end_effect_animator;
   Ecore_Animator *item_update_animator;
   Eext_Rotary_Selector_Selector_State state;
   Eext_Rotary_Selector_Selector_State_Color selector_bg_image_color;
   Eext_Rotary_Selector_State_Data custom_content;
};

typedef struct _Eext_Rotary_Selector_Item_Coords Eext_Rotary_Selector_Item_Coords;
struct _Eext_Rotary_Selector_Item_Coords {
   double x;
   double y;
};

typedef struct _Eext_Rotary_Selector_Item_Color Eext_Rotary_Selector_Item_Color;
struct _Eext_Rotary_Selector_Item_Color {
   int r;
   int g;
   int b;
   int a;
   Eina_Bool set_flag;
};

typedef struct _Eext_Rotary_Selector_Item_State_Color Eext_Rotary_Selector_Item_State_Color;
struct _Eext_Rotary_Selector_Item_State_Color {
   Eext_Rotary_Selector_Item_Color normal_color;
   Eext_Rotary_Selector_Item_Color pressed_color;
   Eext_Rotary_Selector_Item_Color disabled_color;
};

typedef struct _Eext_Rotary_Selector_Item Eext_Rotary_Selector_Item;
struct _Eext_Rotary_Selector_Item {
   EEXT_OBJECT_ITEM;
   int index;
   Evas_Object *parent;
   Evas_Object *rotary_selector;
   Eext_Rotary_Selector_Item_Coords coords;
   char *domain;
   char *text1;
   char *text2;
   Eext_Rotary_Selector_Item_State state;
   Eext_Rotary_Selector_Item_Color color;
   Eext_Rotary_Selector_State_Data item_bg_image;
   Eext_Rotary_Selector_State_Data item_icon;
   Eext_Rotary_Selector_State_Data selector_icon;
   Eext_Rotary_Selector_State_Data selector_content;
   Eext_Rotary_Selector_Item_State_Color item_bg_image_color;
   Eext_Rotary_Selector_Item_State_Color item_icon_color;
   Eext_Rotary_Selector_Item_State_Color selector_icon_color;
   Eext_Rotary_Selector_Item_State_Color custom_content_color;
   Eext_Rotary_Selector_Item_State_Color selector_main_text_color;
   Eext_Rotary_Selector_Item_State_Color selector_sub_text_color;
};


/* More Option data */
typedef struct _Eext_More_Option_Data Eext_More_Option_Data;
struct _Eext_More_Option_Data {
   Evas_Object* parent;
   Evas_Object* more_option;
   Evas_Object* panel;
   Evas_Object* rotary_selector;
   Eext_More_Option_Direction direction;
};

#ifdef __cplusplus
}
#endif


#endif /* __EFL_EXTENSION_COMMON_PRIVATE_H__ */
