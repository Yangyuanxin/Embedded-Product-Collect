/**
 * @file    SWD_flash.c
 * @brief   Program target flash through SWD
 */
#include "swd_host.h"
#include "SWD_flash.h"
#include "prog_if.h"
#include "elf_file.h"
#include "SW_DP_Multi.h"
#include "swd_host_multi.h"
#include "string.h"

extern const program_target_t flash_algo;

/* check_blank 算法 */
static const uint32_t flash_code_check_blank[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2, 
    0x47702000, 0x47702000, 0x0613B510, 0x191B0414, 0x191B0214, 0x1CC9189A, 0x00890889, 0x6803E006, 
    0xD0014293, 0xBD102001, 0x1F091D00, 0xD1F62900, 0xBD102000, 0x00000000};
static const program_target_temp_t flash_algo_check_blank = {
    0x00000001,
    0x00000005,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000009,
    0x00000000,
    0x00000000,
    {0x00000001,0x00000058,2048},
    0x00000058,
    0x00000400,
    0x00000000,
    0x00000058,
};

/* STM32F0_CRC32 算法 */
static const uint32_t flash_code_STM32_CRC32_F0[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2, 
    0x47702000, 0x47702000, 0x4A08B510, 0x24406953, 0x61534323, 0x23014A06, 0xE0026093, 0x6013C808, 
    0x29001F09, 0x6810DCFA, 0x0000BD10, 0x40021000, 0x40023000, 0x00000000};
static const program_target_temp_t flash_algo_STM32_CRC32_F0 = {
    0x00000001,
    0x00000005,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000009,
    0x00000000,
    {0x00000001,0x00000058,2048},
    0x00000058,
    0x00000400,
    0x00000000,
    0x00000058,
};

/* STM32G4XX  CRC32 */
static const uint32_t flash_code_STM32_CRC32_G4[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2, 
    0x6C934A07, 0x5380F443, 0x4A066493, 0x60932301, 0xC808E002, 0x60131F09, 0xDCFA2900, 0x47706810, 
    0x40021000, 0x40023000, 0x00000000};
static const program_target_temp_t flash_algo_STM32_CRC32_G4 = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000001,
    0x00000000,
    {0x00000001,0x0000005C,0x00001000},
    0x0000004C,
    0x00000010,
    0x00000000,
    0x0000004C,
};

