// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2017-2019, The Linux Foundation. All rights reserved. */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/msm-bus.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#include <dt-bindings/msm/msm-bus-ids.h>

#include "clk-debug.h"
#include "common.h"

#define MSM_BUS_VECTOR(_src, _dst, _ab, _ib)	\
{						\
	.src = _src,				\
	.dst = _dst,				\
	.ab = _ab,				\
	.ib = _ib,				\
}

static struct msm_bus_vectors clk_measure_vectors[] = {
	MSM_BUS_VECTOR(MSM_BUS_MASTER_AMPSS_M0,
			MSM_BUS_SLAVE_CAMERA_CFG, 0, 0),
	MSM_BUS_VECTOR(MSM_BUS_MASTER_AMPSS_M0,
			MSM_BUS_SLAVE_VENUS_CFG, 0, 0),
	MSM_BUS_VECTOR(MSM_BUS_MASTER_AMPSS_M0,
			MSM_BUS_SLAVE_DISPLAY_CFG, 0, 0),
	MSM_BUS_VECTOR(MSM_BUS_MASTER_AMPSS_M0,
			MSM_BUS_SLAVE_CAMERA_CFG, 0, 1),
	MSM_BUS_VECTOR(MSM_BUS_MASTER_AMPSS_M0,
			MSM_BUS_SLAVE_VENUS_CFG, 0, 1),
	MSM_BUS_VECTOR(MSM_BUS_MASTER_AMPSS_M0,
			MSM_BUS_SLAVE_DISPLAY_CFG, 0, 1),
};

static struct msm_bus_paths clk_measure_usecases[] = {
	{
		.num_paths = 3,
		.vectors = &clk_measure_vectors[0],
	},
	{
		.num_paths = 3,
		.vectors = &clk_measure_vectors[3],
	}
};

static struct msm_bus_scale_pdata clk_measure_scale_table = {
	.usecase = clk_measure_usecases,
	.num_usecases = ARRAY_SIZE(clk_measure_usecases),
	.name = "clk_measure",
};

static struct measure_clk_data debug_mux_priv = {
	.ctl_reg = 0x62024,
	.status_reg = 0x62028,
	.xo_div4_cbcr = 0x43008,
};

/* Keep only SDM670 clocks with a provider and a known hardware selector. */
static const char *const cpu_cc_debug_mux_parent_names[] = {
	"l3_clk",
	"pwrcl_clk",
	"perfcl_clk",
};

static int cpu_cc_debug_mux_sels[] = {
	0x46,	/* l3_clk */
	0x44,	/* pwrcl_clk */
	0x45,	/* perfcl_clk */
};

static int cpu_cc_debug_mux_pre_divs[] = {
	16,	/* l3_clk */
	16,	/* pwrcl_clk */
	16,	/* perfcl_clk */
};

static struct clk_debug_mux cpu_cc_debug_mux = {
	.priv = &debug_mux_priv,
	.debug_offset = 0x0,
	.post_div_offset = 0x0,
	.cbcr_offset = U32_MAX,
	.src_sel_mask = 0x7F0,
	.src_sel_shift = 4,
	.post_div_mask = 0x7800,
	.post_div_shift = 11,
	.post_div_val = 1,
	.mux_sels = cpu_cc_debug_mux_sels,
	.pre_div_vals = cpu_cc_debug_mux_pre_divs,
	.hw.init = &(struct clk_init_data){
		.name = "cpu_cc_debug_mux",
		.ops = &clk_debug_mux_ops,
		.parent_names = cpu_cc_debug_mux_parent_names,
		.num_parents = ARRAY_SIZE(cpu_cc_debug_mux_parent_names),
		.flags = CLK_IS_MEASURE,
	},
};

