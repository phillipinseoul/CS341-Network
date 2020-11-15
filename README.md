# KAIST-CS341-Introduction-to-Networks (20 Fall)

1. Practice #1: Simple Web Communication (Python)
2. Practice #2: TCP Socket Communication (Python)
3. Practice #3: Congestion Control Simulation (NS3, C++)


* * *


## Practice #2: TCP Socket Communication (Python)
"Implement a connection-oriented, client-server protocol based on given specification."
"Understand the mechanism of TCP Handshake."

### Task 1: Basic String Decryption Service
  * Implement a client which communicates with the server.
  * All protocols are implemented over TCP sockets.
  
#### Protocol Specification
  * Byte order follows network order.
  * Header
  
<img width="400" alt="10" src="https://user-images.githubusercontent.com/59787386/99161710-45585200-2738-11eb-973d-696a4dbfd4ca.png">
 
   + flag field (16 bits): 0 (more packets), 1 (last packet)
   + checksum field (16 bits): error checking
   + keyword field (32 bits)
   + sid field (32 bits): student ID
   + length field (32 bits): MAX length = 10KB
   + data field: string to be transmitted


* * *


## Practice #3: Congestion Control Simulation (NS3)
"Understand why we need TCP congestion algorithm, and how the network can handle the congestion controls."

### Task 1
  1. Write a C++ program code to simulate each network system.
  2. Plot the graph for congestion control mechanism in each scenario (Use gnuplot).
    * TCP Throughput (.rx file)
    * Congestion Window Size (.cwnd file)
  3. How to run the simulation?
    + Copy the .cc file to "scratch/" directory
    + Run on ubuntu terminal: "./waf --run scratch/filename"

#### Network Topology
  * Nodes: n0 - n4
  * Links: n0n1 (2Mbps, 10ms), n1n2 (2Mbps, 10ms), n1n3 (5Mbps, 10ms), n1n4 (1Mbps, 10ms)
  * Simulation Time: t = 0 - 120s
  
<img width="647" alt="4" src="https://user-images.githubusercontent.com/59787386/99152541-bde1f300-26e5-11eb-9d98-cd79488fad97.png">
  
#### Scenario 1: Only App (a) is running.
  * App (a)
    + Starts running on n2, at t = 1s
    + Sends total 25MB to n0 (port 8080)
    + Divides 25MB data into (1000 bytes) x (25,000 packets) & Data-rate is 2Mbps

<img width="718" alt="5" src="https://user-images.githubusercontent.com/59787386/99152961-9f312b80-26e8-11eb-8e36-b1ea16756825.png">
    
#### Scenario 2: App (a), (b) are running.
  * App (a): Same
  * App (b)
    + Starts running on n3, at t = 15s
    + Sends total 25MB to n0 (port 8081)
    + Divides 25MB data into (1000 bytes) x (25,000 packets) & Data-rate is 5Mbps
 
<img width="718" alt="6" src="https://user-images.githubusercontent.com/59787386/99152962-a0625880-26e8-11eb-9f3c-51de2c372c94.png">
    
#### Scenario 3: App (a), (b), (c) are running.
  * App (a): Same
  * App (b): Same
  * App (c)
    + Starts running on n4, at t = 30s
    + Sends total 5MB to n0 (port 8082)
    + Divides 5MB data into (1000 bytes) x (5,000 packets) & Data-rate is 1Mbps

<img width="718" alt="7" src="https://user-images.githubusercontent.com/59787386/99152965-a22c1c00-26e8-11eb-8d65-1382d17e7150.png">
    
#### Scenario 4: App (a), (b), (c) are running & Receive Error from n1 to n0
  * App (a): Same
  * App (b): Same
  * App (c): Same
  * Receive Error Rate (from n1 to n0): 1e-6
    + Using RateErrorModel in ns3 API
    
<img width="718" alt="8" src="https://user-images.githubusercontent.com/59787386/99152966-a48e7600-26e8-11eb-8db0-c7214aac0afd.png">


### Task 2
  1. Change the TCP Congestion Control algorithm.
  2. Plot the graph for congestion control mechanism in each scenario (Use gnuplot).
  

#### Task 2-1: Remove Congestion Avoidance & TCP Recovery, respectively.
  * 2-1-1: Change tcp-congestion-ops.cc (Remove Congestion Avoidance)
    + Default TCP congestion control algorithm: New Reno
    + src/internet/model/tcp-congestion-ops.cc
    + src/internet/model/tcp-recovery-ops.cc
    
<img width="500" alt="20201115_012636" src="https://user-images.githubusercontent.com/59787386/99151941-0ac3ca80-26e2-11eb-9e3e-e951f9a0ac82.png">
<img width="600" alt="2" src="https://user-images.githubusercontent.com/59787386/99152292-4495d080-26e4-11eb-9e90-612af457a090.png">

  * 2-1-2: Change tcp-recovery-ops (Remove TCP Recovery)
<img width="724" alt="3" src="https://user-images.githubusercontent.com/59787386/99152324-75760580-26e4-11eb-985b-5488b9601913.png">


#### Task 2-2: Change Congestion Control Algorithms: Veno, Yeah
  * Change the algorithm to Veno.
  * Change the algorithm to Yeah.
