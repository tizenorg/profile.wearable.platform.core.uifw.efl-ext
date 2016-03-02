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

#ifndef __EFL_EXTENSION_MORE_OPTION_H__
#define __EFL_EXTENSION_MORE_OPTION_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_MORE_OPTION_MODULE Efl Extension More Option
 * @ingroup CAPI_EFL_EXTENSION_COMMON_UI_MODULE
 * @brief More option is composed of the toggle(cue button) and more option view,
 *        and more option can change a visibility through the toggle.
 *
 * @details More option is based on elm_panel, but more option is different
 *          from elm_panel in that the former does not set the scrollable option.
 *          To show the more option view, select the toggle or manually use
 *          eext_more_option_opened_set(). To hide the more option view,
 *          trigger the back event or manually use eext_more_option_opened_set().
 *          Additionally, the toggle disappears when more option view appears.
 *          On the other hand, the toggle appears when more option view disappears.
 *          The more option view can be manually set by eext_more_option_opened_set() API.
 *          An item of more option object can be selected by rotary event.
 *
 *          This widget emits the following signals:
 *          @li "item,selected": When the user selects an item.
 *          @li "item,clicked": When the user selects the already selected item again
 *                              or selects a selector.
 *          @li "more,option,opened": When more option view is shown.
 *          @li "more,option,closed": When more option view is hidden.
 *
 * @{
 */

/**
 * @brief Enumeration of More Option Diection type
 */
typedef enum
{
   EEXT_MORE_OPTION_DIRECTION_TOP,  /**< More option (dis)appears from the top */
   EEXT_MORE_OPTION_DIRECTION_BOTTOM,  /**< More option (dis)appears from the bottom */
   EEXT_MORE_OPTION_DIRECTION_LEFT,  /**< More option (dis)appears from the left */
   EEXT_MORE_OPTION_DIRECTION_RIGHT  /**< More option (dis)appears from the right */
} Eext_More_Option_Direction;

/**
 * @WEARABLE_ONLY
 * @brief Add a new more option.
 *
 * @details Because the return value is the elm_layout handle,
 *          the elm_layout APIs can be applicable to more option handle.
 *
 * @param[in] parent The parent object
 * @return A new more option handle, otherwise @c NULL if it cannot be created
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object * eext_more_option_add(Evas_Object *parent);

/**
 * @WEARABLE_ONLY
 * @brief Set the direction of more option.
 *
 * @param[in] obj The more option
 * @param[in] direction The direction of more option
 *
 * @see eext_more_option_direction_get()
 *      @ref Eext_More_Option_Direction
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_direction_set(Evas_Object *obj, Eext_More_Option_Direction direction);

/**
 * @WEARABLE_ONLY
 * @brief Get @ref Eext_More_Option_Direction.
 *
 * @param[in] obj The more option
 * @return A direction of more option
 *
 * @see eext_more_option_direction_set()
 *      @ref Eext_More_Option_Direction
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_More_Option_Direction eext_more_option_direction_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Append a new item to a more option.
 *
 * @param[in] obj The more option
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_item_append(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief prepend a new item to a more option.
 *
 * @param[in] obj The more option
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_item_prepend(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item into the more option after more option item @a after.
 *
 * @param[in] obj The more option
 * @param[in] after The more option item to insert after
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_item_insert_after(Evas_Object *obj, Eext_Object_Item *after);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item into the more option before more option item @a before.
 *
 * @param[in] obj The more option
 * @param[in] before The more option item to insert before
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_item_insert_before(Evas_Object *obj, Eext_Object_Item *before);

/**
 * @WEARABLE_ONLY
 * @brief Delete an item which is the given more option item @a item.
 *
 * @param[in] item The more option item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_item_del(Eext_Object_Item *item);

/**
 * @WEARABLE_ONLY
 * @brief Remove all items from a given more option object.
 *
 * @param[in] obj The more option
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_items_clear(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the text of a more option object.
 *
 * @details The part name exists with the next.
 *          @li "selector,main_text": This locates in the middle of the selector.
 *          @li "selector,sub_text": This locates under the main_text.
 *
 * @param[in] item The more option item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] text The new text of the part
 *
 * @see eext_more_option_item_part_text_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_item_part_text_set(Eext_Object_Item *item, const char *part_name, const char *text);

/**
 * @WEARABLE_ONLY
 * @brief Set the text of a more option object.
 *
 * @details The part name exists with the next.
 *          @li "selector,main_text": This locates in the middle of the selector.
 *          @li "selector,sub_text": This locates under the main_text.
 *
 * @param[in] item The more option item
 * @param[in] part_name The text part name to get (@c NULL for the default part)
 * @return The text of the part, otherwise @c NULL in case of an error
 *
 * @see eext_more_option_item_part_text_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI const char * eext_more_option_item_part_text_get(const Eext_Object_Item *item, const char *part_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the translatable text of a more option object.
 *
 * @details The part name exists with the next.
 *          @li "selector,main_text": This locates in the middle of the selector.
 *          @li "selector,sub_text": This locates under the main_text.
 *
 * @param[in] item The more option item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] domain The name of the domain which contains translatable text
 * @param[in] text ID of the new translatable text
 *
 * @see eext_more_option_item_part_text_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_item_domain_translatable_part_text_set(Eext_Object_Item *item, const char *part_name, const char *domain, const char *text);

/**
 * @WEARABLE_ONLY
 * @brief Set the content to the part of a given container widget.
 *
 * @details The part name exists with the next.
 *          @li "item,icon": This locates in the middle of the selector.
 *
 * @remarks The more option item may hold child objects as content at given parts.
 *          This sets new content to a given part.
 *          If any object is already set as a content object in the same part,
 *          the previous object is automatically deleted. with this call.
 *
 * @param[in] item The more option item
 * @param[in] part_name The more option item's part name to set
 * @param[in] content The new content for that part
 *
 * @see eext_more_option_item_part_content_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_item_part_content_set(Eext_Object_Item *item, const char *part_name, Evas_Object *content);

/**
 * @WEARABLE_ONLY
 * @brief Get the content from the part of a given container widget.
 *
 * @details The part name exists with the next.
 *          @li "item,icon": This locates in the middle of the selector.
 *
 * @param[in] item The more option item
 * @param[in] part_name The more option item's part name to get
 * @return The content of the object at the given part,
 *         otherwise @c NULL in case of an error
 *
 * @see eext_more_option_item_part_content_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object * eext_more_option_item_part_content_get(const Eext_Object_Item *item, const char *part_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the visibility of more option view.
 *
 * @details If opened is @c EINA_TRUE, more option view in which
 *          the rotary selector is included disappears and toggle appears.
 *          If opened is @c EINA_FALSE, more option view in which
 *          the rotary selector is included appears and toggle disappear.
 *
 * @remarks When more option is closed and opened again, the first item
 *          is always selected.
 *
 * @param[in] obj The more option object
 * @param[in] opened If @c EINA_TRUE, the more option runs the animation to appear,
 *                   If @c EINA_FALSE, the more option runs the animation to disappear
 *
 * @see eext_more_option_opened_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_opened_set(Evas_Object *obj, Eina_Bool opened);

/**
 * @WEARABLE_ONLY
 * @brief Get whether the more option is set to be shown or to be hidden.
 *
 * @param[in] obj The more option object
 * @return If @c EINA_TRUE, more option is shown.
 *         Otherwise @c EINA_FALSE, more option is hidden.
 *
 * @see eext_more_option_opened_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_more_option_opened_get(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Get the list of all items from a given more option object.
 *
 * @param[in] obj The more option object
 * @return All item lists of the more option..
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI const Eina_List* eext_more_option_items_get(const Evas_Object *obj);
/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_MORE_OPTION_H__ */
