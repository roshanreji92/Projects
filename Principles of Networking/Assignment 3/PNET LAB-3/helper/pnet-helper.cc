
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "pnet-helper.h"
#include "ns3/pnet-net-device.h"
#include "ns3/pnet-mac.h"
#include "ns3/pnet-phy.h"
//#include "ns3/pnet-channel.h"

#include "ns3/trace-helper.h"
#include <sstream>
#include <string>

NS_LOG_COMPONENT_DEFINE("PnetHelper");

namespace ns3
{
  
PnetMacHelper::~PnetMacHelper ()
{}

PnetPhyHelper::~PnetPhyHelper ()
{}
/*
PnetChannelHelper::~PnetChannelHelper ()
{}*/

PnetHelper::PnetHelper ()
{}

PnetHelper::~PnetHelper ()
{}

NetDeviceContainer
PnetHelper::Install (NodeContainer c, Ptr<PnetChannel> channel, const PnetPhyHelper &phyHelper, const PnetMacHelper &macHelper) const
{
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      Ptr<Node> node = *i;
      Ptr<PnetNetDevice> device = CreateObject<PnetNetDevice> ();

      Ptr<PnetPhy> phy = phyHelper.Create ();
      Ptr<PnetMac> mac = macHelper.Create ();

      mac->SetAddress (Mac48Address::Allocate ());
      phy->SetChannel (channel);
      device->SetMac (mac);
      device->SetPhy (phy);
      device->SetChannel (channel);

      node->AddDevice (device);
      devices.Add (device);

      NS_LOG_DEBUG ("node="<<node<<", mob="<<node->GetObject<MobilityModel> ());
  }
  return devices;
}

} //end namespace ns3
