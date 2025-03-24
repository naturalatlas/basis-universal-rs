#include "basis_universal/transcoder/basisu_transcoder.h"

extern "C" {
    //
    // ktx2_transcoder
    //

    struct Ktx2Transcoder {
        basist::ktx2_transcoder *pTranscoder;
    };

    Ktx2Transcoder *ktx2_transcoder_new() {
        Ktx2Transcoder *transcoder = new Ktx2Transcoder;
        transcoder->pTranscoder = new basist::ktx2_transcoder();
        return transcoder;
    }

    bool ktx2_transcoder_init(Ktx2Transcoder *transcoder, const void *data, uint32_t data_size) {
        return transcoder->pTranscoder->init(data, data_size);
    }

    void ktx2_transcoder_delete(Ktx2Transcoder *transcoder) {
        delete transcoder->pTranscoder;
        delete transcoder;
    }

    bool ktx2_get_is_hdr(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_hdr();
    }
    bool ktx2_get_is_ldr(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_ldr();
    }
    bool ktx2_get_is_hdr_4x4(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_hdr_4x4();
    }
    bool ktx2_get_is_hdr_6x6(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_hdr_6x6();
    }
    bool ktx2_get_is_video(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_video();
    }
    bool ktx2_get_is_uastc(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_uastc();
    }
    bool ktx2_get_is_etc1s(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->is_etc1s();
    }
    bool ktx2_get_is_srgb(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_transfer_func() == basist::KTX2_KHR_DF_TRANSFER_SRGB;
    }
    bool ktx2_get_is_linear(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_transfer_func() == basist::KTX2_KHR_DF_TRANSFER_LINEAR;
    }
    bool ktx2_get_has_alpha(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_has_alpha() > 0;
    }
    basist::ktx2_df_channel_id ktx2_get_dfd_channel_id0(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_channel_id0();
    }
    basist::ktx2_df_channel_id ktx2_get_dfd_channel_id1(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_channel_id1();
    }
    basist::ktx2_df_color_primaries ktx2_get_dfd_color_primaries(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_color_primaries();
    }
    uint32_t ktx2_get_dfd_color_model(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_color_model();
    }
    uint32_t ktx2_get_dfd_flags(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_flags();
    }
    uint32_t ktx2_get_dfd_total_samples(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_dfd_total_samples();
    }
    uint32_t ktx2_get_block_width(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_block_width();
    }
    uint32_t ktx2_get_block_height(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_block_height();
    }
    uint32_t ktx2_get_layers(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_layers();
    }
    uint32_t ktx2_get_levels(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_levels();
    }
    uint32_t ktx2_get_faces(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_faces();
    }
    uint32_t ktx2_get_pixel_depth(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_header().m_pixel_depth;
    }
    basist::basis_tex_format ktx2_get_basis_tex_format(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->get_basis_tex_format();
    }
    bool ktx2_start_transcoding(const Ktx2Transcoder *transcoder) {
        return transcoder->pTranscoder->start_transcoding();
    }

    bool ktx2_get_image_level_info(
        const Ktx2Transcoder *transcoder,
        basist::ktx2_image_level_info &level_info,
        uint32_t level_index,
        uint32_t layer_index,
        uint32_t face_index
    ) {
        return transcoder->pTranscoder->get_image_level_info(level_info, level_index, layer_index, face_index);
    }

    bool ktx2_transcoder_transcode_image_level(
        Ktx2Transcoder *transcoder,
        uint32_t level_index,
        uint32_t layer_index,
        uint32_t face_index,
        void* pOutput_blocks,
        uint32_t output_blocks_buf_size_in_blocks_or_pixels,
        basist::transcoder_texture_format fmt,
        uint32_t decode_flags,
        uint32_t output_row_pitch_in_blocks_or_pixels,
        uint32_t output_rows_in_pixels,
        int channel0,
        int channel1,
        basist::ktx2_transcoder_state* pState
    ) {
        return transcoder->pTranscoder->transcode_image_level(
            level_index,
            layer_index,
            face_index,
            pOutput_blocks,
            output_blocks_buf_size_in_blocks_or_pixels,
            fmt,
            decode_flags,
            output_row_pitch_in_blocks_or_pixels,
            output_rows_in_pixels,
            channel0,
            channel1,
            pState
        );
    }
}
