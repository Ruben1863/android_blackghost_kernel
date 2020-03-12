
/*----------------------------------------------------------------
* Author : Rubén Espínola (ruben1863@github.com)
* Contact : rubenes2003@gmail.com
* Supported device: Tecno POP 2F
* Reversed for Melek Saidani
* Copyright 2019 © Rubén Espínola
 *---------------------------------------------------------------*/


#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------



#define FRAME_WIDTH                                         (960)
#define FRAME_HEIGHT                                        (480)


#define REGFLAG_DELAY               (0XFE)
#define REGFLAG_END_OF_TABLE        (0x00)


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V3(para_tbl,size,force_update)         lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define read_reg_v2(cmd, buffer, buffer_size)	            lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define write_regs(addr, pdata, byte_nums)	                lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define wrtie_cmd(cmd)	lcm_util.dsi_write_cmd(cmd)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

};	

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = 
{
    { 0x28, 0x01, {0x00}},
    { REGFLAG_DELAY, 20, {0x00}},
    { 0x10, 0x01, {0x00}},
    { REGFLAG_DELAY, 120, {0x00}},
    { REGFLAG_END_OF_TABLE, 0x00, {0x00}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {
        
        unsigned cmd;
        cmd = table[i].cmd;
        
        switch (cmd) {
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
                
            case REGFLAG_END_OF_TABLE :
                break;
                
            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
    
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));	

	params->physical_width = 61;
	params->physical_height = 123;
	params->physical_width_um = 61880;
	params->type = 2;
	params->width = 480;
	params->height = 960;
	params->physical_height_um = 123750;
	params->dsi.lane_swap_en = 6;
	params->dsi.vertical_active_line = 960;
	params->dsi.data_format.color_order = 0;
	params->dsi.data_format.trans_seq = 0;
	params->dsi.data_format.padding = 0;
	params->dsi.horizontal_backporch = 35;
	params->dsi.vertical_sync_active = 8;
	params->dsi.horizontal_active_pixel = 480;
	params->dsi.vertical_backporch = 20;
	params->dsi.vertical_frontporch = 20;
	params->dsi.PLL_CLOCK = 208;
	params->dsi.horizontal_sync_active = 10;
	params->dsi.ssc_range = 4;
	params->lcm_if = 1;
	params->lcm_cmd_if = 1;
	params->dsi.mode = 1;
	params->dsi.LANE_NUM = 2;
	params->dsi.data_format.format = 2;
	params->dsi.PS = 2;
	params->dsi.horizontal_frontporch = 30;
	params->dsi.ssc_disable = 1;
	params->dsi.HS_TRAIL = 15;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 10;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = -100;
	params->dsi.noncont_clock = 1;
	params->dsi.noncont_clock_period = 2;
}

static void lcm_init(void)
{
	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO);
	MDELAY(5);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
	MDELAY(10);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO);
	MDELAY(120);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
}

static void lcm_resume(void)
{
	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO);
	MDELAY(5);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ONE);
	MDELAY(10);
	mt_set_gpio_out(0x80000046, GPIO_OUT_ZERO;
	MDELAY(10);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_compare_id(void)
{
	return 1;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER st7701s_fwvga_dsi_vdo_coe_ivo_b1_lcm_drv = 
{
    .name           = "st7701s_fwvga_dsi_vdo_coe_ivo_b1",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,   
    .compare_id     = lcm_compare_id,

};