/* M3 软件_CRC32 算法 (M0无法执行，比MO效率高一点点，930ms 和 902ms的差异) */
#if 0
static const uint32_t flash_code_SOFT_CRC32_M3[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2, 
    0x47702000, 0x47702000, 0x4903460B, 0x44794602, 0x30FFF04F, 0xB89EF000, 0x00000276, 0xF240B4F0, 
    0x2B0017FE, 0xF002D045, 0x2E000603, 0xF812D031, 0xEA844B01, 0xEA072410, 0x5B0D0444, 0x2000EA85, 
    0x0301F1A3, 0x0601F106, 0xD0322B00, 0xD1EE2E04, 0xF852E01F, 0xEA866B04, 0xEA072410, 0x5B0D0444, 
    0x2000EA85, 0x0400EA86, 0x14D4EA07, 0xEA855B0D, 0xEA862000, 0xEA072400, 0x5B0D34D4, 0x2000EA85, 
    0x4400EA86, 0x54D4EA07, 0xEA855B0D, 0xF1A32000, 0x2B040304, 0xD30AD2DD, 0x4B01F812, 0x2410EA84, 
    0x0444EA07, 0xEA855B0D, 0xF1A32000, 0x2B000301, 0xB280D1F2, 0x4770BCF0, 0xF240B4F0, 0x2B0017FE, 
    0xF002D045, 0x2E000603, 0xF812D031, 0xEA844B01, 0xEA070400, 0x5B0D0444, 0x2010EA85, 0x0301F1A3, 
    0x0601F106, 0xD0322B00, 0xD1EE2E04, 0xF852E01F, 0xEA866B04, 0xEA070400, 0x5B0D0444, 0x2010EA85, 
    0x2416EA80, 0x0444EA07, 0xEA855B0D, 0xEA802010, 0xEA074416, 0x5B0D0444, 0x2010EA85, 0x6416EA80, 
    0x0444EA07, 0xEA855B0D, 0xF1A32010, 0x2B040304, 0xD30AD2DD, 0x4B01F812, 0x0400EA84, 0x0444EA07, 
    0xEA855B0D, 0xF1A32010, 0x2B000301, 0xB280D1F2, 0x4770BCF0, 0xF240B4F0, 0x2B0037FC, 0xF002D045, 
    0x2E000603, 0xF812D031, 0xEA844B01, 0xEA076410, 0x590D0484, 0x2000EA85, 0x0301F1A3, 0x0601F106, 
    0xD0322B00, 0xD1EE2E04, 0xF852E01F, 0xEA866B04, 0xEA076410, 0x590D0484, 0x2000EA85, 0x4410EA86, 
    0x1494EA07, 0xEA85590D, 0xEA862000, 0xEA072410, 0x590D3494, 0x2000EA85, 0x0400EA86, 0x5494EA07, 
    0xEA85590D, 0xF1A32000, 0x2B040304, 0xD30AD2DD, 0x4B01F812, 0x6410EA84, 0x0484EA07, 0xEA85590D, 
    0xF1A32000, 0x2B000301, 0xBCF0D1F2, 0xB4F04770, 0x37FCF240, 0xD0452B00, 0x0603F002, 0xD0312E00, 
    0x4B01F812, 0x0400EA84, 0x0484EA07, 0xEA85590D, 0xF1A32010, 0xF1060301, 0x2B000601, 0x2E04D032, 
    0xE01FD1EE, 0x6B04F852, 0x0406EA80, 0x0484EA07, 0xEA85590D, 0xEA802010, 0xEA072416, 0x590D0484, 
    0x2010EA85, 0x4416EA80, 0x0484EA07, 0xEA85590D, 0xEA802010, 0xEA076416, 0x590D0484, 0x2010EA85, 
    0x0304F1A3, 0xD2DD2B04, 0xF812D30A, 0xEA844B01, 0xEA070400, 0x590D0484, 0x2010EA85, 0x0301F1A3, 
    0xD1F22B00, 0x4770BCF0, 0x00000000, 0xF26B8303, 0xE13B70F7, 0x1350F3F4, 0xC79A971F, 0x35F1141C, 
    0x26A1E7E8, 0xD4CA64EB, 0x8AD958CF, 0x78B2DBCC, 0x6BE22838, 0x9989AB3B, 0x4D43CFD0, 0xBF284CD3, 
    0xAC78BF27, 0x5E133C24, 0x105EC76F, 0xE235446C, 0xF165B798, 0x030E349B, 0xD7C45070, 0x25AFD373, 
    0x36FF2087, 0xC494A384, 0x9A879FA0, 0x68EC1CA3, 0x7BBCEF57, 0x89D76C54, 0x5D1D08BF, 0xAF768BBC, 
    0xBC267848, 0x4E4DFB4B, 0x20BD8EDE, 0xD2D60DDD, 0xC186FE29, 0x33ED7D2A, 0xE72719C1, 0x154C9AC2, 
    0x061C6936, 0xF477EA35, 0xAA64D611, 0x580F5512, 0x4B5FA6E6, 0xB93425E5, 0x6DFE410E, 0x9F95C20D, 
    0x8CC531F9, 0x7EAEB2FA, 0x30E349B1, 0xC288CAB2, 0xD1D83946, 0x23B3BA45, 0xF779DEAE, 0x05125DAD, 
    0x1642AE59, 0xE4292D5A, 0xBA3A117E, 0x4851927D, 0x5B016189, 0xA96AE28A, 0x7DA08661, 0x8FCB0562, 
    0x9C9BF696, 0x6EF07595, 0x417B1DBC, 0xB3109EBF, 0xA0406D4B, 0x522BEE48, 0x86E18AA3, 0x748A09A0, 
    0x67DAFA54, 0x95B17957, 0xCBA24573, 0x39C9C670, 0x2A993584, 0xD8F2B687, 0x0C38D26C, 0xFE53516F, 
    0xED03A29B, 0x1F682198, 0x5125DAD3, 0xA34E59D0, 0xB01EAA24, 0x42752927, 0x96BF4DCC, 0x64D4CECF, 
    0x77843D3B, 0x85EFBE38, 0xDBFC821C, 0x2997011F, 0x3AC7F2EB, 0xC8AC71E8, 0x1C661503, 0xEE0D9600, 
    0xFD5D65F4, 0x0F36E6F7, 0x61C69362, 0x93AD1061, 0x80FDE395, 0x72966096, 0xA65C047D, 0x5437877E, 
    0x4767748A, 0xB50CF789, 0xEB1FCBAD, 0x197448AE, 0x0A24BB5A, 0xF84F3859, 0x2C855CB2, 0xDEEEDFB1, 
    0xCDBE2C45, 0x3FD5AF46, 0x7198540D, 0x83F3D70E, 0x90A324FA, 0x62C8A7F9, 0xB602C312, 0x44694011, 
    0x5739B3E5, 0xA55230E6, 0xFB410CC2, 0x092A8FC1, 0x1A7A7C35, 0xE811FF36, 0x3CDB9BDD, 0xCEB018DE, 
    0xDDE0EB2A, 0x2F8B6829, 0x82F63B78, 0x709DB87B, 0x63CD4B8F, 0x91A6C88C, 0x456CAC67, 0xB7072F64, 
    0xA457DC90, 0x563C5F93, 0x082F63B7, 0xFA44E0B4, 0xE9141340, 0x1B7F9043, 0xCFB5F4A8, 0x3DDE77AB, 
    0x2E8E845F, 0xDCE5075C, 0x92A8FC17, 0x60C37F14, 0x73938CE0, 0x81F80FE3, 0x55326B08, 0xA759E80B, 
    0xB4091BFF, 0x466298FC, 0x1871A4D8, 0xEA1A27DB, 0xF94AD42F, 0x0B21572C, 0xDFEB33C7, 0x2D80B0C4, 
    0x3ED04330, 0xCCBBC033, 0xA24BB5A6, 0x502036A5, 0x4370C551, 0xB11B4652, 0x65D122B9, 0x97BAA1BA, 
    0x84EA524E, 0x7681D14D, 0x2892ED69, 0xDAF96E6A, 0xC9A99D9E, 0x3BC21E9D, 0xEF087A76, 0x1D63F975, 
    0x0E330A81, 0xFC588982, 0xB21572C9, 0x407EF1CA, 0x532E023E, 0xA145813D, 0x758FE5D6, 0x87E466D5, 
    0x94B49521, 0x66DF1622, 0x38CC2A06, 0xCAA7A905, 0xD9F75AF1, 0x2B9CD9F2, 0xFF56BD19, 0x0D3D3E1A, 
    0x1E6DCDEE, 0xEC064EED, 0xC38D26C4, 0x31E6A5C7, 0x22B65633, 0xD0DDD530, 0x0417B1DB, 0xF67C32D8, 
    0xE52CC12C, 0x1747422F, 0x49547E0B, 0xBB3FFD08, 0xA86F0EFC, 0x5A048DFF, 0x8ECEE914, 0x7CA56A17, 
    0x6FF599E3, 0x9D9E1AE0, 0xD3D3E1AB, 0x21B862A8, 0x32E8915C, 0xC083125F, 0x144976B4, 0xE622F5B7, 
    0xF5720643, 0x07198540, 0x590AB964, 0xAB613A67, 0xB831C993, 0x4A5A4A90, 0x9E902E7B, 0x6CFBAD78, 
    0x7FAB5E8C, 0x8DC0DD8F, 0xE330A81A, 0x115B2B19, 0x020BD8ED, 0xF0605BEE, 0x24AA3F05, 0xD6C1BC06, 
    0xC5914FF2, 0x37FACCF1, 0x69E9F0D5, 0x9B8273D6, 0x88D28022, 0x7AB90321, 0xAE7367CA, 0x5C18E4C9, 
    0x4F48173D, 0xBD23943E, 0xF36E6F75, 0x0105EC76, 0x12551F82, 0xE03E9C81, 0x34F4F86A, 0xC69F7B69, 
    0xD5CF889D, 0x27A40B9E, 0x79B737BA, 0x8BDCB4B9, 0x988C474D, 0x6AE7C44E, 0xBE2DA0A5, 0x4C4623A6, 
    0x5F16D052, 0xAD7D5351, 0x00000000
};
static const program_target_temp_t flash_algo_SOFT_CRC32_M3 = {
    0x00000001,
    0x00000005,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000009,
    0x00000000,
    {0x00000001,0x000006BC,0x00000800},
    0x000006AC,
    0x00000010,
    0x00000000,
    0x000006AC, 
};
#endif

