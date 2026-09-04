// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2017-2020, The Linux Foundation. All rights reserved. */

#include <linux/bitops.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include <dt-bindings/clock/qcom,gpucc-sdm670.h>

#include "clk-alpha-pll.h"
#include "clk-branch.h"
#include "clk-rcg.h"
#include "common.h"
#include "reset.h"
#include "vdd-level.h"

static DEFINE_VDD_REGULATORS(vdd_cx, VDD_NUM, 1, vdd_corner);
static DEFINE_VDD_REGULATORS(vdd_mx, VDD_NUM, 1, vdd_corner);
static DEFINE_VDD_REGULATORS(vdd_gfx, VDD_NUM, 1, vdd_corner);

enum {
	P_BI_TCXO,
	P_CORE_BI_PLL_TEST_SE,
	P_GPLL0_OUT_MAIN,
	P_GPLL0_OUT_MAIN_DIV,
	P_GPU_CC_PLL0_OUT_MAIN,
	P_GPU_CC_PLL0_OUT_ODD,
	P_GPU_CC_PLL1_OUT_EVEN,
	P_GPU_CC_PLL1_OUT_MAIN,
	P_GPU_CC_PLL1_OUT_ODD,
	P_CRC_DIV,
};

static const struct parent_map gpu_cc_parent_map_0[] = {
	{ P_BI_TCXO, 0 },
	{ P_GPU_CC_PLL0_OUT_MAIN, 1 },
	{ P_GPU_CC_PLL1_OUT_MAIN, 3 },
	{ P_GPLL0_OUT_MAIN, 5 },
	{ P_GPLL0_OUT_MAIN_DIV, 6 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const gpu_cc_parent_names_0[] = {
	"bi_tcxo",
	"gpu_cc_pll0",
	"gpu_cc_pll1",
	"gcc_gpu_gpll0_clk_src",
	"gcc_gpu_gpll0_div_clk_src",
	"core_bi_pll_test_se",
};

static const struct parent_map gpu_cc_parent_map_1[] = {
	{ P_BI_TCXO, 0 },
	{ P_CRC_DIV, 1 },
	{ P_GPU_CC_PLL0_OUT_ODD, 2 },
	{ P_GPU_CC_PLL1_OUT_EVEN, 3 },
	{ P_GPU_CC_PLL1_OUT_ODD, 4 },
	{ P_GPLL0_OUT_MAIN, 5 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const char * const gpu_cc_parent_names_1[] = {
	"bi_tcxo",
	"crc_div",
	"gpu_cc_pll0_out_odd",
	"gpu_cc_pll1_out_even",
	"gpu_cc_pll1_out_odd",
	"gcc_gpu_gpll0_clk_src",
	"core_bi_pll_test_se",
};

static struct pll_vco fabia_vco[] = {
	{ 249600000, 2000000000, 0 },
	{ 125000000, 1000000000, 1 },
};

static struct alpha_pll_config gpu_cc_pll0_config = {
	.l = 0x1d,
	.alpha = 0x2aaa,
};

static struct alpha_pll_config gpu_cc_pll1_config = {
	.l = 0x1a,
	.alpha = 0xaaaa,
};

static struct clk_alpha_pll gpu_cc_pll0 = {
	.offset = 0x0,
	.config = &gpu_cc_pll0_config,
	.vco_table = fabia_vco,
	.num_vco = ARRAY_SIZE(fabia_vco),
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_FABIA],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpu_cc_pll0",
		.parent_names = (const char *[]){ "bi_tcxo" },
		.num_parents = 1,
		.ops = &clk_alpha_pll_fabia_ops,
		.vdd_class = &vdd_mx,
		.num_rate_max = VDD_NUM,
		.rate_max = (unsigned long[VDD_NUM]) {
			[VDD_MIN] = 615000000,
			[VDD_LOW] = 1066000000,
			[VDD_LOW_L1] = 1600000000,
			[VDD_NOMINAL] = 2000000000,
		},
	},
};

static struct clk_alpha_pll gpu_cc_pll1 = {
	.offset = 0x100,
	.config = &gpu_cc_pll1_config,
	.vco_table = fabia_vco,
	.num_vco = ARRAY_SIZE(fabia_vco),
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_FABIA],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpu_cc_pll1",
		.parent_names = (const char *[]){ "bi_tcxo" },
		.num_parents = 1,
		.ops = &clk_alpha_pll_fabia_ops,
		.vdd_class = &vdd_mx,
		.num_rate_max = VDD_NUM,
		.rate_max = (unsigned long[VDD_NUM]) {
			[VDD_MIN] = 615000000,
			[VDD_LOW] = 1066000000,
			[VDD_LOW_L1] = 1600000000,
			[VDD_NOMINAL] = 2000000000,
		},
	},
};

