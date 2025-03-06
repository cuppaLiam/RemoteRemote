# RemoteRemote
Wanted to be able to control my room's fan/light from where-ever I wanted, so I made this. Took too long tho

Program:
- Hosts a local webserver on an esp32 which displays all the HTML/JS required to interact with the program
- Accepts web requests and sends out different frequencies from a transmitter via the esp32 (dependent on the request)

Notes for recreration:
- Used a flipper Zero to record what the frequencies of the remote where
- Had to forward port and allocate a static address for the esp32 so that it was able to be used externally and relaiably upon restart
- Turned the frequencies into binary via (URH) Universal Radio Hacker
- Records the current and previous states for each aspect of the remote control for more harmonious state management
- Online remote controls allow for extra functionality inlcuding:
  - slider bar for smooth light level choosing
  - precise fan timer customising
  - View the current state of each remote funtion
- nice pretty GUI which took to long :(

Might work one of those wake up light functionality into it in the future
