/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/tcp-socket-state.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Prac3");

class MyApp : public Application
{
public:
  MyApp();
  virtual ~MyApp();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId(void);
  void Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void ScheduleTx(void);
  void SendPacket(void);

  Ptr<Socket> m_socket;
  Address m_peer;
  uint32_t m_packetSize;
  uint32_t m_nPackets;
  DataRate m_dataRate;
  EventId m_sendEvent;
  bool m_running;
  uint32_t m_packetsSent;
};

MyApp::MyApp()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId(void)
{
  static TypeId tid = TypeId("MyApp")
                          .SetParent<Application>()
                          .SetGroupName("Tutorial")
                          .AddConstructor<MyApp>();
  return tid;
}

void MyApp::Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void MyApp::StartApplication(void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind();
  m_socket->Connect(m_peer);
  SendPacket();
}

void MyApp::StopApplication(void)
{
  m_running = false;

  if (m_sendEvent.IsRunning())
  {
    Simulator::Cancel(m_sendEvent);
  }

  if (m_socket)
  {
    m_socket->Close();
  }
}

void MyApp::SendPacket(void)
{
  Ptr<Packet> packet = Create<Packet>(m_packetSize);
  m_socket->Send(packet);

  if (++m_packetsSent < m_nPackets)
  {
    ScheduleTx();
  }
}

void MyApp::ScheduleTx(void)
{
  if (m_running)
  {
    Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
    m_sendEvent = Simulator::Schedule(tNext, &MyApp::SendPacket, this);
  }
}

static void
NotifyChange(Ptr<OutputStreamWrapper> stream, uint32_t oldval, uint32_t newval)
{
  *stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << oldval << "\t" << newval << std::endl;
}


Ptr<PacketSink> sink1;     /* Pointer to the packet sink application */
uint64_t lastTotalRx1(0);  /* The value of the last total received bytes */

void ReceivedBytes(Ptr<OutputStreamWrapper> stream)
{
  Time now = Simulator::Now();                                         /* Return the simulator's virtual time. */
  *stream->GetStream() << now.GetSeconds() << "\t"
                       << (sink1->GetTotalRx() - lastTotalRx1) *8 / 0.1 * 1e-6 << "\t" // throughput in Mbps
                       << sink1->GetTotalRx() << "\t" << std::endl; // received bytes in sink1
  lastTotalRx1 = sink1->GetTotalRx();
  Simulator::Schedule(MilliSeconds(100), &ReceivedBytes, stream);
}

int main(int argc, char *argv[])
{
  CommandLine cmd(__FILE__);
  cmd.Parse(argc, argv);

  NS_LOG_INFO("Create nodes.");
  NodeContainer c;
  c.Create(3);
  NodeContainer n1n0 = NodeContainer(c.Get(1), c.Get(0));
  NodeContainer n2n1 = NodeContainer(c.Get(2), c.Get(1));

  InternetStackHelper internet;
  internet.Install(c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer d1d0 = p2p.Install(n1n0);

  p2p.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer d2d1 = p2p.Install(n2n1);

  // Later, we add IP addresses.
  NS_LOG_INFO("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i0 = ipv4.Assign(d1d0);

  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i1 = ipv4.Assign(d2d1);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  NS_LOG_INFO("Create Applications.");

  uint16_t sinkPort1 = 8080;
  Address sinkAddress1(InetSocketAddress(i1i0.GetAddress(1), sinkPort1));
  PacketSinkHelper packetSinkHelper1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort1));
  ApplicationContainer sinkApp1 = packetSinkHelper1.Install(c.Get(0));
  sink1 = StaticCast<PacketSink>(sinkApp1.Get(0));
  sinkApp1.Start(Seconds(0.)); //sink apps run from t=0s
  sinkApp1.Stop(Seconds(120.));


  Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket(c.Get(2), TcpSocketFactory::GetTypeId());
  Ptr<MyApp> app1 = CreateObject<MyApp>();
  app1->Setup(ns3TcpSocket1, sinkAddress1, 1000, 25000, DataRate("2Mbps"));
  c.Get(2)->AddApplication(app1);
  app1->SetStartTime(Seconds(1.)); //app starts at t=1s
  app1->SetStopTime(Seconds(120.));


  AsciiTraceHelper asciiTraceHelper;

  Simulator::Schedule(Seconds(1.1),
                      &ReceivedBytes,
                      asciiTraceHelper.CreateFileStream("prac3_1-1.rx"));

  ns3TcpSocket1->TraceConnectWithoutContext(
      "CongestionWindow",
      MakeBoundCallback(
          &NotifyChange,
          asciiTraceHelper.CreateFileStream("prac3_1-1.cwnd")));

  ns3TcpSocket1->TraceConnectWithoutContext(
      "SlowStartThreshold",
      MakeBoundCallback(
          &NotifyChange,
          asciiTraceHelper.CreateFileStream("prac3_1-1.ssthresh")));

  Simulator::Stop(Seconds(120));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
