return Device {
    strManufacturer = "H7TOOL",
    strProduct = "H7_TOOL_DAP",
    strSerial = "TUSB123456",
    idVendor = 0x0d28,
    idProduct = 0x0204,
    prefix = "COMP",
    Config {
        USB_HID{
            ReadEp = EndPoint(IN(1),  Interrupt, 64),
            WriteEp = EndPoint(OUT(1), Interrupt, 64),
            report = HID_InOut(64),
        },
        Interface{
            WCID=WinUSB,
            strInterface = "CMSIS-DAP v2",
            GUID="{CDB3B5AD-293B-4663-AA36-1AAE46463776}",
            EndPoint(IN(2),  BulkDouble, 32),
            EndPoint(OUT(2), BulkDouble, 32),
        },
        CDC_ACM{
            EndPoint(IN(9),  Interrupt, 16),
            EndPoint(IN(4), BulkDouble, 32),
            EndPoint(OUT(4),  BulkDouble, 32),
        },
        CDC_ACM{
            EndPoint(IN(10),  Interrupt, 16),
            EndPoint(IN(5), BulkDouble, 32),
            EndPoint(OUT(5),  BulkDouble, 32),
        },
        CDC_ACM{
            EndPoint(IN(11),  Interrupt, 16),
            EndPoint(IN(6), BulkDouble, 32),
            EndPoint(OUT(6),  BulkDouble, 32),
        },
        CDC_ACM{
            EndPoint(IN(12),  Interrupt, 16),
            EndPoint(IN(7), BulkDouble, 32),
            EndPoint(OUT(7),  BulkDouble, 32),
        },
        CDC_ACM{
            EndPoint(IN(13),  Interrupt, 16),
            EndPoint(IN(8), BulkDouble, 32),
            EndPoint(OUT(8),  BulkDouble, 32),
        },
   }
}
