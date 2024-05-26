#include <stdint.h>
#include <stddef.h>

#ifndef COMMON_H
#define COMMON_H

#ifdef Arduino_h
#include "Print.h"
#else
class Print {
public:
    size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));

    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(long long, int = DEC);
    size_t print(unsigned long long, int = DEC);
    size_t print(double, int = 2);
    size_t print(struct tm * timeinfo, const char * format = NULL);

    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(long long, int = DEC);
    size_t println(unsigned long long, int = DEC);
    size_t println(double, int = 2);
    size_t println(struct tm * timeinfo, const char * format = NULL);
    size_t println(void);
};
#endif

typedef int (*MeshSend_t)(const uint8_t *mac_addr, const uint8_t *data, size_t data_len);
typedef int (*MeshAddPeer_t)(const uint8_t *mac_addr, uint8_t channel);
typedef uint64_t (*TimerMonotonic_t)();
typedef void (*Callback)();

class BaseDevice {

protected:
    MeshSend_t meshSendFunc = nullptr;
    MeshAddPeer_t meshAddPeerFunc = nullptr;
    Print* printer;
    TimerMonotonic_t timerGetMono = nullptr;
    Callback rebootFunc = nullptr;

    void SprintMacAddress(const uint8_t mac_addr[6], char* macStr, size_t macStrSize) {
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);
    }

public:
    BaseDevice(Print* printer, Callback rebootFunc) : printer(printer), rebootFunc(rebootFunc) {};
    virtual void PreSetup() {};
    virtual void Setup() {};
    virtual void Loop() {};

    // Callbacks from mesh to Base Device
    virtual void MeshOnSend(const uint8_t *mac_addr, uint8_t status){};
    virtual void MeshOnReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len){};

    // Registering callbacks
    virtual void MeshRegisterSend(MeshSend_t func) {
        this->meshSendFunc = func;
    }
    virtual void MeshRegisterAddPeer(MeshAddPeer_t func)  {
        this->meshAddPeerFunc = func;
    }

    virtual void TimerRegisterMono(TimerMonotonic_t func) {
        this->timerGetMono = func;
    }
};

#endif