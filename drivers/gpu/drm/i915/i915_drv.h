/* i915_drv.h -- Private header for the I915 driver -*- linux-c -*-
 */
/*
 *
 * Copyright 2003 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _I915_DRV_H_
#define _I915_DRV_H_

#include <uapi/drm/i915_drm.h>

#include "i915_reg.h"
#include "intel_bios.h"
#include "intel_ringbuffer.h"
#include <linux/io-mapping.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <drm/intel-gtt.h>
#include <linux/backlight.h>
#include <linux/intel-iommu.h>
#include <linux/kref.h>
#include <linux/pm_qos.h>

/* General customization:
 */

#define DRIVER_AUTHOR		"Tungsten Graphics, Inc."

#define DRIVER_NAME		"i915"
#define DRIVER_DESC		"Intel Graphics"
#define DRIVER_DATE		"20080730"

enum pipe {
	PIPE_A = 0,
	PIPE_B,
	PIPE_C,
	I915_MAX_PIPES
};
#define pipe_name(p) ((p) + 'A')

enum transcoder {
	TRANSCODER_A = 0,
	TRANSCODER_B,
	TRANSCODER_C,
	TRANSCODER_EDP = 0xF,
};
#define transcoder_name(t) ((t) + 'A')

enum plane {
	PLANE_A = 0,
	PLANE_B,
	PLANE_C,
};
#define plane_name(p) ((p) + 'A')

#define sprite_name(p, s) ((p) * dev_priv->num_plane + (s) + 'A')

enum port {
	PORT_A = 0,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	I915_MAX_PORTS
};
#define port_name(p) ((p) + 'A')

enum intel_display_power_domain {
	POWER_DOMAIN_PIPE_A,
	POWER_DOMAIN_PIPE_B,
	POWER_DOMAIN_PIPE_C,
	POWER_DOMAIN_PIPE_A_PANEL_FITTER,
	POWER_DOMAIN_PIPE_B_PANEL_FITTER,
	POWER_DOMAIN_PIPE_C_PANEL_FITTER,
	POWER_DOMAIN_TRANSCODER_A,
	POWER_DOMAIN_TRANSCODER_B,
	POWER_DOMAIN_TRANSCODER_C,
	POWER_DOMAIN_TRANSCODER_EDP = POWER_DOMAIN_TRANSCODER_A + 0xF,
};

#define POWER_DOMAIN_PIPE(pipe) ((pipe) + POWER_DOMAIN_PIPE_A)
#define POWER_DOMAIN_PIPE_PANEL_FITTER(pipe) \
		((pipe) + POWER_DOMAIN_PIPE_A_PANEL_FITTER)
#define POWER_DOMAIN_TRANSCODER(tran) ((tran) + POWER_DOMAIN_TRANSCODER_A)

enum hpd_pin {
	HPD_NONE = 0,
	HPD_PORT_A = HPD_NONE, /* PORT_A is internal */
	HPD_TV = HPD_NONE,     /* TV is known to be unreliable */
	HPD_CRT,
	HPD_SDVO_B,
	HPD_SDVO_C,
	HPD_PORT_B,
	HPD_PORT_C,
	HPD_PORT_D,
	HPD_NUM_PINS
};

#define I915_GEM_GPU_DOMAINS \
	(I915_GEM_DOMAIN_RENDER | \
	 I915_GEM_DOMAIN_SAMPLER | \
	 I915_GEM_DOMAIN_COMMAND | \
	 I915_GEM_DOMAIN_INSTRUCTION | \
	 I915_GEM_DOMAIN_VERTEX)

#define for_each_pipe(p) for ((p) = 0; (p) < INTEL_INFO(dev)->num_pipes; (p)++)

#define for_each_encoder_on_crtc(dev, __crtc, intel_encoder) \
	list_for_each_entry((intel_encoder), &(dev)->mode_config.encoder_list, base.head) \
		if ((intel_encoder)->base.crtc == (__crtc))

struct intel_pch_pll {
	int refcount; /* count of number of CRTCs sharing this PLL */
	int active; /* count of number of active CRTCs (i.e. DPMS on) */
	bool on; /* is the PLL actually active? Disabled during modeset */
	int pll_reg;
	int fp0_reg;
	int fp1_reg;
};
#define I915_NUM_PLLS 2

/* Used by dp and fdi links */
struct intel_link_m_n {
	uint32_t	tu;
	uint32_t	gmch_m;
	uint32_t	gmch_n;
	uint32_t	link_m;
	uint32_t	link_n;
};

void intel_link_compute_m_n(int bpp, int nlanes,
			    int pixel_clock, int link_clock,
			    struct intel_link_m_n *m_n);

struct intel_ddi_plls {
	int spll_refcount;
	int wrpll1_refcount;
	int wrpll2_refcount;
};

/* Interface history:
 *
 * 1.1: Original.
 * 1.2: Add Power Management
 * 1.3: Add vblank support
 * 1.4: Fix cmdbuffer path, add heap destroy
 * 1.5: Add vblank pipe configuration
 * 1.6: - New ioctl for scheduling buffer swaps on vertical blank
 *      - Support vertical blank on secondary display pipe
 */
#define DRIVER_MAJOR		1
#define DRIVER_MINOR		6
#define DRIVER_PATCHLEVEL	0

#define WATCH_COHERENCY	0
#define WATCH_LISTS	0
#define WATCH_GTT	0

#define I915_GEM_PHYS_CURSOR_0 1
#define I915_GEM_PHYS_CURSOR_1 2
#define I915_GEM_PHYS_OVERLAY_REGS 3
#define I915_MAX_PHYS_OBJECT (I915_GEM_PHYS_OVERLAY_REGS)

struct drm_i915_gem_phys_object {
	int id;
	struct page **page_list;
	drm_dma_handle_t *handle;
	struct drm_i915_gem_object *cur_obj;
};

struct opregion_header;
struct opregion_acpi;
struct opregion_swsci;
struct opregion_asle;
struct drm_i915_private;

struct intel_opregion {
	struct opregion_header __iomem *header;
	struct opregion_acpi __iomem *acpi;
	struct opregion_swsci __iomem *swsci;
	struct opregion_asle __iomem *asle;
	void __iomem *vbt;
	u32 __iomem *lid_state;
};
#define OPREGION_SIZE            (8*1024)

struct intel_overlay;
struct intel_overlay_error_state;

struct drm_i915_master_private {
	drm_local_map_t *sarea;
	struct _drm_i915_sarea *sarea_priv;
};
#define I915_FENCE_REG_NONE -1
#define I915_MAX_NUM_FENCES 32
/* 32 fences + sign bit for FENCE_REG_NONE */
#define I915_MAX_NUM_FENCE_BITS 6

struct drm_i915_fence_reg {
	struct list_head lru_list;
	struct drm_i915_gem_object *obj;
	int pin_count;
};

struct sdvo_device_mapping {
	u8 initialized;
	u8 dvo_port;
	u8 slave_addr;
	u8 dvo_wiring;
	u8 i2c_pin;
	u8 ddc_pin;
};

struct intel_display_error_state;

struct drm_i915_error_state {
	struct kref ref;
	u32 eir;
	u32 pgtbl_er;
	u32 ier;
	u32 ccid;
	u32 derrmr;
	u32 forcewake;
	bool waiting[I915_NUM_RINGS];
	u32 pipestat[I915_MAX_PIPES];
	u32 tail[I915_NUM_RINGS];
	u32 head[I915_NUM_RINGS];
	u32 ctl[I915_NUM_RINGS];
	u32 ipeir[I915_NUM_RINGS];
	u32 ipehr[I915_NUM_RINGS];
	u32 instdone[I915_NUM_RINGS];
	u32 acthd[I915_NUM_RINGS];
	u32 semaphore_mboxes[I915_NUM_RINGS][I915_NUM_RINGS - 1];
	u32 semaphore_seqno[I915_NUM_RINGS][I915_NUM_RINGS - 1];
	u32 rc_psmi[I915_NUM_RINGS]; /* sleep state */
	/* our own tracking of ring head and tail */
	u32 cpu_ring_head[I915_NUM_RINGS];
	u32 cpu_ring_tail[I915_NUM_RINGS];
	u32 error; /* gen6+ */
	u32 err_int; /* gen7 */
	u32 instpm[I915_NUM_RINGS];
	u32 instps[I915_NUM_RINGS];
	u32 extra_instdone[I915_NUM_INSTDONE_REG];
	u32 seqno[I915_NUM_RINGS];
	u64 bbaddr;
	u32 fault_reg[I915_NUM_RINGS];
	u32 done_reg;
	u32 faddr[I915_NUM_RINGS];
	u64 fence[I915_MAX_NUM_FENCES];
	struct timeval time;
	struct drm_i915_error_ring {
		struct drm_i915_error_object {
			int page_count;
			u32 gtt_offset;
			u32 *pages[0];
		} *ringbuffer, *batchbuffer, *ctx;
		struct drm_i915_error_request {
			long jiffies;
			u32 seqno;
			u32 tail;
		} *requests;
		int num_requests;
	} ring[I915_NUM_RINGS];
	struct drm_i915_error_buffer {
		u32 size;
		u32 name;
		u32 rseqno, wseqno;
		u32 gtt_offset;
		u32 read_domains;
		u32 write_domain;
		s32 fence_reg:I915_MAX_NUM_FENCE_BITS;
		s32 pinned:2;
		u32 tiling:2;
		u32 dirty:1;
		u32 purgeable:1;
		s32 ring:4;
		u32 cache_level:2;
	} *active_bo, *pinned_bo;
	u32 active_bo_count, pinned_bo_count;
	struct intel_overlay_error_state *overlay;
	struct intel_display_error_state *display;
};

