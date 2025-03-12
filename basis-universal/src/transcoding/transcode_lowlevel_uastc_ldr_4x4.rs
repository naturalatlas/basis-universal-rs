use super::*;
use basis_universal_sys as sys;

pub struct LowLevelUastcLdr4x4Transcoder(*mut sys::LowLevelUastcLdr4x4Transcoder);

impl Default for LowLevelUastcLdr4x4Transcoder {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug, Clone, Copy)]
pub struct SliceParametersUastc {
    pub num_blocks_x: u32,
    pub num_blocks_y: u32,
    pub has_alpha: bool,
    pub original_width: u32,
    pub original_height: u32,
}

impl LowLevelUastcLdr4x4Transcoder {
    pub fn new() -> LowLevelUastcLdr4x4Transcoder {
        transcoder_init();
        unsafe { LowLevelUastcLdr4x4Transcoder(sys::low_level_uastc_ldr_4x4_transcoder_new()) }
    }

    pub fn transcode_slice(
        &self,
        data: &[u8],
        slice_parameters: SliceParametersUastc,
        decode_flags: DecodeFlags,
        transcode_block_format: TranscoderBlockFormat,
    ) -> Result<Vec<u8>, TranscodeError> {
        let bc1_allow_threecolor_blocks = false;
        let transcoder_state = std::ptr::null_mut();
        let channel0 = 0;
        let channel1 = 3;

        let output_row_pitch_in_blocks_or_pixels =
            (slice_parameters.original_width + transcode_block_format.block_width() - 1)
                / transcode_block_format.block_width();
        let output_rows_in_pixels = slice_parameters.original_height;
        let total_slice_blocks = slice_parameters.num_blocks_x * slice_parameters.num_blocks_y;
        let required_buffer_bytes = transcode_block_format.calculate_minimum_output_buffer_bytes(
            slice_parameters.original_width,
            slice_parameters.original_height,
            total_slice_blocks,
            Some(output_row_pitch_in_blocks_or_pixels),
            Some(output_rows_in_pixels),
        ) as usize;

        let output_block_or_pixel_stride_in_bytes =
            transcode_block_format.bytes_per_block_or_pixel();

        let mut output = vec![0_u8; required_buffer_bytes];
        let success = unsafe {
            sys::low_level_uastc_ldr_4x4_transcoder_transcode_slice(
                self.0,
                output.as_mut_ptr() as _,
                slice_parameters.num_blocks_x,
                slice_parameters.num_blocks_y,
                data.as_ptr() as _,
                data.len() as u32,
                transcode_block_format.into(),
                output_block_or_pixel_stride_in_bytes,
                bc1_allow_threecolor_blocks,
                slice_parameters.has_alpha,
                slice_parameters.original_width,
                slice_parameters.original_height,
                output_row_pitch_in_blocks_or_pixels,
                transcoder_state,
                output_rows_in_pixels,
                channel0,
                channel1,
                decode_flags.bits() as i32,
            )
        };

        if success {
            Ok(output)
        } else {
            Err(TranscodeError::TranscodeFailed)
        }
    }
}

impl Drop for LowLevelUastcLdr4x4Transcoder {
    fn drop(&mut self) {
        unsafe {
            sys::low_level_uastc_ldr_4x4_transcoder_delete(self.0);
        }
    }
}
