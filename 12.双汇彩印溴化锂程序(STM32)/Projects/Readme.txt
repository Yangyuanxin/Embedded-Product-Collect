本项目目录结构:

$PROJ_DIR$
$PROJ_DIR$\..\App
$PROJ_DIR$\..\HAL
$PROJ_DIR$\..\HAL\Common
$PROJ_DIR$\..\HAL\include
$PROJ_DIR$\..\HAL\Target
$PROJ_DIR$\..\HAL\Config
$PROJ_DIR$\..\uCOS-II\Ports
$PROJ_DIR$\..\uCOS-II\Source
$PROJ_DIR$\..\uCOS-II\Config
$PROJ_DIR$\..\STM32F10xFWLib\FWLib\library\inc
$PROJ_DIR$\..\STM32F10xFWLib\FWLib\library\src



//HAL Hardware Abstract Layer
HAL\            目录为本项目硬件抽象层驱动
HAL\Common\     目录为系统外设硬件驱动函数
HAL\include\    目录为HAL层的头文件
HAL\Target\     目录为片内外设驱动


$PROJ_DIR$\..\HAL\Config\STM32_Flash.icf	系统连接配置
$PROJ_DIR$\..\HAL\Config\STM32_Flash.mac	系统编译器和调试调用的宏

$PROJ_DIR$\..\HAL\Config\STM32_Flash_XRAM.icf
$PROJ_DIR$\..\HAL\Config\STM32_Flash_XRAM.mac