struct intel_crtc_config;
struct intel_crtc;

struct drm_i915_display_funcs {
	bool (*fbc_enabled)(struct drm_device *dev);
	void (*enable_fbc)(struct drm_crtc *crtc, unsigned long interval);
	void (*disable_fbc)(struct drm_device *dev);
	int (*get_display_clock_speed)(struct drm_device *dev);
	int (*get_fifo_size)(struct drm_device *dev, int plane);
	void (*update_wm)(struct drm_device *dev);
	void (*update_sprite_wm)(struct drm_device *dev, int pipe,
				 uint32_t sprite_width, int pixel_size);
	void (*update_linetime_wm)(struct drm_device *dev, int pipe,
				 struct drm_display_mode *mode);
	void (*modeset_global_resources)(struct drm_device *dev);
	/* Returns the active state of the crtc, and if the crtc is active,
	 * fills out the pipe-config with the hw state. */
	bool (*get_pipe_config)(struct intel_crtc *,
				struct intel_crtc_config *);
	int (*crtc_mode_set)(struct drm_crtc *crtc,
			     int x, int y,
			     struct drm_framebuffer *old_fb);
	void (*crtc_enable)(struct drm_crtc *crtc);
	void (*crtc_disable)(struct drm_crtc *crtc);
	void (*off)(struct drm_crtc *crtc);
	void (*write_eld)(struct drm_connector *connector,
			  struct drm_crtc *crtc);
	void (*fdi_link_train)(struct drm_crtc *crtc);
	void (*init_clock_gating)(struct drm_device *dev);
	int (*queue_flip)(struct drm_device *dev, struct drm_crtc *crtc,
			  struct drm_framebuffer *fb,
			  struct drm_i915_gem_object *obj);
	int (*update_plane)(struct drm_crtc *crtc, struct drm_framebuffer *fb,
			    int x, int y);
	void (*hpd_irq_setup)(struct drm_device *dev);
	/* clock updates for mode set */
	/* cursor updates */
	/* render clock increase/decrease */
	/* display clock increase/decrease */
	/* pll clock increase/decrease */
};

struct drm_i915_gt_funcs {
	void (*force_wake_get)(struct drm_i915_private *dev_priv);
	void (*force_wake_put)(struct drm_i915_private *dev_priv);
};

#define DEV_INFO_FOR_EACH_FLAG(func, sep) \
	func(is_mobile) sep \
	func(is_i85x) sep \
	func(is_i915g) sep \
	func(is_i945gm) sep \
	func(is_g33) sep \
	func(need_gfx_hws) sep \
	func(is_g4x) sep \
	func(is_pineview) sep \
	func(is_broadwater) sep \
	func(is_crestline) sep \
	func(is_ivybridge) sep \
	func(is_valleyview) sep \
	func(is_haswell) sep \
	func(has_force_wake) sep \
	func(has_fbc) sep \
	func(has_pipe_cxsr) sep \
	func(has_hotplug) sep \
	func(cursor_needs_physical) sep \
	func(has_overlay) sep \
	func(overlay_needs_physical) sep \
	func(supports_tv) sep \
	func(has_bsd_ring) sep \
	func(has_blt_ring) sep \
	func(has_llc) sep \
	func(has_ddi) sep \
	func(has_fpga_dbg)

#define DEFINE_FLAG(name) u8 name:1
#define SEP_SEMICOLON ;

struct intel_device_info {
	u32 display_mmio_offset;
	u8 num_pipes:3;
	u8 gen;
	DEV_INFO_FOR_EACH_FLAG(DEFINE_FLAG, SEP_SEMICOLON);
};

#undef DEFINE_FLAG
#undef SEP_SEMICOLON

enum i915_cache_level {
	I915_CACHE_NONE = 0,
	I915_CACHE_LLC,
	I915_CACHE_LLC_MLC, /* gen6+, in docs at least! */
};

typedef uint32_t gen6_gtt_pte_t;

/* The Graphics Translation Table is the way in which GEN hardware translates a
 * Graphics Virtual Address into a Physical Address. In addition to the normal
 * collateral associated with any va->pa translations GEN hardware also has a
 * portion of the GTT which can be mapped by the CPU and remain both coherent
 * and correct (in cases like swizzling). That region is referred to as GMADR in
 * the spec.
 */
struct i915_gtt {
	unsigned long start;		/* Start offset of used GTT */
	size_t total;			/* Total size GTT can map */
	size_t stolen_size;		/* Total size of stolen memory */

	unsigned long mappable_end;	/* End offset that we can CPU map */
	struct io_mapping *mappable;	/* Mapping to our CPU mappable region */
	phys_addr_t mappable_base;	/* PA of our GMADR */

	/** "Graphics Stolen Memory" holds the global PTEs */
	void __iomem *gsm;

	bool do_idle_maps;
	dma_addr_t scratch_page_dma;
	struct page *scratch_page;

	/* global gtt ops */
	int (*gtt_probe)(struct drm_device *dev, size_t *gtt_total,
			  size_t *stolen, phys_addr_t *mappable_base,
			  unsigned long *mappable_end);
	void (*gtt_remove)(struct drm_device *dev);
	void (*gtt_clear_range)(struct drm_device *dev,
				unsigned int first_entry,
				unsigned int num_entries);
	void (*gtt_insert_entries)(struct drm_device *dev,
				   struct sg_table *st,
				   unsigned int pg_start,
				   enum i915_cache_level cache_level);
	gen6_gtt_pte_t (*pte_encode)(struct drm_device *dev,
				     dma_addr_t addr,
				     enum i915_cache_level level);
};
#define gtt_total_entries(gtt) ((gtt).total >> PAGE_SHIFT)

#define I915_PPGTT_PD_ENTRIES 512
#define I915_PPGTT_PT_ENTRIES 1024
struct i915_hw_ppgtt {
	struct drm_device *dev;
	unsigned num_pd_entries;
	struct page **pt_pages;
	uint32_t pd_offset;
	dma_addr_t *pt_dma_addr;
	dma_addr_t scratch_page_dma_addr;

	/* pte functions, mirroring the interface of the global gtt. */
	void (*clear_range)(struct i915_hw_ppgtt *ppgtt,
			    unsigned int first_entry,
			    unsigned int num_entries);
	void (*insert_entries)(struct i915_hw_ppgtt *ppgtt,
			       struct sg_table *st,
			       unsigned int pg_start,
			       enum i915_cache_level cache_level);
	gen6_gtt_pte_t (*pte_encode)(struct drm_device *dev,
				     dma_addr_t addr,
				     enum i915_cache_level level);
	int (*enable)(struct drm_device *dev);
	void (*cleanup)(struct i915_hw_ppgtt *ppgtt);
};


/* This must match up with the value previously used for execbuf2.rsvd1. */
#define DEFAULT_CONTEXT_ID 0
struct i915_hw_context {
	struct kref ref;
	int id;
	bool is_initialized;
	struct drm_i915_file_private *file_priv;
	struct intel_ring_buffer *ring;
	struct drm_i915_gem_object *obj;
};

enum no_fbc_reason {
	FBC_NO_OUTPUT, /* no outputs enabled to compress */
	FBC_STOLEN_TOO_SMALL, /* not enough space to hold compressed buffers */
	FBC_UNSUPPORTED_MODE, /* interlace or doublescanned mode */
	FBC_MODE_TOO_LARGE, /* mode too large for compression */
	FBC_BAD_PLANE, /* fbc not supported on plane */
	FBC_NOT_TILED, /* buffer not tiled */
	FBC_MULTIPLE_PIPES, /* more than one pipe active */
	FBC_MODULE_PARAM,
};

enum intel_pch {
	PCH_NONE = 0,	/* No PCH present */
	PCH_IBX,	/* Ibexpeak PCH */
	PCH_CPT,	/* Cougarpoint PCH */
	PCH_LPT,	/* Lynxpoint PCH */
	PCH_NOP,
};

enum intel_sbi_destination {
	SBI_ICLK,
	SBI_MPHY,
};

#define QUIRK_PIPEA_FORCE (1<<0)
#define QUIRK_LVDS_SSC_DISABLE (1<<1)
#define QUIRK_INVERT_BRIGHTNESS (1<<2)
#define QUIRK_NO_PCH_PWM_ENABLE (1<<3)

struct intel_fbdev;
struct intel_fbc_work;

struct intel_gmbus {
	struct i2c_adapter adapter;
	u32 force_bit;
	u32 reg0;
	u32 gpio_reg;
	struct i2c_algo_bit_data bit_algo;
	struct drm_i915_private *dev_priv;
};

