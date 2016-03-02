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

#ifndef __EFL_EXTENSION_ROTARY_SELECTOR_H__
#define __EFL_EXTENSION_ROTARY_SELECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_ROTARY_SELECTOR_MODULE Efl Extension Rotary Selector
 * @ingroup CAPI_EFL_EXTENSION_COMMON_UI_MODULE
 * @brief Rotary Selector is based on elm_layout, and an item can be
 *        selected by rotary event or user item click.
 *
 * @details This widget emits the following signals.
 *
 *          @li "item,selected": When the user selected an item.
 *          @li "item,clicked": When selecting again the alredy selected item or selecting a selector.
 *
 * @{
 */

/**
 * @brief Enumeration of Rotary Selector state
 */
typedef enum
{
   EEXT_ROTARY_SELECTOR_SELECTOR_STATE_NORMAL = 0, /**< Selector is normal state */
   EEXT_ROTARY_SELECTOR_SELECTOR_STATE_PRESSED, /**< Selector is pressed state */
   EEXT_ROTARY_SELECTOR_SELECTOR_STATE_DISABLED, /**< Selector is disabled state */
} Eext_Rotary_Selector_Selector_State;

/**
 * @brief Enumeration of Rotary Selector item state
 */
typedef enum
{
   EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL = 0, /**< Selector Item is normal state */
   EEXT_ROTARY_SELECTOR_ITEM_STATE_PRESSED, /**< Selector Item is pressed state */
   EEXT_ROTARY_SELECTOR_ITEM_STATE_DISABLED, /**< Selector Item is disabled state */
} Eext_Rotary_Selector_Item_State;

/**
 * @WEARABLE_ONLY
 * @brief Add a new Rotary Selector.
 * @details Because the return value is the elm_layout handle,
            the elm_layout APIs can be applicable to rotary selector handle.
 *
 * @param[in] parent The parent object
 *
 * @return A new rotary selector handle, otherwise @c NULL if it cannot be created
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object* eext_rotary_selector_add(Evas_Object *parent);

/**
 * @WEARABLE_ONLY
 * @brief Append a new item to a rotary selector.
 *
 * @param[in] obj The rotary selector
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_rotary_selector_item_append(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Prepend a new item to a rotary selector.
 *
 * @param[in] obj The rotary selector
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_rotary_selector_item_prepend(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item into a rotary selector after rotary selector item @a after.
 *
 * @param[in] obj The rotary selector
 * @param[in] after The rotary selector item to insert after
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_rotary_selector_item_insert_after(Evas_Object *obj, Eext_Object_Item *after);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item into a rotary selector before rotary selector item @a before.
 *
 * @param[in] obj The rotary selector
 * @param[in] before The rotary selector item to insert before
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_rotary_selector_item_insert_before(Evas_Object *obj, Eext_Object_Item *before);

/**
 * @WEARABLE_ONLY
 * @brief Delete an item which is the given rotary selector item.
 *
 * @param[in] item The rotary selector item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_item_del(Eext_Object_Item *item);

/**
 * @WEARABLE_ONLY
 * @brief Remove all items from a given rotary selector object.
 *
 * @param[in] obj The rotary selector
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_items_clear(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the text of a rotary selector object.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] text The new text of the part
 *
 * @details This api supports following part names by default.
 *
 *          @li "selector,main_text": Selector main text.
 *          @li "selector,sub_text": Selector sub text.
 *
 * @see eext_rotary_selector_item_part_text_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_item_part_text_set(Eext_Object_Item *item, const char *part_name, const char *text);

/**
 * @WEARABLE_ONLY
 * @brief Set the text of a rotary selector object.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The text part name to get (@c NULL for the default part)
 * @return The text of the part, otherwise @c NULL for any error
 *
 * @details This api supports following part names by default.
 *
 *          @li "selector,main_text": Selector main text.
 *          @li "selector,sub_text": Selector sub text.
 *
 * @see eext_rotary_selector_item_part_text_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI const char* eext_rotary_selector_item_part_text_get(const Eext_Object_Item *item, const char *part_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the translatable text of a rotary selector object.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] domain The name of the domain which contains translatable text
 * @param[in] text ID of the new translatable text
 *
 * @details This api supports following part names by default.
 *
 *          @li "selector,main_text": Selector main text.
 *          @li "selector,sub_text": Selector sub text.
 *
 * @see eext_rotary_selector_item_part_text_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_item_domain_translatable_part_text_set(Eext_Object_Item *item, const char *part_name, const char *domain, const char *text);

/**
 * @WEARABLE_ONLY
 * @brief Set the content at a part of a given container widget.
 *
 * @remarks The rotary selector item may hold child objects as its contents in given parts.
 *          This new content to a given part. If any object is already set as a content
 *          object in the given part, the previous object is automatically deleted
 *          with this call.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The rotary selector item's part name to set
 * @param[in] state The rotary selector item part
 * @param[in] content The new content for that part
 *
 * @details This api supports following part names by default.
 *
 *          @li "item,bg_image": Selector Item background image
 *          @li "item,icon": Selector item icon
 *          @li "selector,icon": Selector icon
 *          @li "selector,content": Selector content
 *
 * @see eext_rotary_selector_item_part_content_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_item_part_content_set(Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state, Evas_Object *content);

/**
 * @WEARABLE_ONLY
 * @brief Get the content at a part of a given container widget.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The rotary selector item's part name to get
 * @param[in] state The rotary selector item part
 * @return The content of the object at the given part, otherwise @c NULL in case of an error
 *
 * @details This api supports following part names by default.
 *
 *          @li "item,bg_image": Selector Item background image
 *          @li "item,icon": Selector item icon
 *          @li "selector,icon": Selector icon
 *          @li "selector,content": Selector content
 *
 * @see eext_rotary_selector_item_part_content_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object* eext_rotary_selector_item_part_content_get(const Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state);

/**
 * @WEARABLE_ONLY
 * @brief Set the content at a part of a given container widget.
 *
 * @remarks The rotary selector may hold child objects as its contents in given parts.
 *          This sets a new content to a given part. If any object is already set as a content
 *          object in the given part, the previous object is automatically deleted
 *          with this call.
 *
 * @param[in] obj The rotary selector
 * @param[in] part_name The rotary selector part name to set
 * @param[in] state The selector part
 * @param[in] content The new content for that part
 * @details This api supports following part names by default.
 *
 *          @li "selector,bg_image": Selector background image.
 *
 * @see eext_rotary_selector_selector_part_content_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_part_content_set(Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state, Evas_Object *content);

/**
 * @WEARABLE_ONLY
 * @brief Get the content from the part of the given container widget.
 *
 * @param[in] obj The rotary selector.
 * @param[in] part_name The rotary selector part name to set
 * @param[in] state The selector part
 * @return The content of the object at the given part, otherwise @c NULL in case of an error
 *
 * @details This api supports following part names by default.
 *
 *          @li "selector,bg_image": Selector background image.
 *
 * @see eext_rotary_selector_selector_part_content_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object* eext_rotary_selector_part_content_get(const Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state);


/**
 * @WEARABLE_ONLY
 * @brief Set the item part color of a rotary selector object.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The item part name to set
 * @param[in] state The rotary selector item part
 * @param[in] r Red (0 - 255)
 * @param[in] g Green (0 - 255)
 * @param[in] b Blue (0 - 255)
 * @param[in] a Alpha (0 - 255)
 *
 * @details This api supports following part names by default.
 *
 *          @li "item,bg_image": Selector Item background image
 *          @li "item,icon": Selector item icon
 *          @li "selector,icon": Selector icon
 *          @li "selector,content": Selector content
 *          @li "selector,main_text": Selector main text.
 *          @li "selector,sub_text": Selector sub text.
 *
 * @see eext_rotary_selector_item_part_color_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_item_part_color_set(Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state, int r, int g, int b, int a);

/**
 * @WEARABLE_ONLY
 * @brief Get the item part color of a rotary selector object.
 *
 * @param[in] item The rotary selector item
 * @param[in] part_name The item part name to get
 * @param[in] state The rotary selector item part
 * @param[out] r A pointer to Red (0 - 255)
 * @param[out] g A pointer to Green (0 - 255)
 * @param[out] b A pointer to Blue (0 - 255)
 * @param[out] a A pointer to Alpha (0 - 255)
 * @see eext_rotary_selector_item_part_color_set()
 *
 * @details This api supports following part names by default.
 *
 *          @li "item,bg_image": Selector Item background image
 *          @li "item,icon": Selector item icon
 *          @li "selector,icon": Selector icon
 *          @li "selector,content": Selector content
 *          @li "selector,main_text": Selector main text.
 *          @li "selector,sub_text": Selector sub text.
 *
 * @see eext_rotary_selector_item_part_color_get()
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_item_part_color_get(const Eext_Object_Item *item, const char *part_name, Eext_Rotary_Selector_Item_State state, int *r, int *g, int *b, int *a);

/**
 * @WEARABLE_ONLY
 * @brief Set the selector part color of a rotary selector object.
 *
 * @param[in] obj The rotary selector
 * @param[in] part_name The selector part name to set
 * @param[in] state The selector part
 * @param[in] r Red (0 - 255)
 * @param[in] g Green (0 - 255)
 * @param[in] b Blue (0 - 255)
 * @param[in] a Alpha (0 - 255)
 *
 * @details This api defualt support part names.
 *
 *          @li "selector,bg_image": Selector background image.
 *
 * @see eext_rotary_selector_part_color_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_part_color_set(Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state, int r, int g, int b, int a);

/**
 * @WEARABLE_ONLY
 * @brief Get the selector part color of a rotary selector object.
 *
 * @param[in] obj The rotary selector
 * @param[in] part_name The selector part name to get
 * @param[in] state The selector part
 * @param[out] r A pointer to Red (0 - 255)
 * @param[out] g A pointer to Green (0 - 255)
 * @param[out] b A pointer to Blue (0 - 255)
 * @param[out] a A pointer to Alpha (0 - 255)
 * @see eext_rotary_selector_item_part_color_set()
 *
 * @details This api defualt support part names.
 *
 *          @li "selector,bg_image": Selector background image.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_part_color_get(const Evas_Object *obj, const char *part_name, Eext_Rotary_Selector_Selector_State state, int *r, int *g, int *b, int *a);

/**
 * @WEARABLE_ONLY
 * @brief Set the selected item of a rotary selector object.
 *
 * @param[in] obj The rotary selector
 * @param[in] item The rotary selector item
 * @see eext_rotary_selector_selected_item_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_selector_selected_item_set(Evas_Object *obj, Eext_Object_Item *item);

/**
 * @WEARABLE_ONLY
 * @brief Get the selected item of a rotary selector object.
 *
 * @param[in] obj The rotary selector
 * @see eext_rotary_selector_selected_item_set()
 * @return A selected item handle, otherwise @c NULL if it cannot be created
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_rotary_selector_selected_item_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Get the rotary selector item list of a rotary selector object.
 *
 * @param[in] obj The rotary selector
 * @return A rotary selector item list handle, otherwise @c NULL if it cannot be created
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI const Eina_List* eext_rotary_selector_items_get(const Evas_Object *obj);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_ROTARY_SELECTOR_H__ */