/* M0 软件_CRC32 算法 */
static const uint32_t flash_code_SOFT_CRC32_M0[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2, 
    0x47702000, 0x47702000, 0x4903460B, 0x44794602, 0x30FFF04F, 0xB802F000, 0x000000AA, 0xF04FB4F0, 
    0xEA4F07FF, 0x2B000787, 0xF002D045, 0x2E000603, 0xF812D031, 0xEA844B01, 0xEA076410, 0x590D0484, 
    0x2000EA85, 0x0301F1A3, 0x0601F106, 0xD0322B00, 0xD1EE2E04, 0xF852E01F, 0xEA866B04, 0xEA076410, 
    0x590D0484, 0x2000EA85, 0x4410EA86, 0x1494EA07, 0xEA85590D, 0xEA862000, 0xEA072410, 0x590D3494, 
    0x2000EA85, 0x0400EA86, 0x5494EA07, 0xEA85590D, 0xF1A32000, 0x2B040304, 0xD30AD2DD, 0x4B01F812, 
    0x6410EA84, 0x0484EA07, 0xEA85590D, 0xF1A32000, 0x2B000301, 0xBCF0D1F2, 0x00004770, 0x00000000, 
    0xF26B8303, 0xE13B70F7, 0x1350F3F4, 0xC79A971F, 0x35F1141C, 0x26A1E7E8, 0xD4CA64EB, 0x8AD958CF, 
    0x78B2DBCC, 0x6BE22838, 0x9989AB3B, 0x4D43CFD0, 0xBF284CD3, 0xAC78BF27, 0x5E133C24, 0x105EC76F, 
    0xE235446C, 0xF165B798, 0x030E349B, 0xD7C45070, 0x25AFD373, 0x36FF2087, 0xC494A384, 0x9A879FA0, 
    0x68EC1CA3, 0x7BBCEF57, 0x89D76C54, 0x5D1D08BF, 0xAF768BBC, 0xBC267848, 0x4E4DFB4B, 0x20BD8EDE, 
    0xD2D60DDD, 0xC186FE29, 0x33ED7D2A, 0xE72719C1, 0x154C9AC2, 0x061C6936, 0xF477EA35, 0xAA64D611, 
    0x580F5512, 0x4B5FA6E6, 0xB93425E5, 0x6DFE410E, 0x9F95C20D, 0x8CC531F9, 0x7EAEB2FA, 0x30E349B1, 
    0xC288CAB2, 0xD1D83946, 0x23B3BA45, 0xF779DEAE, 0x05125DAD, 0x1642AE59, 0xE4292D5A, 0xBA3A117E, 
    0x4851927D, 0x5B016189, 0xA96AE28A, 0x7DA08661, 0x8FCB0562, 0x9C9BF696, 0x6EF07595, 0x417B1DBC, 
    0xB3109EBF, 0xA0406D4B, 0x522BEE48, 0x86E18AA3, 0x748A09A0, 0x67DAFA54, 0x95B17957, 0xCBA24573, 
    0x39C9C670, 0x2A993584, 0xD8F2B687, 0x0C38D26C, 0xFE53516F, 0xED03A29B, 0x1F682198, 0x5125DAD3, 
    0xA34E59D0, 0xB01EAA24, 0x42752927, 0x96BF4DCC, 0x64D4CECF, 0x77843D3B, 0x85EFBE38, 0xDBFC821C, 
    0x2997011F, 0x3AC7F2EB, 0xC8AC71E8, 0x1C661503, 0xEE0D9600, 0xFD5D65F4, 0x0F36E6F7, 0x61C69362, 
    0x93AD1061, 0x80FDE395, 0x72966096, 0xA65C047D, 0x5437877E, 0x4767748A, 0xB50CF789, 0xEB1FCBAD, 
    0x197448AE, 0x0A24BB5A, 0xF84F3859, 0x2C855CB2, 0xDEEEDFB1, 0xCDBE2C45, 0x3FD5AF46, 0x7198540D, 
    0x83F3D70E, 0x90A324FA, 0x62C8A7F9, 0xB602C312, 0x44694011, 0x5739B3E5, 0xA55230E6, 0xFB410CC2, 
    0x092A8FC1, 0x1A7A7C35, 0xE811FF36, 0x3CDB9BDD, 0xCEB018DE, 0xDDE0EB2A, 0x2F8B6829, 0x82F63B78, 
    0x709DB87B, 0x63CD4B8F, 0x91A6C88C, 0x456CAC67, 0xB7072F64, 0xA457DC90, 0x563C5F93, 0x082F63B7, 
    0xFA44E0B4, 0xE9141340, 0x1B7F9043, 0xCFB5F4A8, 0x3DDE77AB, 0x2E8E845F, 0xDCE5075C, 0x92A8FC17, 
    0x60C37F14, 0x73938CE0, 0x81F80FE3, 0x55326B08, 0xA759E80B, 0xB4091BFF, 0x466298FC, 0x1871A4D8, 
    0xEA1A27DB, 0xF94AD42F, 0x0B21572C, 0xDFEB33C7, 0x2D80B0C4, 0x3ED04330, 0xCCBBC033, 0xA24BB5A6, 
    0x502036A5, 0x4370C551, 0xB11B4652, 0x65D122B9, 0x97BAA1BA, 0x84EA524E, 0x7681D14D, 0x2892ED69, 
    0xDAF96E6A, 0xC9A99D9E, 0x3BC21E9D, 0xEF087A76, 0x1D63F975, 0x0E330A81, 0xFC588982, 0xB21572C9, 
    0x407EF1CA, 0x532E023E, 0xA145813D, 0x758FE5D6, 0x87E466D5, 0x94B49521, 0x66DF1622, 0x38CC2A06, 
    0xCAA7A905, 0xD9F75AF1, 0x2B9CD9F2, 0xFF56BD19, 0x0D3D3E1A, 0x1E6DCDEE, 0xEC064EED, 0xC38D26C4, 
    0x31E6A5C7, 0x22B65633, 0xD0DDD530, 0x0417B1DB, 0xF67C32D8, 0xE52CC12C, 0x1747422F, 0x49547E0B, 
    0xBB3FFD08, 0xA86F0EFC, 0x5A048DFF, 0x8ECEE914, 0x7CA56A17, 0x6FF599E3, 0x9D9E1AE0, 0xD3D3E1AB, 
    0x21B862A8, 0x32E8915C, 0xC083125F, 0x144976B4, 0xE622F5B7, 0xF5720643, 0x07198540, 0x590AB964, 
    0xAB613A67, 0xB831C993, 0x4A5A4A90, 0x9E902E7B, 0x6CFBAD78, 0x7FAB5E8C, 0x8DC0DD8F, 0xE330A81A, 
    0x115B2B19, 0x020BD8ED, 0xF0605BEE, 0x24AA3F05, 0xD6C1BC06, 0xC5914FF2, 0x37FACCF1, 0x69E9F0D5, 
    0x9B8273D6, 0x88D28022, 0x7AB90321, 0xAE7367CA, 0x5C18E4C9, 0x4F48173D, 0xBD23943E, 0xF36E6F75, 
    0x0105EC76, 0x12551F82, 0xE03E9C81, 0x34F4F86A, 0xC69F7B69, 0xD5CF889D, 0x27A40B9E, 0x79B737BA, 
    0x8BDCB4B9, 0x988C474D, 0x6AE7C44E, 0xBE2DA0A5, 0x4C4623A6, 0x5F16D052, 0xAD7D5351, 0x00000000
};
static const program_target_temp_t flash_algo_SOFT_CRC32_M0 = {
    0x00000001,
    0x00000005,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000009,
    0x00000000,
    {0x00000001,0x000004F0,0x00000800},
    0x000004E0,
    0x00000010,
    0x00000000,
    0x000004E0,
};

