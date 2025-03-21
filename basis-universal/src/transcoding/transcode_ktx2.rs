use std::convert::TryInto;

use super::*;
use basis_universal_sys::transcoding as sys;

/// Extra parameters for transcoding an image
#[derive(Default, Debug, Clone, Copy)]
pub struct Ktx2TranscodeParameters {
    /// The layer to transcode
    pub layer_index: u32,
    /// The face to transcode
    pub face_index: u32,
    /// The mip level of the image to transcode
    pub level_index: u32,
    /// Optional flags can affect transcoding in various ways
    pub decode_flags: Option<DecodeFlags>,
    /// Optional override for row pitch
    pub output_row_pitch_in_blocks_or_pixels: Option<u32>,
    /// Optional override for number of rows to transcode
    pub output_rows_in_pixels: Option<u32>,
    pub channel0: i32,
    pub channel1: i32,
}

pub struct Ktx2ImageLevelInfo {
    /// The mipmap level index (0=largest), texture array layer index, and cubemap face index of the image.
    pub level_index: u32,
    pub layer_index: u32,
    pub face_index: u32,

    /// The image's actual (or the original source image's) width in pixels, which may not be divisible by 4 pixels.
    pub orig_width: u32,
    /// The image's actual (or the original source image's) height in pixels, which may not be divisible by 4 pixels.
    pub orig_height: u32,

    /// The image's physical width, which will always be divisible by 4 pixels.
    pub width: u32,
    /// The image's physical height, which will always be divisible by 4 pixels.
    pub height: u32,

    /// The texture's dimensions in 4x4 or 6x6 texel blocks.
    pub num_blocks_x: u32,
    /// The texture's dimensions in 4x4 or 6x6 texel blocks.
    pub num_blocks_y: u32,

    /// The format's block width (currently either 4 or 6).
    pub block_width: u32,
    /// The format's block height (currently either 4 or 6).
    pub block_height: u32,

    /// The total number of blocks
    pub total_blocks: u32,

    /// true if the image has alpha data
    pub alpha_flag: bool,

    /// true if the image is an I-Frame. Currently, for ETC1S textures, the first frame will always be an I-Frame, and subsequent frames will always be P-Frames.
    pub iframe_flag: bool,
}

impl From<sys::basist_ktx2_image_level_info> for Ktx2ImageLevelInfo {
    fn from(value: sys::basist_ktx2_image_level_info) -> Self {
        Self {
            level_index: value.m_level_index,
            layer_index: value.m_layer_index,
            face_index: value.m_face_index,
            orig_width: value.m_orig_width,
            orig_height: value.m_orig_height,
            width: value.m_width,
            height: value.m_height,
            num_blocks_x: value.m_num_blocks_x,
            num_blocks_y: value.m_num_blocks_y,
            block_width: value.m_block_width,
            block_height: value.m_block_height,
            total_blocks: value.m_total_blocks,
            alpha_flag: value.m_alpha_flag,
            iframe_flag: value.m_iframe_flag,
        }
    }
}

pub struct Ktx2Transcoder<'data> {
    _data: &'data [u8],
    transcoder: *mut sys::Ktx2Transcoder,
    transcoding_started: bool,
}

