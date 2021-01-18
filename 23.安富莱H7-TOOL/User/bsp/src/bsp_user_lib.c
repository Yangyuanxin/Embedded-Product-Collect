/*
*********************************************************************************************************
*
*    模块名称 : 字符串操作\数值转换
*    文件名称 : bsp_user_lib.c
*    版    本 : V1.3a
*    说    明 : 提供一些常用的sting、mem操作函数以及Modbus CRC16函数
*   修改记录 :
*               V1.0  2013-12-05  首版
*               V1.1  2014-06-20  增加大小端整数转换函数
*                V1.2  2015-04-06  增加 BEBufToUint32()和 LEBufToUint32()
*                V1.3  2015-10-09  增加 BcdToChar(), HexToAscll()和 AsciiToUint32()
*                V1.3a 2015-10-09  解决 HexToAscll() 函数末尾补0的BUG
*
*********************************************************************************************************
*/

#include "bsp.h"

#include "ff.h"

// CRC 高位字节值表
static const uint8_t s_CRCHi[] = {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
        0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
        0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};
// CRC 低位字节值表
const uint8_t s_CRCLo[] = {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
        0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
        0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
        0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
        0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
        0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
        0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
        0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
        0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
        0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
        0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
        0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
        0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
        0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
        0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
        0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
        0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
        0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
        0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
        0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
        0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
        0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

const uint32_t Crc32Table[256]=
{
        0x00000000,0x04C11DB7,0x09823B6E,0x0D4326D9,0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005,
        0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD,
        0x4C11DB70,0x48D0C6C7,0x4593E01E,0x4152FDA9,0x5F15ADAC,0x5BD4B01B,0x569796C2,0x52568B75,
        0x6A1936C8,0x6ED82B7F,0x639B0DA6,0x675A1011,0x791D4014,0x7DDC5DA3,0x709F7B7A,0x745E66CD,
        0x9823B6E0,0x9CE2AB57,0x91A18D8E,0x95609039,0x8B27C03C,0x8FE6DD8B,0x82A5FB52,0x8664E6E5,
        0xBE2B5B58,0xBAEA46EF,0xB7A96036,0xB3687D81,0xAD2F2D84,0xA9EE3033,0xA4AD16EA,0xA06C0B5D,
        0xD4326D90,0xD0F37027,0xDDB056FE,0xD9714B49,0xC7361B4C,0xC3F706FB,0xCEB42022,0xCA753D95,
        0xF23A8028,0xF6FB9D9F,0xFBB8BB46,0xFF79A6F1,0xE13EF6F4,0xE5FFEB43,0xE8BCCD9A,0xEC7DD02D,
        0x34867077,0x30476DC0,0x3D044B19,0x39C556AE,0x278206AB,0x23431B1C,0x2E003DC5,0x2AC12072,
        0x128E9DCF,0x164F8078,0x1B0CA6A1,0x1FCDBB16,0x018AEB13,0x054BF6A4,0x0808D07D,0x0CC9CDCA,
        0x7897AB07,0x7C56B6B0,0x71159069,0x75D48DDE,0x6B93DDDB,0x6F52C06C,0x6211E6B5,0x66D0FB02,
        0x5E9F46BF,0x5A5E5B08,0x571D7DD1,0x53DC6066,0x4D9B3063,0x495A2DD4,0x44190B0D,0x40D816BA,
        0xACA5C697,0xA864DB20,0xA527FDF9,0xA1E6E04E,0xBFA1B04B,0xBB60ADFC,0xB6238B25,0xB2E29692,
        0x8AAD2B2F,0x8E6C3698,0x832F1041,0x87EE0DF6,0x99A95DF3,0x9D684044,0x902B669D,0x94EA7B2A,
        0xE0B41DE7,0xE4750050,0xE9362689,0xEDF73B3E,0xF3B06B3B,0xF771768C,0xFA325055,0xFEF34DE2,
        0xC6BCF05F,0xC27DEDE8,0xCF3ECB31,0xCBFFD686,0xD5B88683,0xD1799B34,0xDC3ABDED,0xD8FBA05A,
        0x690CE0EE,0x6DCDFD59,0x608EDB80,0x644FC637,0x7A089632,0x7EC98B85,0x738AAD5C,0x774BB0EB,
        0x4F040D56,0x4BC510E1,0x46863638,0x42472B8F,0x5C007B8A,0x58C1663D,0x558240E4,0x51435D53,
        0x251D3B9E,0x21DC2629,0x2C9F00F0,0x285E1D47,0x36194D42,0x32D850F5,0x3F9B762C,0x3B5A6B9B,
        0x0315D626,0x07D4CB91,0x0A97ED48,0x0E56F0FF,0x1011A0FA,0x14D0BD4D,0x19939B94,0x1D528623,
        0xF12F560E,0xF5EE4BB9,0xF8AD6D60,0xFC6C70D7,0xE22B20D2,0xE6EA3D65,0xEBA91BBC,0xEF68060B,
        0xD727BBB6,0xD3E6A601,0xDEA580D8,0xDA649D6F,0xC423CD6A,0xC0E2D0DD,0xCDA1F604,0xC960EBB3,
        0xBD3E8D7E,0xB9FF90C9,0xB4BCB610,0xB07DABA7,0xAE3AFBA2,0xAAFBE615,0xA7B8C0CC,0xA379DD7B,
        0x9B3660C6,0x9FF77D71,0x92B45BA8,0x9675461F,0x8832161A,0x8CF30BAD,0x81B02D74,0x857130C3,
        0x5D8A9099,0x594B8D2E,0x5408ABF7,0x50C9B640,0x4E8EE645,0x4A4FFBF2,0x470CDD2B,0x43CDC09C,
        0x7B827D21,0x7F436096,0x7200464F,0x76C15BF8,0x68860BFD,0x6C47164A,0x61043093,0x65C52D24,
        0x119B4BE9,0x155A565E,0x18197087,0x1CD86D30,0x029F3D35,0x065E2082,0x0B1D065B,0x0FDC1BEC,
        0x3793A651,0x3352BBE6,0x3E119D3F,0x3AD08088,0x2497D08D,0x2056CD3A,0x2D15EBE3,0x29D4F654,
        0xC5A92679,0xC1683BCE,0xCC2B1D17,0xC8EA00A0,0xD6AD50A5,0xD26C4D12,0xDF2F6BCB,0xDBEE767C,
        0xE3A1CBC1,0xE760D676,0xEA23F0AF,0xEEE2ED18,0xF0A5BD1D,0xF464A0AA,0xF9278673,0xFDE69BC4,
        0x89B8FD09,0x8D79E0BE,0x803AC667,0x84FBDBD0,0x9ABC8BD5,0x9E7D9662,0x933EB0BB,0x97FFAD0C,
        0xAFB010B1,0xAB710D06,0xA6322BDF,0xA2F33668,0xBCB4666D,0xB8757BDA,0xB5365D03,0xB1F740B4
};

/*
    Soft_CRC32_byte和STM32_CRC32_byte计算结果一致
*/
uint32_t Soft_CRC32_byte(uint8_t *pData, uint32_t Length)
{        
    uint32_t nReg;//CRC寄存器
    uint32_t nTemp=0;
    uint32_t i, n;

    nReg = 0xFFFFFFFF;
    for (n = 0; n < Length; n++)
    {
        nReg ^= (uint32_t)pData[n];

        for (i = 0; i < 4; i++)
        {
            nTemp = Crc32Table[(uint8_t)(( nReg >> 24 ) & 0xff)]; //取一个字节，查表
            nReg <<= 8;                        //丢掉计算过的头一个BYTE
            nReg ^= nTemp;                //与前一个BYTE的计算结果异或 
        }
    }
    return nReg;
}

/* 硬件方式，按字节计算 */
uint32_t STM32_CRC32_byte(uint8_t *_pBuf, uint32_t _Len)
{
    uint32_t i;
    
    __HAL_RCC_CRC_CLK_ENABLE();
    
    CRC->CR |= CRC_CR_RESET;
    for (i = 0; i < _Len; i++)
    {
        CRC->DR = *_pBuf++;
    }    
    return (CRC->DR);
}

/*
*********************************************************************************************************
*    函 数 名: STM32_CRC32_Word
*    功能说明: 硬件CRC32计算。必须4字节整数倍. 必须在STM32之间使用. 不足4字节，补0
*    形    参: _pBuf : 缓冲区. 32bit对齐
*              _Len : 字节长度
*    返 回 值: 无
*********************************************************************************************************
*/
uint32_t STM32_CRC32_Word(uint32_t *_pBuf, uint32_t _Len)
{
    uint32_t i;
    
    __HAL_RCC_CRC_CLK_ENABLE();
    
    CRC->CR |= CRC_CR_RESET;
    for (i = 0; i < _Len / 4; i++)
    {
        CRC->DR = *_pBuf++;
    }    
    return (CRC->DR);
}

static const uint32_t crc32c_table[256] = {
	0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
	0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
	0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
	0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
	0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
	0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
	0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
	0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
	0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
	0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
	0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
	0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
	0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
	0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
	0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
	0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
	0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
	0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
	0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
	0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
	0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
	0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
	0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
	0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
	0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
	0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
	0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
	0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
	0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
	0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
	0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
	0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
	0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
	0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
	0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
	0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
	0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
	0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
	0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
	0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
	0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
	0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
	0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
	0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
	0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
	0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
	0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
	0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
	0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
	0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
	0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
	0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
	0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
	0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
	0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
	0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
	0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
	0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
	0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
	0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
	0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
	0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
	0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
	0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L
};

/*
*********************************************************************************************************
*    函 数 名: soft_crc32
*    功能说明: 软件计算CRC32, 和PC机主流一致。 脱机编程器使用这个函数。
*    形    参: _pBuf : 缓冲区. 32bit对齐
*              _Len : 字节长度
*    返 回 值: 无
*********************************************************************************************************
*/
uint32_t soft_crc32_c(uint8_t *pStart, uint32_t uSize)
{
	#define INIT  0xffffffffL
	#define XOROT 0xffffffffL
 
	uint32_t uCRCValue;
	uint8_t *pData;
 
	uCRCValue = INIT;
	pData = pStart;
 
	while (uSize--)
	{
		uCRCValue = crc32c_table[(uCRCValue ^ *pData++) & 0xFFL] ^ (uCRCValue >> 8);
	}
 
	return uCRCValue ^ XOROT;
}

uint32_t  CRC_ChkSumCalcTbl_32Bit (uint32_t   init_val, uint32_t  *ptbl, uint8_t *pdata, uint32_t nbr_octets);
uint32_t soft_crc32(uint8_t *pData, uint32_t uSize)
{   
    return CRC_ChkSumCalcTbl_32Bit(0xffffffffL, (uint32_t *)crc32c_table, pData, uSize);
}

/*
*********************************************************************************************************
*    函 数 名: str_len
*    功能说明: 计算字符串长度.0是结束符
*    形    参: _str : 缓冲区
*    返 回 值: 无
*********************************************************************************************************
*/
int str_len(char *_str)
{
    int len = 0;

    while (*_str++)
        len++;
    return len;
}

/*
*********************************************************************************************************
*    函 数 名: str_cpy
*    功能说明: 复制字符串
*    形    参: tar : 目标缓冲区
*             src : 源缓冲区
*    返 回 值: 无
*********************************************************************************************************
*/
void str_cpy(char *_tar, char *_src)
{
    do
    {
        *_tar++ = *_src;
    } while (*_src++);
}

/*
*********************************************************************************************************
*    函 数 名: str_cmp
*    功能说明: 字符串比较
*    形    参: s1 : 字符串1
*              s2 : 字符串2
*    返 回 值: 0 表示相等 非0表示不等
*********************************************************************************************************
*/
int str_cmp(char *s1, char *s2)
{
    while ((*s1 != 0) && (*s2 != 0) && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/*
*********************************************************************************************************
*    函 数 名: str_copy
*    功能说明: 复制字符串
*    形    参: tar : 目标缓冲区
*             src : 源缓冲区
*    返 回 值: 无
*********************************************************************************************************
*/
void mem_set(char *_tar, char _data, int _len)
{
    while (_len--)
    {
        *_tar++ = _data;
    }
}

/*
*********************************************************************************************************
*    函 数 名: int_to_ascii
*    功能说明: 将整数转换为ASCII数组。支持负数。
*    形    参: _Number : 整数
*             _pBuf : 目标缓冲区, 存放转换后的结果。以0结束的字符串。
*             _len : ASCII字符个数, 字符串长度
*    返 回 值: 无
*********************************************************************************************************
*/
void int_to_str(int _iNumber, char *_pBuf, unsigned char _len)
{
    unsigned char i;
    int iTemp;

    if (_iNumber < 0) /* 负数 */
    {
        iTemp = -_iNumber; /* 转为正数 */
    }
    else
    {
        iTemp = _iNumber;
    }

    mem_set(_pBuf, ' ', _len);

    /* 将整数转换为ASCII字符串 */
    for (i = 0; i < _len; i++)
    {
        _pBuf[_len - 1 - i] = (iTemp % 10) + '0';
        iTemp = iTemp / 10;
        if (iTemp == 0)
        {
            break;
        }
    }
    _pBuf[_len] = 0;

    if (_iNumber < 0) /* 负数 */
    {
        for (i = 0; i < _len; i++)
        {
            if ((_pBuf[i] == ' ') && (_pBuf[i + 1] != ' '))
            {
                _pBuf[i] = '-';
                break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: str_to_int
*    功能说明: 将ASCII码字符串转换成整数。 遇到小数点自动越过。
*    形    参: _pStr :待转换的ASCII码串. 可以以逗号，#或0结束。 2014-06-20 修改为非0-9的字符。
*    返 回 值: 二进制整数值
*********************************************************************************************************
*/
int str_to_int(char *_pStr)
{
    unsigned char flag;
    char *p;
    int ulInt;
    unsigned char i;
    unsigned char ucTemp;

    p = _pStr;
    if (*p == '-')
    {
        flag = 1; /* 负数 */
        p++;
    }
    else
    {
        flag = 0;
    }

    ulInt = 0;
    for (i = 0; i < 15; i++)
    {
        ucTemp = *p;
        if (ucTemp == '.') /* 遇到小数点，自动跳过1个字节 */
        {
            p++;
            ucTemp = *p;
        }
        if ((ucTemp >= '0') && (ucTemp <= '9'))
        {
            ulInt = ulInt * 10 + (ucTemp - '0');
            p++;
        }
        else
        {
            break;
        }
    }

    if (flag == 1)
    {
        return -ulInt;
    }
    return ulInt;
}

/*
*********************************************************************************************************
*    函 数 名: BEBufToUint16
*    功能说明: 将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*    形    参: _pBuf : 数组
*    返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*
*********************************************************************************************************
*    函 数 名: LEBufToUint16
*    功能说明: 将2字节数组(小端Little Endian，低字节在前)转换为16位整数
*    形    参: _pBuf : 数组
*    返 回 值: 16位整数值
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}

/*
*********************************************************************************************************
*    函 数 名: BEBufToUint32
*    功能说明: 将4字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*    形    参: _pBuf : 数组
*    返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/*
*********************************************************************************************************
*    函 数 名: LEBufToUint32
*    功能说明: 将4字节数组(小端Little Endian，低字节在前)转换为16位整数
*    形    参: _pBuf : 数组
*    返 回 值: 16位整数值
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}

/*
*********************************************************************************************************
*    函 数 名: CRC16_Modbus
*    功能说明: 计算CRC。 用于Modbus协议。
*    形    参: _pBuf : 参与校验的数据
*              _usLen : 数据长度
*    返 回 值: 16位整数值。 对于Modbus ，此结果高字节先传送，低字节后传送。
*
*   所有可能的CRC值都被预装在两个数组当中，当计算报文内容时可以简单的索引即可；
*   一个数组包含有16位CRC域的所有256个可能的高位字节，另一个数组含有低位字节的值；
*   这种索引访问CRC的方式提供了比对报文缓冲区的每一个新字符都计算新的CRC更快的方法；
*
*  注意：此程序内部执行高/低CRC字节的交换。此函数返回的是已经经过交换的CRC值；也就是说，该函数的返回值可以直接放置
*        于报文用于发送；
*********************************************************************************************************
*/
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen)
{
    uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
    uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
    uint16_t usIndex;                /* CRC循环中的索引 */

    while (_usLen--)
    {
        usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
        ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
        ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}

/*
*********************************************************************************************************
*    函 数 名: CaculTwoPoint
*    功能说明: 根据2点直线方程，计算Y值
*    形    参:  2个点的坐标和x输入量
*    返 回 值: x对应的y值
*********************************************************************************************************
*/
int32_t CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
    if (x2 == x1)
    {
        return 0;
    }
    return y1 + ((int64_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

/*
*********************************************************************************************************
*    函 数 名: CaculTwoPointFloat
*    功能说明: 根据2点直线方程，计算Y值。 浮点
*    形    参:  2个点的坐标和x输入量
*    返 回 值: x对应的y值
*********************************************************************************************************
*/
float CaculTwoPointFloat(float x1, float y1, float x2, float y2, float x)
{
    if (x2 == x1)
    {
        return 0;
    }
    return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1);
}

/*
*********************************************************************************************************
*    函 数 名: BcdToChar
*    功能说明: 将BCD码转为ASCII字符。 比如 0x0A ==> 'A'
*    形    参: _bcd   ：输入的二进制数。必须小于16
*    返 回 值: 转换结果
*********************************************************************************************************
*/
char BcdToChar(uint8_t _bcd)
{
    if (_bcd < 10)
    {
        return _bcd + '0';
    }
    else if (_bcd < 16)
    {
        return _bcd - 10 + 'A';
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: HexToAscll
*    功能说明: 将二进制数组转换为16进制格式的ASCII字符串。每个2个ASCII字符后保留1个空格。
*              0x12 0x34 转化为 0x31 0x32 0x20 0x33 0x34 0x00  即 "1234"
*    形    参:     _pHex   ：输入的数据，二进制数组
*                _pAscii ：存放转换结果, ASCII字符串，0结束。1个二进制对应2个ASCII字符.
*    返 回 值: 转换得到的整数
*********************************************************************************************************
*/
void HexToAscll(uint8_t *_pHex, char *_pAscii, uint16_t _BinBytes)
{
    uint16_t i;

    if (_BinBytes == 0)
    {
        _pAscii[0] = 0;
    }
    else
    {
        for (i = 0; i < _BinBytes; i++)
        {
            _pAscii[3 * i] = BcdToChar(_pHex[i] >> 4);
            _pAscii[3 * i + 1] = BcdToChar(_pHex[i] & 0x0F);
            _pAscii[3 * i + 2] = ' ';
        }
        _pAscii[3 * (i - 1) + 2] = 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: AsciiToHex
*    功能说明: 将ASCII字符串转换为 二进制数组。自动过滤空格。
*              "FF 12 34 F0" 转化为 0xFF,0x12,0x34,0xF0
*    形    参: _pAscii ：存放转换结果, ASCII字符串，0结束。1个二进制对应2个ASCII字符.
*              _pHex   ：输入的数据，二进制数组      
*              _MaxLen : _pHex 数组最大长度
*    返 回 值: 转换数组长度. 
*********************************************************************************************************
*/
uint16_t AsciiToHex(char *_pAscii, uint8_t *_pHex, uint16_t _MaxLen)
{
    uint32_t hexlen;
    char buf[2];
    char idx = 0;
    char ch;
        
    hexlen = 0;
    while (*_pAscii != 0)
    {        
        ch = *_pAscii;
        if (ch == ' ' || ch == '\t')
        {
            _pAscii++;
            continue;
        }
        
        if (((ch >= 'A') && (ch <= 'F')) ||
                ((ch >= 'a') && (ch <= 'f')) ||
                ((ch >= '0') && (ch <= '9')))
        {                
            buf[idx++] = *_pAscii++;
            if (idx == 2)
            { 
                _pHex[hexlen++] = TwoCharToInt(buf);
                
                if (hexlen == _MaxLen)
                {
                    break;
                }
                
                idx = 0;
            }  
        }
        else
        {
            hexlen = 0; /* 格式错误，返回0 */
            break;
        }
    }
    
    return hexlen;
}

/*
*********************************************************************************************************
*    函 数 名: AsciiToUint32
*    功能说明: 变长的 ASCII 字符转换为32位整数  ASCII 字符以空格或者0结束 。 支持16进制和10进制输入
*    形    参: *pAscii ：要转换的ASCII码
*    返 回 值: 转换得到的整数
*********************************************************************************************************
*/
uint32_t AsciiToUint32(char *pAscii)
{
    char i;
    char bTemp;
    char bIsHex;
    char bLen;
    char bZeroLen;
    uint32_t lResult;
    uint32_t lBitValue;

    /* 判断是否是16进制数 */
    bIsHex = 0;
    if ((pAscii[0] == '0') && ((pAscii[1] == 'x') || (pAscii[1] == 'X')))
    {
        bIsHex = 1;
    }

    lResult = 0;
    // 最大数值为 4294967295, 10位+2字符"0x" //
    if (bIsHex == 0)
    { // 十进制 //
        // 求长度 //
        lBitValue = 1;

        /* 前导去0 */
        for (i = 0; i < 8; i++)
        {
            bTemp = pAscii[i];
            if (bTemp != '0')
                break;
        }
        bZeroLen = i;

        for (i = 0; i < 10; i++)
        {
            if ((pAscii[i] < '0') || (pAscii[i] > '9'))
                break;
            lBitValue = lBitValue * 10;
        }
        bLen = i;
        lBitValue = lBitValue / 10;
        if (lBitValue == 0)
            lBitValue = 1;
        for (i = bZeroLen; i < bLen; i++)
        {
            lResult += (pAscii[i] - '0') * lBitValue;
            lBitValue /= 10;
        }
    }
    else
    { /* 16进制 */
        /* 求长度 */
        lBitValue = 1;

        /* 前导去0 */
        for (i = 0; i < 8; i++)
        {
            bTemp = pAscii[i + 2];
            if (bTemp != '0')
                break;
        }
        bZeroLen = i;
        for (; i < 8; i++)
        {
            bTemp = pAscii[i + 2];
            if (((bTemp >= 'A') && (bTemp <= 'F')) ||
                    ((bTemp >= 'a') && (bTemp <= 'f')) ||
                    ((bTemp >= '0') && (bTemp <= '9')))
            {
                lBitValue = lBitValue * 16;
            }
            else
            {
                break;
            }
        }
        lBitValue = lBitValue / 16;
        if (lBitValue == 0)
            lBitValue = 1;
        bLen = i;
        for (i = bZeroLen; i < bLen; i++)
        {
            bTemp = pAscii[i + 2];
            if ((bTemp >= 'A') && (bTemp <= 'F'))
            {
                bTemp -= 0x37;
            }
            else if ((bTemp >= 'a') && (bTemp <= 'f'))
            {
                bTemp -= 0x57;
            }
            else if ((bTemp >= '0') && (bTemp <= '9'))
            {
                bTemp -= '0';
            }
            lResult += bTemp * lBitValue;
            lBitValue /= 16;
        }
    }
    return lResult;
}

/*
*********************************************************************************************************
*    函 数 名: CharToInt
*    功能说明: 1个HEX格式ASCII字符转换为整数值0-F
*    形    参: chr : 字符
*    返 回 值: 整数值
*********************************************************************************************************
*/
uint8_t CharToInt(char _ch)
{
    uint8_t itemp;

    if ((_ch >= '0') && (_ch <= '9'))
    {
        itemp = _ch - '0';
    }
    else if (_ch >= 'a' && _ch <= 'f')
    {
        itemp = _ch - 'a' + 0x0A;
    }
    else if (_ch >= 'A' && _ch <= 'F')
    {
        itemp = _ch - 'A' + 0x0A;
    }
    else
    {
        itemp = 0;
    }

    return itemp;
}

/*
*********************************************************************************************************
*    函 数 名: TwoCharToInt
*    功能说明: 2个HEX格式ASCII字符转换为整数值0-F
*    形    参: chr : 字符
*    返 回 值: 整数值
*********************************************************************************************************
*/
uint8_t TwoCharToInt(char *_ch)
{
    uint8_t temp;

    temp = CharToInt(_ch[0]) * 16;
    temp += CharToInt(_ch[1]);

    return temp;
}

/*
*********************************************************************************************************
*    函 数 名: str_to_int2
*    功能说明: 将ASCII码字符串转换成整数。 遇到非0-9的字符结束。数字前面不能有空格。
*    形    参: _pStr :待转换的ASCII码串. 可以以逗号，#或0结束。 2014-06-20 修改为非0-9的字符。
*    返 回 值: 二进制整数值
*********************************************************************************************************
*/
int str_to_int2(char *_pStr)
{
    unsigned char flag;
    char *p;
    int ulInt;
    unsigned char i;
    unsigned char ucTemp;

    p = _pStr;
    if (*p == '-')
    {
        flag = 1; /* 负数 */
        p++;
    }
    else
    {
        flag = 0;
    }

    ulInt = 0;
    for (i = 0; i < 15; i++)
    {
        ucTemp = *p;

        if ((ucTemp >= '0') && (ucTemp <= '9'))
        {
            ulInt = ulInt * 10 + (ucTemp - '0');
            p++;
        }
        else
        {
            break;
        }
    }

    if (flag == 1)
    {
        return -ulInt;
    }
    return ulInt;
}

/*
*********************************************************************************************************
*    函 数 名: str_to_int3
*    功能说明: 将ASCII码字符串转换成整数 （用于ini文件）。 遇到空格和TAB自动略过，遇到小数点和非0-9字符结束
*    形    参: _pStr :待转换的ASCII码串，前面可以有空格和tab，结束是非0-9字符。
*    返 回 值: 二进制整数值
*********************************************************************************************************
*/
int str_to_int3(char *_pStr)
{
    unsigned char state = 0;
    unsigned char flag = 0;
    char *p;
    int ulInt;
    unsigned char ucTemp;

    p = _pStr;

    ulInt = 0;
   
    while (1)
    {
        ucTemp = *p++;

        if (state == 0)
        {
            if (ucTemp == ' ' || ucTemp == '\t')
            {
                continue;
            }
            else 
            {
                if (ucTemp == '-')
                {
                    flag = 1;       /* 是负数 */
                }
                else
                {
                    ulInt = ulInt * 10 + (ucTemp - '0');
                    state = 1;
                }
            }
        }
        else if (state == 1)
        {
            if ((ucTemp >= '0') && (ucTemp <= '9'))
            {
                ulInt = ulInt * 10 + (ucTemp - '0');
            }
            else
            {
                break;
            }
        }
    }

    if (flag == 1)
    {
        return -ulInt;
    }
    return ulInt;
}

/*
*********************************************************************************************************
*    函 数 名: ip_str_decode
*    功能说明: 带小数点的IP字符串，转换为4字节数组
*    形    参: _ipstr :待转换的ASCII码串
*              _out : 存放结果二进制整数值
*    返 回 值: 1表示OK， 0表示格式错误
*********************************************************************************************************
*/
uint8_t ip_str_decode(char *_ipstr, uint8_t *_out)
{
    char *p;
    int ip1, ip2, ip3, ip4;

    //192.168.1.12
    //192.168.1.20-30

    p = _ipstr;

    ip1 = str_to_int2(p);

    p = strchr(p, '.');
    if (p == 0)
    {
        goto err_ret; // 格式异常，丢弃
    }
    p++;
    ip2 = str_to_int2(p);

    p = strchr(p, '.');
    if (p == 0)
    {
        goto err_ret; // 格式异常，丢弃
    }
    p++;
    ip3 = str_to_int2(p);

    p = strchr(p, '.');
    if (p == 0)
    {
        goto err_ret; // 格式异常，丢弃
    }
    p++;
    ip4 = str_to_int2(p);

    if (ip1 >= 0 && ip1 <= 255 && ip2 >= 0 && ip2 <= 255 &&
            ip3 >= 0 && ip3 <= 255 && ip4 >= 0 && ip4 <= 255)
    {
        _out[0] = ip1;
        _out[1] = ip2;
        _out[2] = ip3;
        _out[3] = ip4;
        return 1;
    }

err_ret:
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: GetHigh16OfFloat
*    功能说明: 得到浮点数的高16位（stm32为小端）
*    形    参: _ff ：浮点数
*    返 回 值: 转换得到浮点数的高16位整数
*********************************************************************************************************
*/
uint16_t GetHigh16OfFloat(float _ff)
{
    uint16_t temp;
    uint8_t *p;

    p = (uint8_t *)&_ff; /* 将浮点数 */
    temp = (p[3] << 8) + p[2];

    return temp;
}

/*
*********************************************************************************************************
*    函 数 名: GetLow16OfFloat
*    功能说明: 得到浮点数的低16位（stm32为小端）
*    形    参: _ff ：浮点数
*    返 回 值: 转换得到浮点数的低16位整数
*********************************************************************************************************
*/
uint16_t GetLow16OfFloat(float _ff)
{
    uint16_t temp;
    uint8_t *p;

    p = (uint8_t *)&_ff;
    temp = (p[1] << 8) + p[0];

    return temp;
}

/*
*********************************************************************************************************
*    函 数 名: Get32BitOfFloat
*    功能说明: 得到浮点数的32位（stm32为小端）
*    形    参: _ff ：浮点数
*    返 回 值: 转换得到浮点数的32位整数
*********************************************************************************************************
*/
uint32_t Get32BitOfFloat(float _ff)
{
    uint32_t temp;
    uint8_t *p;

    p = (uint8_t *)&_ff;
    temp = (p[3] << 24) + (p[2] << 16) + (p[1] << 8) + p[0];

    return temp;
}

/*
*********************************************************************************************************
*    函 数 名: float_isnan
*    功能说明: 判断单精度浮点数是否有效
*    形    参: _ff ：浮点数
*    返 回 值: 0表示有效， 1表示无效
*********************************************************************************************************
*/
uint8_t float_isnan(float _ff)
{
    if (isnan(_ff))
    {
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: BEBufToFloat
*    功能说明: 将4字节数组转换为浮点数
*    形    参: _pBuf : 数组
*    返 回 值: 16位整数值
*********************************************************************************************************
*/
float BEBufToFloat(uint8_t *_pBuf)
{
    float f;
    uint8_t buf[4];

    buf[0] = _pBuf[3];
    buf[1] = _pBuf[2];
    buf[2] = _pBuf[1];
    buf[3] = _pBuf[0];

    f = *(float *)buf;

    return f;
}

/*
*********************************************************************************************************
*    函 数 名: FloatToBEBuf
*    功能说明: 将浮点数转换为4字节数组
*    形    参: f : 浮点数; _pBuf : 数组
*    返 回 值: 16位整数值
*********************************************************************************************************
*/
void FloatToBEBuf(float _f, uint8_t *_pBuf)
{
    float f;
    uint8_t *p;

    f = _f;
    p = (uint8_t *)&f;

    _pBuf[0] = p[3];
    _pBuf[1] = p[2];
    _pBuf[2] = p[1];
    _pBuf[3] = p[0];
}

/*
*********************************************************************************************************
*    函 数 名: strlwr
*    功能说明: 将字符串转换为小写. 需要 #include <ctype.h>
*    形    参: str 字符串
*    返 回 值: 字符串
*********************************************************************************************************
*/
char *strlwr(char *str)
{
    char *orign = str;
    
    for (; *str != 0; str++)
    {
        *str = tolower(*str);
    }
    return orign;
}

/*
*********************************************************************************************************
*    函 数 名: strupr
*    功能说明: 将字符串转换为大写. 需要 #include <ctype.h>
*    形    参: str 字符串
*    返 回 值: 字符串
*********************************************************************************************************
*/
char *strupr(char *str)
{
    char *orign = str;
    
    for (; *str!= 0; str++)
    {
        *str = toupper(*str);
    }
    return orign;
}

/*
*********************************************************************************************************
*    函 数 名: StrUTF8ToGBK
*    功能说明: 将UTF8字符串转换GBK字符串
*    形    参: utf8 输入字符串   
*              gbk  输出字符串
*              gbk_size 字符串size
*    返 回 值: 字符串
*********************************************************************************************************
*/
char *StrUTF8ToGBK(char *utf8, char *gbk, uint16_t gbk_size)
{    
    uint8_t code1, code2;
    char *_ptr;
    char *_pOut;
    uint16_t len = 0;
    
    _ptr = utf8;
    _pOut = gbk;
    
    /* 开始循环处理字符 */
    while (*_ptr != 0)
    {
        code1 = *_ptr; /* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
        if (code1 < 0x80)
        {
            if (len + 1 < gbk_size)
            {
                *_pOut++ = code1;
                
                len++;
            }
        }
        else
        {
            /* 解读 UTF-8 编码非常简单。
                如果一个字节的第一位是0，则这个字节单独就是一个字符；如果第一位是1，则连续有多少个1，就表示当前字符占用多少个字节。
                UNICODE 最后一个二进制位开始，依次从后向前填入格式中的x，多出的位补0
        
                110XXXXX  10XXXXXX           -- 支持
                1110XXXX  10XXXXXX 10XXXXXX  -- 支持
                11110XXX  10XXXXXX 10XXXXXX 10XXXXXX  -- 本转换程序不支持
            */
            {            
                uint8_t code3;
                uint32_t unicode1;
                uint16_t gb;
                
                if ((code1 & 0xE0) == 0xC0)    /* 2字节 */
                {
                    code2 = *++_ptr;
                    if (code2 == 0)
                    {
                        break;
                    }                            
                    unicode1 = ((uint32_t)(code1 & 0x1F) << 6) + (code2 & 0x3F);                            
                }
                else if ((code1 & 0xF0) == 0xE0)    /* 3字节 */
                {
                    code2 = *++_ptr;
                    code3 = *++_ptr;
                    if (code2 == 0 || code3 == 0)
                    {
                        break;
                    }
                    unicode1 = ((uint32_t)(code1 & 0x0F) << 12) + ((uint32_t)(code2 & 0x3F) << 6) + (code3 & 0x3F);
                }
                else if ((code1 & 0xF8) == 0xF0)    /* 4字节 */
                {
                    code2 = *++_ptr;
                    if (code2 == 0)
                    {
                        break;
                    }                            
                }    
                else
                {
                    code2 = *++_ptr;
                    if (code2 == 0)
                    {
                        break;
                    }                            
                }
                
                /* 将UNICODE码转换为GB2312 */
                if (unicode1 > 0xFFFF)
                {
                    break;
                }
                gb = ff_convert(unicode1, 0);    /* Unicode -> OEM */
                
                code1 = gb >> 8;
                code2 = gb;
                
                if (len + 2 < gbk_size)
                {
                    *_pOut++ = code1;
                    *_pOut++ = code2;
                    
                    len += 2;
                }
            }
        }
        
        _ptr++;
    }
    
    *_pOut = 0;
    
    return gbk;
}


/*
*********************************************************************************************************
*    函 数 名: CheckBlankBuf
*    功能说明: 检查一个缓冲区的数值是否空（空值判定有给定的参数_EmptyValue决定）
*    形    参:  _buf 数据缓冲区
*				_len : 数据字节数
*               _EmptyValue : 空值，一般为0xFF 或者0x00
*    返 回 值: 1表示是空，0表示不空
*********************************************************************************************************
*/
uint8_t CheckBlankBuf(const char *_buf, uint32_t _len, uint8_t _EmptyValue)
{
    uint32_t i;
    
    for (i = 0; i < _len; i++)
    {
        if (_buf[i] != _EmptyValue)
        {
            return 0;   /* 不空 */
        }
    }
    
    return 1;   /* 缓冲区为空 */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