struct i915_suspend_saved_registers {
	u8 saveLBB;
	u32 saveDSPACNTR;
	u32 saveDSPBCNTR;
	u32 saveDSPARB;
	u32 savePIPEACONF;
	u32 savePIPEBCONF;
	u32 savePIPEASRC;
	u32 savePIPEBSRC;
	u32 saveFPA0;
	u32 saveFPA1;
	u32 saveDPLL_A;
	u32 saveDPLL_A_MD;
	u32 saveHTOTAL_A;
	u32 saveHBLANK_A;
	u32 saveHSYNC_A;
	u32 saveVTOTAL_A;
	u32 saveVBLANK_A;
	u32 saveVSYNC_A;
	u32 saveBCLRPAT_A;
	u32 saveTRANSACONF;
	u32 saveTRANS_HTOTAL_A;
	u32 saveTRANS_HBLANK_A;
	u32 saveTRANS_HSYNC_A;
	u32 saveTRANS_VTOTAL_A;
	u32 saveTRANS_VBLANK_A;
	u32 saveTRANS_VSYNC_A;
	u32 savePIPEASTAT;
	u32 saveDSPASTRIDE;
	u32 saveDSPASIZE;
	u32 saveDSPAPOS;
	u32 saveDSPAADDR;
	u32 saveDSPASURF;
	u32 saveDSPATILEOFF;
	u32 savePFIT_PGM_RATIOS;
	u32 saveBLC_HIST_CTL;
	u32 saveBLC_PWM_CTL;
	u32 saveBLC_PWM_CTL2;
	u32 saveBLC_CPU_PWM_CTL;
	u32 saveBLC_CPU_PWM_CTL2;
	u32 saveFPB0;
	u32 saveFPB1;
	u32 saveDPLL_B;
	u32 saveDPLL_B_MD;
	u32 saveHTOTAL_B;
	u32 saveHBLANK_B;
	u32 saveHSYNC_B;
	u32 saveVTOTAL_B;
	u32 saveVBLANK_B;
	u32 saveVSYNC_B;
	u32 saveBCLRPAT_B;
	u32 saveTRANSBCONF;
	u32 saveTRANS_HTOTAL_B;
	u32 saveTRANS_HBLANK_B;
	u32 saveTRANS_HSYNC_B;
	u32 saveTRANS_VTOTAL_B;
	u32 saveTRANS_VBLANK_B;
	u32 saveTRANS_VSYNC_B;
	u32 savePIPEBSTAT;
	u32 saveDSPBSTRIDE;
	u32 saveDSPBSIZE;
	u32 saveDSPBPOS;
	u32 saveDSPBADDR;
	u32 saveDSPBSURF;
	u32 saveDSPBTILEOFF;
	u32 saveVGA0;
	u32 saveVGA1;
	u32 saveVGA_PD;
	u32 saveVGACNTRL;
	u32 saveADPA;
	u32 saveLVDS;
	u32 savePP_ON_DELAYS;
	u32 savePP_OFF_DELAYS;
	u32 saveDVOA;
	u32 saveDVOB;
	u32 saveDVOC;
	u32 savePP_ON;
	u32 savePP_OFF;
	u32 savePP_CONTROL;
	u32 savePP_DIVISOR;
	u32 savePFIT_CONTROL;
	u32 save_palette_a[256];
	u32 save_palette_b[256];
	u32 saveDPFC_CB_BASE;
	u32 saveFBC_CFB_BASE;
	u32 saveFBC_LL_BASE;
	u32 saveFBC_CONTROL;
	u32 saveFBC_CONTROL2;
	u32 saveIER;
	u32 saveIIR;
	u32 saveIMR;
	u32 saveDEIER;
	u32 saveDEIMR;
	u32 saveGTIER;
	u32 saveGTIMR;
	u32 saveFDI_RXA_IMR;
	u32 saveFDI_RXB_IMR;
	u32 saveCACHE_MODE_0;
	u32 saveMI_ARB_STATE;
	u32 saveSWF0[16];
	u32 saveSWF1[16];
	u32 saveSWF2[3];
	u8 saveMSR;
	u8 saveSR[8];
	u8 saveGR[25];
	u8 saveAR_INDEX;
	u8 saveAR[21];
	u8 saveDACMASK;
	u8 saveCR[37];
	uint64_t saveFENCE[I915_MAX_NUM_FENCES];
	u32 saveCURACNTR;
	u32 saveCURAPOS;
	u32 saveCURABASE;
	u32 saveCURBCNTR;
	u32 saveCURBPOS;
	u32 saveCURBBASE;
	u32 saveCURSIZE;
	u32 saveDP_B;
	u32 saveDP_C;
	u32 saveDP_D;
	u32 savePIPEA_GMCH_DATA_M;
	u32 savePIPEB_GMCH_DATA_M;
	u32 savePIPEA_GMCH_DATA_N;
	u32 savePIPEB_GMCH_DATA_N;
	u32 savePIPEA_DP_LINK_M;
	u32 savePIPEB_DP_LINK_M;
	u32 savePIPEA_DP_LINK_N;
	u32 savePIPEB_DP_LINK_N;
	u32 saveFDI_RXA_CTL;
	u32 saveFDI_TXA_CTL;
	u32 saveFDI_RXB_CTL;
	u32 saveFDI_TXB_CTL;
	u32 savePFA_CTL_1;
	u32 savePFB_CTL_1;
	u32 savePFA_WIN_SZ;
	u32 savePFB_WIN_SZ;
	u32 savePFA_WIN_POS;
	u32 savePFB_WIN_POS;
	u32 savePCH_DREF_CONTROL;
	u32 saveDISP_ARB_CTL;
	u32 savePIPEA_DATA_M1;
	u32 savePIPEA_DATA_N1;
	u32 savePIPEA_LINK_M1;
	u32 savePIPEA_LINK_N1;
	u32 savePIPEB_DATA_M1;
	u32 savePIPEB_DATA_N1;
	u32 savePIPEB_LINK_M1;
	u32 savePIPEB_LINK_N1;
	u32 saveMCHBAR_RENDER_STANDBY;
	u32 savePCH_PORT_HOTPLUG;
};

struct intel_gen6_power_mgmt {
	struct work_struct work;
	struct delayed_work vlv_work;
	u32 pm_iir;
	/* lock - irqsave spinlock that protectects the work_struct and
	 * pm_iir. */
	spinlock_t lock;

	/* The below variables an all the rps hw state are protected by
	 * dev->struct mutext. */
	u8 cur_delay;
	u8 min_delay;
	u8 max_delay;
	u8 rpe_delay;
	u8 hw_max;

	struct delayed_work delayed_resume_work;

	/*
	 * Protects RPS/RC6 register access and PCU communication.
	 * Must be taken after struct_mutex if nested.
	 */
	struct mutex hw_lock;
};

/* defined intel_pm.c */
extern spinlock_t mchdev_lock;

struct intel_ilk_power_mgmt {
	u8 cur_delay;
	u8 min_delay;
	u8 max_delay;
	u8 fmax;
	u8 fstart;

	u64 last_count1;
	unsigned long last_time1;
	unsigned long chipset_power;
	u64 last_count2;
	struct timespec last_time2;
	unsigned long gfx_power;
	u8 corr;

	int c_m;
	int r_t;

	struct drm_i915_gem_object *pwrctx;
	struct drm_i915_gem_object *renderctx;
};

struct i915_dri1_state {
	unsigned allow_batchbuffer : 1;
	u32 __iomem *gfx_hws_cpu_addr;

	unsigned int cpp;
	int back_offset;
	int front_offset;
	int current_page;
	int page_flipping;

	uint32_t counter;
};

struct intel_l3_parity {
	u32 *remap_info;
	struct work_struct error_work;
};

struct i915_gem_mm {
	/** Memory allocator for GTT stolen memory */
	struct drm_mm stolen;
	/** Memory allocator for GTT */
	struct drm_mm gtt_space;
	/** List of all objects in gtt_space. Used to restore gtt
	 * mappings on resume */
	struct list_head bound_list;
	/**
	 * List of objects which are not bound to the GTT (thus
	 * are idle and not used by the GPU) but still have
	 * (presumably uncached) pages still attached.
	 */
	struct list_head unbound_list;

	/** Usable portion of the GTT for GEM */
	unsigned long stolen_base; /* limited to low memory (32-bit) */

	int gtt_mtrr;

	/** PPGTT used for aliasing the PPGTT with the GTT */
	struct i915_hw_ppgtt *aliasing_ppgtt;

	struct shrinker inactive_shrinker;
	bool shrinker_no_lock_stealing;

	/**
	 * List of objects currently involved in rendering.
	 *
	 * Includes buffers having the contents of their GPU caches
	 * flushed, not necessarily primitives.  last_rendering_seqno
	 * represents when the rendering involved will be completed.
	 *
	 * A reference is held on the buffer while on this list.
	 */
	struct list_head active_list;

	/**
	 * LRU list of objects which are not in the ringbuffer and
	 * are ready to unbind, but are still in the GTT.
	 *
	 * last_rendering_seqno is 0 while an object is in this list.
	 *
	 * A reference is not held on the buffer while on this list,
	 * as merely being GTT-bound shouldn't prevent its being
	 * freed, and we'll pull it off the list in the free path.
	 */
	struct list_head inactive_list;

	/** LRU list of objects with fence regs on them. */
	struct list_head fence_list;

	/**
	 * We leave the user IRQ off as much as possible,
	 * but this means that requests will finish and never
	 * be retired once the system goes idle. Set a timer to
	 * fire periodically while the ring is running. When it
	 * fires, go retire requests.
	 */
	struct delayed_work retire_work;

	/**
	 * Are we in a non-interruptible section of code like
	 * modesetting?
	 */
	bool interruptible;

	/**
	 * Flag if the X Server, and thus DRM, is not currently in
	 * control of the device.
	 *
	 * This is set between LeaveVT and EnterVT.  It needs to be
	 * replaced with a semaphore.  It also needs to be
	 * transitioned away from for kernel modesetting.
	 */
	int suspended;

	/** Bit 6 swizzling required for X tiling */
	uint32_t bit_6_swizzle_x;
	/** Bit 6 swizzling required for Y tiling */
	uint32_t bit_6_swizzle_y;

	/* storage for physical objects */
	struct drm_i915_gem_phys_object *phys_objs[I915_MAX_PHYS_OBJECT];

	/* accounting, useful for userland debugging */
	size_t object_memory;
	u32 object_count;
};

struct i915_gpu_error {
	/* For hangcheck timer */
#define DRM_I915_HANGCHECK_PERIOD 1500 /* in ms */
#define DRM_I915_HANGCHECK_JIFFIES msecs_to_jiffies(DRM_I915_HANGCHECK_PERIOD)
	struct timer_list hangcheck_timer;
	int hangcheck_count;
	uint32_t last_acthd[I915_NUM_RINGS];
	uint32_t prev_instdone[I915_NUM_INSTDONE_REG];