static const char *const cam_cc_debug_mux_parent_names[] = {
	"cam_cc_bps_ahb_clk",
	"cam_cc_bps_areg_clk",
	"cam_cc_bps_axi_clk",
	"cam_cc_bps_clk",
	"cam_cc_camnoc_atb_clk",
	"cam_cc_camnoc_axi_clk",
	"cam_cc_cci_clk",
	"cam_cc_cpas_ahb_clk",
	"cam_cc_csi0phytimer_clk",
	"cam_cc_csi1phytimer_clk",
	"cam_cc_csi2phytimer_clk",
	"cam_cc_csiphy0_clk",
	"cam_cc_csiphy1_clk",
	"cam_cc_csiphy2_clk",
	"cam_cc_csiphy3_clk",
	"cam_cc_fd_core_clk",
	"cam_cc_fd_core_uar_clk",
	"cam_cc_icp_apb_clk",
	"cam_cc_icp_atb_clk",
	"cam_cc_icp_clk",
	"cam_cc_icp_cti_clk",
	"cam_cc_icp_ts_clk",
	"cam_cc_ife_0_axi_clk",
	"cam_cc_ife_0_clk",
	"cam_cc_ife_0_cphy_rx_clk",
	"cam_cc_ife_0_csid_clk",
	"cam_cc_ife_0_dsp_clk",
	"cam_cc_ife_1_axi_clk",
	"cam_cc_ife_1_clk",
	"cam_cc_ife_1_cphy_rx_clk",
	"cam_cc_ife_1_csid_clk",
	"cam_cc_ife_1_dsp_clk",
	"cam_cc_ife_lite_clk",
	"cam_cc_ife_lite_cphy_rx_clk",
	"cam_cc_ife_lite_csid_clk",
	"cam_cc_ipe_0_ahb_clk",
	"cam_cc_ipe_0_areg_clk",
	"cam_cc_ipe_0_axi_clk",
	"cam_cc_ipe_0_clk",
	"cam_cc_ipe_1_ahb_clk",
	"cam_cc_ipe_1_areg_clk",
	"cam_cc_ipe_1_axi_clk",
	"cam_cc_ipe_1_clk",
	"cam_cc_jpeg_clk",
	"cam_cc_lrme_clk",
	"cam_cc_mclk0_clk",
	"cam_cc_mclk1_clk",
	"cam_cc_mclk2_clk",
	"cam_cc_mclk3_clk",
	"cam_cc_soc_ahb_clk",
	"cam_cc_sys_tmr_clk",
};

static int cam_cc_debug_mux_sels[] = {
	0xE,	/* cam_cc_bps_ahb_clk */
	0xD,	/* cam_cc_bps_areg_clk */
	0xC,	/* cam_cc_bps_axi_clk */
	0xB,	/* cam_cc_bps_clk */
	0x34,	/* cam_cc_camnoc_atb_clk */
	0x2D,	/* cam_cc_camnoc_axi_clk */
	0x2A,	/* cam_cc_cci_clk */
	0x2C,	/* cam_cc_cpas_ahb_clk */
	0x5,	/* cam_cc_csi0phytimer_clk */
	0x7,	/* cam_cc_csi1phytimer_clk */
	0x9,	/* cam_cc_csi2phytimer_clk */
	0x6,	/* cam_cc_csiphy0_clk */
	0x8,	/* cam_cc_csiphy1_clk */
	0xA,	/* cam_cc_csiphy2_clk */
	0x36,	/* cam_cc_csiphy3_clk */
	0x28,	/* cam_cc_fd_core_clk */
	0x29,	/* cam_cc_fd_core_uar_clk */
	0x32,	/* cam_cc_icp_apb_clk */
	0x2F,	/* cam_cc_icp_atb_clk */
	0x26,	/* cam_cc_icp_clk */
	0x30,	/* cam_cc_icp_cti_clk */
	0x31,	/* cam_cc_icp_ts_clk */
	0x1B,	/* cam_cc_ife_0_axi_clk */
	0x17,	/* cam_cc_ife_0_clk */
	0x1A,	/* cam_cc_ife_0_cphy_rx_clk */
	0x19,	/* cam_cc_ife_0_csid_clk */
	0x18,	/* cam_cc_ife_0_dsp_clk */
	0x21,	/* cam_cc_ife_1_axi_clk */
	0x1D,	/* cam_cc_ife_1_clk */
	0x20,	/* cam_cc_ife_1_cphy_rx_clk */
	0x1F,	/* cam_cc_ife_1_csid_clk */
	0x1E,	/* cam_cc_ife_1_dsp_clk */
	0x22,	/* cam_cc_ife_lite_clk */
	0x24,	/* cam_cc_ife_lite_cphy_rx_clk */
	0x23,	/* cam_cc_ife_lite_csid_clk */
	0x12,	/* cam_cc_ipe_0_ahb_clk */
	0x11,	/* cam_cc_ipe_0_areg_clk */
	0x10,	/* cam_cc_ipe_0_axi_clk */
	0xF,	/* cam_cc_ipe_0_clk */
	0x16,	/* cam_cc_ipe_1_ahb_clk */
	0x15,	/* cam_cc_ipe_1_areg_clk */
	0x14,	/* cam_cc_ipe_1_axi_clk */
	0x13,	/* cam_cc_ipe_1_clk */
	0x25,	/* cam_cc_jpeg_clk */
	0x2B,	/* cam_cc_lrme_clk */
	0x1,	/* cam_cc_mclk0_clk */
	0x2,	/* cam_cc_mclk1_clk */
	0x3,	/* cam_cc_mclk2_clk */
	0x4,	/* cam_cc_mclk3_clk */
	0x2E,	/* cam_cc_soc_ahb_clk */
	0x33,	/* cam_cc_sys_tmr_clk */
};

