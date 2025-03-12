#include "basis_universal/transcoder/basisu_transcoder.h"

extern "C" {
    //
    // basisu_lowlevel_uastc_ldr_4x4_transcoder
    //

    struct LowLevelUastcLdr4x4Transcoder {
        basist::basisu_lowlevel_uastc_ldr_4x4_transcoder *pTranscoder;
    };

    LowLevelUastcLdr4x4Transcoder *low_level_uastc_ldr_4x4_transcoder_new() {
        LowLevelUastcLdr4x4Transcoder *transcoder = new LowLevelUastcLdr4x4Transcoder;
        transcoder->pTranscoder = new basist::basisu_lowlevel_uastc_ldr_4x4_transcoder();
        return transcoder;
    }

    void low_level_uastc_ldr_4x4_transcoder_delete(LowLevelUastcLdr4x4Transcoder *transcoder) {
        delete transcoder->pTranscoder;
        delete transcoder;
    }

    bool low_level_uastc_ldr_4x4_transcoder_transcode_slice(
        LowLevelUastcLdr4x4Transcoder *transcoder,
        void* pDst_blocks,
        uint32_t num_blocks_x,
        uint32_t num_blocks_y,
        const uint8_t* pImage_data,
        uint32_t image_data_size,
        basist::block_format fmt,
        uint32_t output_block_or_pixel_stride_in_bytes,
        bool bc1_allow_threecolor_blocks,
        bool has_alpha,
        const uint32_t orig_width,
        const uint32_t orig_height,
        uint32_t output_row_pitch_in_blocks_or_pixels,
        basist::basisu_transcoder_state* pState,
        uint32_t output_rows_in_pixels,
        int channel0,
        int channel1,
        int32_t decode_flags // Enums are reflected as signed integers in Rust.
    ) {
        return transcoder->pTranscoder->transcode_slice(
            pDst_blocks,
            num_blocks_x,
            num_blocks_y,
            pImage_data,
            image_data_size,
            fmt,
            output_block_or_pixel_stride_in_bytes,
            bc1_allow_threecolor_blocks,
            has_alpha,
            orig_width,
            orig_height,
            output_row_pitch_in_blocks_or_pixels,
            pState,
            output_rows_in_pixels,
            channel0,
            channel1,
            decode_flags
        );
    }
}