static const struct clk_div_table post_div_table_fabia_even[] = {
	{ 0x0, 1 },
	{ 0x1, 2 },
	{ 0x3, 4 },
	{ 0x7, 8 },
	{ }
};

static struct clk_alpha_pll_postdiv gpu_cc_pll0_out_even = {
	.offset = 0x0,
	.post_div_shift = 8,
	.post_div_table = post_div_table_fabia_even,
	.num_post_div = ARRAY_SIZE(post_div_table_fabia_even),
	.width = 4,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_FABIA],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpu_cc_pll0_out_even",
		.parent_names = (const char *[]){ "gpu_cc_pll0" },
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_alpha_pll_postdiv_fabia_ops,
	},
};

static const struct freq_tbl ftbl_gpu_cc_gmu_clk_src[] = {
	F(19200000, P_BI_TCXO, 1, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN_DIV, 1.5, 0, 0),
	{ }
};

static struct clk_rcg2 gpu_cc_gmu_clk_src = {
	.cmd_rcgr = 0x1120,
	.hid_width = 5,
	.enable_safe_config = true,
	.parent_map = gpu_cc_parent_map_0,
	.freq_tbl = ftbl_gpu_cc_gmu_clk_src,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpu_cc_gmu_clk_src",
		.parent_names = gpu_cc_parent_names_0,
		.num_parents = ARRAY_SIZE(gpu_cc_parent_names_0),
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_rcg2_ops,
		.vdd_class = &vdd_cx,
		.num_rate_max = VDD_NUM,
		.rate_max = (unsigned long[VDD_NUM]) {
			[VDD_MIN] = 200000000,
		},
	},
};

static struct clk_fixed_factor crc_div = {
	.mult = 1,
	.div = 1,
	.hw.init = &(struct clk_init_data){
		.name = "crc_div",
		.parent_names = (const char *[]){ "gpu_cc_pll0_out_even" },
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_fixed_factor_ops,
	},
};

static const struct freq_tbl ftbl_gpu_cc_gx_gfx3d_clk_src[] = {
	F(180000000, P_CRC_DIV, 1, 0, 0),
	F(267000000, P_CRC_DIV, 1, 0, 0),
	F(355000000, P_CRC_DIV, 1, 0, 0),
	F(430000000, P_CRC_DIV, 1, 0, 0),
	F(504000000, P_CRC_DIV, 1, 0, 0),
	F(565000000, P_CRC_DIV, 1, 0, 0),
	F(610000000, P_CRC_DIV, 1, 0, 0),
	F(650000000, P_CRC_DIV, 1, 0, 0),
	F(700000000, P_CRC_DIV, 1, 0, 0),
	F(750000000, P_CRC_DIV, 1, 0, 0),
	F(780000000, P_CRC_DIV, 1, 0, 0),
	{ }
};

static struct clk_rcg2 gpu_cc_gx_gfx3d_clk_src = {
	.cmd_rcgr = 0x101c,
	.hid_width = 5,
	.parent_map = gpu_cc_parent_map_1,
	.freq_tbl = ftbl_gpu_cc_gx_gfx3d_clk_src,
	.flags = FORCE_ENABLE_RCG,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpu_cc_gx_gfx3d_clk_src",
		.parent_names = gpu_cc_parent_names_1,
		.num_parents = ARRAY_SIZE(gpu_cc_parent_names_1),
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_rcg2_ops,
		.vdd_class = &vdd_gfx,
		.num_rate_max = VDD_NUM,
		.rate_max = (unsigned long[VDD_NUM]) {
			[VDD_MIN] = 180000000,
			[VDD_LOWER] = 267000000,
			[VDD_LOW] = 355000000,
			[VDD_LOW_L1] = 430000000,
			[VDD_NOMINAL] = 565000000,
			[VDD_NOMINAL_L1] = 650000000,
			[VDD_HIGH] = 750000000,
			[VDD_HIGH_L1] = 780000000,
		},
	},
};