	/* For reset and error_state handling. */
	spinlock_t lock;
	/* Protected by the above dev->gpu_error.lock. */
	struct drm_i915_error_state *first_error;
	struct work_struct work;

	unsigned long last_reset;

	/**
	 * State variable and reset counter controlling the reset flow
	 *
	 * Upper bits are for the reset counter.  This counter is used by the
	 * wait_seqno code to race-free noticed that a reset event happened and
	 * that it needs to restart the entire ioctl (since most likely the
	 * seqno it waited for won't ever signal anytime soon).
	 *
	 * This is important for lock-free wait paths, where no contended lock
	 * naturally enforces the correct ordering between the bail-out of the
	 * waiter and the gpu reset work code.
	 *
	 * Lowest bit controls the reset state machine: Set means a reset is in
	 * progress. This state will (presuming we don't have any bugs) decay
	 * into either unset (successful reset) or the special WEDGED value (hw
	 * terminally sour). All waiters on the reset_queue will be woken when
	 * that happens.
	 */
	atomic_t reset_counter;

	/**
	 * Special values/flags for reset_counter
	 *
	 * Note that the code relies on
	 * 	I915_WEDGED & I915_RESET_IN_PROGRESS_FLAG
	 * being true.
	 */
#define I915_RESET_IN_PROGRESS_FLAG	1
#define I915_WEDGED			0xffffffff

	/**
	 * Waitqueue to signal when the reset has completed. Used by clients
	 * that wait for dev_priv->mm.wedged to settle.
	 */
	wait_queue_head_t reset_queue;

	/* For gpu hang simulation. */
	unsigned int stop_rings;
};

enum modeset_restore {
	MODESET_ON_LID_OPEN,
	MODESET_DONE,
	MODESET_SUSPENDED,
};

struct intel_vbt_data {
	struct drm_display_mode *lfp_lvds_vbt_mode; /* if any */
	struct drm_display_mode *sdvo_lvds_vbt_mode; /* if any */

	/* Feature bits */
	unsigned int int_tv_support:1;
	unsigned int lvds_dither:1;
	unsigned int lvds_vbt:1;
	unsigned int int_crt_support:1;
	unsigned int lvds_use_ssc:1;
	unsigned int display_clock_mode:1;
	unsigned int fdi_rx_polarity_inverted:1;
	int lvds_ssc_freq;
	unsigned int bios_lvds_val; /* initial [PCH_]LVDS reg val in VBIOS */

	/* eDP */
	int edp_rate;
	int edp_lanes;
	int edp_preemphasis;
	int edp_vswing;
	bool edp_initialized;
	bool edp_support;
	int edp_bpp;
	struct edp_power_seq edp_pps;

	int crt_ddc_pin;

	int child_dev_num;
	struct child_device_config *child_dev;
};

typedef struct drm_i915_private {
	struct drm_device *dev;
	struct kmem_cache *slab;

	const struct intel_device_info *info;

	int relative_constants_mode;

	void __iomem *regs;

	struct drm_i915_gt_funcs gt;
	/** gt_fifo_count and the subsequent register write are synchronized
	 * with dev->struct_mutex. */
	unsigned gt_fifo_count;
	/** forcewake_count is protected by gt_lock */
	unsigned forcewake_count;
	/** gt_lock is also taken in irq contexts. */
	spinlock_t gt_lock;

	struct intel_gmbus gmbus[GMBUS_NUM_PORTS];


	/** gmbus_mutex protects against concurrent usage of the single hw gmbus
	 * controller on different i2c buses. */
	struct mutex gmbus_mutex;

	/**
	 * Base address of the gmbus and gpio block.
	 */
	uint32_t gpio_mmio_base;

	wait_queue_head_t gmbus_wait_queue;

	struct pci_dev *bridge_dev;
	struct intel_ring_buffer ring[I915_NUM_RINGS];
	uint32_t last_seqno, next_seqno;

	drm_dma_handle_t *status_page_dmah;
	struct resource mch_res;

	atomic_t irq_received;

	/* protects the irq masks */
	spinlock_t irq_lock;

	/* To control wakeup latency, e.g. for irq-driven dp aux transfers. */
	struct pm_qos_request pm_qos;

	/* DPIO indirect register protection */
	struct mutex dpio_lock;

	/** Cached value of IMR to avoid reads in updating the bitfield */
	u32 irq_mask;
	u32 gt_irq_mask;

	struct work_struct hotplug_work;
	bool enable_hotplug_processing;
	struct {
		unsigned long hpd_last_jiffies;
		int hpd_cnt;
		enum {
			HPD_ENABLED = 0,
			HPD_DISABLED = 1,
			HPD_MARK_DISABLED = 2
		} hpd_mark;
	} hpd_stats[HPD_NUM_PINS];
	u32 hpd_event_bits;
	struct timer_list hotplug_reenable_timer;

	int num_pch_pll;
	int num_plane;

	unsigned long cfb_size;
	unsigned int cfb_fb;
	enum plane cfb_plane;
	int cfb_y;
	struct intel_fbc_work *fbc_work;

	struct intel_opregion opregion;
	struct intel_vbt_data vbt;

	/* overlay */
	struct intel_overlay *overlay;
	unsigned int sprite_scaling_enabled;

	/* backlight */
	struct {
		int level;
		bool enabled;
		spinlock_t lock; /* bl registers and the above bl fields */
		struct backlight_device *device;
	} backlight;

	/* LVDS info */
	struct drm_display_mode *lfp_lvds_vbt_mode; /* if any */
	struct drm_display_mode *sdvo_lvds_vbt_mode; /* if any */
	bool no_aux_handshake;

	struct drm_i915_fence_reg fence_regs[I915_MAX_NUM_FENCES]; /* assume 965 */
	int fence_reg_start; /* 4 if userland hasn't ioctl'd us yet */
	int num_fence_regs; /* 8 on pre-965, 16 otherwise */

	unsigned int fsb_freq, mem_freq, is_ddr3;

	struct workqueue_struct *wq;

	/* Display functions */
	struct drm_i915_display_funcs display;

	/* PCH chipset type */
	enum intel_pch pch_type;
	unsigned short pch_id;

	unsigned long quirks;

	enum modeset_restore modeset_restore;
	struct mutex modeset_restore_lock;

	struct i915_gtt gtt;

	struct i915_gem_mm mm;

	/* Kernel Modesetting */

	struct sdvo_device_mapping sdvo_mappings[2];

	struct drm_crtc *plane_to_crtc_mapping[3];
	struct drm_crtc *pipe_to_crtc_mapping[3];
	wait_queue_head_t pending_flip_queue;

	struct intel_pch_pll pch_plls[I915_NUM_PLLS];
	struct intel_ddi_plls ddi_plls;

	/* Reclocking support */
	bool render_reclock_avail;
	bool lvds_downclock_avail;
	/* indicates the reduced downclock for LVDS*/
	int lvds_downclock;
	u16 orig_clock;

	bool mchbar_need_disable;

	struct intel_l3_parity l3_parity;

	/* gen6+ rps state */
	struct intel_gen6_power_mgmt rps;

	/* ilk-only ips/rps state. Everything in here is protected by the global
	 * mchdev_lock in intel_pm.c */
	struct intel_ilk_power_mgmt ips;

	enum no_fbc_reason no_fbc_reason;

	struct drm_mm_node *compressed_fb;
	struct drm_mm_node *compressed_llb;

	struct i915_gpu_error gpu_error;

	struct drm_i915_gem_object *vlv_pctx;

	/* list of fbdev register on this device */
	struct intel_fbdev *fbdev;

	/*
	 * The console may be contended at resume, but we don't
	 * want it to block on it.
	 */
	struct work_struct console_resume_work;

	struct drm_property *broadcast_rgb_property;
	struct drm_property *force_audio_property;

	bool hw_contexts_disabled;
	uint32_t hw_context_size;

	u32 fdi_rx_config;

	struct i915_suspend_saved_registers regfile;

	/* Old dri1 support infrastructure, beware the dragons ya fools entering
	 * here! */
	struct i915_dri1_state dri1;
} drm_i915_private_t;

/* Iterate over initialised rings */
#define for_each_ring(ring__, dev_priv__, i__) \
	for ((i__) = 0; (i__) < I915_NUM_RINGS; (i__)++) \
		if (((ring__) = &(dev_priv__)->ring[(i__)]), intel_ring_initialized((ring__)))

enum hdmi_force_audio {
	HDMI_AUDIO_OFF_DVI = -2,	/* no aux data for HDMI-DVI converter */
	HDMI_AUDIO_OFF,			/* force turn off HDMI audio */
	HDMI_AUDIO_AUTO,		/* trust EDID */
	HDMI_AUDIO_ON,			/* force turn on HDMI audio */
};

#define I915_GTT_RESERVED ((struct drm_mm_node *)0x1)

struct drm_i915_gem_object_ops {
	/* Interface between the GEM object and its backing storage.
	 * get_pages() is called once prior to the use of the associated set
	 * of pages before to binding them into the GTT, and put_pages() is
	 * called after we no longer need them. As we expect there to be
	 * associated cost with migrating pages between the backing storage
	 * and making them available for the GPU (e.g. clflush), we may hold
	 * onto the pages after they are no longer referenced by the GPU
	 * in case they may be used again shortly (for example migrating the
	 * pages to a different memory domain within the GTT). put_pages()
	 * will therefore most likely be called when the object itself is
	 * being released or under memory pressure (where we attempt to
	 * reap pages for the shrinker).
	 */
	int (*get_pages)(struct drm_i915_gem_object *);
	void (*put_pages)(struct drm_i915_gem_object *);
};

