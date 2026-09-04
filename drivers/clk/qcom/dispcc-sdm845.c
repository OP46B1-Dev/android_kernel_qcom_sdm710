// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 */

#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>

#include <dt-bindings/clock/qcom,dispcc-sdm845.h>

#include "clk-alpha-pll.h"
#include "clk-branch.h"
#include "clk-rcg.h"
#include "clk-regmap-divider.h"
#include "common.h"
#include "gdsc.h"
#include "reset.h"
#include "vdd-level.h"

static DEFINE_VDD_REGULATORS(vdd_cx, VDD_NUM, 1, vdd_corner);

enum {
	P_BI_TCXO,
	P_CORE_BI_PLL_TEST_SE,
	P_DISP_CC_PLL0_OUT_MAIN,
	P_DP_PHY_PLL_LINK_CLK,
	P_DP_PHY_PLL_VCO_DIV_CLK,
	P_DSI0_PHY_PLL_OUT_BYTECLK,
	P_DSI0_PHY_PLL_OUT_DSICLK,
	P_DSI1_PHY_PLL_OUT_BYTECLK,
	P_DSI1_PHY_PLL_OUT_DSICLK,
	P_GPLL0_OUT_MAIN,
	P_GPLL0_OUT_MAIN_DIV,
};

static const struct parent_map disp_cc_parent_map_0[] = {
	{ P_BI_TCXO, 0 },
	{ P_DSI0_PHY_PLL_OUT_BYTECLK, 1 },
	{ P_DSI1_PHY_PLL_OUT_BYTECLK, 2 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const disp_cc_parent_names_0[] = {
	"bi_tcxo",
	"dsi0_phy_pll_out_byteclk",
	"dsi1_phy_pll_out_byteclk",
	"core_bi_pll_test_se",
};

static const struct parent_map disp_cc_parent_map_1[] = {
	{ P_BI_TCXO, 0 },
	{ P_DP_PHY_PLL_LINK_CLK, 1 },
	{ P_DP_PHY_PLL_VCO_DIV_CLK, 2 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const disp_cc_parent_names_1[] = {
	"bi_tcxo",
	"dp_link_clk_divsel_ten",
	"dp_vco_divided_clk_src_mux",
	"core_bi_pll_test_se",
};

static const struct parent_map disp_cc_parent_map_2[] = {
	{ P_BI_TCXO, 0 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const disp_cc_parent_names_2[] = {
	"bi_tcxo",
	"core_bi_pll_test_se",
};

static const struct parent_map disp_cc_parent_map_3[] = {
	{ P_BI_TCXO, 0 },
	{ P_DISP_CC_PLL0_OUT_MAIN, 1 },
	{ P_GPLL0_OUT_MAIN, 4 },
	{ P_GPLL0_OUT_MAIN_DIV, 5 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const disp_cc_parent_names_3[] = {
	"bi_tcxo",
	"disp_cc_pll0",
	"gcc_disp_gpll0_clk_src",
	"gcc_disp_gpll0_div_clk_src",
	"core_bi_pll_test_se",
};

static const struct parent_map disp_cc_parent_map_4[] = {
	{ P_BI_TCXO, 0 },
	{ P_DSI0_PHY_PLL_OUT_DSICLK, 1 },
	{ P_DSI1_PHY_PLL_OUT_DSICLK, 2 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const disp_cc_parent_names_4[] = {
	"bi_tcxo",
	"dsi0_phy_pll_out_dsiclk",
	"dsi1_phy_pll_out_dsiclk",
	"core_bi_pll_test_se",
};

static struct pll_vco fabia_vco[] = {
	{ 249600000, 2000000000, 0 },
	{ 125000000, 1000000000, 1 },
};

static struct alpha_pll_config disp_cc_pll0_config = {
	.l = 0x2c,
	.alpha = 0xcaaa,
};

static struct clk_alpha_pll disp_cc_pll0 = {
	.offset = 0x0,
	.vco_table = fabia_vco,
	.num_vco = ARRAY_SIZE(fabia_vco),
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_FABIA],
	.config = &disp_cc_pll0_config,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_pll0",
			.parent_names = (const char *[]){ "bi_tcxo" },
			.num_parents = 1,
			.ops = &clk_alpha_pll_fabia_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_rcg2 disp_cc_mdss_byte0_clk_src = {
	.cmd_rcgr = 0x20d0,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_0,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_byte0_clk_src",
		.parent_names = disp_cc_parent_names_0,
		.num_parents = 4,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_byte2_ops,
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_rcg2 disp_cc_mdss_byte1_clk_src = {
	.cmd_rcgr = 0x20ec,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_0,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_byte1_clk_src",
		.parent_names = disp_cc_parent_names_0,
		.num_parents = 4,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_byte2_ops,
	},
};

static const struct freq_tbl ftbl_disp_cc_mdss_esc0_clk_src[] = {
	F(19200000, P_BI_TCXO, 1, 0, 0),
	{ }
};

static struct clk_rcg2 disp_cc_mdss_dp_aux_clk_src = {
	.cmd_rcgr = 0x219c,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_2,
	.freq_tbl = ftbl_disp_cc_mdss_esc0_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_dp_aux_clk_src",
		.parent_names = disp_cc_parent_names_2,
		.num_parents = ARRAY_SIZE(disp_cc_parent_names_2),
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_rcg2_ops,
	},
};

/* The downstream DP PLL and display consumer express DP rates in kHz. */
static const struct freq_tbl ftbl_disp_cc_mdss_dp_crypto_clk_src[] = {
	F(108000, P_DP_PHY_PLL_LINK_CLK, 3, 0, 0),
	F(180000, P_DP_PHY_PLL_LINK_CLK, 3, 0, 0),
	F(360000, P_DP_PHY_PLL_LINK_CLK, 3, 0, 0),
	F(540000, P_DP_PHY_PLL_LINK_CLK, 3, 0, 0),
	{ }
};

static struct clk_rcg2 disp_cc_mdss_dp_crypto_clk_src = {
	.cmd_rcgr = 0x2154,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_1,
	.freq_tbl = ftbl_disp_cc_mdss_dp_crypto_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_dp_crypto_clk_src",
		.parent_names = disp_cc_parent_names_1,
		.num_parents = ARRAY_SIZE(disp_cc_parent_names_1),
		.flags = CLK_GET_RATE_NOCACHE,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_disp_cc_mdss_dp_link_clk_src[] = {
	F(162000, P_DP_PHY_PLL_LINK_CLK, 1, 0, 0),
	F(270000, P_DP_PHY_PLL_LINK_CLK, 1, 0, 0),
	F(540000, P_DP_PHY_PLL_LINK_CLK, 1, 0, 0),
	F(810000, P_DP_PHY_PLL_LINK_CLK, 1, 0, 0),
	{ }
};

static struct clk_rcg2 disp_cc_mdss_dp_link_clk_src = {
	.cmd_rcgr = 0x2138,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_1,
	.freq_tbl = ftbl_disp_cc_mdss_dp_link_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_dp_link_clk_src",
		.parent_names = disp_cc_parent_names_1,
		.num_parents = ARRAY_SIZE(disp_cc_parent_names_1),
		.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_regmap_div disp_cc_mdss_dp_link_div_clk_src = {
	.reg = 0x2150,
	.shift = 0,
	.width = 2,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_dp_link_div_clk_src",
		.parent_names = (const char *[]){
			"disp_cc_mdss_dp_link_clk_src",
		},
		.num_parents = 1,
		.flags = CLK_GET_RATE_NOCACHE,
		.ops = &clk_regmap_div_ro_ops,
	},
};

static struct clk_rcg2 disp_cc_mdss_dp_pixel1_clk_src = {
	.cmd_rcgr = 0x2184,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_1,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_dp_pixel1_clk_src",
		.parent_names = disp_cc_parent_names_1,
		.num_parents = ARRAY_SIZE(disp_cc_parent_names_1),
		.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
		.ops = &clk_dp_ops,
	},
};

static struct clk_rcg2 disp_cc_mdss_dp_pixel_clk_src = {
	.cmd_rcgr = 0x216c,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_1,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_dp_pixel_clk_src",
		.parent_names = disp_cc_parent_names_1,
		.num_parents = ARRAY_SIZE(disp_cc_parent_names_1),
		.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
		.ops = &clk_dp_ops,
	},
};

static struct clk_rcg2 disp_cc_mdss_esc0_clk_src = {
	.cmd_rcgr = 0x2108,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_0,
	.freq_tbl = ftbl_disp_cc_mdss_esc0_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_esc0_clk_src",
		.parent_names = disp_cc_parent_names_0,
		.num_parents = 4,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 disp_cc_mdss_esc1_clk_src = {
	.cmd_rcgr = 0x2120,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_0,
	.freq_tbl = ftbl_disp_cc_mdss_esc0_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_esc1_clk_src",
		.parent_names = disp_cc_parent_names_0,
		.num_parents = 4,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_disp_cc_mdss_mdp_clk_src[] = {
	F(19200000, P_BI_TCXO, 1, 0, 0),
	F(85714286, P_GPLL0_OUT_MAIN, 7, 0, 0),
	F(100000000, P_GPLL0_OUT_MAIN, 6, 0, 0),
	F(150000000, P_GPLL0_OUT_MAIN, 4, 0, 0),
	F(171428571, P_GPLL0_OUT_MAIN, 3.5, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN, 3, 0, 0),
	F(300000000, P_GPLL0_OUT_MAIN, 2, 0, 0),
	F(344000000, P_DISP_CC_PLL0_OUT_MAIN, 2.5, 0, 0),
	F(430000000, P_DISP_CC_PLL0_OUT_MAIN, 2, 0, 0),
	{ }
};

/* SDM670 uses the SDM845-v2 display clock layout with one additional
 * 286.666 MHz operating point.  Keep this table separate so the SDM845
 * variant retains its original rate contract while SDM670 consumers can
 * request the hardware-supported divider value. */
static const struct freq_tbl ftbl_disp_cc_mdss_mdp_clk_src_sdm670[] = {
	F(19200000, P_BI_TCXO, 1, 0, 0),
	F(85714286, P_GPLL0_OUT_MAIN, 7, 0, 0),
	F(100000000, P_GPLL0_OUT_MAIN, 6, 0, 0),
	F(150000000, P_GPLL0_OUT_MAIN, 4, 0, 0),
	F(171428571, P_GPLL0_OUT_MAIN, 3.5, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN, 3, 0, 0),
	F(286666667, P_DISP_CC_PLL0_OUT_MAIN, 3, 0, 0),
	F(300000000, P_GPLL0_OUT_MAIN, 2, 0, 0),
	F(344000000, P_DISP_CC_PLL0_OUT_MAIN, 2.5, 0, 0),
	F(430000000, P_DISP_CC_PLL0_OUT_MAIN, 2, 0, 0),
	{ }
};

static struct clk_rcg2 disp_cc_mdss_mdp_clk_src = {
	.cmd_rcgr = 0x2088,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_3,
	.freq_tbl = ftbl_disp_cc_mdss_mdp_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_mdp_clk_src",
		.parent_names = disp_cc_parent_names_3,
		.num_parents = 5,
		.ops = &clk_rcg2_shared_ops,
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_rcg2 disp_cc_mdss_pclk0_clk_src = {
	.cmd_rcgr = 0x2058,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_4,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_pclk0_clk_src",
		.parent_names = disp_cc_parent_names_4,
		.num_parents = 4,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_pixel_ops,
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_rcg2 disp_cc_mdss_pclk1_clk_src = {
	.cmd_rcgr = 0x2070,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_4,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_pclk1_clk_src",
		.parent_names = disp_cc_parent_names_4,
		.num_parents = 4,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_pixel_ops,
	},
};

static const struct freq_tbl ftbl_disp_cc_mdss_rot_clk_src[] = {
	F(19200000, P_BI_TCXO, 1, 0, 0),
	F(171428571, P_GPLL0_OUT_MAIN, 3.5, 0, 0),
	F(300000000, P_GPLL0_OUT_MAIN, 2, 0, 0),
	F(344000000, P_DISP_CC_PLL0_OUT_MAIN, 2.5, 0, 0),
	F(430000000, P_DISP_CC_PLL0_OUT_MAIN, 2, 0, 0),
	{ }
};

static struct clk_rcg2 disp_cc_mdss_rot_clk_src = {
	.cmd_rcgr = 0x20a0,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_3,
	.freq_tbl = ftbl_disp_cc_mdss_rot_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_rot_clk_src",
		.parent_names = disp_cc_parent_names_3,
		.num_parents = 5,
		.ops = &clk_rcg2_shared_ops,
	},
};

static struct clk_rcg2 disp_cc_mdss_vsync_clk_src = {
	.cmd_rcgr = 0x20b8,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = disp_cc_parent_map_2,
	.freq_tbl = ftbl_disp_cc_mdss_esc0_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "disp_cc_mdss_vsync_clk_src",
		.parent_names = disp_cc_parent_names_2,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_branch disp_cc_mdss_ahb_clk = {
	.halt_reg = 0x4004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x4004,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_ahb_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_axi_clk = {
	.halt_reg = 0x4008,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x4008,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_axi_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_branch disp_cc_mdss_byte0_clk = {
	.halt_reg = 0x2028,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2028,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_byte0_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_byte0_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_regmap_div disp_cc_mdss_byte0_div_clk_src = {
	.reg = 0x20e8,
	.shift = 0,
	.width = 2,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_byte0_div_clk_src",
			.parent_names = (const char *[]){
				"disp_cc_mdss_byte0_clk_src",
			},
			.num_parents = 1,
			.ops = &clk_regmap_div_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_branch disp_cc_mdss_byte0_intf_clk = {
	.halt_reg = 0x202c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x202c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_byte0_intf_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_byte0_div_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_branch disp_cc_mdss_byte1_clk = {
	.halt_reg = 0x2030,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2030,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_byte1_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_byte1_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_regmap_div disp_cc_mdss_byte1_div_clk_src = {
	.reg = 0x2104,
	.shift = 0,
	.width = 2,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_byte1_div_clk_src",
			.parent_names = (const char *[]){
				"disp_cc_mdss_byte1_clk_src",
			},
			.num_parents = 1,
			.ops = &clk_regmap_div_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_branch disp_cc_mdss_byte1_intf_clk = {
	.halt_reg = 0x2034,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2034,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_byte1_intf_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_byte1_div_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_dp_aux_clk = {
	.halt_reg = 0x2054,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2054,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_dp_aux_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_dp_aux_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_dp_crypto_clk = {
	.halt_reg = 0x2048,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2048,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_dp_crypto_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_dp_crypto_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_dp_link_clk = {
	.halt_reg = 0x2040,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2040,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_dp_link_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_dp_link_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_dp_link_intf_clk = {
	.halt_reg = 0x2044,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2044,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_dp_link_intf_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_dp_link_div_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_GET_RATE_NOCACHE,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_dp_pixel1_clk = {
	.halt_reg = 0x2050,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2050,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_dp_pixel1_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_dp_pixel1_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_dp_pixel_clk = {
	.halt_reg = 0x204c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x204c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_dp_pixel_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_dp_pixel_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_esc0_clk = {
	.halt_reg = 0x2038,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2038,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_esc0_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_esc0_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_esc1_clk = {
	.halt_reg = 0x203c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x203c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_esc1_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_esc1_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_mdp_clk = {
	.halt_reg = 0x200c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x200c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_mdp_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_mdp_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_mdp_lut_clk = {
	.halt_reg = 0x201c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x201c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_mdp_lut_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_mdp_clk_src",
			},
			.num_parents = 1,
			.ops = &clk_branch2_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_branch disp_cc_mdss_pclk0_clk = {
	.halt_reg = 0x2004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2004,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_pclk0_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_pclk0_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

/* Return the HW recalc rate for idle use case */
static struct clk_branch disp_cc_mdss_pclk1_clk = {
	.halt_reg = 0x2008,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2008,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_pclk1_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_pclk1_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_rot_clk = {
	.halt_reg = 0x2014,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2014,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_rot_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_rot_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_rscc_ahb_clk = {
	.halt_reg = 0x5004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x5004,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_rscc_ahb_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_rscc_vsync_clk = {
	.halt_reg = 0x5008,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x5008,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_rscc_vsync_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_vsync_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch disp_cc_mdss_vsync_clk = {
	.halt_reg = 0x2024,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2024,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "disp_cc_mdss_vsync_clk",
			.parent_names = (const char *[]){
				"disp_cc_mdss_vsync_clk_src",
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct gdsc mdss_gdsc = {
	.gdscr = 0x3000,
	.en_few_wait_val = 0x6,
	.en_rest_wait_val = 0x5,
	.pd = {
		.name = "mdss_gdsc",
	},
	.pwrsts = PWRSTS_OFF_ON,
	.flags = HW_CTRL | POLL_CFG_GDSCR,
};

static struct clk_regmap *disp_cc_sdm845_clocks[] = {
	[DISP_CC_MDSS_AHB_CLK] = &disp_cc_mdss_ahb_clk.clkr,
	[DISP_CC_MDSS_AXI_CLK] = &disp_cc_mdss_axi_clk.clkr,
	[DISP_CC_MDSS_BYTE0_CLK] = &disp_cc_mdss_byte0_clk.clkr,
	[DISP_CC_MDSS_BYTE0_CLK_SRC] = &disp_cc_mdss_byte0_clk_src.clkr,
	[DISP_CC_MDSS_BYTE0_INTF_CLK] = &disp_cc_mdss_byte0_intf_clk.clkr,
	[DISP_CC_MDSS_BYTE0_DIV_CLK_SRC] =
					&disp_cc_mdss_byte0_div_clk_src.clkr,
	[DISP_CC_MDSS_BYTE1_CLK] = &disp_cc_mdss_byte1_clk.clkr,
	[DISP_CC_MDSS_BYTE1_CLK_SRC] = &disp_cc_mdss_byte1_clk_src.clkr,
	[DISP_CC_MDSS_BYTE1_INTF_CLK] = &disp_cc_mdss_byte1_intf_clk.clkr,
	[DISP_CC_MDSS_BYTE1_DIV_CLK_SRC] =
					&disp_cc_mdss_byte1_div_clk_src.clkr,
	[DISP_CC_MDSS_DP_AUX_CLK] = &disp_cc_mdss_dp_aux_clk.clkr,
	[DISP_CC_MDSS_DP_AUX_CLK_SRC] = &disp_cc_mdss_dp_aux_clk_src.clkr,
	[DISP_CC_MDSS_DP_CRYPTO_CLK] = &disp_cc_mdss_dp_crypto_clk.clkr,
	[DISP_CC_MDSS_DP_CRYPTO_CLK_SRC] = &disp_cc_mdss_dp_crypto_clk_src.clkr,
	[DISP_CC_MDSS_DP_LINK_CLK] = &disp_cc_mdss_dp_link_clk.clkr,
	[DISP_CC_MDSS_DP_LINK_CLK_SRC] = &disp_cc_mdss_dp_link_clk_src.clkr,
	[DISP_CC_MDSS_DP_LINK_DIV_CLK_SRC] =
					&disp_cc_mdss_dp_link_div_clk_src.clkr,
	[DISP_CC_MDSS_DP_LINK_INTF_CLK] = &disp_cc_mdss_dp_link_intf_clk.clkr,
	[DISP_CC_MDSS_DP_PIXEL1_CLK] = &disp_cc_mdss_dp_pixel1_clk.clkr,
	[DISP_CC_MDSS_DP_PIXEL1_CLK_SRC] = &disp_cc_mdss_dp_pixel1_clk_src.clkr,
	[DISP_CC_MDSS_DP_PIXEL_CLK] = &disp_cc_mdss_dp_pixel_clk.clkr,
	[DISP_CC_MDSS_DP_PIXEL_CLK_SRC] = &disp_cc_mdss_dp_pixel_clk_src.clkr,
	[DISP_CC_MDSS_ESC0_CLK] = &disp_cc_mdss_esc0_clk.clkr,
	[DISP_CC_MDSS_ESC0_CLK_SRC] = &disp_cc_mdss_esc0_clk_src.clkr,
	[DISP_CC_MDSS_ESC1_CLK] = &disp_cc_mdss_esc1_clk.clkr,
	[DISP_CC_MDSS_ESC1_CLK_SRC] = &disp_cc_mdss_esc1_clk_src.clkr,
	[DISP_CC_MDSS_MDP_CLK] = &disp_cc_mdss_mdp_clk.clkr,
	[DISP_CC_MDSS_MDP_CLK_SRC] = &disp_cc_mdss_mdp_clk_src.clkr,
	[DISP_CC_MDSS_MDP_LUT_CLK] = &disp_cc_mdss_mdp_lut_clk.clkr,
	[DISP_CC_MDSS_PCLK0_CLK] = &disp_cc_mdss_pclk0_clk.clkr,
	[DISP_CC_MDSS_PCLK0_CLK_SRC] = &disp_cc_mdss_pclk0_clk_src.clkr,
	[DISP_CC_MDSS_PCLK1_CLK] = &disp_cc_mdss_pclk1_clk.clkr,
	[DISP_CC_MDSS_PCLK1_CLK_SRC] = &disp_cc_mdss_pclk1_clk_src.clkr,
	[DISP_CC_MDSS_ROT_CLK] = &disp_cc_mdss_rot_clk.clkr,
	[DISP_CC_MDSS_ROT_CLK_SRC] = &disp_cc_mdss_rot_clk_src.clkr,
	[DISP_CC_MDSS_RSCC_AHB_CLK] = &disp_cc_mdss_rscc_ahb_clk.clkr,
	[DISP_CC_MDSS_RSCC_VSYNC_CLK] = &disp_cc_mdss_rscc_vsync_clk.clkr,
	[DISP_CC_MDSS_VSYNC_CLK] = &disp_cc_mdss_vsync_clk.clkr,
	[DISP_CC_MDSS_VSYNC_CLK_SRC] = &disp_cc_mdss_vsync_clk_src.clkr,
	[DISP_CC_PLL0] = &disp_cc_pll0.clkr,
};

static const struct qcom_reset_map disp_cc_sdm845_resets[] = {
	[DISP_CC_MDSS_RSCC_BCR] = { 0x5000 },
};

static struct gdsc *disp_cc_sdm845_gdscs[] = {
	[MDSS_GDSC] = &mdss_gdsc,
};

static const struct regmap_config disp_cc_sdm845_regmap_config = {
	.reg_bits	= 32,
	.reg_stride	= 4,
	.val_bits	= 32,
	.max_register	= 0x10000,
	.fast_io	= true,
};

static const struct qcom_cc_desc disp_cc_sdm845_desc = {
	.config = &disp_cc_sdm845_regmap_config,
	.clks = disp_cc_sdm845_clocks,
	.num_clks = DISP_CC_MDSS_BYTE1_DIV_CLK_SRC + 1,
	.resets = disp_cc_sdm845_resets,
	.num_resets = ARRAY_SIZE(disp_cc_sdm845_resets),
	.gdscs = disp_cc_sdm845_gdscs,
	.num_gdscs = ARRAY_SIZE(disp_cc_sdm845_gdscs),
};

/* SDM670's MDSS GDSC is owned by the standalone regulator in the DTS. */
static const struct qcom_cc_desc disp_cc_sdm670_desc = {
	.config = &disp_cc_sdm845_regmap_config,
	.clks = disp_cc_sdm845_clocks,
	.num_clks = ARRAY_SIZE(disp_cc_sdm845_clocks),
	.resets = disp_cc_sdm845_resets,
	.num_resets = ARRAY_SIZE(disp_cc_sdm845_resets),
};

/* SDM670 limits include the SDM845-v2 and SDM670 silicon fixups. */
static struct {
	struct clk_regmap *clkr;
	struct clk_init_data init;
	unsigned long rate_max[VDD_NUM];
} disp_cc_sdm670_vdd[] = {
	{
		.clkr = &disp_cc_pll0.clkr,
		.rate_max = {
			[VDD_MIN] = 615000000,
			[VDD_LOW] = 1066000000,
			[VDD_LOW_L1] = 1600000000,
			[VDD_NOMINAL] = 2000000000,
		},
	},
	{
		.clkr = &disp_cc_mdss_byte0_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
			[VDD_LOWER] = 180000000,
			[VDD_LOW] = 275000000,
			[VDD_LOW_L1] = 358000000,
			[VDD_NOMINAL] = 358000000,
		},
	},
	{
		.clkr = &disp_cc_mdss_byte1_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
			[VDD_LOWER] = 180000000,
			[VDD_LOW] = 275000000,
			[VDD_LOW_L1] = 358000000,
			[VDD_NOMINAL] = 358000000,
		},
	},
	{
		.clkr = &disp_cc_mdss_dp_aux_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
		},
	},
	{
		.clkr = &disp_cc_mdss_dp_crypto_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 12800,
			[VDD_LOWER] = 108000,
			[VDD_LOW] = 180000,
			[VDD_LOW_L1] = 360000,
			[VDD_NOMINAL] = 540000,
		},
	},
	{
		.clkr = &disp_cc_mdss_dp_link_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200,
			[VDD_LOWER] = 162000,
			[VDD_LOW] = 270000,
			[VDD_LOW_L1] = 540000,
			[VDD_NOMINAL] = 810000,
		},
	},
	{
		.clkr = &disp_cc_mdss_dp_pixel1_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200,
			[VDD_LOWER] = 202500,
			[VDD_LOW] = 337500,
			[VDD_LOW_L1] = 675000,
		},
	},
	{
		.clkr = &disp_cc_mdss_dp_pixel_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200,
			[VDD_LOWER] = 202500,
			[VDD_LOW] = 337500,
			[VDD_LOW_L1] = 675000,
		},
	},
	{
		.clkr = &disp_cc_mdss_esc0_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
		},
	},
	{
		.clkr = &disp_cc_mdss_esc1_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
		},
	},
	{
		.clkr = &disp_cc_mdss_mdp_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
			[VDD_LOWER] = 171428571,
			[VDD_LOW] = 300000000,
			[VDD_LOW_L1] = 344000000,
			[VDD_NOMINAL] = 430000000,
		},
	},
	{
		.clkr = &disp_cc_mdss_pclk0_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
			[VDD_LOWER] = 280000000,
			[VDD_LOW] = 430000000,
			[VDD_LOW_L1] = 430000000,
			[VDD_NOMINAL] = 571428571,
		},
	},
	{
		.clkr = &disp_cc_mdss_pclk1_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
			[VDD_LOWER] = 280000000,
			[VDD_LOW] = 430000000,
			[VDD_LOW_L1] = 430000000,
			[VDD_NOMINAL] = 571428571,
		},
	},
	{
		.clkr = &disp_cc_mdss_rot_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
			[VDD_LOWER] = 171428571,
			[VDD_LOW] = 300000000,
			[VDD_LOW_L1] = 344000000,
			[VDD_NOMINAL] = 430000000,
		},
	},
	{
		.clkr = &disp_cc_mdss_vsync_clk_src.clkr,
		.rate_max = {
			[VDD_MIN] = 19200000,
		},
	},
};

static int disp_cc_sdm670_fixup(struct device *dev)
{
	int i;

	vdd_cx.regulator[0] = devm_regulator_get(dev, "vdd_cx");
	if (IS_ERR(vdd_cx.regulator[0])) {
		if (PTR_ERR(vdd_cx.regulator[0]) != -EPROBE_DEFER)
			dev_err(dev, "Unable to get vdd_cx regulator\n");
		return PTR_ERR(vdd_cx.regulator[0]);
	}

	for (i = 0; i < ARRAY_SIZE(disp_cc_sdm670_vdd); i++) {
		struct clk_init_data *init = &disp_cc_sdm670_vdd[i].init;

		*init = *disp_cc_sdm670_vdd[i].clkr->hw.init;
		init->vdd_class = &vdd_cx;
		init->rate_max = disp_cc_sdm670_vdd[i].rate_max;
		init->num_rate_max = VDD_NUM;
		disp_cc_sdm670_vdd[i].clkr->hw.init = init;
	}

	disp_cc_mdss_mdp_clk_src.freq_tbl =
		ftbl_disp_cc_mdss_mdp_clk_src_sdm670;

	return 0;
}

static const struct of_device_id disp_cc_sdm845_match_table[] = {
	{ .compatible = "qcom,sdm845-dispcc" },
	{ .compatible = "qcom,sdm670-dispcc" },
	{ }
};
MODULE_DEVICE_TABLE(of, disp_cc_sdm845_match_table);

static int disp_cc_sdm845_probe(struct platform_device *pdev)
{
	const struct qcom_cc_desc *desc = &disp_cc_sdm845_desc;
	struct regmap *regmap;
	int ret;

	if (of_device_is_compatible(pdev->dev.of_node, "qcom,sdm670-dispcc"))
		desc = &disp_cc_sdm670_desc;

	regmap = qcom_cc_map(pdev, desc);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	if (desc == &disp_cc_sdm670_desc) {
		ret = disp_cc_sdm670_fixup(&pdev->dev);
		if (ret)
			return ret;
	}

	ret = clk_fabia_pll_configure(&disp_cc_pll0, regmap, &disp_cc_pll0_config);
	if (ret)
		return ret;

	/* Enable hardware clock gating for DSI and MDP clocks */
	regmap_update_bits(regmap, 0x8000, 0x7f0, 0x7f0);

	return qcom_cc_really_probe(pdev, desc, regmap);
}

static struct platform_driver disp_cc_sdm845_driver = {
	.probe		= disp_cc_sdm845_probe,
	.driver		= {
		.name	= "disp_cc-sdm845",
		.of_match_table = disp_cc_sdm845_match_table,
	},
};

static int __init disp_cc_sdm845_init(void)
{
	return platform_driver_register(&disp_cc_sdm845_driver);
}
subsys_initcall(disp_cc_sdm845_init);

static void __exit disp_cc_sdm845_exit(void)
{
	platform_driver_unregister(&disp_cc_sdm845_driver);
}
module_exit(disp_cc_sdm845_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("QTI DISPCC SDM845 Driver");