static struct clk_debug_mux cam_cc_debug_mux = {
	.priv = &debug_mux_priv,
	.debug_offset = 0xC000,
	.post_div_offset = 0xC004,
	.cbcr_offset = 0xC008,
	.src_sel_mask = 0xFF,
	.src_sel_shift = 0,
	.post_div_mask = 0x3,
	.post_div_shift = 0,
	.post_div_val = 1,
	.mux_sels = cam_cc_debug_mux_sels,
	.hw.init = &(struct clk_init_data){
		.name = "cam_cc_debug_mux",
		.ops = &clk_debug_mux_ops,
		.parent_names = cam_cc_debug_mux_parent_names,
		.num_parents = ARRAY_SIZE(cam_cc_debug_mux_parent_names),
		.flags = CLK_IS_MEASURE,
	},
};

static const char *const disp_cc_debug_mux_parent_names[] = {
	"disp_cc_mdss_ahb_clk",
	"disp_cc_mdss_axi_clk",
	"disp_cc_mdss_byte0_clk",
	"disp_cc_mdss_byte0_intf_clk",
	"disp_cc_mdss_byte1_clk",
	"disp_cc_mdss_byte1_intf_clk",
	"disp_cc_mdss_esc0_clk",
	"disp_cc_mdss_esc1_clk",
	"disp_cc_mdss_mdp_clk",
	"disp_cc_mdss_mdp_lut_clk",
	"disp_cc_mdss_pclk0_clk",
	"disp_cc_mdss_pclk1_clk",
	"disp_cc_mdss_rot_clk",
	"disp_cc_mdss_rscc_ahb_clk",
	"disp_cc_mdss_rscc_vsync_clk",
	"disp_cc_mdss_vsync_clk",
};

static int disp_cc_debug_mux_sels[] = {
	0x13,	/* disp_cc_mdss_ahb_clk */
	0x14,	/* disp_cc_mdss_axi_clk */
	0x7,	/* disp_cc_mdss_byte0_clk */
	0x8,	/* disp_cc_mdss_byte0_intf_clk */
	0x9,	/* disp_cc_mdss_byte1_clk */
	0xA,	/* disp_cc_mdss_byte1_intf_clk */
	0xB,	/* disp_cc_mdss_esc0_clk */
	0xC,	/* disp_cc_mdss_esc1_clk */
	0x3,	/* disp_cc_mdss_mdp_clk */
	0x5,	/* disp_cc_mdss_mdp_lut_clk */
	0x1,	/* disp_cc_mdss_pclk0_clk */
	0x2,	/* disp_cc_mdss_pclk1_clk */
	0x4,	/* disp_cc_mdss_rot_clk */
	0x17,	/* disp_cc_mdss_rscc_ahb_clk */
	0x18,	/* disp_cc_mdss_rscc_vsync_clk */
	0x6,	/* disp_cc_mdss_vsync_clk */
};

static struct clk_debug_mux disp_cc_debug_mux = {
	.priv = &debug_mux_priv,
	.debug_offset = 0x6000,
	.post_div_offset = 0x6008,
	.cbcr_offset = 0x600C,
	.src_sel_mask = 0xFF,
	.src_sel_shift = 0,
	.post_div_mask = 0x3,
	.post_div_shift = 0,
	.post_div_val = 1,
	.mux_sels = disp_cc_debug_mux_sels,
	.hw.init = &(struct clk_init_data){
		.name = "disp_cc_debug_mux",
		.ops = &clk_debug_mux_ops,
		.parent_names = disp_cc_debug_mux_parent_names,
		.num_parents = ARRAY_SIZE(disp_cc_debug_mux_parent_names),
		.flags = CLK_IS_MEASURE,
	},
};

static const char *const gpu_cc_debug_mux_parent_names[] = {
	"gpu_cc_acd_cxo_clk",
	"gpu_cc_crc_ahb_clk",
	"gpu_cc_cx_apb_clk",
	"gpu_cc_cx_gfx3d_clk",
	"gpu_cc_cx_gfx3d_slv_clk",
	"gpu_cc_cx_gmu_clk",
	"gpu_cc_cx_snoc_dvm_clk",
	"gpu_cc_cxo_aon_clk",
	"gpu_cc_cxo_clk",
	"gpu_cc_gx_gfx3d_clk",
	"gpu_cc_gx_gmu_clk",
	"gpu_cc_gx_vsense_clk",
};

