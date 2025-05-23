use super::*;
use crate::{BasisTextureFormat, UserData};
use basis_universal_sys::encoding as sys;

/// The color space the image to be compressed is encoded in. Using the correct color space will
#[derive(Debug, Copy, Clone)]
pub enum ColorSpace {
    /// Used for normal maps or other "data" images
    Linear,

    /// Used for color maps and other "visual" images
    Srgb,
}

/// Parameters that are used to configure a [Compressor]
pub struct CompressorParams(pub *mut sys::CompressorParams);

impl Default for CompressorParams {
    fn default() -> Self {
        Self::new()
    }
}

impl CompressorParams {
    /// Create a compressor with default options
    pub fn new() -> Self {
        unsafe {
            let mut params = CompressorParams(sys::compressor_params_new());
            params.set_default_options();
            params
        }
    }

    /// Resets the compressor params to default state
    pub fn reset(&mut self) {
        unsafe {
            sys::compressor_params_clear(self.0);
            self.set_default_options();
            self.clear_source_image_list();
        }
    }

    // The default options that are applied when creating a new compressor or calling reset() on it
    fn set_default_options(&mut self) {
        // Set a default quality level. Leaving this unset results in undefined behavior, so we set
        // it to a working value by default
        self.set_etc1s_quality_level(crate::ETC1S_QUALITY_DEFAULT);
        self.set_uastc_quality_level(crate::UASTC_QUALITY_DEFAULT);

        // The library by default prints to stdout, but since this is a library we should disable
        // that by default
        self.set_print_status_to_stdout(false);
    }

    //
    // These function are used to load image data into the compressor
    //

    /// Get a reference to the source index. The internal list of source images is resized as needed
    /// such that the image will exist.
    ///
    /// For HDR images, use [`CompressorParams::source_hdr_image_mut`].
    pub fn source_image_mut(
        &mut self,
        image_index: u32,
    ) -> CompressorImageRef {
        unsafe {
            CompressorImageRef(sys::compressor_params_get_or_create_source_image(
                self.0,
                image_index,
            ))
        }
    }

    /// Get a reference to the source index. The internal list of source images is resized as needed
    /// such that the image will exist.
    pub fn source_hdr_image_mut(
        &mut self,
        image_index: u32,
    ) -> CompressorHdrImageRef {
        unsafe {
            CompressorHdrImageRef(sys::compressor_params_get_or_create_source_hdr_image(
                self.0,
                image_index,
            ))
        }
    }

    /// Resizes the source image list. If the provided length is shorter than the list, the data
    /// beyond the provided length is truncated.
    ///
    /// For HDR images, use [`CompressorParams::resize_source_hdr_image_list`].
    pub fn resize_source_image_list(
        &mut self,
        size: u32,
    ) {
        unsafe {
            sys::compressor_params_resize_source_image_list(self.0, size as _);
        }
    }

    /// Resizes the source image list. If the provided length is shorter than the list, the data
    /// beyond the provided length is truncated.
    pub fn resize_source_hdr_image_list(
        &mut self,
        size: u32,
    ) {
        unsafe {
            sys::compressor_params_resize_source_hdr_image_list(self.0, size as _);
        }
    }

    /// Resets the image list to be zero-length.
    ///
    /// For HDR images, use [`CompressorParams::clear_source_hdr_image_list`].
    pub fn clear_source_image_list(&mut self) {
        unsafe {
            sys::compressor_params_clear_source_image_list(self.0);
        }
    }

    /// Resets the image list to be zero-length
    pub fn clear_source_hdr_image_list(&mut self) {
        unsafe {
            sys::compressor_params_clear_source_hdr_image_list(self.0);
        }
    }

    /// Get a reference to the source index. The internal list of source images is resized as needed
    /// such that the image will exist
    pub fn source_mipmap_image_mut(
        &mut self,
        image_index: u32,
        level: u32,
    ) -> CompressorImageRef {
        unsafe {
            CompressorImageRef(sys::compressor_params_get_or_create_source_mipmap_image(
                self.0,
                image_index,
                level,
            ))
        }
    }

    /// Resizes the source image list. If the provided length is shorter than the list, the data
    /// beyond the provided length is truncated.
    pub fn resize_source_mipmap_image_list(
        &mut self,
        size: u32,
    ) {
        unsafe {
            sys::compressor_params_resize_source_mipmap_image_list(self.0, size as _);
        }
    }

    /// Resizes the source image list. If the provided length is shorter than the list, the data
    /// beyond the provided length is truncated.
    pub fn resize_source_mipmap_level_image_list(
        &mut self,
        level: u32,
        size: u32,
    ) {
        unsafe {
            sys::compressor_params_resize_source_mipmap_image_level_list(
                self.0, level as _, size as _,
            );
        }
    }

    /// Resets the image list to be zero-length
    pub fn clear_source_mipmap_image_list(&mut self) {
        unsafe {
            sys::compressor_params_clear_source_mipmap_image_list(self.0);
        }
    }
    //
    // These set parameters for compression
    //

