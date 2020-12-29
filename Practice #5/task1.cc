/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Pavel Boyko <boyko@iitp.ru>
 *
 * Classical hidden terminal problem and its RTS/CTS solution.
 *
 * Topology: [node 0] <-- -50 dB --> [node 1] <-- -50 dB --> [node 2]
 *
 * This example illustrates the use of
 *  - Wifi in ad-hoc mode
 *  - Matrix propagation loss model
 *  - Use of OnOffApplication to generate CBR stream
 *  - IP flow monitor
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/mobility-helper.h"

using namespace ns3;

/// Run single 10 seconds experiment
void experiment (bool enableCtsRts, std::string wifiManager)
{
  // 0. Enable or disable CTS/RTS
  UintegerValue ctsThr = (enableCtsRts ? UintegerValue (100) : UintegerValue (2200));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);

  // 1. Create 3 nodes
  NodeContainer nodes;
  nodes.Create (3);

  // 2. Place nodes in coordinate
  MobilityHelper mobility;

  Ptr<ListPositionAllocator> posAllocator = CreateObject<ListPositionAllocator>();
  //posAllocator->Add(Vector(15,0,0));  // for task 1.1
  posAllocator->Add(Vector(10,0,0));    // for task 1.2
  posAllocator->Add(Vector(0,0,0));
  posAllocator->Add(Vector(-1,0,0));

  mobility.SetPositionAllocator (posAllocator);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // 3. Create propagation loss matrix
  Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> ();

  // 4. Create & setup wifi channel
  Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
  wifiChannel->SetPropagationLossModel (lossModel);
  wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

  // 5. Install wireless devices
  WifiHelper wifi;
  // wifi.SetStandard (WIFI_PHY_STANDARD_80211g);   // for task 1.1
  wifi.SetStandard (WIFI_PHY_STANDARD_80211_10MHZ);     // for task 1.3

  wifi.SetRemoteStationManager ("ns3::" + wifiManager + "WifiManager");
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel);
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac"); // use ad-hoc MAC
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // uncomment the following to have athstats output
  // AthstatsHelper athstats;
  // athstats.EnableAthstats(enableCtsRts ? "rtscts-athstats-node" : "basic-athstats-node" , nodes);

  // uncomment the following to have pcap output
  // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);


  // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  ipv4.Assign (devices);

  // 7. Install applications: two CBR streams each saturating the channel
  uint16_t cbrPort = 9;

  // flow 1: node 0 -> node 1
  OnOffHelper onOff1 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), cbrPort));
  onOff1.SetAttribute ("PacketSize", UintegerValue (1400));
  onOff1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOff1.SetAttribute ("DataRate", StringValue ("3000000bps"));
  ApplicationContainer onOffApp1 = onOff1.Install (nodes.Get (0));
  onOffApp1.Start (Seconds (1.000000));
  onOffApp1.Stop (Seconds (9));
  
  // flow 2: node 2 -> node 1
  OnOffHelper onOff2 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), cbrPort));
  onOff2.SetAttribute ("PacketSize", UintegerValue (1400));
  onOff2.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onOff2.SetAttribute ("DataRate", StringValue ("3001100bps"));
  ApplicationContainer onOffApp2 = onOff2.Install (nodes.Get (2));
  onOffApp2.Start (Seconds (1.001));
  onOffApp1.Stop (Seconds (9));

  /** \internal
   * We also use separate UDP applications that will send a single
   * packet before the C
   * 
   * BR flows start.
   * This is a workaround for the lack of perfect ARP, see \bugid{187}
   */
  uint16_t  echoPort = 9;
  UdpEchoClientHelper echoClientHelper (Ipv4Address ("10.0.0.2"), echoPort);
  echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClientHelper.SetAttribute ("PacketSize", UintegerValue (10));
  ApplicationContainer pingApps;

  // again using different start times to workaround Bug 388 and Bug 912
  echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.001)));
  pingApps.Add (echoClientHelper.Install (nodes.Get (0)));
  echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.006)));
  pingApps.Add (echoClientHelper.Install (nodes.Get (2)));

  // Change position of A
  Ptr<Node> node = nodes.Get(0);
  Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();

  Simulator::Schedule (Seconds(1.7), &MobilityModel::SetPosition, mob, Vector(24,0,0));
  Simulator::Schedule (Seconds(3.4), &MobilityModel::SetPosition, mob, Vector(38,0,0));
  Simulator::Schedule (Seconds(5.1), &MobilityModel::SetPosition, mob, Vector(52,0,0));
  Simulator::Schedule (Seconds(6.8), &MobilityModel::SetPosition, mob, Vector(66,0,0));
  Simulator::Schedule (Seconds(8.5), &MobilityModel::SetPosition, mob, Vector(80,0,0));
  
  

  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // 9. Run simulation for 10 seconds
  Simulator::Stop (Seconds (10));
  Simulator::Run ();



  // 10. Print per flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      if (i->first > 2)
        {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
          std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
        }
    }

  // 11. Cleanup
  Simulator::Destroy ();
}

int main (int argc, char **argv)
{
  std::string wifiManager ("Arf");
  CommandLine cmd (__FILE__);
  cmd.AddValue ("wifiManager", "Set wifi rate manager (Aarf, Aarfcd, Amrr, Arf, Cara, Ideal, Minstrel, Onoe, Rraa)", wifiManager);
  cmd.Parse (argc, argv);
  //TODO
  experiment (false, wifiManager);
  return 0;
}
