# NanoLeaf for Rocket League
BakkesMod Plugin to integrate Rocket League events with NanoLeafs

If you *really* want to buy me a coffee: [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/gtt1229)


## Discord: [Rocket League Assistant](https://discord.gg/8bNkhCmQXe)

![rlnanobanner](https://github.com/user-attachments/assets/cee2e19f-f2d3-4ec8-a1c9-1db895a8eb62)


## ⭐Special thanks to those in the Discord and notably [Branky](https://github.com/ItsBranK) and [JerryTheBee](https://github.com/ubelhj)⭐
And some other person named Josh

# NanoLeaf Configuration
The plugin utilizes NanoLeaf's local API and requires an authentication token to be generated.

## NanoLeaf Authentication Token:

   1. Populate your NanoLeaf's IP
   
   2. Hold the on-off button down for 5-7 seconds until the LED starts flashing
   
   3. Click "Generate NanoLeaf Token"

## Plugin Configuration Notes:

* Light colors are not 1:1, so please test colors by pressing the "Test Lights Using FreePlay Color" button in the plugin settings.
* The "Enable (Demos/Goals) Based on Teams' Color" option result in brighter colors of the relevant team that scored or demo'd.
* The "Get Panels" button can retrieve your panelID's but it does not currently do anything.
   
### Notes

* NanoLeaf tokens are secrets and should be treated as such. This plugin stores your token in plain text and makes no attempt to obffuscate, encrypt, or hash your token.

* If you manually reload the plugin (through the F6 BakkesMod Console) while in the game, the IP and Authenticatio noken will have to be reentered.

## To Do

* Possibly single panel configuration
* Probably code clean up and Nullchecks