/*
*********************************************************************************************************
*    函 数 名: LoadCheckBlankAlgoToTarget
*    功能说明: 将算法加载到目标CPU内存
*    形    参: 
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
uint8_t LoadCheckBlankAlgoToTarget(void)
{
    uint8_t *pflash_code;
    const program_target_temp_t *pflash_algo;

    pflash_code = (uint8_t *)flash_code_check_blank;
    pflash_algo = &flash_algo_check_blank;
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        if (0 == MUL_swd_write_memory(pflash_algo->algo_start + g_AlgoRam.Addr, 
            pflash_code, pflash_algo->algo_size + 32)) {
            return ERROR_ALGO_DL;
        }         
    }
    else
    {
        if (0 == swd_write_memory(pflash_algo->algo_start + g_AlgoRam.Addr, 
            pflash_code, pflash_algo->algo_size + 32)) {
            return ERROR_ALGO_DL;
        }             
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: LoadCheckCRCAlgoToTarget
*    功能说明: 将算法加载到目标CPU内存
*    形    参: 
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
uint8_t LoadCheckCRCAlgoToTarget(void)
{
    uint8_t *pflash_code;
    const program_target_temp_t *pflash_algo;

    /* 校验模式由lua文件决定 */
    switch(g_tProg.VerifyMode)
    {               
        case VERIFY_SOFT_CRC:
            pflash_code = (uint8_t *)flash_code_SOFT_CRC32_M0;
            pflash_algo = &flash_algo_SOFT_CRC32_M0;            
            break;

        case VERIFY_STM32_CRC:
            if (strstr(flash_algo.algo_file_name, "/STM32G4xx/") || strstr(flash_algo.algo_file_name, "/STM32L4xx/"))
            {
                pflash_code = (uint8_t *)flash_code_STM32_CRC32_G4;
                pflash_algo = &flash_algo_STM32_CRC32_G4;
            }
            else
            {
                pflash_code = (uint8_t *)flash_code_STM32_CRC32_F0;
                pflash_algo = &flash_algo_STM32_CRC32_F0;                
            }
            break;
        
        default:
        case VERIFY_READ_BACK:      /* 无需加载算法 */
            return 0;
            
    }
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        if (0 == MUL_swd_write_memory(pflash_algo->algo_start + g_AlgoRam.Addr, 
            pflash_code, pflash_algo->algo_size + 32)) {
            return ERROR_ALGO_DL;
        }         
    }
    else
    {
        if (0 == swd_write_memory(pflash_algo->algo_start + g_AlgoRam.Addr, 
            pflash_code, pflash_algo->algo_size + 32)) {
            return ERROR_ALGO_DL;
        }             
    }
    return 0;
}


