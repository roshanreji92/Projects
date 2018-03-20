

#ifndef PNET_HELPER_H_
#define PNET_HELPER_H_

#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/pnet-net-device.h"
#include "ns3/trace-helper.h"
//#include "ns3/pnet-mac-helper.h"
//#include "ns3/pnet-phy-helper.h"
//#include "ns3/pnet-channel-helper.h"

namespace ns3
{
class PnetMac;
class PnetPhy;
class PnetChannel;

class PnetMacHelper
{
public:
  virtual ~PnetMacHelper ();
  virtual Ptr<PnetMac> Create (void) const = 0;
};

class PnetPhyHelper
{
public:
  virtual ~PnetPhyHelper ();
  virtual Ptr<PnetPhy> Create (void) const = 0;
};

class PnetHelper
{
public:
  PnetHelper();
  virtual ~PnetHelper();
  NetDeviceContainer Install (NodeContainer c, Ptr<PnetChannel> channel, const PnetPhyHelper &phyHelper, const PnetMacHelper &macHelper) const;
  
private:
  
};


} //end namespace ns3

#endif /* PNET_HELPER_H_ */
