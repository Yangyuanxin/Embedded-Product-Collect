#ifndef __MENU_H
#define __MENU_H
#include "sys.h"
#include "stdlib.h"

typedef enum// 菜单索引
{
    home,//主页面：HOME
    tdwt,//一级菜单：通道微调
    tdzf,//一级菜单：通道正反
    hksz,//一级菜单：混控设置
    jsgl,//一级菜单：接收管理
    scsz,//一级菜单：输出设置
    tdys,//一级菜单：通道映射
    dljs,//一级菜单：舵量监视
    xtsz,//一级菜单：系统设置
    gybj,//一级菜单：关于本机

    tdwt1,//二级菜单：通道1
    tdwt2,//二级菜单：通道2
    tdwt3,//二级菜单：通道3
    tdwt4,//二级菜单：通道4
    tdwt5,//二级菜单：通道5
    tdwt6,//二级菜单：通道6
    tdwt7,//二级菜单：通道7
    tdwt8,//二级菜单：通道8

    tdzf1,//二级菜单：通道1
    tdzf2,//二级菜单：通道2
    tdzf3,//二级菜单：通道3
    tdzf4,//二级菜单：通道4
    tdzf5,//二级菜单：通道5
    tdzf6,//二级菜单：通道6
    tdzf7,//二级菜单：通道7
    tdzf8,//二级菜单：通道8

    sjyhk,//二级菜单：三角翼混控
    vyhk,//二级菜单：V翼混控
    cshk,//二级菜单：差速混控
    jfyhk,//二级菜单：襟副翼混控

    xjjs,//二级菜单：新建接收
    xzmx,//二级菜单：选择模型
    bcmx,//二级菜单：保存模型
    txmm,//二级菜单：通信密码

    ppmsc,//二级菜单：PPM输出
    wxfs,//二级菜单：无线发射
    fsgl,//二级菜单：发射功率

    tdys1,//二级菜单：通道1
    tdys2,//二级菜单：通道2
    tdys3,//二级菜单：通道3
    tdys4,//二级菜单：通道4
    tdys5,//二级菜单：通道5
    tdys6,//二级菜单：通道6
    tdys7,//二级菜单：通道7
    tdys8,//二级菜单：通道8

    dljs18,//二级菜单：通道1-8舵量监视

    cysz,//二级菜单：常用设置
    dysz,//二级菜单：电压设置
    nzsz,//二级菜单：闹钟设置
    ymsz,//二级菜单：油门设置
    xcjz,//二级菜单：行程校准
    hfcc,//二级菜单：恢复出厂设置

    wtdw,//三级菜单：微调单位
    ajyx,//三级菜单：按键音效
    kjhm,//三级菜单：开机画面

    dyjz,//三级菜单：电压校准
    bjdy,//三级菜单：报警电压
    jsbj,//三级菜单：接收报警

    nzbj,//三级菜单：闹钟报警
    nzsc,//三级菜单：闹钟时长
    kjzj,//三级菜单：开机自检

    ymph,//三级菜单：油门偏好
    ymqx,//三级菜单：油门曲线
    skbh,//三级菜单：失控保护

    xcjzTip,//三级菜单：请将遥杆回中
    xcjz14,//三级菜单：通道1-4显示

    hfccTip,//三级菜单：将恢复默认
    hfcg,//三级菜单：恢复成功

    gybjInf,//三级菜单：J20RC开发团队-V2.0

    MENU_NUM, // 菜单页总数
} MENU_INDEX_LIST;


/*通道微调正反混控设置三角翼差速襟副接收管理新建选择模型保存信密码
输出入无线发射功率映左右遥杆开关舵量监视系统常用报警油门行程校准
恢复厂于单位按键音效机画面串口试电压闹钟时长偏好曲失护
自检请将回中包括断继续默认团队菜本成手车船*/
typedef enum// 菜单索引
{
    tongDao, dao, weiTiao, tiao, zheng, fan, hun, kong, sheZhi, zhi, san, jiaoDu, yi,
    cha, su, jin, fuYi, jie, shouDao, guanLi, li, xinJian, jianShe, xuan, ze, moXing, xingHao,
    baoCun, cun, xinXin, mi, ma, shu, chu, ru, wu, xian, fa, sheMen, gong, lv, ying,
    zuo, youShou, yao, gan, kai, guanMen, duo, liang, jianShi, shiYe, xi, tongYi, changYong,
    yong, baoJing, jing, youMen, men, xingCheng, chengDu, jiaoZhun, zhun, huiFu, fuXing, changJia, yu,
    dan, weiZhi, an, jianPan, yin, xiao, jiQi, hua, mian, chuan, kou, shiTi, dian, ya,
    nao, zhongDian, shiChang, changDu, pian, hao, qu, shiQu, hu, zi, jianCha, qing,
    jiang, huiLai, zhongXin, baoKuo, kuo, duan, jiXu, xu, moRen, ren, tuan, dui, cai, ben,
    chengGong, shouJi, che, chuanBoat, gao, di,

    HZ_NUM,//汉字个数
} HZ_INDEX;
typedef struct
{
    u8 current_index;	//存放当前界面的索引号；
    u8 down_index;		//存放按下“down（向下）”键时需要跳转到的索引号；
    u8 up_index;		//存放按下“up（向上）”键时需要跳转到的索引号；
    u8 enter_index;		//存放按下“enter（进入）”键时需要跳转的索引号；
    u8 esc_index;		//存放按下“esc（退出）”键时需要跳转的索引号；
    u8 home_index;		//存放按下“home（主界面）”键时需要跳转的索引号；
    void (*operate)();	//函数指针变量，存放当前索引号下需要执行的函数的入口地址。
} Key_index_struct;


