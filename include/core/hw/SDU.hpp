/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>
#include <functional>
#include <type_traits>

#include "hal.h"

extern SerialUSBDriver SDU1;

static constexpr char NIBBLE[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

NAMESPACE_CORE_HW_BEGIN

template <std::size_t N>
struct SDUDriverTraits {};

#if HAL_USE_SERIAL_USB
template <>
struct SDUDriverTraits<1> {
    static constexpr auto driver = &SDU1;
};
#endif

class USBDescriptors
{
public:
    using GetDescriptorsCallback = std::function<const USBDescriptor*(USBDriver*, uint8_t, uint8_t, uint16_t)>;

    virtual const USBDescriptor*
    get_descriptor(
        USBDriver* usbp,
        uint8_t    dtype,
        uint8_t    dindex,
        uint16_t   lang
    ) = 0;
};

/*INDENT-ON*/
class SDUDefaultDescriptors:
    public USBDescriptors
{
public:
    static const uint8_t USBD1_DATA_REQUEST_EP      = 1;
    static const uint8_t USBD1_DATA_AVAILABLE_EP    = 1;
    static const uint8_t USBD1_INTERRUPT_REQUEST_EP = 2;

    static const USBDescriptor*
    deviceDescriptor()
    {
        /*
         * USB Device Descriptor.
         */
        static const uint8_t vcom_device_descriptor_data[18] = {
            USB_DESC_DEVICE(0x0110,                 /* bcdUSB (1.1).                    */
                            0x02,                   /* bDeviceClass (CDC).              */
                            0x00,                   /* bDeviceSubClass.                 */
                            0x00,                   /* bDeviceProtocol.                 */
                            0x40,                   /* bMaxPacketSize.                  */
                            0x0483,                 /* idVendor (ST).                   */
                            0x5740,                 /* idProduct.                       */
                            0x0200,                 /* bcdDevice.                       */
                            1,                      /* iManufacturer.                   */
                            2,                      /* iProduct.                        */
                            3,                      /* iSerialNumber.                   */
                            1)                      /* bNumConfigurations.              */
        };

        /*
         * Device Descriptor wrapper.
         */
        static const USBDescriptor vcom_device_descriptor = {
            sizeof vcom_device_descriptor_data,
            vcom_device_descriptor_data
        };

        return &vcom_device_descriptor;
    } // deviceDescriptor

    static const USBDescriptor*
    configurationDescriptor()
    {
        /* Configuration Descriptor tree for a CDC.*/
        static const uint8_t vcom_configuration_descriptor_data[67] = {
            /* Configuration Descriptor.*/
            USB_DESC_CONFIGURATION(67,              /* wTotalLength.                    */
                                   0x02,            /* bNumInterfaces.                  */
                                   0x01,            /* bConfigurationValue.             */
                                   0,               /* iConfiguration.                  */
                                   0xC0,            /* bmAttributes (self powered).     */
                                   50),             /* bMaxPower (100mA).               */
            /* Interface Descriptor.*/
            USB_DESC_INTERFACE(0x00,                /* bInterfaceNumber.                */
                               0x00,                /* bAlternateSetting.               */
                               0x01,                /* bNumEndpoints.                   */
                               0x02,                /* bInterfaceClass (Communications
                                                       Interface Class, CDC section
                                                       4.2).                            */
                               0x02,                /* bInterfaceSubClass (Abstract
                                                     Control Model, CDC section 4.3).   */
                               0x01,                /* bInterfaceProtocol (AT commands,
                                                       CDC section 4.4).                */
                               0),                  /* iInterface.                      */
            /* Header Functional Descriptor (CDC section 5.2.3).*/
            USB_DESC_BYTE(5),                       /* bLength.                         */
            USB_DESC_BYTE(0x24),                    /* bDescriptorType (CS_INTERFACE).  */
            USB_DESC_BYTE(0x00),                    /* bDescriptorSubtype (Header
                                                       Functional Descriptor.           */
            USB_DESC_BCD(0x0110),                   /* bcdCDC.                          */
            /* Call Management Functional Descriptor. */
            USB_DESC_BYTE(5),                       /* bFunctionLength.                 */
            USB_DESC_BYTE(0x24),                    /* bDescriptorType (CS_INTERFACE).  */
            USB_DESC_BYTE(0x01),                    /* bDescriptorSubtype (Call Management
                                                       Functional Descriptor).          */
            USB_DESC_BYTE(0x00),                    /* bmCapabilities (D0+D1).          */
            USB_DESC_BYTE(0x01),                    /* bDataInterface.                  */
            /* ACM Functional Descriptor.*/
            USB_DESC_BYTE(4),                       /* bFunctionLength.                 */
            USB_DESC_BYTE(0x24),                    /* bDescriptorType (CS_INTERFACE).  */
            USB_DESC_BYTE(0x02),                    /* bDescriptorSubtype (Abstract
                                                       Control Management Descriptor).  */
            USB_DESC_BYTE(0x02),                    /* bmCapabilities.                  */
            /* Union Functional Descriptor.*/
            USB_DESC_BYTE(5),                       /* bFunctionLength.                 */
            USB_DESC_BYTE(0x24),                    /* bDescriptorType (CS_INTERFACE).  */
            USB_DESC_BYTE(0x06),                    /* bDescriptorSubtype (Union
                                                       Functional Descriptor).          */
            USB_DESC_BYTE(0x00),                    /* bMasterInterface (Communication
                                                       Class Interface).                */
            USB_DESC_BYTE(0x01),                    /* bSlaveInterface0 (Data Class
                                                       Interface).                      */
            /* Endpoint 2 Descriptor.*/
            USB_DESC_ENDPOINT(USBD1_INTERRUPT_REQUEST_EP | 0x80,
                              0x03,                 /* bmAttributes (Interrupt).        */
                              0x0008,               /* wMaxPacketSize.                  */
                              0xFF),                /* bInterval.                       */
            /* Interface Descriptor.*/
            USB_DESC_INTERFACE(0x01,                /* bInterfaceNumber.                */
                               0x00,                /* bAlternateSetting.               */
                               0x02,                /* bNumEndpoints.                   */
                               0x0A,                /* bInterfaceClass (Data Class
                                                       Interface, CDC section 4.5).     */
                               0x00,                /* bInterfaceSubClass (CDC section
                                                       4.6).                            */
                               0x00,                /* bInterfaceProtocol (CDC section
                                                       4.7).                            */
                               0x00),               /* iInterface.                      */
            /* Endpoint 3 Descriptor.*/
            USB_DESC_ENDPOINT(USBD1_DATA_AVAILABLE_EP,              /* bEndpointAddress.*/
                              0x02,                 /* bmAttributes (Bulk).             */
                              0x0040,               /* wMaxPacketSize.                  */
                              0x00),                /* bInterval.                       */
            /* Endpoint 1 Descriptor.*/
            USB_DESC_ENDPOINT(USBD1_DATA_REQUEST_EP | 0x80,         /* bEndpointAddress.*/
                              0x02,                 /* bmAttributes (Bulk).             */
                              0x0040,               /* wMaxPacketSize.                  */
                              0x00)                 /* bInterval.                       */
        };

        /*
         * Configuration Descriptor wrapper.
         */
        static const USBDescriptor vcom_configuration_descriptor = {
            sizeof vcom_configuration_descriptor_data,
            vcom_configuration_descriptor_data
        };

        return &vcom_configuration_descriptor;
    } // configurationDescriptor

    static const USBDescriptor*
    stringDescriptor(
        int idx
    )
    {
        /*
         * U.S. English language identifier.
         */
        static const uint8_t vcom_string0[] = {
            USB_DESC_BYTE(4),                       /* bLength.                         */
            USB_DESC_BYTE(USB_DESCRIPTOR_STRING),   /* bDescriptorType.                 */
            USB_DESC_WORD(0x0409)                   /* wLANGID (U.S. English).          */
        };

        /*
         * Vendor string.
         */
        static const uint8_t vcom_string1[] = {
            USB_DESC_BYTE(38),/* bLength.                         */
            USB_DESC_BYTE(USB_DESCRIPTOR_STRING),/* bDescriptorType.                 */
            'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
            'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
            'c', 0, 's', 0
        };

        /*
         * Device Description string.
         */
        static const uint8_t vcom_string2[] = {
            USB_DESC_BYTE(56),/* bLength.                         */
            USB_DESC_BYTE(USB_DESCRIPTOR_STRING),/* bDescriptorType.                 */
            'C', 0, 'h', 0, 'i', 0, 'b', 0, 'i', 0, 'O', 0, 'S', 0, '/', 0,
            'R', 0, 'T', 0, ' ', 0, 'V', 0, 'i', 0, 'r', 0, 't', 0, 'u', 0,
            'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0, 'M', 0, ' ', 0, 'P', 0,
            'o', 0, 'r', 0, 't', 0
        };

        /*
         * Serial Number string.
         */
        static const uint8_t vcom_string3[(2 + 12 * 4)] = {
            USB_DESC_BYTE((2 + 12 * 4)),                       /* bLength.                         */
            USB_DESC_BYTE(USB_DESCRIPTOR_STRING),              /* bDescriptorType.                 */
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AC)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AC)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AD)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AD)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AE)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AE)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AF)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7AF)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B0)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B0)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B1)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B1)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B2)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B2)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B3)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B3)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B4)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B4)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B5)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B5)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B6)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B6)) & 0x0F]), 0,
            (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B7)) >> 4]), 0, (uint8_t)(NIBBLE[(*(uint8_t*)(0x1FFFF7B7)) & 0x0F]), 0,
        };

        /*
         * Strings wrappers array.
         */
        static const USBDescriptor vcom_strings[] = {
            {sizeof vcom_string0, vcom_string0},
            {sizeof vcom_string1, vcom_string1},
            {sizeof vcom_string2, vcom_string2},
            {sizeof vcom_string3, vcom_string3}
        };

        return &vcom_strings[idx];
    } // stringDescriptor

    /*
     * Handles the GET_DESCRIPTOR callback. All required descriptors must be
     * handled here.
     */
    const USBDescriptor*
    get_descriptor(
        USBDriver* usbp,
        uint8_t    dtype,
        uint8_t    dindex,
        uint16_t   lang
    )
    {
        (void)usbp;
        (void)lang;

        switch (dtype) {
          case USB_DESCRIPTOR_DEVICE:
              return deviceDescriptor();

          case USB_DESCRIPTOR_CONFIGURATION:
              return configurationDescriptor();

          case USB_DESCRIPTOR_STRING:

              if (dindex < 4) {
                  return stringDescriptor(dindex);
              }
        }
        return nullptr;
    } // get_descriptor

    GetDescriptorsCallback
    callback()
    {
        return std::bind(&SDUDefaultDescriptors::get_descriptor, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    }

    static GetDescriptorsCallback
    static_callback()
    {
        static SDUDefaultDescriptors _descriptor;
        static auto _callback = std::bind(&SDUDefaultDescriptors::get_descriptor, &_descriptor, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        return _callback;
    }
};


class SDU
{
public:
    static const uint8_t USBD1_DATA_REQUEST_EP      = 1;
    static const uint8_t USBD1_DATA_AVAILABLE_EP    = 1;
    static const uint8_t USBD1_INTERRUPT_REQUEST_EP = 2;

    static void
    setDescriptors(
        USBDescriptors::GetDescriptorsCallback descriptors_callback
    )
    {
        get_descriptor_impl = descriptors_callback;
    }

    static void
    init()
    {
        sduObjectInit(&SDU1);
    }

    static void
    start()
    {
        sduStart(&SDU1, serusbcfg());
    }

    const USBConfig*
    usbcfg()
    {
        return &_usbcfg;
    }

    static const SerialUSBConfig*
    serusbcfg()
    {
        /*
         * Serial over USB driver configuration.
         */
        static const SerialUSBConfig _serusbcfg = {
            &USBD1,
            USBD1_DATA_REQUEST_EP,
            USBD1_DATA_AVAILABLE_EP,
            USBD1_INTERRUPT_REQUEST_EP
        };

        return &_serusbcfg;
    }

private:
    /*
     * Handles the USB driver global events.
     */
    static void
    usb_event(
        USBDriver* usbp,
        usbevent_t event
    )
    {
        /**
         * @brief   IN EP1 state.
         */
        static USBInEndpointState ep1instate;

        /**
         * @brief   OUT EP1 state.
         */
        static USBOutEndpointState ep1outstate;

        /**
         * @brief   EP1 initialization structure (both IN and OUT).
         */
        static constexpr USBEndpointConfig ep1config = {
            USB_EP_MODE_TYPE_BULK,
            NULL,
            sduDataTransmitted,
            sduDataReceived,
            0x0040,
            0x0040,
            &ep1instate,
            &ep1outstate,
            2,
            NULL
        };

        /**
         * @brief   IN EP2 state.
         */
        static USBInEndpointState ep2instate;

        /**
         * @brief   EP2 initialization structure (IN only).
         */
        static constexpr USBEndpointConfig ep2config = {
            USB_EP_MODE_TYPE_INTR,
            NULL,
            sduInterruptTransmitted,
            NULL,
            0x0010,
            0x0000,
            &ep2instate,
            NULL,
            1,
            NULL
        };


        switch (event) {
          case USB_EVENT_RESET:
              return;

          case USB_EVENT_ADDRESS:
              return;

          case USB_EVENT_CONFIGURED:
              chSysLockFromISR();

              /* Enables the endpoints specified into the configuration.
                 Note, this callback is invoked from an ISR so I-Class functions
                 must be used.*/
              usbInitEndpointI(usbp, USBD1_DATA_REQUEST_EP, &ep1config);
              usbInitEndpointI(usbp, USBD1_INTERRUPT_REQUEST_EP, &ep2config);

              /* Resetting the state of the CDC subsystem.*/
              sduConfigureHookI(&SDU1);

              chSysUnlockFromISR();
              return;

          case USB_EVENT_UNCONFIGURED:
              return;

          case USB_EVENT_SUSPEND:
              chSysLockFromISR();

              /* Disconnection event on suspend.*/
              sduDisconnectI(&SDU1);

              chSysUnlockFromISR();
              return;

          case USB_EVENT_WAKEUP:
              return;

          case USB_EVENT_STALLED:
              return;
        } // switch
        return;
    } // usb_event

    /*
     * Handles the USB driver global events.
     */
    static void
    sof_handler(
        USBDriver* usbp
    )
    {
        (void)usbp;

        osalSysLockFromISR();
        sduSOFHookI(&SDU1);
        osalSysUnlockFromISR();
    }

    static const USBDescriptor*
    get_descriptor(
        USBDriver* usbp,
        uint8_t    dtype,
        uint8_t    dindex,
        uint16_t   lang
    )
    {
        return get_descriptor_impl(usbp, dtype, dindex, lang);
    }

    /*
     * USB driver configuration.
     */
    USBConfig _usbcfg = {
        usb_event,
        get_descriptor,
        sduRequestsHook,
        sof_handler
    };

private:
    static USBDescriptors::GetDescriptorsCallback get_descriptor_impl;
};

USBDescriptors::GetDescriptorsCallback SDU::get_descriptor_impl;

// --- Aliases -----------------------------------------------------------------

using SDU_1 = SDUDriverTraits<1>;

NAMESPACE_CORE_HW_END
