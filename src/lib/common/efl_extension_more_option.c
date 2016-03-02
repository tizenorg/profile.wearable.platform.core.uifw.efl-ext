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

#define EEXT_MORE_OPTION_DATA_KEY "eext_more_option_data"

/* === /internal object cb & util === */
static void _drawer_back_cb(void *data, Evas_Object *obj, void *event_info);
static void _panel_active_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _panel_inactive_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _panel_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

/* === internal object creation (elm-demo-tizen/drawer.c) === */
static Evas_Object *_drawer_layout_create(Evas_Object *parent);
static Evas_Object *_panel_create(Evas_Object *parent, Eext_More_Option_Data *mold);

/* === more option API === */
static Eext_More_Option_Data *_more_option_data_init(Evas_Object *obj, Evas_Object *parent);
static Eext_More_Option_Data *_more_option_data_get(const Evas_Object *more_option);
static void _more_option_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

/* === internal object cb & util === */
static void
_drawer_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *panel = obj;
   if (!panel)
     {
        LOGE("panel is NULL!!");
        return;
     }

   Eext_More_Option_Data *mold = data;
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }

   if (!elm_panel_hidden_get(panel)) elm_panel_toggle(panel);

   //selected_index is initialized as zero.
   if (mold->rotary_selector)
     {
        const Eina_List *list = eext_more_option_items_get(mold->more_option);
        Eext_Object_Item *item = NULL;

        item = eina_list_data_get(list);
        eext_rotary_selector_selected_item_set(mold->rotary_selector, item);
     }
}

static void
_panel_active_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_More_Option_Data *mold = data;

   if (mold)
     {
        evas_object_smart_callback_call(mold->more_option, "more,option,opened", NULL);
        eext_rotary_object_event_activated_set(mold->rotary_selector, EINA_TRUE);
        eext_object_event_callback_add(obj, EEXT_CALLBACK_BACK, _drawer_back_cb, mold);
     }
}

static void
_panel_inactive_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_More_Option_Data *mold = data;
   if (mold)
     {
        evas_object_smart_callback_call(mold->more_option, "more,option,closed", NULL);
        eext_rotary_object_event_activated_set(mold->rotary_selector, EINA_FALSE);
     }

   eext_object_event_callback_del(obj, EEXT_CALLBACK_BACK, _drawer_back_cb);
}

static void
_panel_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   elm_layout_signal_callback_del(obj, "elm,state,active,finished", "elm", _panel_active_cb);
   elm_layout_signal_callback_del(obj, "elm,state,inactive,finished", "elm", _panel_inactive_cb);
}

/* === internal object creation (elm-demo-tizen/drawer.c) === */
static Evas_Object *
_drawer_layout_create(Evas_Object *parent)
{
   Evas_Object *layout;
   layout = elm_layout_add(parent);
   if (!layout)
     {
        LOGE("drawer layout is NULL");
        return NULL;
     }

   elm_layout_theme_set(layout, "layout", "drawer", "panel");
   evas_object_show(layout);

   return layout;
}

static Evas_Object *
_panel_create(Evas_Object *parent, Eext_More_Option_Data *mold)
{
   Evas_Object *panel;

   panel = elm_panel_add(parent);
   if (!panel)
     {
        LOGE("panel is NULL");
        return NULL;
     }

   elm_object_style_set(panel, "circle/default");
   evas_object_show(panel);

   //Default : right
   elm_panel_orient_set(panel, ELM_PANEL_ORIENT_RIGHT);
   elm_object_part_content_set(parent, "elm.swallow.right", panel);

   evas_object_event_callback_add(panel, EVAS_CALLBACK_DEL, _panel_del_cb, NULL);

   return panel;
}

/* === more option API === */
static Eext_More_Option_Data *
_more_option_data_init(Evas_Object *obj, Evas_Object *parent)
{
   Eext_More_Option_Data *mold = NULL;

   mold = (Eext_More_Option_Data *)calloc(1, sizeof(Eext_More_Option_Data));
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   mold->parent = parent;
   mold->more_option = obj;
   mold->panel = NULL;
   mold->rotary_selector = NULL;
   mold->direction = EEXT_MORE_OPTION_DIRECTION_RIGHT;

   return mold;
}

static Eext_More_Option_Data *
_more_option_data_get(const Evas_Object *more_option)
{
   if (!more_option)
     {
        LOGE("more_option is NULL!!");
        return NULL;
     }

   return evas_object_data_get(more_option, EEXT_MORE_OPTION_DATA_KEY);
}

static void
_more_option_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Data *mold = data;

   evas_object_data_del(obj, EEXT_MORE_OPTION_DATA_KEY);

   free(mold);
}

static void
_rotary_selector_item_clicked(void *data, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Data *mold = (Eext_More_Option_Data *)data;
   evas_object_smart_callback_call(mold->more_option, "item,clicked", event_info);
}