extern void PG_PrintText(char *_str);
error_t target_flash_enter_debug_program(void)
{
    g_tProg.FLMFuncTimeout = 500;    /* 超时 */  
    
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
//        /* SWD进入debug状态 */
//        if (MUL_swd_init_debug() == 0)
//        {
//            PG_PrintText("SWD初始化失败！");        
//            return ERROR_FAILURE;   
//        }

//        if (0 == MUL_swd_set_target_state_sw(RESET_PROGRAM)) 
//        {
//            printf("error: swd_set_target_state_sw(RESET_PROGRAM)\r\n");
//            
//            if (MUL_swd_init_debug() == 0)
//            {
//                PG_PrintText("SWD初始化失败！");        
//                return ERROR_FAILURE;   
//            }
//        
//            if (0 == MUL_swd_set_target_state_hw(RESET_PROGRAM)) 
//            {
//                printf("error: swd_set_target_state_hw(RESET_PROGRAM)\r\n");
//                return ERROR_RESET;
//            }                
//        }   

        if (MUL_swd_enter_debug_program() == 0)
        {
            return ERROR_FAILURE; 
        }
    }
    else
    {
//        if (swd_init_debug() == 0)
//        {
//            PG_PrintText("SWD初始化失败！");        
//            return ERROR_FAILURE;     
//        }
//        
//        if (0 == swd_set_target_state_sw(RESET_PROGRAM)) 
//        {
//            printf("error: swd_set_target_state_sw(RESET_PROGRAM)\r\n");

//            if (swd_init_debug() == 0)
//            {
//                PG_PrintText("SWD初始化失败！");        
//                return ERROR_FAILURE;     
//            }
//        
//            if (0 == swd_set_target_state_hw(RESET_PROGRAM)) 
//            {
//                printf("error: swd_set_target_state_hw(RESET_PROGRAM)\r\n");
//                return ERROR_RESET;
//            }              
//        }         
        
        if (swd_enter_debug_program() == 0)
        {
            return ERROR_FAILURE; 
        }
    }
    
    return ERROR_SUCCESS;
}

/*
*********************************************************************************************************
*    函 数 名: target_flash_init
*    功能说明: 将算法加载到目标CPU内存
*    形    参: 
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
error_t target_flash_init(uint32_t flash_start, unsigned long clk, unsigned long fnc)
{   
	
	/*
		http://www.keil.com/pack/doc/cmsis/Pack/html/algorithmFunc.html#Verify

	int Init (unsigned long adr, unsigned long clk, unsigned long fnc);
    
    Parameters
        adr	Device base address
        clk	Clock frequency (Hz)
        fnc	Function code
	Returns
        status information:
        0 on success.
        1 on failure.
    
    The function Init initializes the microcontroller for Flash programming. 
       It is invoked whenever an attempt is made to download the program to Flash.

        The argument adr specifies the base address of the device.

        The argument clk specifies the clock frequency for prgramming the device.

        The argument fnc is a number:

	1 stands for Erase.
	2 stands for Program.
	3 stands for Verify.
	
        Thus, different initialization sections can be implemented for each individual Flash programming step.
        The argument fnc is a number:
    */
    
    g_tProg.FLMFuncTimeout = 500;    /* 超时 */  
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {    
        if (0 == MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, flash_start, clk, fnc, 0)) {
            return ERROR_INIT;
        }
    }
    else
    {
        if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, flash_start, clk, fnc, 0)) {
            return ERROR_INIT;
        }        
    }
    return ERROR_SUCCESS;
}

/* 这个函数暂时未用到 */
error_t target_flash_uninit(void)
{
#if 1
    g_tProg.FLMFuncTimeout = 500;    /* 超时 */  
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {    
        if (0 == MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.uninit, 0, 0, 0, 0)) {
            return ERROR_INIT;
        }
    }
    else
    {
        if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.uninit, 0, 0, 0, 0)) {
            return ERROR_INIT;
        }        
    }
    return ERROR_SUCCESS;
