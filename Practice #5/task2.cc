#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/applications-module.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;




int main (int argc, char *argv[])
{
  // Network Configurations
  std::string phyMode ("DsssRate1Mbps");
  double distance = 100;  // m
  uint32_t numNodes = 64;  // 8x8 grid
  double interval = 0.1; // seconds
  uint32_t packetSize = 1500; // bytes

  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  // Implement RTS/CTS
  // Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (100));

  /* 
  Configuring the network
  */

  // Create 64 nodes
  NodeContainer nodes;
  nodes.Create (numNodes);

  // Set the nodes in 8x8 grid 
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (distance),
                                 "DeltaY", DoubleValue (distance),
                                 "GridWidth", UintegerValue (8),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // Create propagation loss matrix
  Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> ();
  
  // Create & Set up wifi channel
  Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
  wifiChannel->SetPropagationLossModel (lossModel);
  wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

  // Confiture wifi
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel);

  // Set to adhoc mode
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // Enable OLSR routing
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);

  // Install TCP/IP stack & Assign IP addresses
  InternetStackHelper internet;
  internet.SetRoutingHelper (list);
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  ipv4.Assign (devices);


  // Create Application container & Install applications
  uint16_t cbrPort = 9;
 
  // flow 1: node 0 -> node 63
  OnOffHelper onoff1 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.64"), cbrPort));
  onoff1.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff1.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp1 = onoff1.Install (nodes.Get (0));
  onOffApp1.Start (Seconds (10));
  onOffApp1.Stop (Seconds (90));

  // flow 2: node 56 -> node 7
  OnOffHelper onoff2 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.8"), cbrPort));
  onoff2.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff2.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff2.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp2 = onoff2.Install (nodes.Get (56));
  onOffApp2.Start (Seconds (10.5));
  onOffApp2.Stop (Seconds (90));

  // flow 3: node 16-> node 23
  OnOffHelper onoff3 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.24"), cbrPort));
  onoff3.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff3.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff3.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff3.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp3 = onoff3.Install (nodes.Get (16));
  onOffApp3.Start (Seconds (11));
  onOffApp3.Stop (Seconds (90));

  // flow 4: node 40 -> node 47
  OnOffHelper onoff4 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.48"), cbrPort));
  onoff4.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff4.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff4.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff4.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp4 = onoff4.Install (nodes.Get (40));
  onOffApp4.Start (Seconds (11.5));
  onOffApp4.Stop (Seconds (90));

  // flow 5: node 58 -> node 2
  OnOffHelper onoff5 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.3"), cbrPort));
  onoff5.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff5.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff5.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff5.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp5 = onoff5.Install (nodes.Get (58));
  onOffApp5.Start (Seconds (12));
  onOffApp5.Stop (Seconds (90));

  // flow 6: node 5 -> node 61
  OnOffHelper onoff6 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.62"), cbrPort));
  onoff6.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff6.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff6.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff6.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp6 = onoff6.Install (nodes.Get (5));
  onOffApp6.Start (Seconds (12.5));
  onOffApp6.Stop (Seconds (90));

  // flow 7: node 26-> node 53
  OnOffHelper onoff7 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.54"), cbrPort));
  onoff7.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff7.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff7.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff7.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp7 = onoff7.Install (nodes.Get (26));
  onOffApp7.Start (Seconds (13));
  onOffApp7.Stop (Seconds (90));

  // flow 8: node 13-> node 51
  OnOffHelper onoff8 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.52"), cbrPort));
  onoff8.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff8.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff3.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff8.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp8 = onoff8.Install (nodes.Get (13));
  onOffApp8.Start (Seconds (13.5));
  onOffApp8.Stop (Seconds (90));


  // Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.Install(nodes); // TODO You can change the name of the NodeContainer if you wish

  Simulator::Stop (Seconds (100.0));
  Simulator::Run ();

  // Print per flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

      std::cout<< "Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress << "\n";
      std::cout<< "Tx Packets = " << iter->second.txPackets << "\n";
      std::cout<< "Rx Packets = " << iter->second.rxPackets << "\n";
      std::cout<< "Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps" << "\n";
    }
  monitor->SerializeToXmlFile("scratch/practice_5.flowmon", false, true);
  Simulator::Destroy ();

  return 0;
}

