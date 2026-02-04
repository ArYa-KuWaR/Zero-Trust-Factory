# Zero-Trust-Factory

A hands-on, micro-segmented **smart factory** simulation that demonstrates how a Zero Trust security model can stop lateral-movement attacks inside an Industry 5.0 environment.

This project runs multiple “factory components” as containers inside a Kubernetes (Minikube) cluster. A default-deny policy is applied first, then only the minimum required connections are allowed using Kubernetes NetworkPolicies.

## Project idea
Traditional “castle-and-moat” security assumes everything inside the network is safe. In a highly connected smart factory, a single compromised component (like an IoT sensor or a robot controller) can be used to move laterally to other internal systems.

This repo demonstrates:
-How a breach can originate from a less-secure component.
-How attackers attempt **lateral movement** to reach sensitive systems.
-How Zero Trust + micro-segmentation contains the breach by denying all traffic by default and allowing only explicitly approved communication.

## Architecture (simulated components)
The Kubernetes cluster runs a small set of services to mimic a smart factory network:

-`robot-controller`: a critical component that needs access to the design database.
-`iot-sensor`: represents IoT telemetry generation.
-`design-db`: a legitimate database the robot needs.
-`payroll-db`: a sensitive database that should never be reachable from the robot.
-`dashboard`: a simple visualization/monitoring UI.

## Security model
This repo applies Zero Trust principles:
-**Never trust, always verify**: nothing is implicitly trusted, even inside the cluster.
-**Micro-segmentation**: the network is segmented into small zones (down to single services).
-**Default deny**: all traffic is blocked first.
-**Least privilege allow-listing**: only required traffic paths are re-enabled.

NetworkPolicies used:
-`kubernetes/1-default-deny-policy.yaml`: blocks all traffic by default.
-`kubernetes/2-allow-traffic-policy.yaml`: allows only the required connections.

## Prerequisites
-Minikube
-Docker
-kubectl
-Calico CNI (used for enforcing NetworkPolicy)

> The steps below assume Windows PowerShell (because of `Invoke-Expression`).

## Run locally (Minikube + Calico)
### 1) Start Minikube with Calico
```powershell
minikube start --network-plugin=cni --cni=calico
```

### 2) Point Docker to Minikube’s Docker daemon
```powershell
minikube docker-env | Invoke-Expression
```

### 3) Build images
```powershell
cd src
docker build -t robot-controller ./robot_controller
docker build -t iot-sensor ./iot_sensor
docker build -t design-db ./design_db
docker build -t payroll-db ./payroll_db
docker build -t dashboard ./dashboard
```

### 4) Deploy services
> Run these commands from the project root folder.

```powershell
kubectl apply -f kubernetes/robot-controller-deployment.yaml
kubectl apply -f kubernetes/iot-sensor-deployment.yaml
kubectl apply -f kubernetes/design-db-deployment.yaml
kubectl apply -f kubernetes/payroll-db-deployment.yaml
kubectl apply -f kubernetes/dashboard-deployment.yaml
```

### 5) Open Kubernetes dashboard
```powershell
minikube dashboard
```

### 6) Apply Zero Trust NetworkPolicies
First, block all traffic:
```powershell
kubectl apply -f kubernetes/1-default-deny-policy.yaml
```

Then, allow only specific traffic:
```powershell
kubectl apply -f kubernetes/2-allow-traffic-policy.yaml
```

## What to demo
A typical demo flow:
-Show normal behavior (robot accesses design-db).
-Simulate a compromise/attack attempt from inside a container.
-Show that lateral movement to restricted targets (like payroll-db) is blocked by the NetworkPolicy.

## Folder structure
-`src/`: source code + Dockerfiles for each simulated component.
-`kubernetes/`: Kubernetes deployments and NetworkPolicy YAMLs.

## Future scope
-Add AI-powered anomaly detection for suspicious east-west traffic patterns.
-Add stronger identity controls (MFA) for operator access.
-Expand the simulation with more Industry 5.0 components (SCADA, cloud analytics, etc.).