static int gpu_cc_debug_mux_sels[] = {
	0x1F,	/* gpu_cc_acd_cxo_clk */
	0x12,	/* gpu_cc_crc_ahb_clk */
	0x15,	/* gpu_cc_cx_apb_clk */
	0x1A,	/* gpu_cc_cx_gfx3d_clk */
	0x1B,	/* gpu_cc_cx_gfx3d_slv_clk */
	0x19,	/* gpu_cc_cx_gmu_clk */
	0x16,	/* gpu_cc_cx_snoc_dvm_clk */
	0xB,	/* gpu_cc_cxo_aon_clk */
	0xA,	/* gpu_cc_cxo_clk */
	0xC,	/* gpu_cc_gx_gfx3d_clk */
	0x10,	/* gpu_cc_gx_gmu_clk */
	0xD,	/* gpu_cc_gx_vsense_clk */
};

static struct clk_debug_mux gpu_cc_debug_mux = {
	.priv = &debug_mux_priv,
	.debug_offset = 0x1568,
	.post_div_offset = 0x10FC,
	.cbcr_offset = 0x1100,
	.src_sel_mask = 0xFF,
	.src_sel_shift = 0,
	.post_div_mask = 0x3,
	.post_div_shift = 0,
	.post_div_val = 1,
	.mux_sels = gpu_cc_debug_mux_sels,
	.hw.init = &(struct clk_init_data){
		.name = "gpu_cc_debug_mux",
		.ops = &clk_debug_mux_ops,
		.parent_names = gpu_cc_debug_mux_parent_names,
		.num_parents = ARRAY_SIZE(gpu_cc_debug_mux_parent_names),
		.flags = CLK_IS_MEASURE,
	},
};

static const char *const video_cc_debug_mux_parent_names[] = {
	"video_cc_apb_clk",
	"video_cc_at_clk",
	"video_cc_qdss_trig_clk",
	"video_cc_qdss_tsctr_div8_clk",
	"video_cc_vcodec0_axi_clk",
	"video_cc_vcodec0_core_clk",
	"video_cc_vcodec1_axi_clk",
	"video_cc_vcodec1_core_clk",
	"video_cc_venus_ahb_clk",
	"video_cc_venus_ctl_axi_clk",
	"video_cc_venus_ctl_core_clk",
};

static int video_cc_debug_mux_sels[] = {
	0x8,	/* video_cc_apb_clk */
	0xB,	/* video_cc_at_clk */
	0x7,	/* video_cc_qdss_trig_clk */
	0xA,	/* video_cc_qdss_tsctr_div8_clk */
	0x5,	/* video_cc_vcodec0_axi_clk */
	0x2,	/* video_cc_vcodec0_core_clk */
	0x6,	/* video_cc_vcodec1_axi_clk */
	0x3,	/* video_cc_vcodec1_core_clk */
	0x9,	/* video_cc_venus_ahb_clk */
	0x4,	/* video_cc_venus_ctl_axi_clk */
	0x1,	/* video_cc_venus_ctl_core_clk */
};

static struct clk_debug_mux video_cc_debug_mux = {
	.priv = &debug_mux_priv,
	.debug_offset = 0xA4C,
	.post_div_offset = 0xA50,
	.cbcr_offset = 0xA58,
	.src_sel_mask = 0x3F,
	.src_sel_shift = 0,
	.post_div_mask = 0x7,
	.post_div_shift = 0,
	.post_div_val = 1,
	.mux_sels = video_cc_debug_mux_sels,
	.hw.init = &(struct clk_init_data){
		.name = "video_cc_debug_mux",
		.ops = &clk_debug_mux_ops,
		.parent_names = video_cc_debug_mux_parent_names,
		.num_parents = ARRAY_SIZE(video_cc_debug_mux_parent_names),
		.flags = CLK_IS_MEASURE,
	},
};

