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

// Adhoc Mode with RTS

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi = 6;
  bool tracing = false;

  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Check for valid number of wifi nodes
  // 250 should be enough, otherwise IP addresses 
  // soon become an issue
  if (nWifi > 250)
    {
      std::cout << "Too many wifi or cnodes, no more than 250 each." << std::endl;
      return 1;
    }

  if (verbose)
    {      
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
      }
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold",UintegerValue(1));   

  NodeContainer wifiadhocnodes;
  wifiadhocnodes.Create (nWifi);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());


  WifiHelper wifi;
  wifi.SetStandard(WIFI_PHY_STANDARD_80211g);
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
 

  //Standard MAC without QOS https://www.nsnam.org/doxygen/classns3_1_1_nqos_wifi_mac_helper.html 
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();

  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac"); //For Adhoc Mode

  NetDeviceContainer adDevices;
  adDevices = wifi.Install (phy, mac, wifiadhocnodes);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobility.Install (wifiadhocnodes);

  InternetStackHelper stack;
  stack.Install (wifiadhocnodes);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces;
  wifiInterfaces = address.Assign (adDevices);
  
  UdpEchoServerHelper echoServer (20);

  ApplicationContainer serverApps = echoServer.Install (wifiadhocnodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient1 (wifiInterfaces.GetAddress (0), 20);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (wifiadhocnodes.Get (5));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  
  UdpEchoClientHelper echoClient2 (wifiInterfaces.GetAddress (0), 20);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = echoClient2.Install (wifiadhocnodes.Get (4));
  clientApps2.Start (Seconds (3.0));
  clientApps2.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  phy.EnablePcap("A2T1WithRTS-node2",adDevices.Get(2),true);
  Simulator::Stop (Seconds (10.0));


  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
