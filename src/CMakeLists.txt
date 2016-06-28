# Add executable targets
########################
ADD_LIBRARY(gpio OBJECT gpio.c)
ADD_LIBRARY(spi_common OBJECT spi_common.c)
ADD_LIBRARY(pwm_common OBJECT pwm_common.c)

ADD_EXECUTABLE(motion_click_app motion_click_app.c $<TARGET_OBJECTS:gpio>)
ADD_EXECUTABLE(bargraph_click_app bargraph_click_app.c $<TARGET_OBJECTS:spi_common> $<TARGET_OBJECTS:pwm_common>)

# Add install targets
######################
INSTALL(TARGETS motion_click_app bargraph_click_app
		RUNTIME DESTINATION /usr/bin)
