
#include "my-pnet-phy.h"
#include "pnet-net-device.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include "ns3/pointer.h"
#include "ns3/net-device.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/boolean.h"
#include "ns3/node.h"
#include <cmath>
#include <queue>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyPnetPhy");

NS_OBJECT_ENSURE_REGISTERED (MyPnetPhy);

TypeId
MyPnetPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyPnetPhy")
    .SetParent<PnetPhy> ()
    .AddConstructor<MyPnetPhy> ()   
    .AddAttribute ("DataRate",
                   "The default data rate for point to point links. Zero means infinite",
                   DataRateValue (DataRate ("0b/s")),
                   MakeDataRateAccessor (&MyPnetPhy::m_bps),
                   MakeDataRateChecker ())   
  ;
  return tid;
}

MyPnetPhy::MyPnetPhy ()
{
  NS_LOG_FUNCTION (this);
  
}

MyPnetPhy::~MyPnetPhy ()
{
  NS_LOG_FUNCTION (this);
}

void
MyPnetPhy::SetDevice (Ptr<PnetNetDevice> device)
{
  NS_LOG_FUNCTION (this);
  m_device = device;
}

Ptr<PnetNetDevice>
MyPnetPhy::GetDevice (void) const
{
  return m_device;
}


Ptr<PnetChannel>
MyPnetPhy::GetChannel (void) const
{
  return m_channel;
}

void
MyPnetPhy::SetChannel (Ptr<PnetChannel> channel)
{ p =0;
  m_channel = channel;
  m_channel->Add (this);
}

void
MyPnetPhy::SetReceiveCallback (PnetPhy::RxCallback callback)
{
  m_rxCallback= callback;
}


void
MyPnetPhy::SendPacket (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);
  Time txDuration = CalculateTxDuration (packet);
  NS_ASSERT (txDuration > NanoSeconds (0));
   
  
  endtx = Simulator::Now() + txDuration; // Calculating the time at which the transmission will end
if (endtx > Simulator::Now ()) // If that time is greater than the current time
{
NS_LOG_INFO("The Channel is transmitting");  // Then the channel is transmitting
tx = true;
}
m_channel->Send (this, packet, txDuration);
  Simulator::Schedule (txDuration, &MyPnetPhy::TransmitComplete, this); // Scheduling the transmit complete function after txduration
}

void
MyPnetPhy::TransmitComplete () 
{// Reached here means channel is idle
  NS_LOG_FUNCTION (this);
tx = false;
NS_LOG_INFO("The channel is idle"); 
  return;
}


void
MyPnetPhy::StartReceivePacket (Ptr<Packet> packet, Time rxDuration)
{
   NS_LOG_FUNCTION (this);
 p++;
   Simulator::Schedule (rxDuration, &MyPnetPhy::ReceiveComplete, this, packet); // Scheduling Receive complete function after rxDuration
   endrx = Simulator::Now() + rxDuration;  // Calculating the time at which reception will end
if(p>=2)  // If receiving multiple packets then collision happening 
{
NS_LOG_INFO("Collision is happening");
}
if (endrx > Simulator::Now ())
{
NS_LOG_INFO("The channel is receiving"); // If current time is less than endrx then channel is still receiving
rx = true;
}
}
void
MyPnetPhy::ReceiveComplete (Ptr<Packet> packet)
{ 
  NS_LOG_FUNCTION (this);
 rx = false;
p--;  // Decrementing p each time reception is complete

if(p==0)
{
NS_LOG_INFO("The channel is idle"); // No packet receiving means channel idle.
}
  m_rxCallback (packet);
  return;
}

Time
MyPnetPhy::CalculateTxDuration(Ptr<const Packet> packet)
{
  Time txTime = Time (0);
  if (m_bps > DataRate (0))
    {
      txTime = m_bps.CalculateBytesTxTime (packet->GetSize ());
    }
  return txTime;
}


} //namespace ns3