struct drm_i915_gem_object {
	struct drm_gem_object base;

	const struct drm_i915_gem_object_ops *ops;

	/** Current space allocated to this object in the GTT, if any. */
	struct drm_mm_node *gtt_space;
	/** Stolen memory for this object, instead of being backed by shmem. */
	struct drm_mm_node *stolen;
	struct list_head gtt_list;

	/** This object's place on the active/inactive lists */
	struct list_head ring_list;
	struct list_head mm_list;
	/** This object's place in the batchbuffer or on the eviction list */
	struct list_head exec_list;

	/**
	 * This is set if the object is on the active lists (has pending
	 * rendering and so a non-zero seqno), and is not set if it i s on
	 * inactive (ready to be unbound) list.
	 */
	unsigned int active:1;

	/**
	 * This is set if the object has been written to since last bound
	 * to the GTT
	 */
	unsigned int dirty:1;

	/**
	 * Fence register bits (if any) for this object.  Will be set
	 * as needed when mapped into the GTT.
	 * Protected by dev->struct_mutex.
	 */
	signed int fence_reg:I915_MAX_NUM_FENCE_BITS;

	/**
	 * Advice: are the backing pages purgeable?
	 */
	unsigned int madv:2;

	/**
	 * Current tiling mode for the object.
	 */
	unsigned int tiling_mode:2;
	/**
	 * Whether the tiling parameters for the currently associated fence
	 * register have changed. Note that for the purposes of tracking
	 * tiling changes we also treat the unfenced register, the register
	 * slot that the object occupies whilst it executes a fenced
	 * command (such as BLT on gen2/3), as a "fence".
	 */
	unsigned int fence_dirty:1;

	/** How many users have pinned this object in GTT space. The following
	 * users can each hold at most one reference: pwrite/pread, pin_ioctl
	 * (via user_pin_count), execbuffer (objects are not allowed multiple
	 * times for the same batchbuffer), and the framebuffer code. When
	 * switching/pageflipping, the framebuffer code has at most two buffers
	 * pinned per crtc.
	 *
	 * In the worst case this is 1 + 1 + 1 + 2*2 = 7. That would fit into 3
	 * bits with absolutely no headroom. So use 4 bits. */
	unsigned int pin_count:4;
#define DRM_I915_GEM_OBJECT_MAX_PIN_COUNT 0xf

	/**
	 * Is the object at the current location in the gtt mappable and
	 * fenceable? Used to avoid costly recalculations.
	 */
	unsigned int map_and_fenceable:1;

	/**
	 * Whether the current gtt mapping needs to be mappable (and isn't just
	 * mappable by accident). Track pin and fault separate for a more
	 * accurate mappable working set.
	 */
	unsigned int fault_mappable:1;
	unsigned int pin_mappable:1;

	/*
	 * Is the GPU currently using a fence to access this buffer,
	 */
	unsigned int pending_fenced_gpu_access:1;
	unsigned int fenced_gpu_access:1;

	unsigned int cache_level:2;

	unsigned int has_aliasing_ppgtt_mapping:1;
	unsigned int has_global_gtt_mapping:1;
	unsigned int has_dma_mapping:1;

	struct sg_table *pages;
	int pages_pin_count;

	/* prime dma-buf support */
	void *dma_buf_vmapping;
	int vmapping_count;

	/**
	 * Used for performing relocations during execbuffer insertion.
	 */
	struct hlist_node exec_node;
	unsigned long exec_handle;
	struct drm_i915_gem_exec_object2 *exec_entry;

	/**
	 * Current offset of the object in GTT space.
	 *
	 * This is the same as gtt_space->start
	 */
	uint32_t gtt_offset;

	struct intel_ring_buffer *ring;

	/** Breadcrumb of last rendering to the buffer. */
	uint32_t last_read_seqno;
	uint32_t last_write_seqno;
	/** Breadcrumb of last fenced GPU access to the buffer. */
	uint32_t last_fenced_seqno;

	/** Current tiling stride for the object, if it's tiled. */
	uint32_t stride;

	/** Record of address bit 17 of each page at last unbind. */
	unsigned long *bit_17;

	/** User space pin count and filp owning the pin */
	uint32_t user_pin_count;
	struct drm_file *pin_filp;

	/** for phy allocated objects */
	struct drm_i915_gem_phys_object *phys_obj;
};
#define to_gem_object(obj) (&((struct drm_i915_gem_object *)(obj))->base)

#define to_intel_bo(x) container_of(x, struct drm_i915_gem_object, base)

/**
 * Request queue structure.
 *
 * The request queue allows us to note sequence numbers that have been emitted
 * and may be associated with active buffers to be retired.
 *
 * By keeping this list, we can avoid having to do questionable
 * sequence-number comparisons on buffer last_rendering_seqnos, and associate
 * an emission time with seqnos for tracking how far ahead of the GPU we are.
 */
struct drm_i915_gem_request {
	/** On Which ring this request was generated */
	struct intel_ring_buffer *ring;

	/** GEM sequence number associated with this request. */
	uint32_t seqno;

	/** Postion in the ringbuffer of the end of the request */
	u32 tail;

	/** Context related to this request */
	struct i915_hw_context *ctx;

	/** Time at which this request was emitted, in jiffies. */
	unsigned long emitted_jiffies;

	/** global list entry for this request */
	struct list_head list;

	struct drm_i915_file_private *file_priv;
	/** file_priv list entry for this request */
	struct list_head client_list;
};

struct drm_i915_file_private {
	struct {
		spinlock_t lock;
		struct list_head request_list;
	} mm;
	struct idr context_idr;
};

#define INTEL_INFO(dev)	(((struct drm_i915_private *) (dev)->dev_private)->info)

#define IS_I830(dev)		((dev)->pci_device == 0x3577)
#define IS_845G(dev)		((dev)->pci_device == 0x2562)
#define IS_I85X(dev)		(INTEL_INFO(dev)->is_i85x)
#define IS_I865G(dev)		((dev)->pci_device == 0x2572)
#define IS_I915G(dev)		(INTEL_INFO(dev)->is_i915g)
#define IS_I915GM(dev)		((dev)->pci_device == 0x2592)
#define IS_I945G(dev)		((dev)->pci_device == 0x2772)
#define IS_I945GM(dev)		(INTEL_INFO(dev)->is_i945gm)
#define IS_BROADWATER(dev)	(INTEL_INFO(dev)->is_broadwater)
#define IS_CRESTLINE(dev)	(INTEL_INFO(dev)->is_crestline)
#define IS_GM45(dev)		((dev)->pci_device == 0x2A42)
#define IS_G4X(dev)		(INTEL_INFO(dev)->is_g4x)
#define IS_PINEVIEW_G(dev)	((dev)->pci_device == 0xa001)
#define IS_PINEVIEW_M(dev)	((dev)->pci_device == 0xa011)
#define IS_PINEVIEW(dev)	(INTEL_INFO(dev)->is_pineview)
#define IS_G33(dev)		(INTEL_INFO(dev)->is_g33)
#define IS_IRONLAKE_D(dev)	((dev)->pci_device == 0x0042)
#define IS_IRONLAKE_M(dev)	((dev)->pci_device == 0x0046)
#define IS_IVYBRIDGE(dev)	(INTEL_INFO(dev)->is_ivybridge)
#define IS_IVB_GT1(dev)		((dev)->pci_device == 0x0156 || \
				 (dev)->pci_device == 0x0152 ||	\
				 (dev)->pci_device == 0x015a)
#define IS_SNB_GT1(dev)		((dev)->pci_device == 0x0102 || \
				 (dev)->pci_device == 0x0106 ||	\
				 (dev)->pci_device == 0x010A)
#define IS_VALLEYVIEW(dev)	(INTEL_INFO(dev)->is_valleyview)
#define IS_HASWELL(dev)	(INTEL_INFO(dev)->is_haswell)
#define IS_MOBILE(dev)		(INTEL_INFO(dev)->is_mobile)
#define IS_ULT(dev)		(IS_HASWELL(dev) && \
				 ((dev)->pci_device & 0xFF00) == 0x0A00)

/*
 * The genX designation typically refers to the render engine, so render
 * capability related checks should use IS_GEN, while display and other checks
 * have their own (e.g. HAS_PCH_SPLIT for ILK+ display, IS_foo for particular
 * chips, etc.).
 */
#define IS_GEN2(dev)	(INTEL_INFO(dev)->gen == 2)
#define IS_GEN3(dev)	(INTEL_INFO(dev)->gen == 3)
#define IS_GEN4(dev)	(INTEL_INFO(dev)->gen == 4)
#define IS_GEN5(dev)	(INTEL_INFO(dev)->gen == 5)
#define IS_GEN6(dev)	(INTEL_INFO(dev)->gen == 6)
#define IS_GEN7(dev)	(INTEL_INFO(dev)->gen == 7)

#define HAS_BSD(dev)            (INTEL_INFO(dev)->has_bsd_ring)
#define HAS_BLT(dev)            (INTEL_INFO(dev)->has_blt_ring)
#define HAS_LLC(dev)            (INTEL_INFO(dev)->has_llc)
#define I915_NEED_GFX_HWS(dev)	(INTEL_INFO(dev)->need_gfx_hws)

#define HAS_HW_CONTEXTS(dev)	(INTEL_INFO(dev)->gen >= 6)
#define HAS_ALIASING_PPGTT(dev)	(INTEL_INFO(dev)->gen >=6 && !IS_VALLEYVIEW(dev))

#define HAS_OVERLAY(dev)		(INTEL_INFO(dev)->has_overlay)
#define OVERLAY_NEEDS_PHYSICAL(dev)	(INTEL_INFO(dev)->overlay_needs_physical)