impl<'data> Ktx2Transcoder<'data> {
    pub fn new(data: &'data [u8]) -> Result<Ktx2Transcoder<'data>, TranscodeError> {
        transcoder_init();
        let data_size: u32 = data.len().try_into().unwrap();
        let transcoder = unsafe { sys::ktx2_transcoder_new() };
        let success =
            unsafe { sys::ktx2_transcoder_init(transcoder, data.as_ptr() as _, data_size) };

        if !success {
            return Err(TranscodeError::TranscodeFormatNotSupported);
        }

        Ok(Self {
            _data: data,
            transcoder,
            transcoding_started: false,
        })
    }
    pub fn is_etc1s(&self) -> bool {
        unsafe { sys::ktx2_get_is_etc1s(self.transcoder) }
    }
    pub fn is_ldr(&self) -> bool {
        unsafe { sys::ktx2_get_is_ldr(self.transcoder) }
    }
    pub fn is_hdr(&self) -> bool {
        unsafe { sys::ktx2_get_is_hdr(self.transcoder) }
    }
    pub fn is_hdr_4x4(&self) -> bool {
        unsafe { sys::ktx2_get_is_hdr_4x4(self.transcoder) }
    }
    pub fn is_hdr_6x6(&self) -> bool {
        unsafe { sys::ktx2_get_is_hdr_6x6(self.transcoder) }
    }
    pub fn is_srgb(&self) -> bool {
        unsafe { sys::ktx2_get_is_srgb(self.transcoder) }
    }
    pub fn is_linear(&self) -> bool {
        unsafe { sys::ktx2_get_is_linear(self.transcoder) }
    }
    pub fn is_video(&self) -> bool {
        unsafe { sys::ktx2_get_is_video(self.transcoder) }
    }
    pub fn is_uastc(&self) -> bool {
        unsafe { sys::ktx2_get_is_uastc(self.transcoder) }
    }
    pub fn has_alpha(&self) -> bool {
        unsafe { sys::ktx2_get_has_alpha(self.transcoder) }
    }
    pub fn block_width(&self) -> u32 {
        unsafe { sys::ktx2_get_block_width(self.transcoder) }
    }
    pub fn block_height(&self) -> u32 {
        unsafe { sys::ktx2_get_block_height(self.transcoder) }
    }
    pub fn block_size(&self) -> (u32, u32) {
        (self.block_width(), self.block_height())
    }
    pub fn basis_texture_format(&self) -> BasisTextureFormat {
        unsafe { sys::ktx2_get_basis_tex_format(self.transcoder).into() }
    }
    pub fn pixel_depth(&self) -> u32 {
        unsafe { sys::ktx2_get_pixel_depth(self.transcoder) }
    }

    /// Returns the number of layers in the KTX2 file.
    pub fn layer_count(&self) -> u32 {
        unsafe { sys::ktx2_get_layers(self.transcoder) }
    }
    /// Returns the number of mipmap levels in the KTX2 file.
    pub fn level_count(&self) -> u32 {
        unsafe { sys::ktx2_get_levels(self.transcoder) }
    }
    /// Returns the number of faces in the KTX2 file.
    pub fn face_count(&self) -> u32 {
        unsafe { sys::ktx2_get_faces(self.transcoder) }
    }

    /// Returns information about the specified image's mipmap level.
    pub fn image_level_info(
        &self,
        level_index: u32,
        layer_index: u32,
        face_index: u32,
    ) -> Option<Ktx2ImageLevelInfo> {
        let mut image_level_info =
            unsafe { std::mem::zeroed::<sys::basist_ktx2_image_level_info>() };

        unsafe {
            if sys::ktx2_get_image_level_info(
                self.transcoder,
                &mut image_level_info,
                level_index,
                layer_index,
                face_index,
            ) {
                Some(image_level_info.into())
            } else {
                None
            }
        }
    }

    /// Returns the KTX2 texture type.
    ///
    /// https://github.khronos.org/KTX-Specification/ktxspec.v2.html#_texture_type
    pub fn texture_type(&self) -> Result<BasisTextureType, TranscodeError> {
        let face_count = self.face_count();
        let layer_count = self.layer_count();

        Ok(if self.is_video() {
            BasisTextureType::TextureTypeVideoFrames
        } else if self.pixel_depth() > 0 {
            BasisTextureType::TextureTypeVolume
        } else if face_count > 1 {
            if face_count % 6 != 0 {
                return Err(TranscodeError::Invalid(format!(
                    "KTX2 file with cube map array texture an unexpected number of faces: {face_count} (must be divisible by 6)",
                )));
            }
            BasisTextureType::TextureTypeCubemapArray
        } else if layer_count > 0 {
            BasisTextureType::TextureType2DArray
        } else {
            BasisTextureType::TextureType2D
        })
    }

    pub fn prepare_transcoding(&mut self) -> Result<(), TranscodeError> {
        if !self.transcoding_started {
            self.transcoding_started = true;
            let success = unsafe { sys::ktx2_start_transcoding(self.transcoder) };
            if !success {
                return Err(TranscodeError::TranscodeFailed);
            }
        }
        Ok(())
    }

    pub fn transcode_image_level(
        &self,
        target_format: TranscoderTextureFormat,
        transcode_parameters: Ktx2TranscodeParameters,
    ) -> Result<Vec<u8>, TranscodeError> {
        if !self.transcoding_started {
            panic!("Before calling transcode_image_level(), transcoding must be started by calling prepare_transcoding()");
        }

        // Based off basis_universal/webgl/transcoder/basis_wrappers.cpp
        let layer_index = transcode_parameters.layer_index;
        let level_index = transcode_parameters.level_index;
        let face_index = transcode_parameters.face_index;
        let channel0 = transcode_parameters.channel0;
        let channel1 = transcode_parameters.channel1;

        //
        // Check that the transcode format is supported for the stored texture's basis format
        //
        let basis_format = self.basis_texture_format();
        if !basis_format.can_transcode_to_format(target_format) {
            return Err(TranscodeError::TranscodeFormatNotSupported);
        }

        //
        // Determine required size for the buffer
        //
        let level_info = self
            .image_level_info(level_index, layer_index, face_index)
            .ok_or(TranscodeError::ImageLevelNotFound)?;

        let required_buffer_bytes = target_format.calculate_minimum_output_buffer_bytes(
            level_info.orig_width,
            level_info.orig_height,
            transcode_parameters.output_row_pitch_in_blocks_or_pixels,
            transcode_parameters.output_rows_in_pixels,
        ) as usize;

        //
        // unwrap_or() the optional parameters
        //
        let decode_flags = transcode_parameters
            .decode_flags
            .unwrap_or_else(DecodeFlags::empty);
        let output_row_pitch_in_blocks_or_pixels = transcode_parameters
            .output_row_pitch_in_blocks_or_pixels
            .unwrap_or(0);
        let output_rows_in_pixels = transcode_parameters.output_rows_in_pixels.unwrap_or(0);
        let transcoder_state = std::ptr::null_mut();

        //
        // Transcode
        //

        let mut output = vec![0_u8; required_buffer_bytes];

        let success = if !target_format.is_compressed() {
            let output_blocks_buf_size_in_blocks_or_pixels =
                level_info.orig_width * level_info.orig_height;

            unsafe {
                sys::ktx2_transcoder_transcode_image_level(
                    self.transcoder,
                    level_index,
                    layer_index,
                    face_index,
                    output.as_mut_ptr() as _,
                    output_blocks_buf_size_in_blocks_or_pixels,
                    target_format.into(),
                    decode_flags.bits(),
                    level_info.orig_width,
                    level_info.orig_height,
                    channel0,
                    channel1,
                    transcoder_state,
                )
            }
        } else {
            let output_blocks_buf_size_in_blocks_or_pixels =
                (required_buffer_bytes / target_format.bytes_per_block_or_pixel() as usize) as u32;
            unsafe {
                sys::ktx2_transcoder_transcode_image_level(
                    self.transcoder,
                    level_index,
                    layer_index,
                    face_index,
                    output.as_mut_ptr() as _,
                    output_blocks_buf_size_in_blocks_or_pixels,
                    target_format.into(),
                    decode_flags.bits(),
                    output_row_pitch_in_blocks_or_pixels,
                    output_rows_in_pixels,
                    channel0,
                    channel1,
                    transcoder_state,
                )
            }
        };

        if success {
            Ok(output)
        } else {
            Err(TranscodeError::TranscodeFailed)
        }
    }
}

impl Drop for Ktx2Transcoder<'_> {
    fn drop(&mut self) {
        unsafe {
            sys::ktx2_transcoder_delete(self.transcoder);
        }
    }
}
