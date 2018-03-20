#include "ns3/uinteger.h"
#include "ns3/event-id.h"
#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "my-pnet-mac.h"
#include "pnet-phy.h"
#include "my-pnet-phy.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/double.h"




namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyPnetMac");

NS_OBJECT_ENSURE_REGISTERED (MyPnetMac);

  

/* static */
TypeId
MyPnetMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyPnetMac")
    .SetParent<PnetMac> ()
    .AddConstructor<MyPnetMac> ()
    .AddAttribute ("DataRetryLimit",
                   "Maximum Limit for Data Retransmission",
                   UintegerValue (2),      // Considering Maximum limit as 2 for retransmission
                   MakeUintegerAccessor (&MyPnetMac::m_dataRetryLimit),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

MyPnetMac::MyPnetMac ()
{
  NS_LOG_FUNCTION (this);
}

MyPnetMac::~MyPnetMac ()
{
  NS_LOG_FUNCTION (this);
}

Time timeout;
int count;
void
MyPnetMac::SetPhy (Ptr<PnetPhy> phy)
{
  NS_LOG_FUNCTION (this);
  m_phy = phy;
  m_phy->SetReceiveCallback (MakeCallback (&MyPnetMac::ReceiveFrame, this));
}

Ptr<Packet> ackpacket=Create<Packet>(8);

Ptr<PnetPhy>
MyPnetMac::GetPhy (void) const
{
  return m_phy;
}

void
MyPnetMac::SetAddress (Mac48Address ad)
{
  NS_LOG_FUNCTION (ad);
  m_address = ad;
}


Mac48Address
MyPnetMac::GetAddress (void) const
{
  return m_address;
}

void
MyPnetMac::SetForwardUpCallback (PnetMac::UpCallback upCallback)
{
  m_forwardUpCb = upCallback;
}

void
MyPnetMac::Enqueue (Ptr<Packet> packet, Mac48Address to)
{
  NS_LOG_FUNCTION (this);
  PnetMacHeader header = PnetMacHeader (m_address, to, THZ_PKT_TYPE_DATA);
  packet->AddHeader (header);
  m_queue.push(packet); 
  SendFrame();
  starttimeout(header,packet,to);   // Called the start time out function
}
void MyPnetMac::SendAck(Ptr<Packet> packet,Mac48Address to,Mac48Address from) //Created a function SendAck to send the acknowledgement
{
  NS_LOG_FUNCTION (this);
  
  PnetMacHeader header = PnetMacHeader(to,from, THZ_PKT_TYPE_ACK);  
  packet->AddHeader (header);//added the header to the packet
  m_queue.push(packet);
  NS_LOG_INFO("Sending acknowledgement from "<< from <<" to "<< to <<std::endl);
  SendFrame();
}
void
MyPnetMac::starttimeout(PnetMacHeader header,Ptr<Packet> packet, Mac48Address to) // Created a new function for time out
{
if(header.GetType()==THZ_PKT_TYPE_DATA && header.GetDestination()!= "ff:ff:ff:ff:ff:ff") 
// Scheduling timeout only if the sent packet was data packet and was not broadcasted
{
  count = 1;
  timeout=NanoSeconds(12500000+Simulator::Now ());
  std::cout<<"Backoffcounter started "<<timeout<<std::endl;  
  
Id = Simulator::Schedule(NanoSeconds(12500000),&MyPnetMac::Retransmit,this,packet, to,m_address); 
//Calling retransmit function after the scheduled time out
  
}

}

void
MyPnetMac::SendFrame (void)
{
  NS_LOG_FUNCTION (this);
  
  m_phy->SendPacket (m_queue.front ());
  m_queue.pop ();
  return;  
}

void
MyPnetMac::ReceiveFrame (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);
  PnetMacHeader header;
  packet->RemoveHeader (header);
  check1=header.GetType();
  
  m_forwardUpCb (packet, header.GetSource (), header.GetDestination ());
  if(check1==THZ_PKT_TYPE_DATA && header.GetDestination()!= "ff:ff:ff:ff:ff:ff")
{ // Sending acknowledgement only if the received packet was data and also not a broadcast packet
  SendAck(ackpacket, header.GetSource(), header.GetDestination());
  
}
else
{Id.Cancel();}  // Cancelling the scheduled timeout if the received frame is an acknowledgement for the data frame sent earlier
  return;
}
void
MyPnetMac::Retransmit(Ptr<Packet> packet, Mac48Address toa,Mac48Address froma)

{
 NS_LOG_FUNCTION (this);
  
  PnetMacHeader header = PnetMacHeader (froma, toa, THZ_PKT_TYPE_DATA);
  packet->AddHeader (header);
  m_queue.push(packet); 
  SendFrame();
 NS_LOG_INFO("At time "<<Simulator::Now ()<<" frame is being retransmitted from "<<froma <<" to "<< toa <<std::endl);
  if(header.GetType()==THZ_PKT_TYPE_DATA && m_dataRetryLimit>=count) // Retransmission will be done only 2 times 
{
  // Again scheduling retransmission for the data packet
  timeout=NanoSeconds(12500000+Simulator::Now ());
  std::cout<<" Backoffcounter started again "<<timeout<<std::endl;

  Id = Simulator::Schedule(NanoSeconds(12500000),&MyPnetMac::Retransmit,this,packet, toa,froma);
  
  count++;
}

}


} //namespace ns3