/* Early gen2 have a totally busted CS tlb and require pinned batches. */
#define HAS_BROKEN_CS_TLB(dev)		(IS_I830(dev) || IS_845G(dev))

/* With the 945 and later, Y tiling got adjusted so that it was 32 128-byte
 * rows, which changed the alignment requirements and fence programming.
 */
#define HAS_128_BYTE_Y_TILING(dev) (!IS_GEN2(dev) && !(IS_I915G(dev) || \
						      IS_I915GM(dev)))
#define SUPPORTS_DIGITAL_OUTPUTS(dev)	(!IS_GEN2(dev) && !IS_PINEVIEW(dev))
#define SUPPORTS_INTEGRATED_HDMI(dev)	(IS_G4X(dev) || IS_GEN5(dev))
#define SUPPORTS_INTEGRATED_DP(dev)	(IS_G4X(dev) || IS_GEN5(dev))
#define SUPPORTS_EDP(dev)		(IS_IRONLAKE_M(dev))
#define SUPPORTS_TV(dev)		(INTEL_INFO(dev)->supports_tv)
#define I915_HAS_HOTPLUG(dev)		 (INTEL_INFO(dev)->has_hotplug)
/* dsparb controlled by hw only */
#define DSPARB_HWCONTROL(dev) (IS_G4X(dev) || IS_IRONLAKE(dev))

#define HAS_FW_BLC(dev) (INTEL_INFO(dev)->gen > 2)
#define HAS_PIPE_CXSR(dev) (INTEL_INFO(dev)->has_pipe_cxsr)
#define I915_HAS_FBC(dev) (INTEL_INFO(dev)->has_fbc)

#define HAS_PIPE_CONTROL(dev) (INTEL_INFO(dev)->gen >= 5)

#define HAS_DDI(dev)		(INTEL_INFO(dev)->has_ddi)
#define HAS_POWER_WELL(dev)	(IS_HASWELL(dev))
#define HAS_FPGA_DBG_UNCLAIMED(dev)	(INTEL_INFO(dev)->has_fpga_dbg)

#define INTEL_PCH_DEVICE_ID_MASK		0xff00
#define INTEL_PCH_IBX_DEVICE_ID_TYPE		0x3b00
#define INTEL_PCH_CPT_DEVICE_ID_TYPE		0x1c00
#define INTEL_PCH_PPT_DEVICE_ID_TYPE		0x1e00
#define INTEL_PCH_LPT_DEVICE_ID_TYPE		0x8c00
#define INTEL_PCH_LPT_LP_DEVICE_ID_TYPE		0x9c00

#define INTEL_PCH_TYPE(dev) (((struct drm_i915_private *)(dev)->dev_private)->pch_type)
#define HAS_PCH_LPT(dev) (INTEL_PCH_TYPE(dev) == PCH_LPT)
#define HAS_PCH_CPT(dev) (INTEL_PCH_TYPE(dev) == PCH_CPT)
#define HAS_PCH_IBX(dev) (INTEL_PCH_TYPE(dev) == PCH_IBX)
#define HAS_PCH_NOP(dev) (INTEL_PCH_TYPE(dev) == PCH_NOP)
#define HAS_PCH_SPLIT(dev) (INTEL_PCH_TYPE(dev) != PCH_NONE)

#define HAS_FORCE_WAKE(dev) (INTEL_INFO(dev)->has_force_wake)

#define HAS_L3_GPU_CACHE(dev) (IS_IVYBRIDGE(dev) || IS_HASWELL(dev))

#define GT_FREQUENCY_MULTIPLIER 50

#include "i915_trace.h"

/**
 * RC6 is a special power stage which allows the GPU to enter an very
 * low-voltage mode when idle, using down to 0V while at this stage.  This
 * stage is entered automatically when the GPU is idle when RC6 support is
 * enabled, and as soon as new workload arises GPU wakes up automatically as well.
 *
 * There are different RC6 modes available in Intel GPU, which differentiate
 * among each other with the latency required to enter and leave RC6 and
 * voltage consumed by the GPU in different states.
 *
 * The combination of the following flags define which states GPU is allowed
 * to enter, while RC6 is the normal RC6 state, RC6p is the deep RC6, and
 * RC6pp is deepest RC6. Their support by hardware varies according to the
 * GPU, BIOS, chipset and platform. RC6 is usually the safest one and the one
 * which brings the most power savings; deeper states save more power, but
 * require higher latency to switch to and wake up.
 */
#define INTEL_RC6_ENABLE			(1<<0)
#define INTEL_RC6p_ENABLE			(1<<1)
#define INTEL_RC6pp_ENABLE			(1<<2)

extern struct drm_ioctl_desc i915_ioctls[];
extern int i915_max_ioctl;
extern unsigned int i915_fbpercrtc __always_unused;
extern int i915_panel_ignore_lid __read_mostly;
extern unsigned int i915_powersave __read_mostly;
extern int i915_semaphores __read_mostly;
extern unsigned int i915_lvds_downclock __read_mostly;
extern int i915_lvds_channel_mode __read_mostly;
extern int i915_panel_use_ssc __read_mostly;
extern int i915_vbt_sdvo_panel_type __read_mostly;
extern int i915_enable_rc6 __read_mostly;
extern int i915_enable_fbc __read_mostly;
extern bool i915_enable_hangcheck __read_mostly;
extern int i915_enable_ppgtt __read_mostly;
extern unsigned int i915_preliminary_hw_support __read_mostly;
extern int i915_disable_power_well __read_mostly;

extern int i915_suspend(struct drm_device *dev, pm_message_t state);
extern int i915_resume(struct drm_device *dev);
extern int i915_master_create(struct drm_device *dev, struct drm_master *master);
extern void i915_master_destroy(struct drm_device *dev, struct drm_master *master);

				/* i915_dma.c */
void i915_update_dri1_breadcrumb(struct drm_device *dev);
extern void i915_kernel_lost_context(struct drm_device * dev);
extern int i915_driver_load(struct drm_device *, unsigned long flags);
extern int i915_driver_unload(struct drm_device *);
extern int i915_driver_open(struct drm_device *dev, struct drm_file *file_priv);
extern void i915_driver_lastclose(struct drm_device * dev);
extern void i915_driver_preclose(struct drm_device *dev,
				 struct drm_file *file_priv);
extern void i915_driver_postclose(struct drm_device *dev,
				  struct drm_file *file_priv);
extern int i915_driver_device_is_agp(struct drm_device * dev);
#ifdef CONFIG_COMPAT
extern long i915_compat_ioctl(struct file *filp, unsigned int cmd,
			      unsigned long arg);
#endif
extern int i915_emit_box(struct drm_device *dev,
			 struct drm_clip_rect *box,
			 int DR1, int DR4);
extern int intel_gpu_reset(struct drm_device *dev);
extern int i915_reset(struct drm_device *dev);
extern unsigned long i915_chipset_val(struct drm_i915_private *dev_priv);
extern unsigned long i915_mch_val(struct drm_i915_private *dev_priv);
extern unsigned long i915_gfx_val(struct drm_i915_private *dev_priv);
extern void i915_update_gfx_val(struct drm_i915_private *dev_priv);

extern void intel_console_resume(struct work_struct *work);

/* i915_irq.c */
void i915_hangcheck_elapsed(unsigned long data);
void i915_handle_error(struct drm_device *dev, bool wedged);

extern void intel_irq_init(struct drm_device *dev);
extern void intel_pm_init(struct drm_device *dev);
extern void intel_hpd_init(struct drm_device *dev);
extern void intel_gt_init(struct drm_device *dev);
extern void intel_gt_sanitize(struct drm_device *dev);

void i915_error_state_free(struct kref *error_ref);

void
i915_enable_pipestat(drm_i915_private_t *dev_priv, int pipe, u32 mask);

void
i915_disable_pipestat(drm_i915_private_t *dev_priv, int pipe, u32 mask);

#ifdef CONFIG_DEBUG_FS
extern void i915_destroy_error_state(struct drm_device *dev);
#else
#define i915_destroy_error_state(x)
#endif


/* i915_gem.c */
int i915_gem_init_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_create_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *file_priv);
int i915_gem_pread_ioctl(struct drm_device *dev, void *data,
			 struct drm_file *file_priv);
int i915_gem_pwrite_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *file_priv);
int i915_gem_mmap_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_mmap_gtt_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_set_domain_ioctl(struct drm_device *dev, void *data,
			      struct drm_file *file_priv);
int i915_gem_sw_finish_ioctl(struct drm_device *dev, void *data,
			     struct drm_file *file_priv);