static const char *const gcc_debug_mux_parent_names[] = {
	"cpu_cc_debug_mux",
	"cam_cc_debug_mux",
	"disp_cc_debug_mux",
	"gpu_cc_debug_mux",
	"video_cc_debug_mux",
	"measure_only_snoc_clk",
	"measure_only_cnoc_clk",
	"measure_only_bimc_clk",
	"measure_only_ipa_2x_clk",
	"gcc_aggre_ufs_phy_axi_clk",
	"gcc_aggre_usb3_prim_axi_clk",
	"gcc_apc_vs_clk",
	"gcc_boot_rom_ahb_clk",
	"gcc_camera_ahb_clk",
	"gcc_camera_axi_clk",
	"gcc_camera_xo_clk",
	"gcc_ce1_ahb_clk",
	"gcc_ce1_axi_clk",
	"gcc_ce1_clk",
	"gcc_cfg_noc_usb3_prim_axi_clk",
	"gcc_cpuss_ahb_clk",
	"gcc_cpuss_dvm_bus_clk",
	"gcc_cpuss_gnoc_clk",
	"gcc_cpuss_rbcpr_clk",
	"gcc_ddrss_gpu_axi_clk",
	"gcc_disp_ahb_clk",
	"gcc_disp_axi_clk",
	"gcc_disp_gpll0_clk_src",
	"gcc_disp_gpll0_div_clk_src",
	"gcc_disp_xo_clk",
	"gcc_gp1_clk",
	"gcc_gp2_clk",
	"gcc_gp3_clk",
	"gcc_gpu_cfg_ahb_clk",
	"gcc_gpu_gpll0_clk_src",
	"gcc_gpu_gpll0_div_clk_src",
	"gcc_gpu_memnoc_gfx_clk",
	"gcc_gpu_snoc_dvm_gfx_clk",
	"gcc_gpu_vs_clk",
	"gcc_mss_axis2_clk",
	"gcc_mss_cfg_ahb_clk",
	"gcc_mss_gpll0_div_clk_src",
	"gcc_mss_mfab_axis_clk",
	"gcc_mss_q6_memnoc_axi_clk",
	"gcc_mss_snoc_axi_clk",
	"gcc_mss_vs_clk",
	"gcc_pdm2_clk",
	"gcc_pdm_ahb_clk",
	"gcc_pdm_xo4_clk",
	"gcc_prng_ahb_clk",
	"gcc_qmip_camera_ahb_clk",
	"gcc_qmip_disp_ahb_clk",
	"gcc_qmip_video_ahb_clk",
	"gcc_qupv3_wrap0_s0_clk",
	"gcc_qupv3_wrap0_s1_clk",
	"gcc_qupv3_wrap0_s2_clk",
	"gcc_qupv3_wrap0_s3_clk",
	"gcc_qupv3_wrap0_s4_clk",
	"gcc_qupv3_wrap0_s5_clk",
	"gcc_qupv3_wrap0_s6_clk",
	"gcc_qupv3_wrap0_s7_clk",
	"gcc_qupv3_wrap1_s0_clk",
	"gcc_qupv3_wrap1_s1_clk",
	"gcc_qupv3_wrap1_s2_clk",
	"gcc_qupv3_wrap1_s3_clk",
	"gcc_qupv3_wrap1_s4_clk",
	"gcc_qupv3_wrap1_s5_clk",
	"gcc_qupv3_wrap1_s6_clk",
	"gcc_qupv3_wrap1_s7_clk",
	"gcc_qupv3_wrap_0_m_ahb_clk",
	"gcc_qupv3_wrap_0_s_ahb_clk",
	"gcc_qupv3_wrap_1_m_ahb_clk",
	"gcc_qupv3_wrap_1_s_ahb_clk",
	"gcc_sdcc2_ahb_clk",
	"gcc_sdcc2_apps_clk",
	"gcc_sdcc4_ahb_clk",
	"gcc_sdcc4_apps_clk",
	"gcc_sys_noc_cpuss_ahb_clk",
	"gcc_tsif_ahb_clk",
	"gcc_tsif_inactivity_timers_clk",
	"gcc_tsif_ref_clk",
	"gcc_ufs_phy_ahb_clk",
	"gcc_ufs_phy_axi_clk",
	"gcc_ufs_phy_ice_core_clk",
	"gcc_ufs_phy_phy_aux_clk",
	"gcc_ufs_phy_rx_symbol_0_clk",
	"gcc_ufs_phy_tx_symbol_0_clk",
	"gcc_ufs_phy_unipro_core_clk",
	"gcc_usb30_prim_master_clk",
	"gcc_usb30_prim_mock_utmi_clk",
	"gcc_usb30_prim_sleep_clk",
	"gcc_usb3_prim_phy_aux_clk",
	"gcc_usb3_prim_phy_com_aux_clk",
	"gcc_usb3_prim_phy_pipe_clk",
	"gcc_usb_phy_cfg_ahb2phy_clk",
	"gcc_vdda_vs_clk",
	"gcc_vddcx_vs_clk",
	"gcc_vddmx_vs_clk",
	"gcc_video_ahb_clk",
	"gcc_video_axi_clk",
	"gcc_video_xo_clk",
	"gcc_vs_ctrl_ahb_clk",
	"gcc_vs_ctrl_clk",
	"gcc_sdcc1_ahb_clk",
	"gcc_sdcc1_apps_clk",
	"gcc_sdcc1_ice_core_clk",
};

