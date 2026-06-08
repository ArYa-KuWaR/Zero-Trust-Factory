# Zero Trust Factory Control Plane (ZTFCP)

> **Purdue-Aligned Zero Trust Architecture for Smart Industrial Environments**

A next-generation cybersecurity framework for Industry 5.0 environments that combines Purdue-model segmentation, Zero Trust identity enforcement, AI-driven anomaly detection, and OT/IT micro-segmentation to secure modern industrial control systems (ICS).

---

![ZTFCP Network Topology](images/image_0b6385.png)

[![IEC 62443](https://img.shields.io/badge/IEC%2062443-Compliant-blue)](https://www.iec.ch/cyber-security)
[![Industry 5.0](https://img.shields.io/badge/Industry%205.0-Ready-green)](https://ec.europa.eu/info/research-and-innovation/research-area/industrial-research-and-innovation/industry-50_en)
[![Zero Trust](https://img.shields.io/badge/Zero%20Trust-Architecture-red)](https://www.nist.gov/publications/zero-trust-architecture)
[![OT Security](https://img.shields.io/badge/OT-Security-orange)](#)
[![AI Threat Detection](https://img.shields.io/badge/AI-Anomaly%20Detection-purple)](#)

---

# 📌 Executive Summary

Modern industrial environments are rapidly converging Operational Technology (OT) and Information Technology (IT) infrastructures. Traditional Purdue-model factories were designed primarily for isolation and deterministic communication — not for highly connected, cloud-integrated, AI-enabled Industry 5.0 ecosystems.

This convergence introduces major cybersecurity risks:

- Flat OT network architectures
- Unencrypted industrial communication
- Weak device authentication
- Lack of behavioral monitoring
- Increased lateral movement possibilities
- Delayed incident response capabilities

A single compromised edge device can potentially propagate attacks across an entire industrial plant, causing operational disruption, safety hazards, or production downtime.

The **Zero Trust Factory Control Plane (ZTFCP)** addresses these risks through a deployable hardware-backed architecture that enforces:

- Strict Purdue-level segmentation
- Mutual TLS (mTLS) identity verification
- ACL-based micro-segmentation
- AI-powered behavioral anomaly detection
- Autonomous edge-level response mechanisms
- SOC-ready observability and SIEM integration

The system is specifically designed for:

- Smart factories
- Industrial IoT (IIoT)
- Critical infrastructure labs
- Secure cyber-physical systems
- Industry 5.0 research environments

---

# 🏗️ System Architecture

The architecture follows the **Purdue Enterprise Reference Architecture (PERA)** and implements strict OT/IT segregation using controlled routing policies and network isolation.

All communication between industrial zones is explicitly governed through:

- Controlled subnet routing
- ICS gateway enforcement
- Firewall-based traffic filtering
- Mutual TLS authentication
- Topic-level broker isolation

---

# 🧠 Architecture Overview

## Physical & Network Architecture

*(Defines the hardware layers, trust boundaries, and Purdue segmentation model)*

![Architecture Diagram](images/image_0b63a4.jpg)

---

## Data & Security Pipeline

*(Illustrates telemetry flow, AI evaluation, anomaly scoring, and autonomous actuation)*

![Flow Diagram](images/image_0b63c6.jpg)

---

# 🏭 Purdue Level Mapping

| Purdue Level | Zone | Components | Security Controls |
|---|---|---|---|
| **Level 4** | Enterprise IT | ASUS ROG Laptop (`192.168.137.1` / `10.130.190.53`) | Subnet isolation, SOC visualization, SIEM event ingestion, RBAC-controlled Grafana access |
| **Level 3.5** | Industrial DMZ | Seeed reComputer J1010 (Jetson Nano - `192.168.137.200`) | UFW firewall enforcement, default-deny policy, secure gateway filtering, mTLS termination |
| **Level 3** | Operations & Control | Jetson Nano Core Services | Mosquitto MQTT broker, Isolation Forest AI engine, Prometheus metrics collection, Grafana dashboards |
| **Level 1–2** | Basic Control | ESP32 Edge Controllers (`10.130.190.x`) | Cryptographic device identity, ACL micro-segmentation, secure MQTT topic enforcement |
| **Level 0** | Field Devices | DHT22 Sensors & SG90 Servo Actuator | Physical telemetry acquisition and autonomous safety actuation |

---

# 🔐 Core Security Features

# 1️⃣ Mutual TLS (mTLS) & Zero Trust Identity

Every device and service inside the control plane is authenticated using **Mutual TLS (mTLS)**.

Both the client and server must present valid CA-signed X.509 certificates before communication is permitted.

## Key Capabilities

- Certificate-based device authentication
- Elimination of implicit network trust
- Socket-layer rejection of unauthorized devices
- Secure encrypted telemetry channels
- Offline-safe certificate handling for air-gapped environments

## Offline Time Anchoring

Industrial environments frequently operate without reliable NTP access.

To prevent certificate validation failures caused by incorrect device clocks, the ESP32 firmware implements a custom epoch-time anchoring mechanism that:

- Establishes trusted baseline time
- Prevents false certificate expiration states
- Enables reliable offline authentication

---

# 2️⃣ ACL-Based Micro-Segmentation

The MQTT broker enforces strict topic-level segmentation using Mosquitto Access Control Lists (ACLs).

Each ESP32 device is cryptographically restricted to only its designated telemetry paths.

## Security Benefits

- Prevents lateral movement
- Stops unauthorized actuator access
- Isolates compromised edge nodes
- Enforces least-privilege communication

### Example Restrictions

| Device | Allowed Actions |
|---|---|
| `esp32-1` | Publish only to sensor telemetry topics |
| `esp32-2` | Publish only to assigned telemetry streams |
| Edge Sensors | Denied access to actuator command channels |
| Servo Controller | Accepts commands only from authenticated AI engine |

---

# 3️⃣ AI-Powered Predictive Threat Detection

The platform deploys an unsupervised **Isolation Forest** anomaly detection model running directly on the Jetson Nano edge gateway.

The AI layer continuously evaluates environmental telemetry streams:

- Temperature (`t1`, `t2`)
- Humidity (`h1`, `h2`)
- Temporal behavioral patterns
- Rate-of-change dynamics

---

## Behavioral Baselining

The system establishes mathematical baselines representing normal industrial operating conditions.

Incoming telemetry is continuously compared against this baseline to detect:

- Sensor spoofing
- Environmental anomalies
- Unexpected telemetry drift
- Rapid thermal escalation
- Manipulated sensor behavior

---

## Predictive Horizon Analysis

A sliding window forecasting engine computes real-time:

- Temperature velocity (`dT/dt`)
- Humidity velocity
- Trend acceleration
- Critical threshold prediction horizons

This allows the system to predict dangerous conditions *before* operational limits are breached.

---

## Autonomous Safety Response

If anomaly severity exceeds the configured threshold:

```text
Anomaly Score ≥ 0.85
```

The AI engine autonomously:

1. Bypasses manual intervention
2. Publishes a secure emergency command
3. Triggers the SG90 servo actuator
4. Executes physical mitigation actions
5. Responds in under 1 second

This closed-loop response mechanism enables real-time cyber-physical protection directly at the edge.

---

# 4️⃣ SOC Visibility & SIEM Integration

The platform includes a fully observable industrial SOC monitoring pipeline.

All anomalous activity is:

- Logged persistently
- Enriched with MITRE ATT&CK for ICS mappings
- Exported in SIEM-compatible formats
- Streamed to Grafana dashboards

---

## MITRE ATT&CK for ICS Mapping

| Technique ID | Description |
|---|---|
| `T0814` | Denial of Control |
| `T0856` | Spoof Reporting Message |
| `T0803` | Block Command Message |
| `T0827` | Alarm Suppression |

---

## SIEM Event Storage

Security events are written to:

```bash
/data/siem/events/alerts.json
```

Compatible output formats include:

- Syslog
- CEF
- JSON event pipelines

Designed for downstream integration with:

- Wazuh
- Splunk
- Elastic SIEM
- QRadar
- Microsoft Sentinel

---

# 📊 Observability Stack

| Component | Purpose |
|---|---|
| **Eclipse Mosquitto 2.0.18** | Secure MQTT message broker |
| **Prometheus 2.45.0** | Metrics aggregation and time-series collection |
| **Grafana 12.4.1** | SOC visualization and dashboarding |
| **Node Exporter** | Infrastructure metrics |
| **Python MQTT Exporter** | MQTT telemetry metric exposure |
| **Jetson Nano AI Engine** | Real-time anomaly detection |

---

# 🧰 Technology Stack

## Hardware

- Seeed reComputer J1010 (Jetson Nano)
- ESP32 Development Boards
- DHT22 Sensors
- SG90 Servo Motor
- ASUS ROG Laptop

---

## Software & Frameworks

- Ubuntu Linux
- Mosquitto MQTT Broker
- Python
- Prometheus
- Grafana
- OpenSSL
- UFW Firewall
- Docker/Kubernetes (Simulation Layer)
- Calico CNI Policies

---

## AI & Security

- Isolation Forest
- Behavioral Anomaly Detection
- MITRE ATT&CK for ICS
- X.509 PKI Infrastructure
- mTLS Authentication
- ACL Segmentation

---

# 📂 Repository Structure

```text
Zero-Trust-Factory/
│
├── Physical_Hardware_Implementation/
│   │
│   ├── edge_nodes/
│   │   ├── esp32_sensor_nodes/
│   │   └── servo_controller/
│   │
│   ├── ai_security_layer/
│   │   ├── isolation_forest/
│   │   ├── training_scripts/
│   │   └── baseline_datasets/
│   │
│   ├── siem_pipeline/
│   │   ├── prometheus_exporter/
│   │   ├── grafana_dashboards/
│   │   └── alert_pipeline/
│   │
│   ├── mosquitto_config/
│   │   ├── acl/
│   │   ├── certificates/
│   │   └── broker_configs/
│   │
│   └── scripts/
│       ├── startup/
│       ├── health_checks/
│       └── automation/
│
├── Zero-Trust-Factory_Simulation/
│   ├── kubernetes_lab/
│   ├── calico_policies/
│   └── simulated_ot_environment/
│
├── References/
│   ├── IEC62443/
│   ├── Threat_Models/
│   └── Research_Papers/
│
├── images/
│
├── LICENSE
│
└── README.md
```

---

# 🚀 Getting Started

# 1️⃣ Start Core Services

Run the startup orchestration script on the Jetson Nano:

```bash
./Physical_Hardware_Implementation/scripts/start_ztfcp.sh
```

This initializes:

- Mosquitto Broker
- Prometheus
- Grafana
- MQTT Exporter
- AI Detection Engine
- SIEM Pipeline

---

# 2️⃣ Verify Infrastructure Health

The startup script automatically validates all critical interfaces.

| Port | Service |
|---|---|
| `1883` | Internal MQTT Wiretap |
| `8883` | Secure MQTTS (mTLS Enforced) |
| `3000` | Grafana SOC Dashboard |
| `8000` | Python MQTT Exporter |
| `9090` | Prometheus TSDB |
| `9100` | Node Exporter |

---

# 3️⃣ Access the SOC Dashboard

Open Grafana from an authorized administrative workstation:

```text
http://192.168.137.200:3000
```

### Default Credentials

```text
Username: admin
Password: admin123
```

> ⚠️ Change default credentials before production deployment.

---

# 🔬 Research & Academic Relevance

This project aligns with:

- IEC 62443 Industrial Security Standards
- NIST Zero Trust Architecture
- Industry 5.0 Cyber-Physical Security
- Smart Manufacturing Security Models
- Industrial SOC Modernization
- Secure IIoT Infrastructure Design

The architecture demonstrates practical implementation of:

- OT/IT convergence security
- Real-time edge AI security
- Autonomous industrial defense systems
- Secure industrial telemetry pipelines

---

# 🛣️ Future Scope

## Expand Physical Infrastructure

- Integrate PLCs and RTUs
- Add Modbus and OPC-UA support
- Introduce brownfield industrial devices
- Expand sensor diversity

---

## Advanced AI Security Models

Future AI enhancements include:

- LSTM Autoencoders
- Time-series behavioral learning
- Replay attack detection
- Telemetry spoofing analysis
- Federated industrial learning pipelines

---

## Production SOC Pilot

Planned deployment goals include:

- Wazuh SIEM integration
- Threat hunting workflows
- Incident correlation pipelines
- Multi-factory monitoring support

---

## Compliance Certification

Future compliance objectives:

- IEC 62443 SL-2 alignment
- Formal risk assessment documentation
- Security audit readiness
- Industrial penetration testing validation

---

# 📚 References

## Industrial Security Standards & Frameworks

1. **IEC 62443 — Industrial Communication Networks & System Security**  
   https://www.iec.ch/cyber-security

2. **NIST SP 800-207 — Zero Trust Architecture**  
   https://csrc.nist.gov/publications/detail/sp/800-207/final

3. **MITRE ATT&CK for ICS Framework**  
   https://attack.mitre.org/matrices/ics/

4. **Purdue Enterprise Reference Architecture (PERA)**  
   https://www.cisa.gov/resources-tools/resources/purdue-model-control-hierarchy

---

## Industry 5.0 & Smart Manufacturing

5. **European Commission — Industry 5.0**  
   https://ec.europa.eu/info/research-and-innovation/research-area/industrial-research-and-innovation/industry-50_en

6. **NIST Cybersecurity Framework (CSF)**  
   https://www.nist.gov/cyberframework

---

## Technologies & Platforms Used

7. **Eclipse Mosquitto MQTT Broker**  
   https://mosquitto.org/

8. **Prometheus Monitoring System**  
   https://prometheus.io/

9. **Grafana Observability Platform**  
   https://grafana.com/

10. **OpenSSL Cryptographic Toolkit**  
    https://www.openssl.org/

11. **Calico Network Policies**  
    https://www.tigera.io/project-calico/

---

## AI & Machine Learning References

12. **Isolation Forest Paper — Liu et al.**  
    https://ieeexplore.ieee.org/document/4781136

13. **Scikit-learn Isolation Forest Documentation**  
    https://scikit-learn.org/stable/modules/generated/sklearn.ensemble.IsolationForest.html

---

## Hardware Documentation

14. **ESP32 Documentation — Espressif Systems**  
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/

15. **NVIDIA Jetson Nano Developer Kit**  
    https://developer.nvidia.com/embedded/jetson-nano-developer-kit

16. **DHT22 Sensor Datasheet**  
    https://cdn.sparkfun.com/assets/f/7/d/9/c/DHT22.pdf

17. **SG90 Servo Motor Datasheet**  
    https://components101.com/motors/servo-motor-basics-pinout-datasheet

---

## Research & Academic Inspiration

18. Research papers and industrial whitepapers related to:
   - OT/IT Convergence Security
   - Industrial Zero Trust Architecture
   - AI-based ICS Threat Detection
   - Behavioral Anomaly Detection in IIoT
   - Smart Factory Security Models

---

# 🏆 Project Team

Developed as a Final Year Engineering Capstone Project under the academic guidance of **Prof. Amol Suryawanshi**.

## Team Members


- **Shardul Bangale** — 22311187
- **Arya Kuwar** — 22311452
- **Shambhavi Raj** — 22311465
- **Shravya Bhandary** — 22310645
  

---


# 📜 License

This project is released under the terms defined in the `LICENSE` file.

---

# ⭐ Acknowledgements

Special thanks to:

- VIIT Pune Faculty & Research Mentors
- Open-source security communities
- IEC 62443 industrial security references
- MITRE ATT&CK for ICS contributors
- Eclipse Mosquitto developers
- Grafana & Prometheus maintainers

---



# 🔥 Final Vision

The **Zero Trust Factory Control Plane (ZTFCP)** represents a step toward resilient, autonomous, and security-first industrial environments where cyber defense is embedded directly into the operational fabric of the factory itself.

By combining:

- Zero Trust principles
- AI-driven edge intelligence
- Purdue-aligned segmentation
- Real-time autonomous response

the architecture demonstrates how future Industry 5.0 ecosystems can remain both highly connected and operationally secure.
