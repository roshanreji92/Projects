#include "pnet-channel-helper.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/dca-txop.h"

namespace ns3 {

PnetChannelHelper::PnetChannelHelper ()
{
  m_channel.SetTypeId ("ns3::PnetChannel");
}

PnetChannelHelper::~PnetChannelHelper ()
{
}

void
PnetChannelHelper::SetType (std::string type,
                        std::string n0, const AttributeValue &v0,
                        std::string n1, const AttributeValue &v1,
                        std::string n2, const AttributeValue &v2,
                        std::string n3, const AttributeValue &v3,
                        std::string n4, const AttributeValue &v4,
                        std::string n5, const AttributeValue &v5,
                        std::string n6, const AttributeValue &v6,
                        std::string n7, const AttributeValue &v7,
                        std::string n8, const AttributeValue &v8,
                        std::string n9, const AttributeValue &v9,
                        std::string n10, const AttributeValue &v10)
{
  m_channel.SetTypeId (type);
  m_channel.Set (n0, v0);
  m_channel.Set (n1, v1);
  m_channel.Set (n2, v2);
  m_channel.Set (n3, v3);
  m_channel.Set (n4, v4);
  m_channel.Set (n5, v5);
  m_channel.Set (n6, v6);
  m_channel.Set (n7, v7);
  m_channel.Set (n8, v8);
  m_channel.Set (n9, v9);
  m_channel.Set (n10, v10);
}

void
PnetChannelHelper::Set(std::string n1, const AttributeValue &v1)
{
  m_channel.Set (n1, v1);
}

Ptr<PnetChannel>
PnetChannelHelper::Create (void) const
{
  Ptr<PnetChannel> channel = m_channel.Create<PnetChannel> ();
  return channel;
}

} //namespace ns3
