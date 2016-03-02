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

#ifndef __EFL_EXTENSION_CIRCLE_OBJECT_SPINNER_H__
#define __EFL_EXTENSION_CIRCLE_OBJECT_SPINNER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_SPINNER_MODULE Efl Extension Circle Spinner
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Circle spinner is based on elm_spinner and is operated by rotary event.
 *
 * @details Circle spinner is to display and handle spinner value by rotary event.
 *          If rotary event is activated by eext_rotary_object_event_activated_set(),
 *          circle spinner increases/decreases value of elm_spinner
 *          by the clockwise/counter clockwise rotary event.
 *
 *          The available circle object items are as follows:
 *          @li @c "default": Default circle item. It draws a marker.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle spinner object.
 *
 * @param[in] spinner The spinner object
 * @param[in] surface The @ref Eext_Circle_Surface object to render the circle object.
 *                    If @c NULL, circle object will be rendered independently.
 * @return A new circle object handle, otherwise @c NULL if it cannot be created
 *
 * @see @ref CAPI_EFL_EXTENSION_CIRCLE_OBJECT_MODULE
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object *eext_circle_object_spinner_add(Evas_Object *spinner, Eext_Circle_Surface *surface);

/**
 * @WEARABLE_ONLY
 * @brief Set the circle spinner angle per each spinner value.
 *
 * @details This is not set a circle spinner picker angle offset.
 *          It's set the angle per each spinner value.
 *          default angle per each spinner value is calculated below formula.
 *          (360/ max - min) * step
 *
 * @param[in] obj The circle spinner object
 * @param[in] angle The angle value per each spinner value.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */

EAPI void eext_circle_object_spinner_angle_set(Evas_Object *obj, double angle);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_OBJECT_SPINNER_H__ */