#else    
    g_tProg.FLMFuncTimeout = 200;       /* 函数执行超时 */
    
    swd_set_target_state_hw(RESET_RUN);

    swd_off();
    
    return ERROR_SUCCESS;
#endif    
}

error_t target_flash_program_page(uint32_t addr, const uint8_t *buf, uint32_t size)
{
    if ( flash_algo.program_page == 0)
    {
        return ERROR_SUCCESS;
    }

    if (g_tProg.FLMFuncDispProgress == 1)
    {
        /*
            FLM中的超时3秒, 如果是解除读保护，这个时间就不够，因此不要在此赋值
            g_tProg.FLMFuncTimeout = g_tFLM.Device.toProg;  // page编程超时
        
            STM32F429BI 大概16秒 2MB
        */        
        g_tProg.FLMFuncTimeout = g_tProg.EraseChipTime;
        
        if (g_tFLM.Device.toProg > g_tProg.FLMFuncTimeout)
        {
            g_tProg.FLMFuncTimeout = g_tFLM.Device.toProg;
        }
    }
    else
    {
        g_tProg.FLMFuncTimeout = g_tFLM.Device.toProg;
    }
    
    /* AT32F403的FLM，UOB page size = 1K, 应该等于OB的size */
    if (size > g_tFLM.Device.szDev)
    {
        size = g_tFLM.Device.szDev;
    }
    
    while (size > 0) {
        uint32_t write_size;

        write_size = flash_algo.program_buffer_size;
        if (write_size > g_tFLM.Device.szDev)
        {
            write_size = g_tFLM.Device.szDev;
        }

        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            // Write page to buffer
            if (!MUL_swd_write_memory(flash_algo.program_buffer, (uint8_t *)buf, write_size)) {
                return ERROR_ALGO_DATA_SEQ;
            }
        
            // Run flash programming
            if (!MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s,
                                        flash_algo.program_page,
                                        addr,
                                        write_size,
                                        flash_algo.program_buffer,
                                        0)) {
                return ERROR_WRITE;
            }
        }
        else
        {
            // Write page to buffer
            if (!swd_write_memory(flash_algo.program_buffer, (uint8_t *)buf, write_size)) {
                return ERROR_ALGO_DATA_SEQ;
            }
        

            // Run flash programming
            if (!swd_flash_syscall_exec(&flash_algo.sys_call_s,
                                        flash_algo.program_page,
                                        addr,
                                        write_size,
                                        flash_algo.program_buffer,
                                        0)) {
                return ERROR_WRITE;
            }
        }
		addr += write_size;
		buf  += write_size;
		size -= write_size;
    }

    return ERROR_SUCCESS;
}

/* 校验函数 */
error_t target_flash_verify_page(uint32_t addr, const uint8_t *buf, uint32_t size)
{    
    if ( flash_algo.verify == 0)
    {
        return ERROR_SUCCESS;
    }

    /* AT32F403的FLM，UOB page size = 1K, 应该等于OB的size */
    if (size > g_tFLM.Device.szDev)
    {
        size = g_tFLM.Device.szDev;
    }
    
    g_tProg.FLMFuncTimeout = 3000;
    while (size > 0) 
    {
        uint32_t write_size;

        write_size = flash_algo.program_buffer_size;
        if (write_size > g_tFLM.Device.szDev)
        {
            write_size = g_tFLM.Device.szDev;
        }
        
        /* 2020-12-14 修复bug */
        if (write_size > size)
        {
            write_size = size;
        }

        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            uint32_t *ret_val;
            uint8_t i;
            uint8_t err = 0;
            
            // Write page to buffer
            if (!MUL_swd_write_memory(flash_algo.program_buffer, (uint8_t *)buf, write_size)) {
                return ERROR_ALGO_DATA_SEQ;
            }
        
            // Run verify programming
            ret_val = (uint32_t *)MUL_swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                        flash_algo.verify,
                                        addr,
                                        write_size,
                                        flash_algo.program_buffer,
                                        0);
            
            if (ret_val == 0)   /* 通信错误 */
            {
                err = 1;
            }
            else
            {
                if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
                {
                    for (i = 0; i < 4; i++)
                    {
                        if (g_gMulSwd.Active[i] == 1 && ret_val[i] != addr + size)
                        {
                            g_gMulSwd.Error[i] = 1;
                            err = 1;        /* 结果错误 */
                        }
                    }
                }
                else
                {
                    err = 1;
                    
                    for (i = 0; i < 4; i++)
                    {
                        if (g_gMulSwd.Active[i] == 1 && ret_val[i] != addr + size)
                        {
                            g_gMulSwd.Error[i] = 1;
                        }
                        else
                        {
                            err = 0;    /* 有1路OK */
                        }
                    }                    
                }
            }
                
            if (err == 1)
            {
                return ERROR_WRITE;
            }
        }
        else    /* 单路模式 */
        {
            uint32_t *ret_val;
            
            // Write page to buffer
            if (!swd_write_memory(flash_algo.program_buffer, (uint8_t *)buf, write_size)) {
                return ERROR_ALGO_DATA_SEQ;
            }
        
            // Run verify programming
            ret_val = (uint32_t *)swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                        flash_algo.verify,
                                        addr,
                                        write_size,
                                        flash_algo.program_buffer,
                                        0);
            if (ret_val == 0)   /* 通信错误 */
            {
                return ERROR_WRITE;
            }
            else
            {
                if (ret_val[0] != addr + size)
                {
                    g_gMulSwd.Error[0] = 1;
                    printf("target_flash_verify_page() %08X != %08X(ok)\r\n", ret_val[0], addr + size);
					return ERROR_WRITE;
                }
            }           
        }
		addr += write_size;
		buf  += write_size;
		size -= write_size;
    }

    return ERROR_SUCCESS;
}


