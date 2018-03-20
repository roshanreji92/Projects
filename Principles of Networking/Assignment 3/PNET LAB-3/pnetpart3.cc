#include "ns3/applications-module.h"
#include "ns3/mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/my-pnet-mac.h"
#include "ns3/my-pnet-phy.h"
#include "ns3/pnet-channel.h"
#include "ns3/my-pnet-mac-helper.h"
#include "ns3/my-pnet-phy-helper.h"
#include "ns3/pnet-channel-helper.h"
#include "ns3/internet-module.h"
#include "ns3/pnet-module.h"
#include "ns3/rectangle.h"
#include "ns3/constant-position-mobility-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  
  LogComponentEnable("MyPnetPhy", LOG_LEVEL_INFO);
  //LogComponentEnable("PnetChannel", LOG_LEVEL_ALL);
  
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (3);

  PnetChannelHelper channel;
 
  
  MyPnetMacHelper mac = MyPnetMacHelper::Default ();

  MyPnetPhyHelper phy = MyPnetPhyHelper::Default ();
  phy.Set ("DataRate", StringValue ("5Mbps"));
  

 
  PnetHelper pnet;
  NetDeviceContainer devices;
  devices = pnet.Install (nodes, channel.Create(), phy, mac);

 MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds",
                              RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobility.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient1 (interfaces.GetAddress (1), 9);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (256));

  ApplicationContainer clientApps = echoClient1.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

   UdpEchoClientHelper echoClient2 (interfaces.GetAddress (1), 9);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (256));

  ApplicationContainer clientApps1 = echoClient2.Install (nodes.Get (2));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  Simulator::Stop (Seconds (10.0));//

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
