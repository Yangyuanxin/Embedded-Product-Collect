copy Objects\output.hex ..\app0.hex

hex2bin ..\app0.hex

LinkBin ..\h7_tool_app.bin =VAR @0x0 ..\app0.bin @0xE0000 ..\dap.bin

del ..\app0.bin
del ..\app0.hex