    /// Enable stdout logging
    pub fn set_print_status_to_stdout(
        &mut self,
        print_status_to_stdout: bool,
    ) {
        unsafe { sys::compressor_params_set_status_output(self.0, print_status_to_stdout) }
    }

    /// Set ETC1S quality level. The value MUST be >= [ETC1S_QUALITY_MIN](crate::ETC1S_QUALITY_MIN)
    /// and <= [ETC1S_QUALITY_MAX](crate::ETC1S_QUALITY_MAX).
    pub fn set_etc1s_quality_level(
        &mut self,
        quality_level: u32,
    ) {
        assert!(quality_level >= crate::ETC1S_QUALITY_MIN);
        assert!(quality_level <= crate::ETC1S_QUALITY_MAX);

        unsafe {
            sys::compressor_params_set_quality_level(self.0, quality_level as i32);
        }
    }

    /// Sets UASTC quality level. The value MUST be >= [UASTC_QUALITY_MIN](crate::UASTC_QUALITY_MIN)
    /// and <= [UASTC_QUALITY_MAX](crate::UASTC_QUALITY_MAX).
    pub fn set_uastc_quality_level(
        &mut self,
        quality_level: u32,
    ) {
        assert!(quality_level >= crate::UASTC_QUALITY_MIN);
        assert!(quality_level <= crate::UASTC_QUALITY_MAX);

        unsafe {
            let mut flags = sys::compressor_params_get_pack_uastc_flags(self.0);
            flags |= quality_level;
            sys::compressor_params_set_pack_uastc_flags(self.0, flags);
        }
    }

    /// Set the basis format we will compress to. See basis documentation for details. This
    /// corresponds to the -uastc flag in the basisu command line tool and the m_uastc boolean param
    /// on `basis_compressor_params` in the original library
    ///
    /// UASTC encoding result in significantly higher texture quality, but larger files.
    pub fn set_basis_format(
        &mut self,
        basis_format: BasisTextureFormat,
    ) {
        unsafe {
            sys::compressor_params_set_format_mode(self.0, basis_format.into());
        }
    }

    /// Sets the color space the images to be compressed is encoded in
    ///
    /// Setting a linear color space will:
    /// * Use linear colorspace metrics (instead of the default sRGB)
    /// * By default use linear (not sRGB) mipmap filtering
    pub fn set_color_space(
        &mut self,
        color_space: ColorSpace,
    ) {
        let perceptual = match color_space {
            ColorSpace::Linear => false,
            ColorSpace::Srgb => true,
        };
        unsafe {
            sys::compressor_params_set_perceptual(self.0, perceptual);
            sys::compressor_params_set_ktx2_srgb_transfer_func(self.0, perceptual);
        }
    }

    /// Override the mipmap generation color space behavior. This function is not necessary to call
    /// if you call [set_color_space] with the correct value.
    ///
    /// * If the color space is sRGB, convert image to linear before filtering, then back to sRGB
    /// * If the color space is linear, we keep the image in linear during mipmap filtering
    ///   (i.e. do not convert to/from sRGB for filtering purposes)
    pub fn set_mip_color_space(
        &mut self,
        color_space: ColorSpace,
    ) {
        let mip_srgb = match color_space {
            ColorSpace::Linear => false,
            ColorSpace::Srgb => true,
        };
        unsafe {
            sys::compressor_params_set_mip_srgb(self.0, mip_srgb);
        }
    }

    /// Disable backend's selector rate distortion optimizations (slightly faster, less noisy
    /// output, but lower quality per output bit)
    pub fn set_no_selector_rdo(
        &mut self,
        no_selector_rdo: bool,
    ) {
        unsafe {
            sys::compressor_params_set_no_selector_rdo(self.0, no_selector_rdo);
        }
    }

    /// Disable backend's endpoint rate distortion optimizations (slightly faster, less noisy
    /// output, but lower quality per output bit)
    pub fn set_no_endpoint_rdo(
        &mut self,
        no_endpoint_rdo: bool,
    ) {
        unsafe {
            sys::compressor_params_set_no_endpoint_rdo(self.0, no_endpoint_rdo);
        }
    }

    /// Enable/disable UASTC RDO post-processing and set UASTC RDO quality scalar to X. Lower
    /// values=higher quality/larger LZ compressed files, higher values=lower quality/smaller LZ
    /// compressed files. Good range to try is [.2-4]
    pub fn set_rdo_uastc(
        &mut self,
        rdo_uastc_quality_scalar: Option<f32>,
    ) {
        unsafe {
            match rdo_uastc_quality_scalar {
                Some(quality_scalar) => {
                    sys::compressor_params_set_rdo_uastc(self.0, true);
                    sys::compressor_params_set_rdo_uastc_quality_scalar(self.0, quality_scalar);
                }
                None => {
                    sys::compressor_params_set_rdo_uastc(self.0, false);
                }
            }
        }
    }

