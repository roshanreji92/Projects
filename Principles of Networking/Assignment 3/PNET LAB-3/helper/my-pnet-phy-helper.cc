
#include "my-pnet-phy-helper.h"
#include "ns3/pnet-phy.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/dca-txop.h"

namespace ns3 {

MyPnetPhyHelper::MyPnetPhyHelper ()
{
  m_phy.SetTypeId ("ns3::MyPnetPhy");
}

MyPnetPhyHelper::~MyPnetPhyHelper ()
{
}

MyPnetPhyHelper
MyPnetPhyHelper::Default (void)
{
  MyPnetPhyHelper helper;
  helper.SetType ("ns3::MyPnetPhy");
  return helper;
}

void
MyPnetPhyHelper::SetType (std::string type,
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
  m_phy.SetTypeId (type);
  m_phy.Set (n0, v0);
  m_phy.Set (n1, v1);
  m_phy.Set (n2, v2);
  m_phy.Set (n3, v3);
  m_phy.Set (n4, v4);
  m_phy.Set (n5, v5);
  m_phy.Set (n6, v6);
  m_phy.Set (n7, v7);
  m_phy.Set (n8, v8);
  m_phy.Set (n9, v9);
  m_phy.Set (n10, v10);
}

void
MyPnetPhyHelper::Set(std::string n1, const AttributeValue &v1)
{
  m_phy.Set (n1, v1);
}

Ptr<PnetPhy>
MyPnetPhyHelper::Create (void) const
{
  Ptr<PnetPhy> phy = m_phy.Create<PnetPhy> ();
  return phy;
}

} //namespace ns3