static struct clk_branch gpu_cc_acd_ahb_clk = {
	.halt_reg = 0x1168,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1168,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_acd_ahb_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_acd_cxo_clk = {
	.halt_reg = 0x1164,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1164,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_acd_cxo_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_crc_ahb_clk = {
	.halt_reg = 0x107c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x107c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_crc_ahb_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cx_apb_clk = {
	.halt_reg = 0x1088,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1088,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cx_apb_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cx_gfx3d_clk = {
	.halt_reg = 0x10a4,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x10a4,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cx_gfx3d_clk",
			.parent_names = (const char *[]){ "gpu_cc_gx_gfx3d_clk_src" },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cx_gfx3d_slv_clk = {
	.halt_reg = 0x10a8,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x10a8,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cx_gfx3d_slv_clk",
			.parent_names = (const char *[]){ "gpu_cc_gx_gfx3d_clk_src" },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cx_gmu_clk = {
	.halt_reg = 0x1098,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1098,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cx_gmu_clk",
			.parent_names = (const char *[]){ "gpu_cc_gmu_clk_src" },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cx_snoc_dvm_clk = {
	.halt_reg = 0x108c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x108c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cx_snoc_dvm_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cxo_aon_clk = {
	.halt_reg = 0x1004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1004,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cxo_aon_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_cxo_clk = {
	.halt_reg = 0x109c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x109c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_cxo_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_gx_gfx3d_clk = {
	.halt_reg = 0x1054,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1054,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_gx_gfx3d_clk",
			.parent_names = (const char *[]){ "gpu_cc_gx_gfx3d_clk_src" },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_gx_gmu_clk = {
	.halt_reg = 0x1064,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1064,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_gx_gmu_clk",
			.parent_names = (const char *[]){ "gpu_cc_gmu_clk_src" },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_gx_vsense_clk = {
	.halt_reg = 0x1058,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1058,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_gx_vsense_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gpu_cc_pll_test_clk = {
	.halt_reg = 0x110c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x110c,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "gpu_cc_pll_test_clk",
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_regmap *gpu_cc_sdm670_clocks[] = {
	[GPU_CC_ACD_AHB_CLK] = &gpu_cc_acd_ahb_clk.clkr,
	[GPU_CC_ACD_CXO_CLK] = &gpu_cc_acd_cxo_clk.clkr,
	[GPU_CC_CRC_AHB_CLK] = &gpu_cc_crc_ahb_clk.clkr,
	[GPU_CC_CX_APB_CLK] = &gpu_cc_cx_apb_clk.clkr,
	[GPU_CC_CX_GFX3D_CLK] = &gpu_cc_cx_gfx3d_clk.clkr,
	[GPU_CC_CX_GFX3D_SLV_CLK] = &gpu_cc_cx_gfx3d_slv_clk.clkr,
	[GPU_CC_CX_GMU_CLK] = &gpu_cc_cx_gmu_clk.clkr,
	[GPU_CC_CX_SNOC_DVM_CLK] = &gpu_cc_cx_snoc_dvm_clk.clkr,
	[GPU_CC_CXO_AON_CLK] = &gpu_cc_cxo_aon_clk.clkr,
	[GPU_CC_CXO_CLK] = &gpu_cc_cxo_clk.clkr,
	[GPU_CC_GMU_CLK_SRC] = &gpu_cc_gmu_clk_src.clkr,
	[GPU_CC_GX_GMU_CLK] = &gpu_cc_gx_gmu_clk.clkr,
	[GPU_CC_GX_VSENSE_CLK] = &gpu_cc_gx_vsense_clk.clkr,
	[GPU_CC_PLL_TEST_CLK] = &gpu_cc_pll_test_clk.clkr,
	[GPU_CC_PLL0] = &gpu_cc_pll0.clkr,
	[GPU_CC_PLL1] = &gpu_cc_pll1.clkr,
	[GPU_CC_PLL0_OUT_EVEN] = &gpu_cc_pll0_out_even.clkr,
	[GPU_CC_GX_GFX3D_CLK_SRC] = &gpu_cc_gx_gfx3d_clk_src.clkr,
	[GPU_CC_GX_GFX3D_CLK] = &gpu_cc_gx_gfx3d_clk.clkr,
};

static const struct qcom_reset_map gpu_cc_sdm670_resets[] = {
	[GPUCC_GPU_CC_ACD_BCR] = { 0x1160 },
	[GPUCC_GPU_CC_CX_BCR] = { 0x1068 },
	[GPUCC_GPU_CC_GFX3D_AON_BCR] = { 0x10a0 },
	[GPUCC_GPU_CC_GMU_BCR] = { 0x111c },
	[GPUCC_GPU_CC_GX_BCR] = { 0x1008 },
	[GPUCC_GPU_CC_SPDM_BCR] = { 0x1110 },
	[GPUCC_GPU_CC_XO_BCR] = { 0x1000 },
};

static const struct regmap_config gpu_cc_sdm670_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.max_register = 0x8008,
	.fast_io = true,
};

static const struct qcom_cc_desc gpu_cc_sdm670_desc = {
	.config = &gpu_cc_sdm670_regmap_config,
	.clks = gpu_cc_sdm670_clocks,
	.num_clks = ARRAY_SIZE(gpu_cc_sdm670_clocks),
	.resets = gpu_cc_sdm670_resets,
	.num_resets = ARRAY_SIZE(gpu_cc_sdm670_resets),
};

static int gpu_cc_sdm670_probe(struct platform_device *pdev)
{
	struct regmap *regmap;
	int ret;

	regmap = qcom_cc_map(pdev, &gpu_cc_sdm670_desc);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	vdd_cx.regulator[0] = devm_regulator_get(&pdev->dev, "vdd_cx");
	if (IS_ERR(vdd_cx.regulator[0]))
		return PTR_ERR(vdd_cx.regulator[0]);

	vdd_mx.regulator[0] = devm_regulator_get(&pdev->dev, "vdd_mx");
	if (IS_ERR(vdd_mx.regulator[0]))
		return PTR_ERR(vdd_mx.regulator[0]);

	vdd_gfx.regulator[0] = devm_regulator_get(&pdev->dev, "vdd_gfx");
	if (IS_ERR(vdd_gfx.regulator[0]))
		return PTR_ERR(vdd_gfx.regulator[0]);

	/* GX is powered by KGSL; clock registration must not turn it on. */
	vdd_gfx.skip_handoff = true;

	ret = clk_fabia_pll_configure(&gpu_cc_pll0, regmap,
				    &gpu_cc_pll0_config);
	if (ret)
		return ret;
	ret = clk_fabia_pll_configure(&gpu_cc_pll1, regmap,
				    &gpu_cc_pll1_config);
	if (ret)
		return ret;

	ret = regmap_update_bits(regmap, gpu_cc_cx_gmu_clk.clkr.enable_reg,
				 GENMASK(11, 4), GENMASK(11, 4));
	if (ret)
		return ret;

	ret = devm_clk_hw_register(&pdev->dev, &crc_div.hw);
	if (ret)
		return ret;

	return qcom_cc_really_probe(pdev, &gpu_cc_sdm670_desc, regmap);
}

static const struct of_device_id gpu_cc_sdm670_match_table[] = {
	{ .compatible = "qcom,gpucc-sdm670" },
	{ }
};
MODULE_DEVICE_TABLE(of, gpu_cc_sdm670_match_table);

static struct platform_driver gpu_cc_sdm670_driver = {
	.probe = gpu_cc_sdm670_probe,
	.driver = {
		.name = "gpucc-sdm670",
		.of_match_table = gpu_cc_sdm670_match_table,
	},
};

static int __init gpu_cc_sdm670_init(void)
{
	return platform_driver_register(&gpu_cc_sdm670_driver);
}
subsys_initcall(gpu_cc_sdm670_init);

static void __exit gpu_cc_sdm670_exit(void)
{
	platform_driver_unregister(&gpu_cc_sdm670_driver);
}
module_exit(gpu_cc_sdm670_exit);

MODULE_DESCRIPTION("Qualcomm SDM670/SDM710 GPU Clock Controller");
MODULE_LICENSE("GPL v2");
