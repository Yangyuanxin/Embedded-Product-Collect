#ifndef IO_H_
#define IO_H_

extern void io_evc_poweron(void);
extern void io_evc_poweroff(void);
extern void io_runled_on(void);
extern void io_runled_off(void);
extern void io_errled_on(void);
extern void io_errled_off(void);
extern void io_fun_on(void);
extern void io_fun_off(void);
extern void io_bms_sng1_en(void);
extern void io_bms_sng1_dis(void);
extern void io_bms_sng2_en(void);
extern void io_bms_sng2_dis(void);
extern void io_bms_dc_out_en(void);
extern void io_bms_dc_out_dis(void);
extern u8 io_ac_status(void);
extern u8 io_dc1_status(void);
extern u8 io_dc2_status(void);
extern void io_bms_12v_en(void);
extern void io_bms_12v_dis(void);
extern void io_bms_24v_en(void);
extern void io_bms_24v_dis(void);

#endif