//#define CHK_BLANK_ERROR     0   /* 执行查空失败 */
//#define CHK_IS_BLANK        1   /* 是空片 */
//#define CHK_NOT_BLANK       2   /* 不是空片 */

/* 查空函数, 1表示不空需要擦除 */
uint8_t target_flash_check_blank(uint32_t addr, uint32_t size)
{    
    if ( flash_algo.check_blank == 0)
    {
        return CHK_IS_BLANK;
    }
    
    g_tProg.FLMFuncTimeout = 3000;
        
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        uint32_t *ret_val;
        
        /* FLM函数返回1表示芯片不空，0表示芯片是空的 */
        ret_val = (uint32_t *)MUL_swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                    flash_algo.check_blank,
                                    addr,
                                    size,
                                    g_tFLM.Device.valEmpty,     /* 空值，多半为0xFF,  STM32L051为0x00 */
                                    0);
        if (ret_val == 0)    /* 通信异常 */
        {
            return CHK_BLANK_ERROR;
        }
        else    /* > 0 通信OK */
        {
            {
                uint8_t blank = 1;
                uint8_t i;
    
                /* FLM函数返回1表示芯片不空，0表示芯片是空的 */
                for (i = 0; i < 4; i++)
                {
                    if (g_gMulSwd.Active[i] == 1 && ret_val[i] != 0)
                    {
                        blank = 0;
                        break;
                    }
                }
                
                if (blank == 0)
                {
                    return CHK_NOT_BLANK;
                }
                else
                {
                    return CHK_IS_BLANK;
                }
            }
        }
    }
    else
    {
        uint32_t *ret_val;
        
        ret_val = (uint32_t *)swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                    flash_algo.check_blank,
                                    addr,
                                    size,
                                    g_tFLM.Device.valEmpty,     /* 空值，多半为0xFF,  STM32L051为0x00 */
                                    0);
        if (ret_val == 0)   /* 通信异常 */
        {
            return CHK_BLANK_ERROR;
        }
        else    /* > 0 通信OK */
        {
            /* FLM函数返回1表示芯片不空，0表示芯片是空的 */
            if (1 == ret_val[0]) 
            {
                return CHK_NOT_BLANK;
            }
            else
            {
                return CHK_IS_BLANK;
            }
        }
    }
}

/* 
    查空函数, 1表示不空需要擦除 
    芯片厂家FLM文件没有 check_blank 函数，则加载一个查空函数
*/
uint8_t target_flash_check_blank_ex(uint32_t addr, uint32_t size)
{        
    program_syscall_t sys_call_s;
    
    if (flash_algo_check_blank.check_blank == 0)
    {
        return ERROR_SUCCESS;
    }

    g_tProg.FLMFuncTimeout = 3000;
    
    sys_call_s.breakpoint = flash_algo_check_blank.sys_call_s.breakpoint + g_AlgoRam.Addr;
    sys_call_s.stack_pointer = flash_algo_check_blank.sys_call_s.stack_pointer + g_AlgoRam.Addr;
    sys_call_s.static_base = flash_algo_check_blank.program_buffer + flash_algo_check_blank.program_buffer_size  + g_AlgoRam.Addr;
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        uint32_t *ret_val;
        
        ret_val = (uint32_t *)MUL_swd_flash_syscall_exec_ex(&sys_call_s,
                                    flash_algo_check_blank.check_blank + g_AlgoRam.Addr + 32,
                                    addr,
                                    size,
                                    g_tFLM.Device.valEmpty,     /* 空值，多半为0xFF,  STM32L051为0x00 */
                                    0);
        if (ret_val == 0)    /* 通信异常 */
        {
            return CHK_BLANK_ERROR;
        }
        else    /* > 0 通信OK */
        {
            {
                uint8_t blank = 1;
                uint8_t i;
    
                /* FLM函数返回1表示芯片不空，0表示芯片是空的 */
                for (i = 0; i < 4; i++)
                {
                    if (g_gMulSwd.Active[i] == 1 && ret_val[i] != 0)
                    {
                        blank = 0;
                        break;
                    }
                }
                
                if (blank == 0)
                {
                    return CHK_NOT_BLANK;
                }
                else
                {
                    return CHK_IS_BLANK;
                }
            }
        }
    }
    else
    {
        uint8_t ret_val;
        
        ret_val = swd_flash_syscall_exec(&sys_call_s,
                                    flash_algo_check_blank.check_blank + g_AlgoRam.Addr + 32,
                                    addr,
                                    size,
                                    g_tFLM.Device.valEmpty,     /* 空值，多半为0xFF,  STM32L051为0x00 */
                                    0);

        /* FLM函数返回1表示芯片不空，0表示芯片是空的 */
        if (ret_val == 0) 
        {
            return CHK_NOT_BLANK;
        }
        else
        {
            return CHK_IS_BLANK;
        }
    }
}

