SET(LIB_INCS
	efl_extension_events.h
	circle/efl_extension_circle_surface.h
	circle/efl_extension_circle_object.h
	circle/efl_extension_circle_object_progressbar.h
	circle/efl_extension_circle_object_slider.h
	circle/efl_extension_circle_object_genlist.h
	circle/efl_extension_circle_object_datetime.h
	circle/efl_extension_circle_object_scroller.h
	circle/efl_extension_circle_object_spinner.h
	circle/efl_extension_rotary.h
	common/efl_extension_common.h
	common/efl_extension_rotary_selector.h
	common/efl_extension_more_option.h)

FOREACH(inc ${LIB_INCS})
	SET(LIB_HEADERS "${LIB_HEADERS}#include \"${inc}\"\n")
ENDFOREACH(inc)

CONFIGURE_FILE(
	efl_extension.h.in efl_extension.h
	@ONLY)

SET(LIB_INCS
	${LIB_INCS}
	efl_extension.h)

FOREACH(inc ${LIB_INCS})
	GET_FILENAME_COMPONENT(dir ${inc} PATH)
	INSTALL(FILES ${inc} DESTINATION include/${LIB_NAME}/${dir})
ENDFOREACH()
