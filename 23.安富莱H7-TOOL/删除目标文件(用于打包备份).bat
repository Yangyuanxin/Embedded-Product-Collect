rd /Q /S project\MDK-ARM(uV5)\Objects
rd /Q /S project\MDK-ARM(uV5)\Listings
rd /Q /S project\MDK-ARM(uV5)\DebugConfig
del /Q project\MDK-ARM(uV5)\*.bak
del /Q project\MDK-ARM(uV5)\*.dep
del /Q project\MDK-ARM(uV5)\JLink*
del /Q project\MDK-ARM(uV5)\project.uvguix.*
del /Q project\MDK-ARM(uV5)\*.scvd

rd /Q /S project\MDK5-DAPLink\Objects
rd /Q /S project\MDK5-DAPLink\Listings
rd /Q /S project\MDK5-DAPLink\DebugConfig
del /Q project\MDK5-DAPLink\*.bak
del /Q project\MDK5-DAPLink\*.dep
del /Q project\MDK5-DAPLink\JLink*
del /Q project\MDK5-DAPLink\project.uvgui.*

del /Q project\EWARMv8\Project.dep
del /Q project\EWARMv8\Debug
del /Q project\EWARMv8\Flash
del /Q project\EWARMv8\settings
del /Q project\EWARMv8\Debug
rd  /Q /S project\EWARMv8\Flash
rd /Q /S project\EWARMv8\settings
rd /Q /S project\EWARMv8\Debug