static int gcc_debug_mux_sels[] = {
	0xD6,	/* cpu_cc_debug_mux */
	0x46,	/* cam_cc_debug_mux */
	0x47,	/* disp_cc_debug_mux */
	0x144,	/* gpu_cc_debug_mux */
	0x48,	/* video_cc_debug_mux */
	0x7,	/* measure_only_snoc_clk */
	0x15,	/* measure_only_cnoc_clk */
	0xc2,	/* measure_only_bimc_clk */
	0x128,	/* measure_only_ipa_2x_clk */
	0x11D,	/* gcc_aggre_ufs_phy_axi_clk */
	0x11B,	/* gcc_aggre_usb3_prim_axi_clk */
	0x113,	/* gcc_apc_vs_clk */
	0x94,	/* gcc_boot_rom_ahb_clk */
	0x3A,	/* gcc_camera_ahb_clk */
	0x40,	/* gcc_camera_axi_clk */
	0x43,	/* gcc_camera_xo_clk */
	0xA9,	/* gcc_ce1_ahb_clk */
	0xA8,	/* gcc_ce1_axi_clk */
	0xA7,	/* gcc_ce1_clk */
	0x1D,	/* gcc_cfg_noc_usb3_prim_axi_clk */
	0xCE,	/* gcc_cpuss_ahb_clk */
	0xD3,	/* gcc_cpuss_dvm_bus_clk */
	0xCF,	/* gcc_cpuss_gnoc_clk */
	0xD0,	/* gcc_cpuss_rbcpr_clk */
	0xBB,	/* gcc_ddrss_gpu_axi_clk */
	0x3B,	/* gcc_disp_ahb_clk */
	0x41,	/* gcc_disp_axi_clk */
	0x4C,	/* gcc_disp_gpll0_clk_src */
	0x4D,	/* gcc_disp_gpll0_div_clk_src */
	0x44,	/* gcc_disp_xo_clk */
	0xDE,	/* gcc_gp1_clk */
	0xDF,	/* gcc_gp2_clk */
	0xE0,	/* gcc_gp3_clk */
	0x142,	/* gcc_gpu_cfg_ahb_clk */
	0x148,	/* gcc_gpu_gpll0_clk_src */
	0x149,	/* gcc_gpu_gpll0_div_clk_src */
	0x145,	/* gcc_gpu_memnoc_gfx_clk */
	0x147,	/* gcc_gpu_snoc_dvm_gfx_clk */
	0x112,	/* gcc_gpu_vs_clk */
	0x12F,	/* gcc_mss_axis2_clk */
	0x12D,	/* gcc_mss_cfg_ahb_clk */
	0x133,	/* gcc_mss_gpll0_div_clk_src */
	0x12E,	/* gcc_mss_mfab_axis_clk */
	0x135,	/* gcc_mss_q6_memnoc_axi_clk */
	0x134,	/* gcc_mss_snoc_axi_clk */
	0x111,	/* gcc_mss_vs_clk */
	0x8E,	/* gcc_pdm2_clk */
	0x8C,	/* gcc_pdm_ahb_clk */
	0x8D,	/* gcc_pdm_xo4_clk */
	0x8F,	/* gcc_prng_ahb_clk */
	0x3D,	/* gcc_qmip_camera_ahb_clk */
	0x3E,	/* gcc_qmip_disp_ahb_clk */
	0x3C,	/* gcc_qmip_video_ahb_clk */
	0x78,	/* gcc_qupv3_wrap0_s0_clk */
	0x79,	/* gcc_qupv3_wrap0_s1_clk */
	0x7A,	/* gcc_qupv3_wrap0_s2_clk */
	0x7B,	/* gcc_qupv3_wrap0_s3_clk */
	0x7C,	/* gcc_qupv3_wrap0_s4_clk */
	0x7D,	/* gcc_qupv3_wrap0_s5_clk */
	0x7E,	/* gcc_qupv3_wrap0_s6_clk */
	0x7F,	/* gcc_qupv3_wrap0_s7_clk */
	0x84,	/* gcc_qupv3_wrap1_s0_clk */
	0x85,	/* gcc_qupv3_wrap1_s1_clk */
	0x86,	/* gcc_qupv3_wrap1_s2_clk */
	0x87,	/* gcc_qupv3_wrap1_s3_clk */
	0x88,	/* gcc_qupv3_wrap1_s4_clk */
	0x89,	/* gcc_qupv3_wrap1_s5_clk */
	0x8A,	/* gcc_qupv3_wrap1_s6_clk */
	0x8B,	/* gcc_qupv3_wrap1_s7_clk */
	0x74,	/* gcc_qupv3_wrap_0_m_ahb_clk */
	0x75,	/* gcc_qupv3_wrap_0_s_ahb_clk */
	0x82,	/* gcc_qupv3_wrap_1_m_ahb_clk */
	0x83,	/* gcc_qupv3_wrap_1_s_ahb_clk */
	0x71,	/* gcc_sdcc2_ahb_clk */
	0x70,	/* gcc_sdcc2_apps_clk */
	0x73,	/* gcc_sdcc4_ahb_clk */
	0x72,	/* gcc_sdcc4_apps_clk */
	0xC,	/* gcc_sys_noc_cpuss_ahb_clk */
	0x90,	/* gcc_tsif_ahb_clk */
	0x92,	/* gcc_tsif_inactivity_timers_clk */
	0x91,	/* gcc_tsif_ref_clk */
	0xFC,	/* gcc_ufs_phy_ahb_clk */
	0xFB,	/* gcc_ufs_phy_axi_clk */
	0x102,	/* gcc_ufs_phy_ice_core_clk */
	0x103,	/* gcc_ufs_phy_phy_aux_clk */
	0xFE,	/* gcc_ufs_phy_rx_symbol_0_clk */
	0xFD,	/* gcc_ufs_phy_tx_symbol_0_clk */
	0x101,	/* gcc_ufs_phy_unipro_core_clk */
	0x5F,	/* gcc_usb30_prim_master_clk */
	0x61,	/* gcc_usb30_prim_mock_utmi_clk */
	0x60,	/* gcc_usb30_prim_sleep_clk */
	0x62,	/* gcc_usb3_prim_phy_aux_clk */
	0x63,	/* gcc_usb3_prim_phy_com_aux_clk */
	0x64,	/* gcc_usb3_prim_phy_pipe_clk */
	0x6F,	/* gcc_usb_phy_cfg_ahb2phy_clk */
	0x10E,	/* gcc_vdda_vs_clk */
	0x10C,	/* gcc_vddcx_vs_clk */
	0x10D,	/* gcc_vddmx_vs_clk */
	0x39,	/* gcc_video_ahb_clk */
	0x3F,	/* gcc_video_axi_clk */
	0x42,	/* gcc_video_xo_clk */
	0x110,	/* gcc_vs_ctrl_ahb_clk */
	0x10F,	/* gcc_vs_ctrl_clk */
	0x15C,	/* gcc_sdcc1_ahb_clk */
	0x15B,	/* gcc_sdcc1_apps_clk */
	0x15D,	/* gcc_sdcc1_ice_core_clk */
};

