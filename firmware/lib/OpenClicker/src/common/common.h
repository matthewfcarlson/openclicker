#include <stdint.h>
#include <stddef.h>
// Before importing functional, you have to clear out the unity macros
#undef isnan
#undef isinf
#include <functional>
#include "print.hpp"

#ifndef COMMON_H
#define COMMON_H

typedef int mesh_err_t;

/* Definitions for error constants. */
#define MESH_OK          0       /*!< esp_err_t value indicating success (no error) */
#define MESH_FAIL        -1      /*!< Generic esp_err_t code indicating failure */

#define MESH_ERR_NO_MEM              0x101   /*!< Out of memory */
#define MESH_ERR_INVALID_ARG         0x102   /*!< Invalid argument */
#define MESH_ERR_INVALID_STATE       0x103   /*!< Invalid state */
#define MESH_ERR_INVALID_SIZE        0x104   /*!< Invalid size */
#define MESH_ERR_NOT_FOUND           0x105   /*!< Requested resource not found */
#define MESH_ERR_NOT_SUPPORTED       0x106   /*!< Operation or feature not supported */
#define MESH_ERR_TIMEOUT             0x107   /*!< Operation timed out */
#define MESH_ERR_INVALID_RESPONSE    0x108   /*!< Received response was invalid */
#define MESH_ERR_INVALID_CRC         0x109   /*!< CRC or checksum was invalid */
#define MESH_ERR_INVALID_VERSION     0x10A   /*!< Version was invalid */
#define MESH_ERR_INVALID_MAC         0x10B   /*!< MAC address was invalid */
#define MESH_ERR_NOT_FINISHED        0x10C   /*!< There are items remained to retrieve */


#define MESH_ERR_WIFI_BASE           0x3000  /*!< Starting number of WiFi error codes */
#define MESH_ERR_MESH_BASE           0x4000  /*!< Starting number of MESH error codes */
#define MESH_ERR_FLASH_BASE          0x6000  /*!< Starting number of flash error codes */
#define MESH_ERR_HW_CRYPTO_BASE      0xc000  /*!< Starting number of HW cryptography module error codes */
#define MESH_ERR_MEMPROT_BASE        0xd000  /*!< Starting number of Memory Protection API error codes */
#define MESH_ERR_TEST_BASE           0xe000  /*!< Starting number of Memory Protection API error codes */

#define MESH_ERR_ESPNOW_BASE         (MESH_ERR_WIFI_BASE + 100) /*!< ESPNOW error number base. */
#define MESH_ERR_ESPNOW_NOT_INIT     (MESH_ERR_ESPNOW_BASE + 1) /*!< ESPNOW is not initialized. */
#define MESH_ERR_ESPNOW_ARG          (MESH_ERR_ESPNOW_BASE + 2) /*!< Invalid argument */
#define MESH_ERR_ESPNOW_NO_MEM       (MESH_ERR_ESPNOW_BASE + 3) /*!< Out of memory */
#define MESH_ERR_ESPNOW_FULL         (MESH_ERR_ESPNOW_BASE + 4) /*!< ESPNOW peer list is full */
#define MESH_ERR_ESPNOW_NOT_FOUND    (MESH_ERR_ESPNOW_BASE + 5) /*!< ESPNOW peer is not found */
#define MESH_ERR_ESPNOW_INTERNAL     (MESH_ERR_ESPNOW_BASE + 6) /*!< Internal error */
#define MESH_ERR_ESPNOW_EXIST        (MESH_ERR_ESPNOW_BASE + 7) /*!< ESPNOW peer has existed */
#define MESH_ERR_ESPNOW_IF           (MESH_ERR_ESPNOW_BASE + 8) /*!< Interface error */

#define MESH_MAX_DATA_LEN         250       /*!< Maximum length of ESPNOW data which is sent very time */

const uint8_t PRESENTER_MAC[6] = {0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF};
const char*   PRESENTER_MAC_STR = "af:af:af:af:af:af";

typedef std::function<mesh_err_t(const uint8_t* mac_addr, uint8_t channel)> MeshAddPeer_t;
typedef void (*Callback)();
typedef std::function<mesh_err_t(const uint8_t*, const uint8_t*, uint32_t)> MeshSend_t;
typedef std::function<void(const uint8_t*, const uint8_t*, uint32_t)> MeshReceive_t;
typedef std::function<void(const uint8_t*, const uint8_t*, const uint8_t*, uint32_t)> MeshMessageReceive_t;

class BaseDevice {

private:
    MeshSend_t meshSendFunc = nullptr;
    MeshAddPeer_t meshAddPeerFunc = nullptr;
    Callback rebootFunc = nullptr;

protected:
    uint8_t macAddress[6] = {0};
    Print* printer;
    void SprintMacAddress(const uint8_t mac_addr[6], char* macStr, uint32_t macStrSize) {
        snprintf(macStr, macStrSize, "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);
    }

    void Reboot() {
        if (this->rebootFunc == nullptr) {
            this->printer->printf("WARNING: rebootFunc is null\n");
        }
        this->rebootFunc();
    }

    int MeshSend(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) {
        if (this->meshSendFunc == nullptr) {
            this->printer->printf("WARNING: meshSendFunc is null\n");
            Reboot();
            return MESH_ERR_INVALID_STATE;
        }
        return this->meshSendFunc(mac_addr, data, data_len);
    }

    int MeshAddPeer(const uint8_t* mac_addr, uint8_t channel) {
        if (this->meshAddPeerFunc == nullptr) {
            this->printer->printf("WARNING: meshAddPeerFunc is null\n");
            Reboot();
            return MESH_ERR_INVALID_STATE;
        }
        return this->meshAddPeerFunc(mac_addr, channel);
    }

public:
    BaseDevice(Print* printer, Callback rebootFunc, const uint8_t* mac = nullptr) : printer(printer), rebootFunc(rebootFunc) {
        if (mac != nullptr) this->AssignMacAddress(mac);
    };
    ~BaseDevice(){};
    virtual void PreSetup() {};
    virtual void Setup() {};
    virtual void Loop() {};

    // Callbacks from mesh to Base Device
    virtual void MeshOnSend(const uint8_t* mac_addr, uint8_t status){};
    virtual void MeshOnReceive(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_size){};

    // Registering callbacks
    virtual void MeshRegisterSend(MeshSend_t func) {
        this->meshSendFunc = func;
    }
    virtual void MeshRegisterAddPeer(MeshAddPeer_t func)  {
        this->meshAddPeerFunc = func;
    }

    virtual MeshReceive_t CreateReceiveCallback() = 0;

    virtual void ButtonPressed(uint8_t index) {}
    virtual void ButtonReleased(uint8_t index) {}

    virtual void AssignMacAddress(const uint8_t* mac_address) {
        memcpy(this->macAddress, mac_address, sizeof(this->macAddress));
    }

    virtual void CopyMacAddress(uint8_t* mac_address) {
        memcpy(mac_address, this->macAddress, sizeof(this->macAddress));
    }

};

#endif