#include <algorithm>
#include "ns3/mobility-model.h"
#include "pnet-channel.h"
#include "pnet-phy.h"
#include "pnet-net-device.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/my-pnet-phy.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PnetChannel");

NS_OBJECT_ENSURE_REGISTERED (PnetChannel);

TypeId 
PnetChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PnetChannel")
    .SetParent<Channel> ()
    .AddConstructor<PnetChannel> ()
    .AddAttribute ("Delay", "Transmission delay through the channel",
                   TimeValue (MilliSeconds (2)),
                   MakeTimeAccessor (&PnetChannel::m_delay),
                   MakeTimeChecker ())
  ;
  return tid;
}

PnetChannel::PnetChannel ()
{
  NS_LOG_FUNCTION (this);
}

void
PnetChannel::Send (Ptr<PnetPhy> sender, Ptr<Packet> p, Time duration)
{
Ptr<MobilityModel> first_sender= sender->GetDevice ()->GetNode ()->GetObject<MobilityModel> (); //Getting the mobility model for the sender using sender pointer
  NS_LOG_FUNCTION (this );
  uint32_t j = 0;
     
  for (PhyList::const_iterator i = m_phyList.begin (); i != m_phyList.end (); i++,j++)
    {
     
      Ptr<PnetPhy> tmp = *i;
      if (tmp != sender)
        {
				  NS_LOG_INFO ("Node id"<<tmp->GetDevice()->GetNode ()->GetId ());
                             Ptr<MobilityModel> first_receiver=tmp->GetDevice()->GetNode ()->GetObject<MobilityModel>(); //Getting the mobility model for the sender using sender pointer
                                  float distance=first_sender->GetDistanceFrom(first_receiver); // Calculating the distance
                                  float time =(distance/300000000)*1000000000;  // Calculating propogation time in nanoseconds
                          
                             m_delay=NanoSeconds(time);
Simulator::ScheduleWithContext (tmp->GetDevice()->GetNode ()->GetId (), m_delay,
				                                  &PnetChannel::Receive, this, j, p->Copy (), duration); //Scheduled packet reception after propogation delay
NS_LOG_INFO("Distance between "<< sender->GetDevice()->GetNode ()->GetId () << " to node  "<<tmp->GetDevice()->GetNode ()->GetId ()<<" is "<<distance<<" and the time delay is "<<time<< " ns "<<std::endl); // Using LOG INFO to display the delay
				  //NS_LOG_INFO ("after schedule");
        }
    }
}

void
PnetChannel::Receive (uint32_t i, Ptr<Packet> packet, Time rxDuration) const
{
  m_phyList[i]->StartReceivePacket (packet, rxDuration);
}

void
PnetChannel::Add (Ptr<PnetPhy> phy)
{
  NS_LOG_FUNCTION (this << phy);
  m_phyList.push_back (phy);
}

uint32_t
PnetChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phyList.size ();
}

Ptr<NetDevice>
PnetChannel::GetDevice (uint32_t i) const
{
  NS_LOG_FUNCTION (this << i);
  return m_phyList[i]->GetDevice ()->GetObject<NetDevice> ();
}


} // namespace ns3
