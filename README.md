# 💡 User Presence-Based Smart Table Lamp with Study Time Tracking Application

## 📌 Project Overview

This IoT-enabled **Smart Table Lamp** was designed to enhance **productivity** and promote **energy efficiency** by adapting intelligently to user presence and ambient lighting conditions.

Developed as part of the **Individual Design Project (IDP)**, this system integrates real-time sensing, automation, and cloud-based data tracking to offer a seamless, human-centric lighting experience.

---

## ⚙️ Key Features

- **🔍 Presence Detection:**  
  Utilized the **LD2410B radar sensor** to detect user presence near the lamp for automated on/off control.

- **🌞 Ambient Light Adjustment:**  
  Integrated an **LDR** (Light Dependent Resistor) to measure room brightness and adjust LED intensity accordingly.

- **🕒 Real-Time Clock (RTC):**  
  Used the **RTC module** to accurately timestamp study sessions and track daily usage patterns.

- **🌐 Node-RED Dashboard:**  
  Developed a real-time **web interface** using **Node-RED** to visualize activity and control system behavior.

- **☁️ Cloud Data Logging:**  
  Study session start/stop times were stored in **Firestore**, enabling:
  - Daily study duration tracking  
  - Usage history and behavior insights  
  - Remote access to time logs

---

## 🎯 Project Goals

- Enhance **focus and productivity** by minimizing manual intervention in lighting control  
- Support **energy conservation** through automation based on actual usage  
- Provide students with **personalized insights** into their study habits via real-time data analytics

---

## 🧩 Technologies & Components

| Component        | Function                          |
|------------------|-----------------------------------|
| ESP32            | Main microcontroller (Wi-Fi + I/O) |
| LD2410B          | Human presence radar sensor        |
| RTC Module       | Real-time timekeeping              |
| LDR              | Ambient brightness sensing         |
| Node-RED         | Web-based monitoring dashboard     |
| Firestore        | Cloud database for data storage    |
| 12V LED Strip    | Primary lighting source            |

---

## 📈 Future Improvements

- Add **mode switching** (reading, movie, ambient)  
- Implement **mobile notifications** for inactivity reminders  
- Expand Firestore analytics to track weekly/monthly trends  

---

## 🙌 Acknowledgment

This project was carried out as part of the **Electronics and Telecommunication Engineering curriculum** at  
**General Sir John Kotelawala Defence University** under the **Individual Design Project** module.
