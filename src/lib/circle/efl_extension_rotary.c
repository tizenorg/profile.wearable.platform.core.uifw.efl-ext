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

#include <Elementary.h>
#include "circle/efl_extension_rotary.h"
#include "efl_extension_private.h"

#define TIZEN_DEV "tizen_detent"

static const char SIG_ACTIVATED[] = "rotary,activated";
static const char SIG_DEACTIVATED[] = "rotary,deactivated";

EAPI Eina_Bool
eext_rotary_event_handler_add(Eext_Rotary_Handler_Cb func, void *data)
{
   /* This function is disabled temporarily because of X11 dependency. */
   return EINA_TRUE;
}

EAPI void *
eext_rotary_event_handler_del(Eext_Rotary_Handler_Cb func)
{
   /* This function is disabled temporarily because of X11 dependency. */
   return NULL;
}

EAPI Eina_Bool
eext_rotary_object_event_callback_add(Evas_Object *obj, Eext_Rotary_Event_Cb func, void *data)
{
   /* This function is disabled temporarily because of X11 dependency. */
   return EINA_TRUE;
}

EAPI Eina_Bool
eext_rotary_object_event_callback_priority_add(Evas_Object *obj, Eext_Callback_Priority priority, Eext_Rotary_Event_Cb func, void *data)
{
   /* This function is disabled temporarily because of X11 dependency. */
   return EINA_TRUE;
}

EAPI void *
eext_rotary_object_event_callback_del(Evas_Object *obj, Eext_Rotary_Event_Cb func)
{
   /* This function is disabled temporarily because of X11 dependency. */
   return NULL;
}

EAPI void
eext_rotary_object_event_activated_set(Evas_Object *obj, Eina_Bool activated)
{
   /* This function is disabled temporarily because of X11 dependency. */
}