static struct clk_debug_mux gcc_debug_mux = {
	.priv = &debug_mux_priv,
	.debug_offset = 0x62008,
	.post_div_offset = 0x62000,
	.cbcr_offset = 0x62004,
	.src_sel_mask = 0x3FF,
	.src_sel_shift = 0,
	.post_div_mask = 0xF,
	.post_div_shift = 0,
	.post_div_val = 4,
	.mux_sels = gcc_debug_mux_sels,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_debug_mux",
		.ops = &clk_debug_mux_ops,
		.parent_names = gcc_debug_mux_parent_names,
		.num_parents = ARRAY_SIZE(gcc_debug_mux_parent_names),
		.flags = CLK_IS_MEASURE,
	},
};

static struct mux_regmap_names mux_list[] = {
	{ .mux = &cpu_cc_debug_mux, .regmap_name = "qcom,cpucc" },
	{ .mux = &cam_cc_debug_mux, .regmap_name = "qcom,camcc" },
	{ .mux = &disp_cc_debug_mux, .regmap_name = "qcom,dispcc" },
	{ .mux = &gpu_cc_debug_mux, .regmap_name = "qcom,gpucc" },
	{ .mux = &video_cc_debug_mux, .regmap_name = "qcom,videocc" },
	{ .mux = &gcc_debug_mux, .regmap_name = "qcom,gcc" },
};

static struct clk_dummy measure_only_snoc_clk = {
	.rrate = 1000,
	.hw.init = &(struct clk_init_data){
		.name = "measure_only_snoc_clk",
		.ops = &clk_dummy_ops,
	},
};

static struct clk_dummy measure_only_cnoc_clk = {
	.rrate = 1000,
	.hw.init = &(struct clk_init_data){
		.name = "measure_only_cnoc_clk",
		.ops = &clk_dummy_ops,
	},
};

static struct clk_dummy measure_only_bimc_clk = {
	.rrate = 1000,
	.hw.init = &(struct clk_init_data){
		.name = "measure_only_bimc_clk",
		.ops = &clk_dummy_ops,
	},
};