    /// Generate mipmaps for each source image
    ///
    /// By default, sRGB textures will be converted from sRGB to linear before mipmap filtering.
    /// This can be changed by calling [set_color_space] or [set_mip_color_space]
    pub fn set_generate_mipmaps(
        &mut self,
        generate_mipmaps: bool,
    ) {
        unsafe {
            sys::compressor_params_set_generate_mipmaps(self.0, generate_mipmaps);
        }
    }

    /// Sets the smallest dimension mipmap that will be generated
    pub fn set_mipmap_smallest_dimension(
        &mut self,
        smallest_dimension: u32,
    ) {
        unsafe {
            sys::compressor_params_set_mip_smallest_dimension(self.0, smallest_dimension as _);
        }
    }

    /// Use faster mipmap generation.
    ///
    /// On by default.
    pub fn set_mipmap_fast(
        &mut self,
        fast: bool,
    ) {
        unsafe { sys::compressor_params_set_mip_fast(self.0, fast) }
    }

    /// Set arbitrary userdata to be included with the basis-universal binary data
    pub fn set_userdata(
        &mut self,
        userdata: UserData,
    ) {
        unsafe {
            sys::compressor_params_set_userdata(self.0, userdata.userdata0, userdata.userdata1);
        }
    }

    /// The `basisu` command line compressor offers a -normal_map parameter that sets several
    /// values automatically. This convenience function mimics that parameter.
    ///
    /// * linear colorspace metrics
    /// * linear mipmap filtering
    /// * no selector RDO
    /// * no sRGB
    pub fn tune_for_normal_maps(&mut self) {
        //TODO
        unsafe {
            sys::compressor_params_set_perceptual(self.0, false);
            sys::compressor_params_set_mip_srgb(self.0, false);
            sys::compressor_params_set_no_selector_rdo(self.0, true);
            sys::compressor_params_set_no_endpoint_rdo(self.0, true);
        }
    }

    /// If set to `true`, the output will be KTX2 instead of Basis Universal.
    pub fn set_create_ktx2_file(
        &mut self,
        ktx2_file: bool,
    ) {
        unsafe {
            sys::compressor_params_set_create_ktx2_file(self.0, ktx2_file);
        }
    }

    /// Enables or disables ZStandard supercompression on KTX2 UASTC output.
    ///
    /// To set the compression level, see [`set_create_ktx2_zstd_supercompression_level`].
    pub fn set_ktx2_uastc_supercompression(
        &mut self,
        supercompression: bool,
    ) {
        let supercompression_mode = if supercompression {
            sys::basist_ktx2_supercompression_KTX2_SS_ZSTANDARD
        } else {
            sys::basist_ktx2_supercompression_KTX2_SS_NONE
        };
        unsafe {
            sys::compressor_params_set_ktx2_uastc_supercompression(self.0, supercompression_mode);
        }
    }

    /// Sets the ZStandard compression level for KTX2 UASTC output.
    ///
    /// - `1` = Fastest
    /// - `22` = Best (Slow)
    pub fn set_ktx2_zstd_supercompression_level(
        &mut self,
        level: u32,
    ) {
        unsafe {
            sys::compressor_params_set_ktx2_zstd_supercompression_level(self.0, level as i32);
        }
    }

    /// Enables or disables HDR mode.
    pub fn set_hdr(
        &mut self,
        hdr: bool,
    ) {
        unsafe {
            sys::compressor_params_set_hdr(self.0, hdr);
        }
    }

    /// Sets the HDR encoding method to use.
    pub fn set_hdr_mode(
        &mut self,
        hdr_mode: HdrMode,
    ) {
        unsafe {
            sys::compressor_params_set_hdr_mode(self.0, hdr_mode.into());
        }
    }

    // Favors ASTC HDR quality more than BC6H quality. Otherwise a rough balance.
    pub fn set_hdr_favor_uastc(
        &mut self,
        favor_uastc: bool,
    ) {
        unsafe {
            sys::compressor_params_set_hdr_favor_astc(self.0, favor_uastc);
        }
    }

    // set_multithreaded not implemented here as this is controlled by thread count passed to
    // `Compressor::new()`
}

impl Drop for CompressorParams {
    fn drop(&mut self) {
        unsafe {
            sys::compressor_params_delete(self.0);
        }
    }
}

#[allow(non_camel_case_types)]
#[derive(Copy, Clone, Debug, PartialEq)]
#[repr(i32)]
pub enum HdrMode {
    HDR_UASTC_HDR_4x4 = sys::basisu_hdr_modes_cUASTC_HDR_4X4,
    HDR_ASTC_6x6 = sys::basisu_hdr_modes_cASTC_HDR_6X6,
    HDR_ASTC_6x6_INTERMEDIATE = sys::basisu_hdr_modes_cASTC_HDR_6X6_INTERMEDIATE,
}

impl Into<sys::basisu_hdr_modes> for HdrMode {
    fn into(self) -> sys::basisu_hdr_modes {
        self as sys::basisu_hdr_modes
    }
}

impl From<sys::basisu_hdr_modes> for HdrMode {
    fn from(value: sys::basisu_hdr_modes) -> Self {
        unsafe { std::mem::transmute(value as i32) }
    }
}
