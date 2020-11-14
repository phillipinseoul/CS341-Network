# CS341-Introduction-to-Networks

1. Practice #1: Simple Web Communication (Python)
2. Practice #2: TCP Socket Communication (Python)
3. Practice #3: Congestion Control Simulation (NS3)


## Practice #3: Congestion Control Simulation (NS3)
"Understand why we need TCP congestion algorithm, and how the network can handle the congestion controls."

### Task 1
  1. Write a C++ program code to simulate each network system.
  2. Plot the graph for congestion control mechanism in each scenario (Use gnuplot).
    * TCP Throughput (.rx file)
    * Congestion Window Size (.cwnd file)
    

#### Network Topology
  * Nodes: n0 - n4
  * Links: n0n1 (2Mbps, 10ms), n1n2 (2Mbps, 10ms), n1n3 (5Mbps, 10ms), n1n4 (1Mbps, 10ms)
  * Simulation Time: t = 0 - 120s
  
#### Scenario 1: Only App (a) is running.
  * App (a)
    + Starts running on n2, at t = 1s
    + Sends total 25MB to n0 (port 8080)
    + Divides 25MB data into (1000 bytes) x (25,000 packets) & Data-rate is 2Mbps
    
#### Scenario 2: App (a), (b) are running.
  * App (a): Same
  * App (b)
    + Starts running on n3, at t = 15s
    + Sends total 25MB to n0 (port 8081)
    + Divides 25MB data into (1000 bytes) x (25,000 packets) & Data-rate is 5Mbps
    
#### Scenario 3: App (a), (b), (c) are running.
  * App (a): Same
  * App (b): Same
  * App (c)
    + Starts running on n4, at t = 30s
    + Sends total 5MB to n0 (port 8082)
    + Divides 5MB data into (1000 bytes) x (5,000 packets) & Data-rate is 1Mbps
    
#### Scenario 4: App (a), (b), (c) are running & Receive Error from n1 to n0
  * App (a): Same
  * App (b): Same
  * App (c): Same
  * Receive Error Rate (from n1 to n0): 1e-6
    + Using RateErrorModel in ns3 API


### Task 2
  1. Change the TCP Congestion Control algorithm.
  2. Plot the graph for congestion control mechanism in each scenario (Use gnuplot).
  

#### Task 2-1: Remove Congestion Avoidance & TCP Recovery, respectively.
  * 2-1-1: Change tcp-congestion-ops.cc (Remove Congestion Avoidance)
<img width="500" alt="20201115_012636" src="https://user-images.githubusercontent.com/59787386/99151941-0ac3ca80-26e2-11eb-9e3e-e951f9a0ac82.png">
<img width="600" alt="2" src="https://user-images.githubusercontent.com/59787386/99152292-4495d080-26e4-11eb-9e90-612af457a090.png">

  * 2-1-2: Change tcp-recovery-ops (Remove TCP Recovery)
<img width="724" alt="3" src="https://user-images.githubusercontent.com/59787386/99152324-75760580-26e4-11eb-985b-5488b9601913.png">