static struct clk_dummy measure_only_ipa_2x_clk = {
	.rrate = 1000,
	.hw.init = &(struct clk_init_data){
		.name = "measure_only_ipa_2x_clk",
		.ops = &clk_dummy_ops,
	},
};

static struct clk_hw *debugcc_sdm670_hws[] = {
	&measure_only_snoc_clk.hw,
	&measure_only_cnoc_clk.hw,
	&measure_only_bimc_clk.hw,
	&measure_only_ipa_2x_clk.hw,
};

static const struct of_device_id clk_debug_match_table[] = {
	{ .compatible = "qcom,sdm670-debugcc" },
	{ }
};
MODULE_DEVICE_TABLE(of, clk_debug_match_table);

static void clk_debug_bus_unregister(void *data)
{
	struct clk_debug_mux *mux = data;

	msm_bus_scale_unregister_client(mux->bus_cl_id);
	mux->bus_cl_id = 0;
}

static int clk_debug_sdm670_probe(struct platform_device *pdev)
{
	struct clk *clk;
	int i, ret;

	BUILD_BUG_ON(ARRAY_SIZE(cpu_cc_debug_mux_parent_names) !=
		     ARRAY_SIZE(cpu_cc_debug_mux_sels));
	BUILD_BUG_ON(ARRAY_SIZE(cam_cc_debug_mux_parent_names) !=
		     ARRAY_SIZE(cam_cc_debug_mux_sels));
	BUILD_BUG_ON(ARRAY_SIZE(disp_cc_debug_mux_parent_names) !=
		     ARRAY_SIZE(disp_cc_debug_mux_sels));
	BUILD_BUG_ON(ARRAY_SIZE(gpu_cc_debug_mux_parent_names) !=
		     ARRAY_SIZE(gpu_cc_debug_mux_sels));
	BUILD_BUG_ON(ARRAY_SIZE(video_cc_debug_mux_parent_names) !=
		     ARRAY_SIZE(video_cc_debug_mux_sels));
	BUILD_BUG_ON(ARRAY_SIZE(gcc_debug_mux_parent_names) !=
		     ARRAY_SIZE(gcc_debug_mux_sels));
	BUILD_BUG_ON(ARRAY_SIZE(cpu_cc_debug_mux_parent_names) !=
		     ARRAY_SIZE(cpu_cc_debug_mux_pre_divs));

	clk = devm_clk_get(&pdev->dev, "xo_clk_src");
	if (IS_ERR(clk)) {
		if (PTR_ERR(clk) != -EPROBE_DEFER)
			dev_err(&pdev->dev, "Unable to get XO clock\n");
		return PTR_ERR(clk);
	}
	debug_mux_priv.cxo = clk;

	/* Resolve every register bank before publishing any measurement mux. */
	for (i = 0; i < ARRAY_SIZE(mux_list); i++) {
		ret = map_debug_bases(pdev, mux_list[i].regmap_name,
				      mux_list[i].mux);
		if (ret)
			return ret;
	}

	gcc_debug_mux.bus_cl_id =
		msm_bus_scale_register_client(&clk_measure_scale_table);
	if (!gcc_debug_mux.bus_cl_id)
		return -EPROBE_DEFER;

	ret = devm_add_action_or_reset(&pdev->dev, clk_debug_bus_unregister,
				       &gcc_debug_mux);
	if (ret)
		return ret;

	for (i = 0; i < ARRAY_SIZE(debugcc_sdm670_hws); i++) {
		clk = devm_clk_register(&pdev->dev, debugcc_sdm670_hws[i]);
		if (IS_ERR(clk))
			return PTR_ERR(clk);
	}

	for (i = 0; i < ARRAY_SIZE(mux_list); i++) {
		clk = devm_clk_register(&pdev->dev, &mux_list[i].mux->hw);
		if (IS_ERR(clk)) {
			dev_err(&pdev->dev, "Unable to register %s: %ld\n",
				mux_list[i].regmap_name, PTR_ERR(clk));
			return PTR_ERR(clk);
		}
	}

	return clk_debug_measure_register(&gcc_debug_mux.hw);
}

static struct platform_driver clk_debug_driver = {
	.probe = clk_debug_sdm670_probe,
	.driver = {
		.name = "sdm670-debugcc",
		.of_match_table = clk_debug_match_table,
	},
};

static int __init clk_debug_sdm670_init(void)
{
	return platform_driver_register(&clk_debug_driver);
}
fs_initcall(clk_debug_sdm670_init);

MODULE_DESCRIPTION("Qualcomm SDM670/SDM710 Debug Clock Controller");
MODULE_LICENSE("GPL v2");