/* 计算flash crc32 */
uint32_t target_flash_cacul_crc32(uint32_t addr, uint32_t size, uint32_t ini_value)
{    
    uint32_t crc;
   
    if ( flash_algo.cacul_crc32 == 0)
    {
        return 0;
    }       
    // Run verify programming
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */    
    {
        /* 返回值是存放4路CRC的变量地址，需要(uint32_t *)转换后使用 */
        crc = MUL_swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                    flash_algo.cacul_crc32,
                                    addr,
                                    size,
                                    ini_value,
                                    0);
    }
    else
    {
        crc = swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                    flash_algo.cacul_crc32,
                                    addr,
                                    size,
                                    ini_value,
                                    0);        
    }
    return crc;
}

/* 
    计算flash crc32 
*/
uint32_t target_flash_cacul_crc32_ex(uint32_t addr, uint32_t size, uint32_t ini_value)
{    
    uint32_t crc;
    program_syscall_t sys_call_s;

    const program_target_temp_t *pflash_algo;

    /* 校验模式由lua文件决定 */
    switch(g_tProg.VerifyMode)
    {
        case VERIFY_READ_BACK:      /* 无需加载算法 */
            return 0;
               
        case VERIFY_SOFT_CRC:
            pflash_algo = &flash_algo_SOFT_CRC32_M0;            
            break;

        case VERIFY_STM32_CRC:
            if (strstr(flash_algo.algo_file_name, "/STM32G4xx/") || strstr(flash_algo.algo_file_name, "/STM32L4xx/"))
            {
                pflash_algo = &flash_algo_STM32_CRC32_G4;
            }
            else
            {
                pflash_algo = &flash_algo_STM32_CRC32_F0;                
            }
            break;       
    }    

    g_tProg.FLMFuncTimeout = 3000;
    
    sys_call_s.breakpoint = pflash_algo->sys_call_s.breakpoint + g_AlgoRam.Addr;
    sys_call_s.stack_pointer = pflash_algo->sys_call_s.stack_pointer + g_AlgoRam.Addr;
    sys_call_s.static_base = pflash_algo->program_buffer + pflash_algo->program_buffer_size  + g_AlgoRam.Addr;    
       
    // Run verify programming
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */    
    {
        /* 返回值是存放4路CRC的变量地址，需要(uint32_t *)转换后使用 */
        crc = MUL_swd_flash_syscall_exec_ex(&sys_call_s,
                                    pflash_algo->cacul_crc32 + g_AlgoRam.Addr + 32,
                                    addr,
                                    size,
                                    ini_value,
                                    0);
    }
    else
    {
        crc = swd_flash_syscall_exec_ex(&sys_call_s,
                                    pflash_algo->cacul_crc32 + g_AlgoRam.Addr + 32,
                                    addr,
                                    size,
                                    ini_value,
                                    0);        
    }
    return crc;
}

error_t target_flash_erase_sector(uint32_t addr)
{   
    if ( flash_algo.erase_sector == 0)
    {
        return ERROR_SUCCESS;
    } 
    
    g_tProg.FLMFuncTimeout = g_tFLM.Device.toErase;
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        if (0 == MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.erase_sector, addr, 0, 0, 0)) {
            return ERROR_ERASE_SECTOR;
        }
    }
    else
    {
        if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.erase_sector, addr, 0, 0, 0)) {
            return ERROR_ERASE_SECTOR;
        }        
    }
    return ERROR_SUCCESS;
}

error_t target_flash_erase_chip(void)
{   
    error_t status = ERROR_SUCCESS;  

    if ( flash_algo.erase_chip == 0)
    {
        return ERROR_SUCCESS;
    }     

    /* 2020-05-05 调试灵动时，需要额外增加如下代码. 原因未知 */
    #if 0
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {    
        if (0 == MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, g_tFLM.Device.DevAdr, 0, 1, 0)) {
            bsp_DelayMS(100);
            if (0 == MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, g_tFLM.Device.DevAdr, 0, 1, 0))  {
                return ERROR_INIT;
            }
        }
    }
    else
    {
        if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, g_tFLM.Device.DevAdr, 0, 1, 0)) {
            if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, g_tFLM.Device.DevAdr, 0, 1, 0)) {
                return ERROR_INIT;
            }
        }        
    } 
    #endif
    
    g_tProg.FLMFuncDispProgress = 1;
    g_tProg.FLMFuncTimeout = g_tProg.EraseChipTime;     /* 由lua文件配置 */
    g_tProg.FLMFuncDispAddr = g_tFLM.Device.DevAdr;
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        if (0 == MUL_swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.erase_chip, 0, 0, 0, 0)) 
        {
            return ERROR_ERASE_ALL;
        }
    }
    else
    {
        if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.erase_chip, 0, 0, 0, 0)) 
        {
            return ERROR_ERASE_ALL;
        }       
    }
    return status;
}

/* 读取外部SPI FLASH的id */
uint32_t target_flash_read_extid(uint32_t addr)
{    
    uint32_t id;    
    g_tProg.FLMFuncTimeout = 500;    /* 超时 */  
    
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        /* 返回值是存放4路ID的变量地址，需要(uint32_t *)转换后使用 */
        id = MUL_swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                    flash_algo.read_extid,
                                    addr,
                                    0,
                                    0,
                                    0);        
    }
    else
    {
        id = MUL_swd_flash_syscall_exec_ex(&flash_algo.sys_call_s,
                                    flash_algo.read_extid,
                                    addr,
                                    0,
                                    0,
                                    0);        
    }
    return id;    
}
