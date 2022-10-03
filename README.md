# Foxess-T-series-ESPHome-Home-Assistant
Read out Foxess T-Series Inverter to Home Assistant by using ESPHome

Compatibale with:
- Foxess T-series
- Possibly Foxess F-series and S-series as well (not tested!)

![Screenshot 2022-10-02 153019](https://user-images.githubusercontent.com/106449034/193456801-99b3201c-d321-4a15-a520-aaa8ef501432.jpg)
![Screenshot 2022-10-02 153051](https://user-images.githubusercontent.com/106449034/193456806-b54805a2-b9ea-435d-a3b7-6fe42b514c9a.jpg)

Instructions:
- Copy the YAML and the .h files to your ESPHome directory in Home Assistant
- Fill out individual data in the YAML file (SSID, password, manual ip, etc.)
- Optionally comment sensors you don't need or uncomment sensors you do need

The hardware setup including a wiring diagramm can be found in the [Wiki](https://github.com/assembly12/Foxess-T-series-ESPHome-Home-Assistant/wiki/Hardware-setup).

Designing a custom pcb and enclosure is next on my to do list. I'll update here with the corresponding gerber and stl files when done. It should making the circuit a lot easier.

There is some more info being send (like error messages and so on), however this is really not to usefull so I left it out of this component.

Some basic electronics skills (like soldering) are needed to realize this project. I do not take any responsibility for the use of this custom component or anything that it written down in this repository. Use at your own risk.