static void
_rotary_selector_item_selected(void *data, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Data *mold = (Eext_More_Option_Data *)data;
   evas_object_smart_callback_call(mold->more_option, "item,selected", event_info);
}

/* === public API implementation === */

EAPI Evas_Object *
eext_more_option_add(Evas_Object *parent)
{
   if (!parent)
     {
        LOGE("parent is NULL!!");
        return NULL;
     }

   Eext_More_Option_Data *mold = NULL;
   Evas_Object *more_option = NULL;

   more_option = _drawer_layout_create(parent);
   if (!more_option)
     {
        LOGE("more_option is NULL!!");
        return NULL;
     }

   mold = _more_option_data_init(more_option, parent);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   evas_object_data_set(mold->more_option, EEXT_MORE_OPTION_DATA_KEY, mold);
   evas_object_event_callback_add(mold->more_option, EVAS_CALLBACK_DEL, _more_option_del_cb, mold);

   mold->panel = _panel_create(mold->more_option, mold);
   if (!mold->panel)
     {
        LOGE("panel is NULL!!");
        return NULL;
     }

   elm_layout_signal_callback_add(mold->panel, "elm,state,active,finished", "elm", _panel_active_cb, mold);
   elm_layout_signal_callback_add(mold->panel, "elm,state,inactive,finished", "elm", _panel_inactive_cb, mold);

   mold->rotary_selector = eext_rotary_selector_add(mold->more_option);
   if (!mold->rotary_selector)
     {
        LOGE("rotary_selector is NULL!!");
        return NULL;
     }

   evas_object_smart_callback_add(mold->rotary_selector, "item,selected", _rotary_selector_item_selected, mold);
   evas_object_smart_callback_add(mold->rotary_selector, "item,clicked", _rotary_selector_item_clicked, mold);

   Evas_Object *selector_bg = elm_layout_add(mold->rotary_selector);
   if (!selector_bg)
     {
        LOGE("selector_bg is NULL!!");
        return NULL;
     }

   elm_layout_theme_set(selector_bg, "layout/rotary_selector", "selector", "more_option");

   eext_rotary_selector_part_content_set(mold->rotary_selector, "selector,bg_image", EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL, selector_bg);
   eext_rotary_selector_part_content_set(mold->rotary_selector, "selector,bg_image", EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED, selector_bg);

   Evas_Object *selector_content = elm_layout_add(mold->rotary_selector);
   if (!selector_content)
     {
        LOGE("selector_content is NULL!!");
        return NULL;
     }

   elm_layout_theme_set(selector_content, "layout/rotary_selector", "content", "more_option");

   elm_object_part_content_set(mold->rotary_selector, "content", selector_content);

   elm_object_content_set(mold->panel, mold->rotary_selector);

   elm_panel_toggle(mold->panel);

   return mold->more_option;
}

EAPI void
eext_more_option_direction_set(Evas_Object *obj, Eext_More_Option_Direction direction)
{
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return;
     }

   Eext_More_Option_Data *mold = NULL;
   mold = _more_option_data_get(obj);
   if (mold)
     {
        elm_panel_orient_set(mold->panel, direction);
        mold->direction = direction;
     }
}

EAPI Eext_More_Option_Direction
eext_more_option_direction_get(const Evas_Object *obj)
{
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return EEXT_MORE_OPTION_DIRECTION_RIGHT; // defalut
     }

   Eext_More_Option_Data *mold = NULL;
   mold = _more_option_data_get((Evas_Object *)obj);
   if (mold)
     {
        Elm_Panel_Orient orient = elm_panel_orient_get(mold->panel);
        return (Eext_More_Option_Direction)orient;
     }

   return EEXT_MORE_OPTION_DIRECTION_RIGHT; // defalut
}

EAPI Eext_Object_Item *
eext_more_option_item_append(Evas_Object *obj)
{
   Eext_Object_Item *item = NULL;
   Eext_Object_Item *first_item = NULL;
   Eext_More_Option_Data *mold = NULL;

   mold = _more_option_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   item = eext_rotary_selector_item_append(mold->rotary_selector);

   if (!eext_more_option_opened_get(obj))
     {
        const Eina_List *list = eext_more_option_items_get(mold->more_option);

        first_item = eina_list_data_get(list);
        eext_rotary_selector_selected_item_set(mold->rotary_selector, first_item);
     }

   return item;
}

EAPI Eext_Object_Item *
eext_more_option_item_prepend(Evas_Object *obj)
{
   Eext_More_Option_Data *mold = NULL;
   Eext_Object_Item *item = NULL;
   Eext_Object_Item *first_item = NULL;

   mold = _more_option_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   item = eext_rotary_selector_item_prepend(mold->rotary_selector);

   if (!eext_more_option_opened_get(obj))
     {
        const Eina_List *list = eext_more_option_items_get(mold->more_option);

        first_item = eina_list_data_get(list);
        eext_rotary_selector_selected_item_set(mold->rotary_selector, first_item);
     }

   return item;
}

