# Add executable targets
########################
ADD_LIBRARY(gpio OBJECT gpio.c)
ADD_LIBRARY(spi_common OBJECT spi_common.c)

ADD_EXECUTABLE(motion_click_app motion_click_app.c $<TARGET_OBJECTS:gpio>)

# Add install targets
######################
INSTALL(TARGETS motion_click_app
		RUNTIME DESTINATION /usr/bin)
