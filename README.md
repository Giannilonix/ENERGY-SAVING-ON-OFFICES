# ENERGY-SAVING-ON-OFFICES
A service based on Openstack and Stack4Things that minimizes the energy consumption of an office.It is a scalabale solution.

It was tested on a office with this hardware setup:
- 1 RaspberryPI 3B+
- 4 ESP32 to get data from sensors (Temperature, Lux, Presence)
- 1 ESP32 to simulate the actuators with leds

Software setup:
- Openstack
- Stack4Things
- Arancino OS for the Raspberry
- mDNS_Web_Server for the ESP32 boards