EAPI Eext_Object_Item *
eext_more_option_item_insert_after(Evas_Object *obj, Eext_Object_Item *after)
{
   Eext_More_Option_Data *mold = NULL;
   Eext_Object_Item *item = NULL;
   Eext_Object_Item *first_item = NULL;

   mold = _more_option_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   item = eext_rotary_selector_item_insert_after(mold->rotary_selector, after);

   if (!eext_more_option_opened_get(obj))
     {
        const Eina_List *list = eext_more_option_items_get(mold->more_option);

        first_item = eina_list_data_get(list);
        eext_rotary_selector_selected_item_set(mold->rotary_selector, first_item);
     }

   return item;
}

EAPI Eext_Object_Item *
eext_more_option_item_insert_before(Evas_Object *obj, Eext_Object_Item *before)
{
   Eext_More_Option_Data *mold = NULL;
   Eext_Object_Item *item = NULL;
   Eext_Object_Item *first_item = NULL;

   mold = _more_option_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   item = eext_rotary_selector_item_insert_before(mold->rotary_selector, before);

   if (!eext_more_option_opened_get(obj))
     {
        const Eina_List *list = eext_more_option_items_get(mold->more_option);

        first_item = eina_list_data_get(list);
        eext_rotary_selector_selected_item_set(mold->rotary_selector, first_item);
     }

   return item;
}

EAPI void
eext_more_option_item_del(Eext_Object_Item *item)
{
   //Temporary
   Eext_Rotary_Selector_Item *rotary_selector_item = (Eext_Rotary_Selector_Item *)item;
   if (!rotary_selector_item)
     {
        LOGE("rotary_selector_item is NULL!!");
        return;
     }

   Eext_More_Option_Data *mold = _more_option_data_get(rotary_selector_item->parent);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }

   eext_rotary_selector_item_del(item);

   return;
}

EAPI void
eext_more_option_items_clear(Evas_Object *obj)
{
   Evas_Object *more_option = obj;
   Eext_More_Option_Data *mold = NULL;

   if (!more_option)
     {
        LOGE("more_option is NULL!!");
        return;
     }

   mold = _more_option_data_get(more_option);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }
   eext_rotary_selector_items_clear(mold->rotary_selector);

   return;
}

EAPI void
eext_more_option_item_part_text_set(Eext_Object_Item *item, const char *part_name, const char *text)
{
   eext_rotary_selector_item_part_text_set(item, part_name, text);

   return;
}

EAPI void
eext_more_option_item_domain_translatable_part_text_set(Eext_Object_Item *item, const char *part_name, const char *domain, const char *text)
{
   eext_rotary_selector_item_domain_translatable_part_text_set(item, part_name, domain, text);

   return;
}

EAPI const char *
eext_more_option_item_part_text_get(const Eext_Object_Item *item, const char *part_name)
{
   return eext_rotary_selector_item_part_text_get(item, part_name);
}

EAPI void
eext_more_option_item_part_content_set(Eext_Object_Item *item, const char *part_name, Evas_Object *content)
{
   eext_rotary_selector_item_part_content_set(item, part_name, EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL, content);

   return;
}

EAPI Evas_Object *
eext_more_option_item_part_content_get(const Eext_Object_Item *item, const char *part_name)
{
   return eext_rotary_selector_item_part_content_get(item, part_name, EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL);
}

EAPI void
eext_more_option_opened_set(Evas_Object *obj, Eina_Bool opened)
{
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return;
     }

   Eext_More_Option_Data *mold = NULL;
   mold = _more_option_data_get(obj);
   if (mold)
     {
        const Eina_List *list = eext_more_option_items_get(mold->more_option);
        if (eina_list_count(list) > 0)
          {
             //Initialize item to zero
             Eext_Object_Item *item = NULL;
             item = eina_list_data_get(list);
             eext_rotary_selector_selected_item_set(mold->rotary_selector, item);
          }

        if (elm_panel_hidden_get(mold->panel) && opened) elm_panel_toggle(mold->panel);
        else if (!elm_panel_hidden_get(mold->panel) && !opened) elm_panel_toggle(mold->panel);
     }
}

EAPI Eina_Bool
eext_more_option_opened_get(Evas_Object *obj)
{
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return EINA_FALSE;
     }

   Eext_More_Option_Data *mold = NULL;
   mold = _more_option_data_get(obj);
   if (mold)
     {
        return !elm_panel_hidden_get(mold->panel);
     }

   return EINA_FALSE;
}

EAPI const Eina_List*
eext_more_option_items_get(const Evas_Object *obj)
{
   const Evas_Object *more_option = obj;
   Eext_More_Option_Data *mold = NULL;

   if (!more_option)
     {
        LOGE("more_option is NULL!!");
        return NULL;
     }

   mold = _more_option_data_get(more_option);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   return eext_rotary_selector_items_get(mold->rotary_selector);
}

