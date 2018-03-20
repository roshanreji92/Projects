#ifndef MY_PNET_PHY_HELPER_H
#define MY_PNET_PHY_HELPER_H

//#include <string>
//#include "ns3/attribute.h"
//#include "ns3/object-factory.h"
#include <string>
#include "pnet-helper.h"

namespace ns3 {

class PnetPhy;

class MyPnetPhyHelper  : public PnetPhyHelper
{
public:
  
  MyPnetPhyHelper ();
  
  virtual ~MyPnetPhyHelper ();
  static MyPnetPhyHelper Default (void);

  virtual void SetType (std::string type,
                        std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                        std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                        std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                        std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                        std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                        std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                        std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                        std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue (),
                        std::string n8 = "", const AttributeValue &v8 = EmptyAttributeValue (),
                        std::string n9 = "", const AttributeValue &v9 = EmptyAttributeValue (),
                        std::string n10 = "", const AttributeValue &v10 = EmptyAttributeValue ());

  void Set(std::string n1, const AttributeValue &v1);

  
  virtual Ptr<PnetPhy> Create (void) const;


protected:
  ObjectFactory m_phy;
};

} // namespace ns3

#endif /* MY_PNET_PHY_HELPER_H */
