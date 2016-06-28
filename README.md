# click-apps-for-openwrt

###Overview

Applications to show how mikro-e [sensor clicks](http://www.mikroe.com/add-on-boards/sensors/) can be used with Ci40.

###How to install applications of this package in OpenWrt's filesystem

    $ make menuconfig

Select Utilities -> click-apps -> application

Then build OpenWrt in [usual way](https://github.com/IMGCreator/openwrt/).

###Application to detect motion using [motion click](http://www.mikroe.com/click/motion/)

After starting the app move your hand above the motion click.

Press ctrl+c to exit the application.

mikroBUS number can be given in option to specify where motion click is connected.

Default value for [mikroBUS](http://www.mikroe.com/mikrobus/) is 1

    $ motion_click_app -m <mikroBUS>

    Ready for detecting motion...
    detected

###Application to display binary representation of a number using [bargraph click](http://www.mikroe.com/click/bargraph/)

Start the app and enter number to display.

mikroBUS number can be given in option to specify where bargraph click is connected.

    $ bargraph_click_app -m <mikroBUS>

    Enter number to display: 2
	Enter number to display: 5
