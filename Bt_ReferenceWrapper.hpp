//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  BT::ReferenceWrapper
//  
//*************************************************************************************************

#ifndef INC__Bt_ReferenceWrapper__hpp
#define INC__Bt_ReferenceWrapper__hpp

namespace Bt {

template<class T> class ReferenceWrapper
{
public:
    typedef T type;

    explicit ReferenceWrapper(T& iT): mT(&iT) {}

    operator T& () const { return *mT; }

    T& get() const { return *mT; }

    T* get_pointer() const { return mT; }

private:

    T* mT;
};

template<class T> inline ReferenceWrapper<T> ref(T& iT)
{
    return ReferenceWrapper<T>(iT);
}

} // namespace Bt

#endif // INC__Bt_ReferenceWrapper__hpp