int i915_gem_execbuffer(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_execbuffer2(struct drm_device *dev, void *data,
			 struct drm_file *file_priv);
int i915_gem_pin_ioctl(struct drm_device *dev, void *data,
		       struct drm_file *file_priv);
int i915_gem_unpin_ioctl(struct drm_device *dev, void *data,
			 struct drm_file *file_priv);
int i915_gem_busy_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_get_caching_ioctl(struct drm_device *dev, void *data,
			       struct drm_file *file);
int i915_gem_set_caching_ioctl(struct drm_device *dev, void *data,
			       struct drm_file *file);
int i915_gem_throttle_ioctl(struct drm_device *dev, void *data,
			    struct drm_file *file_priv);
int i915_gem_madvise_ioctl(struct drm_device *dev, void *data,
			   struct drm_file *file_priv);
int i915_gem_entervt_ioctl(struct drm_device *dev, void *data,
			   struct drm_file *file_priv);
int i915_gem_leavevt_ioctl(struct drm_device *dev, void *data,
			   struct drm_file *file_priv);
int i915_gem_set_tiling(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_get_tiling(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int i915_gem_get_aperture_ioctl(struct drm_device *dev, void *data,
				struct drm_file *file_priv);
int i915_gem_wait_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
void i915_gem_load(struct drm_device *dev);
void *i915_gem_object_alloc(struct drm_device *dev);
void i915_gem_object_free(struct drm_i915_gem_object *obj);
int i915_gem_init_object(struct drm_gem_object *obj);
void i915_gem_object_init(struct drm_i915_gem_object *obj,
			 const struct drm_i915_gem_object_ops *ops);
struct drm_i915_gem_object *i915_gem_alloc_object(struct drm_device *dev,
						  size_t size);
void i915_gem_free_object(struct drm_gem_object *obj);

int __must_check i915_gem_object_pin(struct drm_i915_gem_object *obj,
				     uint32_t alignment,
				     bool map_and_fenceable,
				     bool nonblocking);
void i915_gem_object_unpin(struct drm_i915_gem_object *obj);
int __must_check i915_gem_object_unbind(struct drm_i915_gem_object *obj);
int i915_gem_object_put_pages(struct drm_i915_gem_object *obj);
void i915_gem_release_mmap(struct drm_i915_gem_object *obj);
void i915_gem_lastclose(struct drm_device *dev);

int __must_check i915_gem_object_get_pages(struct drm_i915_gem_object *obj);
static inline struct page *i915_gem_object_get_page(struct drm_i915_gem_object *obj, int n)
{
	struct sg_page_iter sg_iter;

	for_each_sg_page(obj->pages->sgl, &sg_iter, obj->pages->nents, n)
		return sg_page_iter_page(&sg_iter);

	return NULL;
}
static inline void i915_gem_object_pin_pages(struct drm_i915_gem_object *obj)
{
	BUG_ON(obj->pages == NULL);
	obj->pages_pin_count++;
}
static inline void i915_gem_object_unpin_pages(struct drm_i915_gem_object *obj)
{
	BUG_ON(obj->pages_pin_count == 0);
	obj->pages_pin_count--;
}

int __must_check i915_mutex_lock_interruptible(struct drm_device *dev);
int i915_gem_object_sync(struct drm_i915_gem_object *obj,
			 struct intel_ring_buffer *to);
void i915_gem_object_move_to_active(struct drm_i915_gem_object *obj,
				    struct intel_ring_buffer *ring);

int i915_gem_dumb_create(struct drm_file *file_priv,
			 struct drm_device *dev,
			 struct drm_mode_create_dumb *args);
int i915_gem_mmap_gtt(struct drm_file *file_priv, struct drm_device *dev,
		      uint32_t handle, uint64_t *offset);
int i915_gem_dumb_destroy(struct drm_file *file_priv, struct drm_device *dev,
			  uint32_t handle);
/**
 * Returns true if seq1 is later than seq2.
 */
static inline bool
i915_seqno_passed(uint32_t seq1, uint32_t seq2)
{
	return (int32_t)(seq1 - seq2) >= 0;
}

int __must_check i915_gem_get_seqno(struct drm_device *dev, u32 *seqno);
int __must_check i915_gem_set_seqno(struct drm_device *dev, u32 seqno);
int __must_check i915_gem_object_get_fence(struct drm_i915_gem_object *obj);
int __must_check i915_gem_object_put_fence(struct drm_i915_gem_object *obj);

static inline bool
i915_gem_object_pin_fence(struct drm_i915_gem_object *obj)
{
	if (obj->fence_reg != I915_FENCE_REG_NONE) {
		struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
		dev_priv->fence_regs[obj->fence_reg].pin_count++;
		return true;
	} else
		return false;
}

static inline void
i915_gem_object_unpin_fence(struct drm_i915_gem_object *obj)
{
	if (obj->fence_reg != I915_FENCE_REG_NONE) {
		struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
		dev_priv->fence_regs[obj->fence_reg].pin_count--;
	}
}

void i915_gem_retire_requests(struct drm_device *dev);
void i915_gem_retire_requests_ring(struct intel_ring_buffer *ring);
int __must_check i915_gem_check_wedge(struct i915_gpu_error *error,
				      bool interruptible);
static inline bool i915_reset_in_progress(struct i915_gpu_error *error)
{
	return unlikely(atomic_read(&error->reset_counter)
			& I915_RESET_IN_PROGRESS_FLAG);
}

static inline bool i915_terminally_wedged(struct i915_gpu_error *error)
{
	return atomic_read(&error->reset_counter) == I915_WEDGED;
}

void i915_gem_reset(struct drm_device *dev);
void i915_gem_clflush_object(struct drm_i915_gem_object *obj);
int __must_check i915_gem_object_set_domain(struct drm_i915_gem_object *obj,
					    uint32_t read_domains,
					    uint32_t write_domain);
int __must_check i915_gem_object_finish_gpu(struct drm_i915_gem_object *obj);
int __must_check i915_gem_init(struct drm_device *dev);
int __must_check i915_gem_init_hw(struct drm_device *dev);
void i915_gem_l3_remap(struct drm_device *dev);
void i915_gem_init_swizzling(struct drm_device *dev);
void i915_gem_cleanup_ringbuffer(struct drm_device *dev);
int __must_check i915_gpu_idle(struct drm_device *dev);
int __must_check i915_gem_idle(struct drm_device *dev);
int i915_add_request(struct intel_ring_buffer *ring,
		     struct drm_file *file,
		     u32 *seqno);
int __must_check i915_wait_seqno(struct intel_ring_buffer *ring,
				 uint32_t seqno);
int i915_gem_fault(struct vm_area_struct *vma, struct vm_fault *vmf);
int __must_check
i915_gem_object_set_to_gtt_domain(struct drm_i915_gem_object *obj,
				  bool write);
int __must_check
i915_gem_object_set_to_cpu_domain(struct drm_i915_gem_object *obj, bool write);
int __must_check
i915_gem_object_pin_to_display_plane(struct drm_i915_gem_object *obj,
				     u32 alignment,
				     struct intel_ring_buffer *pipelined);
int i915_gem_attach_phys_object(struct drm_device *dev,
				struct drm_i915_gem_object *obj,
				int id,
				int align);
void i915_gem_detach_phys_object(struct drm_device *dev,
				 struct drm_i915_gem_object *obj);
void i915_gem_free_all_phys_object(struct drm_device *dev);
void i915_gem_release(struct drm_device *dev, struct drm_file *file);

uint32_t
i915_gem_get_gtt_size(struct drm_device *dev, uint32_t size, int tiling_mode);
uint32_t
i915_gem_get_gtt_alignment(struct drm_device *dev, uint32_t size,
			    int tiling_mode, bool fenced);

int i915_gem_object_set_cache_level(struct drm_i915_gem_object *obj,
				    enum i915_cache_level cache_level);

struct drm_gem_object *i915_gem_prime_import(struct drm_device *dev,
				struct dma_buf *dma_buf);

struct dma_buf *i915_gem_prime_export(struct drm_device *dev,
				struct drm_gem_object *gem_obj, int flags);

void i915_gem_restore_fences(struct drm_device *dev);

/* i915_gem_context.c */
void i915_gem_context_init(struct drm_device *dev);
void i915_gem_context_fini(struct drm_device *dev);
void i915_gem_context_close(struct drm_device *dev, struct drm_file *file);
int i915_switch_context(struct intel_ring_buffer *ring,
			struct drm_file *file, int to_id);
void i915_gem_context_free(struct kref *ctx_ref);
static inline void i915_gem_context_reference(struct i915_hw_context *ctx)
{
	kref_get(&ctx->ref);
}

static inline void i915_gem_context_unreference(struct i915_hw_context *ctx)
{
	kref_put(&ctx->ref, i915_gem_context_free);
}

int i915_gem_context_create_ioctl(struct drm_device *dev, void *data,
				  struct drm_file *file);
int i915_gem_context_destroy_ioctl(struct drm_device *dev, void *data,
				   struct drm_file *file);

/* i915_gem_gtt.c */
void i915_gem_cleanup_aliasing_ppgtt(struct drm_device *dev);
void i915_ppgtt_bind_object(struct i915_hw_ppgtt *ppgtt,
			    struct drm_i915_gem_object *obj,
			    enum i915_cache_level cache_level);
void i915_ppgtt_unbind_object(struct i915_hw_ppgtt *ppgtt,
			      struct drm_i915_gem_object *obj);

void i915_gem_restore_gtt_mappings(struct drm_device *dev);
int __must_check i915_gem_gtt_prepare_object(struct drm_i915_gem_object *obj);
void i915_gem_gtt_bind_object(struct drm_i915_gem_object *obj,
				enum i915_cache_level cache_level);
void i915_gem_gtt_unbind_object(struct drm_i915_gem_object *obj);
void i915_gem_gtt_finish_object(struct drm_i915_gem_object *obj);
void i915_gem_init_global_gtt(struct drm_device *dev);
void i915_gem_setup_global_gtt(struct drm_device *dev, unsigned long start,
			       unsigned long mappable_end, unsigned long end);
int i915_gem_gtt_init(struct drm_device *dev);
static inline void i915_gem_chipset_flush(struct drm_device *dev)
{
	if (INTEL_INFO(dev)->gen < 6)
		intel_gtt_chipset_flush();
}


/* i915_gem_evict.c */
int __must_check i915_gem_evict_something(struct drm_device *dev, int min_size,
					  unsigned alignment,
					  unsigned cache_level,
					  bool mappable,
					  bool nonblock);
int i915_gem_evict_everything(struct drm_device *dev);

/* i915_gem_stolen.c */
int i915_gem_init_stolen(struct drm_device *dev);
int i915_gem_stolen_setup_compression(struct drm_device *dev, int size);
void i915_gem_stolen_cleanup_compression(struct drm_device *dev);
void i915_gem_cleanup_stolen(struct drm_device *dev);
struct drm_i915_gem_object *
i915_gem_object_create_stolen(struct drm_device *dev, u32 size);
struct drm_i915_gem_object *
i915_gem_object_create_stolen_for_preallocated(struct drm_device *dev,
					       u32 stolen_offset,
					       u32 gtt_offset,
					       u32 size);
void i915_gem_object_release_stolen(struct drm_i915_gem_object *obj);

/* i915_gem_tiling.c */
inline static bool i915_gem_object_needs_bit17_swizzle(struct drm_i915_gem_object *obj)
{
	drm_i915_private_t *dev_priv = obj->base.dev->dev_private;

	return dev_priv->mm.bit_6_swizzle_x == I915_BIT_6_SWIZZLE_9_10_17 &&
		obj->tiling_mode != I915_TILING_NONE;
}

void i915_gem_detect_bit_6_swizzle(struct drm_device *dev);
void i915_gem_object_do_bit_17_swizzle(struct drm_i915_gem_object *obj);
void i915_gem_object_save_bit_17_swizzle(struct drm_i915_gem_object *obj);

/* i915_gem_debug.c */
void i915_gem_dump_object(struct drm_i915_gem_object *obj, int len,
			  const char *where, uint32_t mark);
#if WATCH_LISTS
int i915_verify_lists(struct drm_device *dev);
#else
#define i915_verify_lists(dev) 0
#endif
void i915_gem_object_check_coherency(struct drm_i915_gem_object *obj,
				     int handle);
void i915_gem_dump_object(struct drm_i915_gem_object *obj, int len,
			  const char *where, uint32_t mark);

/* i915_debugfs.c */
int i915_debugfs_init(struct drm_minor *minor);
void i915_debugfs_cleanup(struct drm_minor *minor);

/* i915_suspend.c */
extern int i915_save_state(struct drm_device *dev);
extern int i915_restore_state(struct drm_device *dev);

/* i915_ums.c */
void i915_save_display_reg(struct drm_device *dev);
void i915_restore_display_reg(struct drm_device *dev);

/* i915_sysfs.c */
void i915_setup_sysfs(struct drm_device *dev_priv);
void i915_teardown_sysfs(struct drm_device *dev_priv);

/* intel_i2c.c */
extern int intel_setup_gmbus(struct drm_device *dev);
extern void intel_teardown_gmbus(struct drm_device *dev);
static inline bool intel_gmbus_is_port_valid(unsigned port)
{
	return (port >= GMBUS_PORT_SSC && port <= GMBUS_PORT_DPD);
}

extern struct i2c_adapter *intel_gmbus_get_adapter(
		struct drm_i915_private *dev_priv, unsigned port);
extern void intel_gmbus_set_speed(struct i2c_adapter *adapter, int speed);
extern void intel_gmbus_force_bit(struct i2c_adapter *adapter, bool force_bit);
static inline bool intel_gmbus_is_forced_bit(struct i2c_adapter *adapter)
{
	return container_of(adapter, struct intel_gmbus, adapter)->force_bit;
}
extern void intel_i2c_reset(struct drm_device *dev);

/* intel_opregion.c */
extern int intel_opregion_setup(struct drm_device *dev);
#ifdef CONFIG_ACPI
extern void intel_opregion_init(struct drm_device *dev);
extern void intel_opregion_fini(struct drm_device *dev);
extern void intel_opregion_asle_intr(struct drm_device *dev);
#else
static inline void intel_opregion_init(struct drm_device *dev) { return; }
static inline void intel_opregion_fini(struct drm_device *dev) { return; }
static inline void intel_opregion_asle_intr(struct drm_device *dev) { return; }
#endif

/* intel_acpi.c */
#ifdef CONFIG_ACPI
extern void intel_register_dsm_handler(void);
extern void intel_unregister_dsm_handler(void);
#else
static inline void intel_register_dsm_handler(void) { return; }
static inline void intel_unregister_dsm_handler(void) { return; }
#endif /* CONFIG_ACPI */

/* modesetting */
extern void intel_modeset_init_hw(struct drm_device *dev);
extern void intel_modeset_suspend_hw(struct drm_device *dev);
extern void intel_modeset_init(struct drm_device *dev);
extern void intel_modeset_gem_init(struct drm_device *dev);
extern void intel_modeset_cleanup(struct drm_device *dev);
extern int intel_modeset_vga_set_state(struct drm_device *dev, bool state);
extern void intel_modeset_setup_hw_state(struct drm_device *dev,
					 bool force_restore);
extern void i915_redisable_vga(struct drm_device *dev);
extern bool intel_fbc_enabled(struct drm_device *dev);
extern void intel_disable_fbc(struct drm_device *dev);
extern bool ironlake_set_drps(struct drm_device *dev, u8 val);
extern void intel_init_pch_refclk(struct drm_device *dev);
extern void gen6_set_rps(struct drm_device *dev, u8 val);
extern void valleyview_set_rps(struct drm_device *dev, u8 val);
extern int valleyview_rps_max_freq(struct drm_i915_private *dev_priv);
extern int valleyview_rps_min_freq(struct drm_i915_private *dev_priv);
extern void intel_detect_pch(struct drm_device *dev);
extern int intel_trans_dp_port_sel(struct drm_crtc *crtc);
extern int intel_enable_rc6(const struct drm_device *dev);

extern bool i915_semaphore_is_enabled(struct drm_device *dev);
int i915_reg_read_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file);

/* overlay */
#ifdef CONFIG_DEBUG_FS
extern struct intel_overlay_error_state *intel_overlay_capture_error_state(struct drm_device *dev);
extern void intel_overlay_print_error_state(struct seq_file *m, struct intel_overlay_error_state *error);

extern struct intel_display_error_state *intel_display_capture_error_state(struct drm_device *dev);
extern void intel_display_print_error_state(struct seq_file *m,
					    struct drm_device *dev,
					    struct intel_display_error_state *error);
#endif

/* On SNB platform, before reading ring registers forcewake bit
 * must be set to prevent GT core from power down and stale values being
 * returned.
 */
void gen6_gt_force_wake_get(struct drm_i915_private *dev_priv);
void gen6_gt_force_wake_put(struct drm_i915_private *dev_priv);
int __gen6_gt_wait_for_fifo(struct drm_i915_private *dev_priv);

int sandybridge_pcode_read(struct drm_i915_private *dev_priv, u8 mbox, u32 *val);
int sandybridge_pcode_write(struct drm_i915_private *dev_priv, u8 mbox, u32 val);
int valleyview_punit_read(struct drm_i915_private *dev_priv, u8 addr, u32 *val);
int valleyview_punit_write(struct drm_i915_private *dev_priv, u8 addr, u32 val);
int valleyview_nc_read(struct drm_i915_private *dev_priv, u8 addr, u32 *val);

int vlv_gpu_freq(int ddr_freq, int val);
int vlv_freq_opcode(int ddr_freq, int val);

#define __i915_read(x, y) \
	u##x i915_read##x(struct drm_i915_private *dev_priv, u32 reg);

__i915_read(8, b)
__i915_read(16, w)
__i915_read(32, l)
__i915_read(64, q)
#undef __i915_read

#define __i915_write(x, y) \
	void i915_write##x(struct drm_i915_private *dev_priv, u32 reg, u##x val);

__i915_write(8, b)
__i915_write(16, w)
__i915_write(32, l)
__i915_write(64, q)
#undef __i915_write

#define I915_READ8(reg)		i915_read8(dev_priv, (reg))
#define I915_WRITE8(reg, val)	i915_write8(dev_priv, (reg), (val))

#define I915_READ16(reg)	i915_read16(dev_priv, (reg))
#define I915_WRITE16(reg, val)	i915_write16(dev_priv, (reg), (val))
#define I915_READ16_NOTRACE(reg)	readw(dev_priv->regs + (reg))
#define I915_WRITE16_NOTRACE(reg, val)	writew(val, dev_priv->regs + (reg))

#define I915_READ(reg)		i915_read32(dev_priv, (reg))
#define I915_WRITE(reg, val)	i915_write32(dev_priv, (reg), (val))
#define I915_READ_NOTRACE(reg)		readl(dev_priv->regs + (reg))
#define I915_WRITE_NOTRACE(reg, val)	writel(val, dev_priv->regs + (reg))

#define I915_WRITE64(reg, val)	i915_write64(dev_priv, (reg), (val))
#define I915_READ64(reg)	i915_read64(dev_priv, (reg))

#define POSTING_READ(reg)	(void)I915_READ_NOTRACE(reg)
#define POSTING_READ16(reg)	(void)I915_READ16_NOTRACE(reg)

/* "Broadcast RGB" property */
#define INTEL_BROADCAST_RGB_AUTO 0
#define INTEL_BROADCAST_RGB_FULL 1
#define INTEL_BROADCAST_RGB_LIMITED 2

static inline uint32_t i915_vgacntrl_reg(struct drm_device *dev)
{
	if (HAS_PCH_SPLIT(dev))
		return CPU_VGACNTRL;
	else if (IS_VALLEYVIEW(dev))
		return VLV_VGACNTRL;
	else
		return VGACNTRL;
}

static inline void __user *to_user_ptr(u64 address)
{
	return (void __user *)(uintptr_t)address;
}

static inline unsigned long msecs_to_jiffies_timeout(const unsigned int m)
{
	unsigned long j = msecs_to_jiffies(m);

	return min_t(unsigned long, MAX_JIFFY_OFFSET, j + 1);
}

static inline unsigned long
timespec_to_jiffies_timeout(const struct timespec *value)
{
	unsigned long j = timespec_to_jiffies(value);

	return min_t(unsigned long, MAX_JIFFY_OFFSET, j + 1);
}

#endif
