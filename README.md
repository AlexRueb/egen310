# egen310
This is the code for our RC car

Should be noted that this is editted code from:
https://www.hackster.io/gerrikoiot/ble-led-controller-feather-32u4-bluefruit-le-mit-app-inv-75163d

Also, there should never be Serial commands in the loop, unless for debugging purposes. Failure to take out the commmands
results in heavy lag between button presses on the phone and device feedback. This information was given to me by Gerrikoio on this google discussion from the MIT App inventor help section, which I posted to figure out the issue:
https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/mitappinventortest/6yBWV1t7Olc/5_Ipn4ExBwAJ

The joystick added to the MIT App Inventor 2 app was code taken from Scott Ferguson in this thread:
https://groups.google.com/forum/#!category-topic/mitappinventortest/mit-appinventor-2/4VLUBlP1FxA

All in all this was a great experience and figuring out how Bluetooth Low Energy works was an amazing learning experience for me.
