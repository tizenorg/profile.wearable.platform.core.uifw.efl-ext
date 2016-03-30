SET(LIB_SRCS
	efl_extension.c
	efl_extension_events.c
	circle/efl_extension_circle_surface.c
	circle/efl_extension_circle_object.c
	circle/efl_extension_circle_object_item.c
	circle/efl_extension_circle_object_progressbar.c
	circle/efl_extension_circle_object_slider.c
	circle/efl_extension_circle_object_genlist.c
	circle/efl_extension_circle_object_datetime.c
	circle/efl_extension_circle_object_scroller.c
	circle/efl_extension_circle_object_spinner.c
	circle/efl_extension_rotary.c
	common/efl_extension_rotary_selector.c
	common/efl_extension_more_option.c)

ADD_LIBRARY(${LIB_NAME} SHARED ${LIB_SRCS})

ADD_DEFINITIONS("-DEXPORT_API=__attribute__((visibility(\"default\")))")
IF(DEFINED WITH_X)
	message("Build with X")
	ADD_DEFINITIONS("-DWITH_X")
	PKG_CHECK_MODULES(LIB_PKGS REQUIRED
		ecore
		ecore-x
		elementary
		dlog
		cairo
		x11
		xext
		xi
		inputproto
		fontconfig
		)
ENDIF()
IF(DEFINED WITH_WAYLAND)
	message("Build with Wayland")
	ADD_DEFINITIONS("-DWITH_WAYLAND")
	PKG_CHECK_MODULES(LIB_PKGS REQUIRED
		ecore
		ecore-wayland
		ecore-input
		elementary
		dlog
		cairo
		fontconfig
		)
ENDIF()

FOREACH(flag ${LIB_PKGS_CFLAGS})
	SET(LIB_CFLAGS "${LIB_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES COMPILE_FLAGS "${LIB_CFLAGS}")
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES SOVERSION ${VERSION_MAJOR})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${LIB_PKGS_LDFLAGS} ${LIB_TARGET_PKGS_LDFLAGS})

INSTALL(TARGETS ${LIB_NAME} DESTINATION lib)

