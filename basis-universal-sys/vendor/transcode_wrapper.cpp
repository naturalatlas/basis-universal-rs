#include "transcode_basis_wrapper.h"
#include "transcode_ktx2_wrapper.h"
#include "transcode_lowlevel_uastc_ldr_4x4_wrapper.h"
#include "transcode_lowlevel_uastc_hdr_4x4_wrapper.h"

#include "basis_universal/transcoder/basisu_transcoder.h"

extern "C" {
    //
    // "Loose" global functions
    //

    uint32_t basis_get_bytes_per_block_or_pixel(basist::transcoder_texture_format fmt) {
        return basist::basis_get_bytes_per_block_or_pixel(fmt);
    }

    const char* basis_get_format_name(basist::transcoder_texture_format fmt) {
        return basist::basis_get_format_name(fmt);
    }

    const char* basis_get_block_format_name(basist::block_format fmt) {
        return basist::basis_get_block_format_name(fmt);
    }

    bool basis_transcoder_format_has_alpha(basist::transcoder_texture_format fmt) {
        return basist::basis_transcoder_format_has_alpha(fmt);
    }

    basisu::texture_format basis_get_basisu_texture_format(basist::transcoder_texture_format fmt) {
        return basist::basis_get_basisu_texture_format(fmt);
    }

    const char* basis_get_texture_type_name(basist::basis_texture_type tex_type) {
        return basist::basis_get_texture_type_name(tex_type);
    }

    // Returns true if the transcoder texture type is an uncompressed (raw pixel) format.
    bool basis_transcoder_format_is_uncompressed(basist::transcoder_texture_format tex_type) {
        return basist::basis_transcoder_format_is_uncompressed(tex_type);
    }

    // Returns true if the block format is an uncompressed (raw pixel) format.
    bool basis_block_format_is_uncompressed(basist::block_format fmt) {
        return basist::basis_block_format_is_uncompressed(fmt);
    }

    // Returns the # of bytes per pixel for uncompressed formats, or 0 for block texture formats.
    uint32_t basis_get_uncompressed_bytes_per_pixel(basist::transcoder_texture_format fmt) {
        return basist::basis_get_uncompressed_bytes_per_pixel(fmt);
    }

    // Returns the block width for the specified texture format, which is currently either 4 or 8 for FXT1.
    uint32_t basis_get_block_width(basist::transcoder_texture_format tex_type) {
        return basist::basis_get_block_width(tex_type);
    }

    // Returns the block height for the specified texture format, which is currently always 4.
    uint32_t basis_get_block_height(basist::transcoder_texture_format tex_type) {
        return basist::basis_get_block_height(tex_type);
    }

    // Returns true if the specified format was enabled at compile time.
    bool basis_is_format_supported(basist::transcoder_texture_format tex_type, basist::basis_tex_format fmt) {
        return basist::basis_is_format_supported(tex_type, fmt);
    }

    // Validates that the output buffer is large enough to hold the entire transcoded texture.
    // For uncompressed texture formats, most input parameters are in pixels, not blocks. Blocks are 4x4 pixels.
    bool basis_validate_output_buffer_size(
        basist::transcoder_texture_format target_format,
        uint32_t output_blocks_buf_size_in_blocks_or_pixels,
        uint32_t orig_width,
        uint32_t orig_height,
        uint32_t output_row_pitch_in_blocks_or_pixels,
        uint32_t output_rows_in_pixels
    ) {
        // Note: `source_format` is unused, so we pass a dummy value
        return basist::basis_validate_output_buffer_size(
            basist::basis_tex_format::cTotalFormats,
            target_format,
            output_blocks_buf_size_in_blocks_or_pixels,
            orig_width,
            orig_height,
            output_row_pitch_in_blocks_or_pixels,
            output_rows_in_pixels
        );
    }

    //
    // Global functions
    //

    // basisu_transcoder_init() must be called before a .basis file can be transcoded.
    void basisu_transcoder_init() {
        basist::basisu_transcoder_init();
    }

    basist::debug_flags_t get_debug_flags() {
        return (basist::debug_flags_t) basist::get_debug_flags();
    }

    void set_debug_flags(basist::debug_flags_t f) {
        basist::set_debug_flags(f);
    }
}