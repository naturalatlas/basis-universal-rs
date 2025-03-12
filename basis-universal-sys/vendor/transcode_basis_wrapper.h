#include "basis_universal/transcoder/basisu_transcoder.h"

extern "C" {
    // A copy of basist::basisu_file_info with problematic fields removed
    struct BasisUniversalFileInfo
    {
        void reset(basist::basisu_file_info file_info) {
            m_version = file_info.m_version;
            m_total_header_size = file_info.m_total_header_size;

            m_total_selectors = file_info.m_total_selectors;
            m_selector_codebook_ofs = file_info.m_selector_codebook_ofs;
            m_selector_codebook_size = file_info.m_selector_codebook_size;

            m_total_endpoints = file_info.m_total_endpoints;
            m_endpoint_codebook_ofs = file_info.m_endpoint_codebook_ofs;
            m_endpoint_codebook_size = file_info.m_endpoint_codebook_size;

            m_tables_ofs = file_info.m_tables_ofs;
            m_tables_size = file_info.m_tables_size;

            m_slices_size = file_info.m_slices_size;

            m_tex_type = file_info.m_tex_type;
            m_us_per_frame = file_info.m_us_per_frame;

            m_total_images = file_info.m_total_images;

            m_userdata0 = file_info.m_userdata0;
            m_userdata1 = file_info.m_userdata1;

            m_tex_format = file_info.m_tex_format;

            m_y_flipped = file_info.m_y_flipped;
            m_etc1s = file_info.m_etc1s;
            m_has_alpha_slices = file_info.m_has_alpha_slices;
        }

        uint32_t m_version;
        uint32_t m_total_header_size;

        uint32_t m_total_selectors;
        uint32_t m_selector_codebook_ofs;
        uint32_t m_selector_codebook_size;

        uint32_t m_total_endpoints;
        uint32_t m_endpoint_codebook_ofs;
        uint32_t m_endpoint_codebook_size;

        uint32_t m_tables_ofs;
        uint32_t m_tables_size;

        uint32_t m_slices_size;

        basist::basis_texture_type m_tex_type;
        uint32_t m_us_per_frame;

        // Low-level slice information (1 slice per image for color-only basis files, 2 for alpha basis files)
        //basist::basisu_slice_info_vec m_slice_info;

        uint32_t m_total_images;	 // total # of images
        //std::vector<uint32_t> m_image_mipmap_levels; // the # of mipmap levels for each image

        uint32_t m_userdata0;
        uint32_t m_userdata1;

        basist::basis_tex_format m_tex_format; // ETC1S, UASTC, etc.

        bool m_y_flipped;				// true if the image was Y flipped
        bool m_etc1s;					// true if the file is ETC1S
        bool m_has_alpha_slices;	// true if the texture has alpha slices (for ETC1S: even slices RGB, odd slices alpha)
    };

    //
    // basisu_transcoder
    //
    struct BasisUniversalTranscoder {
        basist::basisu_transcoder *pTranscoder;
    };

    BasisUniversalTranscoder *basis_universal_transcoder_new() {
        BasisUniversalTranscoder *transcoder = new BasisUniversalTranscoder;
        transcoder->pTranscoder = new basist::basisu_transcoder();
        return transcoder;
    };

    void basis_universal_transcoder_delete(BasisUniversalTranscoder *transcoder) {
        delete transcoder->pTranscoder;
        delete transcoder;
    }

    // Validates the .basis file. This computes a crc16 over the entire file, so it's slow.
    bool basis_universal_transcoder_validate_file_checksums(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, bool full_validation) {
        return transcoder->pTranscoder->validate_file_checksums(pData, data_size, full_validation);
    }

    // Quick header validation - no crc16 checks.
    bool basis_universal_transcoder_validate_header(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size) {
        return transcoder->pTranscoder->validate_header(pData, data_size);
    }

    basist::basis_texture_type basis_universal_transcoder_get_texture_type(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size) {
        return transcoder->pTranscoder->get_texture_type(pData, data_size);
    }

    bool basis_universal_transcoder_get_userdata(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, uint32_t &userdata0, uint32_t &userdata1) {
        return transcoder->pTranscoder->get_userdata(pData, data_size, userdata0, userdata1);
    }

    // Returns the total number of images in the basis file (always 1 or more).
    // Note that the number of mipmap levels for each image may differ, and that images may have different resolutions.
    uint32_t basis_universal_transcoder_get_total_images(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size) {
        return transcoder->pTranscoder->get_total_images(pData, data_size);
    }

    basist::basis_tex_format basis_universal_transcoder_get_tex_format(const BasisUniversalTranscoder *transcoder, const void* pData, uint32_t data_size) {
        return transcoder->pTranscoder->get_basis_tex_format(pData, data_size);
    }

    // Returns the number of mipmap levels in an image.
    uint32_t basis_universal_transcoder_get_total_image_levels(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, uint32_t image_index) {
        return transcoder->pTranscoder->get_total_image_levels(pData, data_size, image_index);
    }

    // Returns basic information about an image. Note that orig_width/orig_height may not be a multiple of 4.
    bool basis_universal_transcoder_get_image_level_desc(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, uint32_t image_index, uint32_t level_index, uint32_t &orig_width, uint32_t &orig_height, uint32_t &total_blocks) {
        return transcoder->pTranscoder->get_image_level_desc(pData, data_size, image_index, level_index, orig_width, orig_height, total_blocks);
    }

    // Returns information about the specified image.
    bool basis_universal_transcoder_get_image_info(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, basist::basisu_image_info &image_info, uint32_t image_index) {
        return transcoder->pTranscoder->get_image_info(pData, data_size, image_info, image_index);
    }

    // Returns information about the specified image's mipmap level.
    bool basis_universal_transcoder_get_image_level_info(const BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, basist::basisu_image_level_info &level_info, uint32_t image_index, uint32_t level_index) {
        return transcoder->pTranscoder->get_image_level_info(pData, data_size, level_info, image_index, level_index);
    }

    // Get a description of the basis file and low-level information about each slice.
    bool basis_universal_transcoder_get_file_info(BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size, BasisUniversalFileInfo &file_info) {
        basist::basisu_file_info fi;
        if (!transcoder->pTranscoder->get_file_info(pData, data_size, fi)) {
            return false;
        }

        file_info.reset(fi);
        return true;
    }

    // start_transcoding() must be called before calling transcode_slice() or transcode_image_level().
    // For ETC1S files, this call decompresses the selector/endpoint codebooks, so ideally you would only call this once per .basis file (not each image/mipmap level).
    bool basis_universal_transcoder_start_transcoding(BasisUniversalTranscoder *transcoder, const void *pData, uint32_t data_size) {
        return transcoder->pTranscoder->start_transcoding(pData, data_size);
    }

    bool basis_universal_transcoder_stop_transcoding(BasisUniversalTranscoder *transcoder) {
        return transcoder->pTranscoder->stop_transcoding();
    }

    // Returns true if start_transcoding() has been called.
    bool basis_universal_transcoder_get_ready_to_transcode(const BasisUniversalTranscoder *transcoder) {
        return transcoder->pTranscoder->get_ready_to_transcode();
    }

    // transcode_image_level() decodes a single mipmap level from the .basis file to any of the supported output texture formats.
    // It'll first find the slice(s) to transcode, then call transcode_slice() one or two times to decode both the color and alpha texture data (or RG texture data from two slices for BC5).
    // If the .basis file doesn't have alpha slices, the output alpha blocks will be set to fully opaque (all 255's).
    // Currently, to decode to PVRTC1 the basis texture's dimensions in pixels must be a power of 2, due to PVRTC1 format requirements.
    // output_blocks_buf_size_in_blocks_or_pixels should be at least the image level's total_blocks (num_blocks_x * num_blocks_y), or the total number of output pixels if fmt==cTFRGBA32.
    // output_row_pitch_in_blocks_or_pixels: Number of blocks or pixels per row. If 0, the transcoder uses the slice's num_blocks_x or orig_width (NOT num_blocks_x * 4). Ignored for PVRTC1 (due to texture swizzling).
    // output_rows_in_pixels: Ignored unless fmt is cRGBA32. The total number of output rows in the output buffer. If 0, the transcoder assumes the slice's orig_height (NOT num_blocks_y * 4).
    // Notes:
    // - basisu_transcoder_init() must have been called first to initialize the transcoder lookup tables before calling this function.
    // - This method assumes the output texture buffer is readable. In some cases to handle alpha, the transcoder will write temporary data to the output texture in
    // a first pass, which will be read in a second pass.
    bool basis_universal_transcoder_transcode_image_level(
        BasisUniversalTranscoder *transcoder,
        const void *pData,
        uint32_t data_size,
        uint32_t image_index,
        uint32_t level_index,
        void *pOutput_blocks,
        uint32_t output_blocks_buf_size_in_blocks_or_pixels,
        basist::transcoder_texture_format fmt,
        basist::basisu_decode_flags decode_flags, // default: 0
        uint32_t output_row_pitch_in_blocks_or_pixels, // default: 0
        basist::basisu_transcoder_state *pState, // default: nullptr
        uint32_t output_rows_in_pixels // default: 0
    ) {
        return transcoder->pTranscoder->transcode_image_level(
                pData,
                data_size,
                image_index,
                level_index,
                pOutput_blocks,
                output_blocks_buf_size_in_blocks_or_pixels,
                fmt,
                decode_flags,
                output_row_pitch_in_blocks_or_pixels,
                pState,
                output_rows_in_pixels
        );
    }

    //
    //    // Finds the basis slice corresponding to the specified image/level/alpha params, or -1 if the slice can't be found.
    //    int find_slice(const void *pData, uint32_t data_size, uint32_t image_index, uint32_t level_index, bool alpha_data) const;
    //
    //    // transcode_slice() decodes a single slice from the .basis file. It's a low-level API - most likely you want to use transcode_image_level().
    //    // This is a low-level API, and will be needed to be called multiple times to decode some texture formats (like BC3, BC5, or ETC2).
    //    // output_blocks_buf_size_in_blocks_or_pixels is just used for verification to make sure the output buffer is large enough.
    //    // output_blocks_buf_size_in_blocks_or_pixels should be at least the image level's total_blocks (num_blocks_x * num_blocks_y), or the total number of output pixels if fmt==cTFRGBA32.
    //    // output_block_stride_in_bytes: Number of bytes between each output block.
    //    // output_row_pitch_in_blocks_or_pixels: Number of blocks or pixels per row. If 0, the transcoder uses the slice's num_blocks_x or orig_width (NOT num_blocks_x * 4). Ignored for PVRTC1 (due to texture swizzling).
    //    // output_rows_in_pixels: Ignored unless fmt is cRGBA32. The total number of output rows in the output buffer. If 0, the transcoder assumes the slice's orig_height (NOT num_blocks_y * 4).
    //    // Notes:
    //    // - basisu_transcoder_init() must have been called first to initialize the transcoder lookup tables before calling this function.
    //    bool transcode_slice(const void *pData, uint32_t data_size, uint32_t slice_index,
    //                         void *pOutput_blocks, uint32_t output_blocks_buf_size_in_blocks_or_pixels,
    //                         block_format fmt, uint32_t output_block_stride_in_bytes, uint32_t decode_flags = 0, uint32_t output_row_pitch_in_blocks_or_pixels = 0, basisu_transcoder_state * pState = nullptr, void* pAlpha_blocks = nullptr,
    //                         uint32_t output_rows_in_pixels = 0, int channel0 = -1, int channel1 = -1) const;
    //
    //    static void write_opaque_alpha_blocks(
    //            uint32_t num_blocks_x, uint32_t num_blocks_y,
    //            void* pOutput_blocks, block_format fmt,
    //            uint32_t block_stride_in_bytes, uint32_t output_row_pitch_in_blocks_or_pixels);

}
