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

Two options can be given:

1) mikroBUS number to specify where motion click is connected.

Default value for [mikroBUS](http://www.mikroe.com/mikrobus/) is 1

    $ motion_click_app -m <mikroBUS>

    Ready for detecting motion...
    detected
