
#include "my-pnet-mac-helper.h"
#include "ns3/pnet-mac.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/dca-txop.h"

namespace ns3 {

MyPnetMacHelper::MyPnetMacHelper ()
{
  
}

MyPnetMacHelper::~MyPnetMacHelper ()
{
}

MyPnetMacHelper
MyPnetMacHelper::Default (void)
{
  MyPnetMacHelper helper;
  
  helper.SetType ("ns3::MyPnetMac");
  return helper;
}
void
MyPnetMacHelper::SetType (std::string type,
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
  m_mac.SetTypeId (type);
  m_mac.Set (n0, v0);
  m_mac.Set (n1, v1);
  m_mac.Set (n2, v2);
  m_mac.Set (n3, v3);
  m_mac.Set (n4, v4);
  m_mac.Set (n5, v5);
  m_mac.Set (n6, v6);
  m_mac.Set (n7, v7);
  m_mac.Set (n8, v8);
  m_mac.Set (n9, v9);
  m_mac.Set (n10, v10);
}

void
MyPnetMacHelper::Set(std::string n1, const AttributeValue &v1)
{
  m_mac.Set (n1, v1);
}

Ptr<PnetMac>
MyPnetMacHelper::Create (void) const
{
  Ptr<PnetMac> mac = m_mac.Create<PnetMac> ();
  return mac;
}

} //namespace ns3
