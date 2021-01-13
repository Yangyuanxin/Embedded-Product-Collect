![J20RC](./images/logo.png)<br>
# J20航模遥控器
[![Latest version](https://img.shields.io/github/v/release/J20RC/STM32_RC_Transmitter)](https://github.com/J20RC/STM32_RC_Transmitter/releases)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)<br>
强烈推荐Star(￣▽￣)／
我们开源团队会持续优化整个项目喔~<br>
项目使用STM32标准库/HAL库来开发一套通用的航模遥控器和接收机，软硬件完全开源，还附带教程哦~
- STM32F0接收机<br>
Github：https://github.com/J20RC/STM32F0_Receiver<br>
码云(Gitee)：https://gitee.com/J20RC/STM32F0_Receiver<br>
- STM32F1接收机<br>
Github：https://github.com/J20RC/STM32F1_Receiver<br>
码云(Gitee)：https://gitee.com/J20RC/STM32F1_Receiver<br>
## 拥抱开源，我们期待你的加入！
 - QQ群：1091996634，密码：J20

## 材料清单 
| 器件名称        			| 数量   	| 上传图片名称  			| 备注 |
| ------------- 			| :------:	| :-------------:			|------|
| STM32F103C8T6最小系统板 	| 2 		| images/stm32f103c8t6.png 	|
| USB转TTL模块			 	| 1 		| images/usbttl(1/2).png 	|
| ST-LINK V2仿真器下载器 	| 1 		| images/stlink.png 		|调试STM32性价比极高|
| OLED屏幕（4管脚）		 	| 1 		| images/OLED.png	 		|
| NRF24L01模块			 	| 2 		| images/nrf24l01(1/2/3).png|
| 航模遥控器所用的遥杆	 	| 2 		| images/yg(ykq).png		|该遥控器15RMB，使用上面的遥杆即可|
| 两段/三段钮子开关		 	| >=2 		| images/nzkg.png		 	|
| 50k电位器				 	| >=2 		| images/dwq.png			|
| 100uF电解电容			 	| 2 		| images/100uf.png	 		|
| 0.1uF独石电容			 	| 2 		| images/104.png	 		|
| 金属膜电阻（1k、10k、20k）| 3 		| images/(1k/10k/20k).png	|
| 轻触开关（键帽）			| 8 		| images/qckg(m).png	 	|
| SS8050三极管			 	| 2 		| images/ss8050.png	 		|
| 洞洞板（7*9cm）			| 2 		| images/ddb.png	 		|
| XH2.54mm插针插座			| n 		| images/cz(1).png	 		|
| L7805CDT稳压芯片			| 1 		| images/l7805.png	 		|
| AMS1117-3.3稳压芯片		| 2 		| images/ams1117-3.3.png	|
| 旋转编码器EC11			| 1 		| images/bmq.png	 		|
| 肖特基二极管1N5819		| 1 		| images/1n5819.png	 		|
| 三脚拨动开关 				| 1 		| images/3jkg.png	 		|
| 3.7V锂电池			 	| 1 		| images/bat.png	 		|
| 锂电池充电模块			| 1 		| images/cd.png		 		|


## 相关教程
- 安装Keil5(MDK) ：https://blog.csdn.net/weixin_42911200/article/details/81590158
- 注意要安装Keil.STM32F1xx_DFP.2.3.0.pack支持包，因为我们要用STM32F103系列芯片的库函数编写。
- 新建keil库函数工程，参考链接 https://www.cnblogs.com/zeng-1995/p/11308622.html
- 多按键检测(基于有限状态机)：https://blog.csdn.net/weixin_42268054/article/details/107393669
- 多级菜单(树形拓扑结构、状态机)：https://blog.csdn.net/weixin_42268054/article/details/107613192
- 制作教程：https://www.cnblogs.com/cai-zi/p/13438285.html	<br>
- 使用说明：https://www.cnblogs.com/cai-zi/p/13438406.html	<br>
- 开发说明：https://www.cnblogs.com/cai-zi/p/13536862.html	<br>
- PCB打印：https://www.cnblogs.com/cai-zi/p/13536693.html		<br>

## 版本说明
- [基础版1.0发布（2020.08.01）](https://github.com/J20RC/STM32_RC_Transmitter/releases/tag/1.0)	<br>
实现了8通道遥控和接收，遥控器菜单包含通道微调、通道正反、行程校准功能；	<br>
Bilibili视频：https://www.bilibili.com/video/BV1ea4y1J7WV	<br/>
- [基础版1.1发布（2020.09.25）](https://github.com/J20RC/STM32_RC_Transmitter/releases/tag/1.1)	<br>
遥控器端系统设置增加左右手油门设置、报警电压、校准电压、微调单位等；	<br>
遥控器的mini板PCB修复电压供电波动问题，使用AMS1117-3.3芯片替代原来的H7233芯片；	<br>
- [基础版2.0发布（2020.10.25）](https://github.com/J20RC/STM32_RC_Transmitter/releases/tag/2.1)<br>
全新的菜单界面，更丰富的设置功能。<br>
Bilibili视频：https://www.bilibili.com/video/BV1wy4y1k718

## 非常感谢项目中所有的贡献者：
 * 蔡子CaiZi（B站up主）@Cai-Zi  https://cai-zi.github.io/
 * A-XIN;
 * 小何;
 * ZL_Studio（B站up主）;
 * 王天天@WangTiantian139  https://wangtiantian139.github.io/
 * 刘桶桶;
 * 简繁;
 * 王博;
 * 与非门;
 * Talons;
 * 大米粥（B站up主）;
 * P a S S ||;
 * 佚名;
 * 小吴;
 * xuech007;