extern volatile u8 nowMenuIndex;
extern volatile u8 lastMenuIndex;

void OLED_display(void);
void homeWindow(void);//主界面
void showSwState(void);

void menu_tdwt(void);//一级菜单：通道微调
void menu_tdzf(void);//一级菜单：通道正反
void menu_hksz(void);//一级菜单：混控设置
void menu_jsgl(void);//一级菜单：接收管理
void menu_scsz(void);//一级菜单：输出设置
void menu_tdys(void);//一级菜单：通道映射
void menu_dljs(void);//一级菜单：舵量监视
void menu_xtsz(void);//一级菜单：系统设置
void menu_gybj(void);//一级菜单：关于本机

void menu_tdwt1(void);//二级菜单：通道1
void menu_tdwt2(void);//二级菜单：通道2
void menu_tdwt3(void);//二级菜单：通道3
void menu_tdwt4(void);//二级菜单：通道4
void menu_tdwt5(void);//二级菜单：通道5
void menu_tdwt6(void);//二级菜单：通道6
void menu_tdwt7(void);//二级菜单：通道7
void menu_tdwt8(void);//二级菜单：通道8

void menu_tdzf1(void);//二级菜单：通道1
void menu_tdzf2(void);//二级菜单：通道2
void menu_tdzf3(void);//二级菜单：通道3
void menu_tdzf4(void);//二级菜单：通道4
void menu_tdzf5(void);//二级菜单：通道5
void menu_tdzf6(void);//二级菜单：通道6
void menu_tdzf7(void);//二级菜单：通道7
void menu_tdzf8(void);//二级菜单：通道8

void menu_sjyhk(void);//二级菜单：三角翼混控
void menu_vyhk(void);//二级菜单：V翼混控
void menu_cshk(void);//二级菜单：差速混控
void menu_jfyhk(void);//二级菜单：襟副翼混控

void menu_xjjs(void);//二级菜单：新建接收
void menu_xzmx(void);//二级菜单：选择模型
void menu_bcmx(void);//二级菜单：保存模型
void menu_txmm(void);//二级菜单：通信密码

void menu_ppmsc(void);//二级菜单：PPM输出
void menu_wxfs(void);//二级菜单：无线发射
void menu_fsgl(void);//二级菜单：发射功率

void menu_tdys1(void);//二级菜单：通道1
void menu_tdys2(void);//二级菜单：通道2
void menu_tdys3(void);//二级菜单：通道3
void menu_tdys4(void);//二级菜单：通道4
void menu_tdys5(void);//二级菜单：通道5
void menu_tdys6(void);//二级菜单：通道6
void menu_tdys7(void);//二级菜单：通道7
void menu_tdys8(void);//二级菜单：通道8

void menu_dljs18(void);//二级菜单：通道1-8舵量监视

void menu_cysz(void);//二级菜单：常用设置
void menu_dysz(void);//二级菜单：电压设置
void menu_nzsz(void);//二级菜单：闹钟设置
void menu_ymsz(void);//二级菜单：油门设置
void menu_xcjz(void);//二级菜单：行程校准
void menu_hfcc(void);//二级菜单：恢复出厂设置

void menu_wtdw(void);//三级菜单：微调单位
void menu_ajyx(void);//三级菜单：按键音效
void menu_kjhm(void);//三级菜单：开机画面

void menu_dyjz(void);//三级菜单：电压校准
void menu_bjdy(void);//三级菜单：报警电压
void menu_jsbj(void);//三级菜单：接收报警

void menu_nzbj(void);//三级菜单：闹钟报警
void menu_nzsc(void);//三级菜单：闹钟时长
void menu_kjzj(void);//三级菜单：开机自检

void menu_ymph(void);//三级菜单：油门偏好
void menu_ymqx(void);//三级菜单：油门曲线
void menu_skbh(void);//三级菜单：失控保护

void menu_xcjzTip(void);//三级菜单：请将遥杆回中
void menu_xcjz14(void);//三级菜单：通道1-4显示

void menu_hfccTip(void);//三级菜单：将恢复默认
void menu_hfcg(void);//三级菜单：恢复成功

void menu_gybjInf(void);//三级菜单：J20RC开发团队-V2.0
u16 IsInKeyMenu(u16 num);//判断是否为用户编辑页面

#endif

