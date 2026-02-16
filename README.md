# Smart Health Urban Infrastructure Monitoring (SUIM) System



**Davidson Inventors Challenge 2025/2026 - Team JEPAS**  

**City of London Academy, Southwark**



---



## Project Overview



SUIM: a low-cost sensor network measuring seven pollutants (NO₂, O₃, CO, SO₂, VOCs, PM2.5, PM10) 
at 95% lower cost. Three solar-powered ESP32 nodes transmit real-time data to a Raspberry Pi 
hub with a web dashboard. This demonstrates community-scale air quality monitoring is 
technically and economically viable, supporting UN SDG 9 through innovation in resilient urban 
infrastructure. 




### Monitored Pollutants

- **Environmental:** Temperature, Humidity, Pressure, VOC (BME680)

- **Gases:** NO₂, CO, NH₃ (MICS-6814), CO₂ (MQ-135), CO (MQ-7), O₃ (MQ-131)

- **Particulates:** PM2.5, PM10 (PMS5003)



---



### Components

- **Hardware:** ESP32 (x3), BME680, MQ-135, MQ-7, MQ-131, MICS-6814, PMS5003

- **Backend:** Raspberry Pi 4, Flask API, CSV data storage

- **Frontend:** React, TanStack Query, Tailwind CSS

- **Power:** Solar power banks - 2x 38,800mAh, weatherproof (and 1x 10,000mAh powerbank as another solar power bank wouldn't arrive in time, learn more in documentation)

- **Enclosures:** IP54 Weatherproof Electrical Connection boxes



---



## Repository Structure



Smart-Health-Urban-Infrastructure-Monitoring-SUIM-System/

├── hardware/           ***ESP32 code and wiring diagrams***

├── backend/            ***Raspberry Pi Flask server***

├── frontend/           ***React dashboard (separate folder)***

├── docs/               ***Assembly guides and documentation***

└── data/               ***Sample sensor reading***



---



## Current Status (Feb 13, 2026)



✅ **COMPLETED:**

- Raspberry Pi hub fully operational (Flask server, auto-start service)

- ESP32 Node 1 (rough prototype using 2 sensors) assembled and transmitting data via Wi-Fi
   - BME680 + MQ-135 sensors integrated and working

- React dashboard displaying live data (temperature, humidity, pressure, CO)



⏳ **IN PROGRESS:**

- MQ-7, MICS-6814 sensors (arriving by Feb 18?)

- PMS5003 integration (awaiting breakout boards, may not arrive in time, nuclear option is to cut the wire to separate connections manually)

- Nodes 2 & 3 assembly

- Weatherproof enclosure assembly



📅 **UPCOMING:**

- Final Assembly (13-16th February)

- 10-day data collection (16 - 26 February)

- Final report submission (27 February)

- Final Report Feedback (13th March)

- Shortlisted for final? (hopefully - 16th March)

- **Davidson Inventors Challenge 2026 Final and Awards - (Thursday 26  March, God Willing🙏)**
  



---



## Documentation



- [Assembly Guide](docs/ASSEMBLY.md) *(coming soon)*

- [Deployment Guide](docs/DEPLOYMENT.md) *(coming soon)*

- [Weatherproofing Guide](docs/WEATHERPROOFING.md) *(coming soon)*



---



## License



MIT License - See [LICENSE](LICENSE) for details.



---



## Acknowledgments



- University of Cambridge's Davidson Inventors Challenge (2025-2026)

- City of London Academy Southwark

