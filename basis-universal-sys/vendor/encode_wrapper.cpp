#include "basis_universal/encoder/basisu_comp.h"
#include "basis_universal/encoder/basisu_enc.h"

//TODO: constants

extern "C" {
    union ColorU8 {
        struct Channels {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } channels;

        uint8_t components[4];
        uint32_t combined;
    };

    union ColorF {
        struct Channels {
            float r;
            float g;
            float b;
            float a;
        } channels;

        float components[4];
    };

    // These are in an anonymous enum, so wrap them
    enum UastcPackFlags {
        PackUASTCLevelFastest = basisu::cPackUASTCLevelFastest,
        PackUASTCLevelFaster = basisu::cPackUASTCLevelFaster,
        PackUASTCLevelDefault = basisu::cPackUASTCLevelDefault,
        PackUASTCLevelSlower = basisu::cPackUASTCLevelSlower,
        PackUASTCLevelVerySlow = basisu::cPackUASTCLevelVerySlow,
        PackUASTCLevelMask = basisu::cPackUASTCLevelMask,
        PackUASTCFavorUASTCError = basisu::cPackUASTCFavorUASTCError,
        PackUASTCFavorBC7Error = basisu::cPackUASTCFavorBC7Error,
        PackUASTCETC1FasterHints = basisu::cPackUASTCETC1FasterHints,
        PackUASTCETC1FastestHints = basisu::cPackUASTCETC1FastestHints,
        PackUASTCETC1DisableFlipAndIndividual = basisu::cPackUASTCETC1DisableFlipAndIndividual,
    };

    //
    // These mirror the constructors (we don't expose new/delete)
    //
    void image_resize_with_pitch(basisu::image *image, uint32_t w, uint32_t h, uint32_t p) {
        image->resize(w, h, p);
    }

    void image_resize(basisu::image *image, uint32_t w, uint32_t h) {
        image_resize_with_pitch(image, w, h, UINT32_MAX);
    }

    void image_init(basisu::image *image, const uint8_t *pData, uint32_t width, uint32_t height, uint32_t comps) {
        image->init(pData, width, height, comps);
    }

    bool image_get_pixel_at_checked(basisu::image *image, uint32_t x, uint32_t y, ColorU8 *pOutColor) {
        if (x >= image->get_width() || y >= image->get_height()) {
            return false;
        }

        basisu::color_rgba basis_color = (*image)(x, y);
        *pOutColor = *reinterpret_cast<ColorU8*>(&basis_color);
        return true;
    }

    ColorU8 image_get_pixel_at_unchecked(basisu::image *image, uint32_t x, uint32_t y) {
        basisu::color_rgba basis_color = (*image)(x, y);
        return *reinterpret_cast<ColorU8*>(&basis_color);
    }

    uint32_t image_get_width(basisu::image *image) {
        return image->get_width();
    }

    uint32_t image_get_height(basisu::image *image) {
        return image->get_height();
    }

    uint32_t image_get_pitch(basisu::image *image) {
        return image->get_pitch();
    }

    uint32_t image_get_total_pixels(basisu::image *image) {
        return image->get_total_pixels();
    }

    uint32_t image_get_block_width(basisu::image *image, uint32_t w) {
        return image->get_block_width(w);
    }

    uint32_t image_get_block_height(basisu::image *image, uint32_t h) {
        return image->get_block_height(h);
    }

    uint32_t image_get_total_blocks(basisu::image *image, uint32_t w, uint32_t h) {
        return image->get_total_blocks(w, h);
    }

    void image_clear(basisu::image *image) {
        image->clear();
    }

    struct PixelData {
        ColorU8 *pData;
        size_t length;
    };

    PixelData image_get_pixel_data(basisu::image *image) {
        PixelData data;
        basisu::color_rgba *colorPtr = image->get_pixels().data();
        data.pData = reinterpret_cast<ColorU8 *>(colorPtr);
        data.length = image->get_pixels().size();
        return data;
    }

    //
    // HDR Image
    //
    void imagef_resize_with_pitch(basisu::imagef *image, uint32_t w, uint32_t h, uint32_t p) {
        image->resize(w, h, p);
    }

    void imagef_resize(basisu::imagef *image, uint32_t w, uint32_t h) {
        imagef_resize_with_pitch(image, w, h, UINT32_MAX);
    }

    void imagef_init(basisu::imagef *image, const float *pData, uint32_t width, uint32_t height, uint32_t comps) {
        image->resize(width, height, UINT32_MAX);
        basisu::vec4F_vec &pixels = image->get_pixels();

        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                uint32_t i = y * width + x;

                if (comps == 1) {
                    pixels[i].set(
                        pData[i * comps],  // R
                        pData[i * comps],  // G (copy R)
                        pData[i * comps],  // B (copy R)
                        1.0f               // A
                    );
                } else if (comps == 2) {
                    pixels[i].set(
                        pData[i * comps],      // R
                        pData[i * comps],      // G (copy R)
                        pData[i * comps],      // B (copy R)
                        pData[i * comps + 1]   // A
                    );
                } else if (comps == 3) {
                    pixels[i].set(
                        pData[i * comps],      // R
                        pData[i * comps + 1],  // G
                        pData[i * comps + 2],  // B
                        1.0f                   // A
                    );
                } else { // comps >= 4
                    pixels[i].set(
                        pData[i * comps],      // R
                        pData[i * comps + 1],  // G
                        pData[i * comps + 2],  // B
                        pData[i * comps + 3]   // A
                    );
                }
            }
        }
    }

    bool imagef_get_pixel_at_checked(basisu::imagef *image, uint32_t x, uint32_t y, ColorF *pOutColor) {
        if (x >= image->get_width() || y >= image->get_height()) {
            return false;
        }

        basisu::vec4F basis_color = (*image)(x, y);
        *pOutColor = *reinterpret_cast<ColorF*>(&basis_color);
        return true;
    }

    ColorF imagef_get_pixel_at_unchecked(basisu::imagef *image, uint32_t x, uint32_t y) {
        basisu::vec4F basis_color = (*image)(x, y);
        return *reinterpret_cast<ColorF*>(&basis_color);
    }

    uint32_t imagef_get_width(basisu::imagef *image) {
        return image->get_width();
    }

    uint32_t imagef_get_height(basisu::imagef *image) {
        return image->get_height();
    }

    uint32_t imagef_get_pitch(basisu::imagef *image) {
        return image->get_pitch();
    }

    uint32_t imagef_get_total_pixels(basisu::imagef *image) {
        return image->get_total_pixels();
    }

    uint32_t imagef_get_block_width(basisu::imagef *image, uint32_t w) {
        return image->get_block_width(w);
    }

    uint32_t imagef_get_block_height(basisu::imagef *image, uint32_t h) {
        return image->get_block_height(h);
    }

    uint32_t imagef_get_total_blocks(basisu::imagef *image, uint32_t w, uint32_t h) {
        return image->get_total_blocks(w, h);
    }

    void imagef_clear(basisu::imagef *image) {
        image->clear();
    }

    struct HdrPixelData {
        ColorF *pData;
        size_t length;
    };

    HdrPixelData imagef_get_pixel_data(basisu::imagef *image) {
        HdrPixelData data;
        basisu::vec4F *colorPtr = image->get_pixels().data();
        data.pData = reinterpret_cast<ColorF *>(colorPtr);
        data.length = image->get_pixels().size();
        return data;
    }

    //
    // basisu::basis_compressor_params
    //
    struct CompressorParams {
        basisu::basis_compressor_params *pParams;
    };

    CompressorParams *compressor_params_new() {
        CompressorParams *params = new CompressorParams;
        params->pParams = new basisu::basis_compressor_params();
        return params;
    };

    void compressor_params_delete(CompressorParams *params) {
        delete params->pParams;
        delete params;
    }

    void compressor_params_clear(CompressorParams *params) {
        params->pParams->clear();
    }

    //
    // These function are used to load LDR image data into the compressor
    //
    basisu::image *compressor_params_get_or_create_source_image(CompressorParams *params, uint32_t index) {
        if (params->pParams->m_source_images.size() < index + 1) {
            params->pParams->m_source_images.resize(index + 1);
        }

        return &params->pParams->m_source_images[index];
    }

    void compressor_params_resize_source_image_list(CompressorParams *params, size_t size) {
        params->pParams->m_source_images.resize(size);
    }

    void compressor_params_clear_source_image_list(CompressorParams *params) {
        params->pParams->m_source_images.clear();
    }

    //
    // These function are used to load HDR image data into the compressor
    //
    basisu::imagef *compressor_params_get_or_create_source_hdr_image(CompressorParams *params, uint32_t index) {
        if (params->pParams->m_source_images_hdr.size() < index + 1) {
            params->pParams->m_source_images_hdr.resize(index + 1);
        }

        return &params->pParams->m_source_images_hdr[index];
    }

    void compressor_params_resize_source_hdr_image_list(CompressorParams *params, size_t size) {
        params->pParams->m_source_images_hdr.resize(size);
    }

    void compressor_params_clear_source_hdr_image_list(CompressorParams *params) {
        params->pParams->m_source_images_hdr.clear();
    }

    // These function are used to load custom mip map image data into the compressor
    //
    basisu::image *compressor_params_get_or_create_source_mipmap_image(CompressorParams *params, uint32_t index, uint32_t level) {
        if (params->pParams->m_source_mipmap_images.size() < index + 1) {
            params->pParams->m_source_mipmap_images.resize(index + 1);
        }
        if (params->pParams->m_source_mipmap_images[index].size() < level + 1) {
            params->pParams->m_source_mipmap_images[index].resize(level + 1);
        }

        return &params->pParams->m_source_mipmap_images[index][level];
    }

    void compressor_params_resize_source_mipmap_image_list(CompressorParams *params, size_t size) {
        params->pParams->m_source_mipmap_images.resize(size);
    }

    void compressor_params_resize_source_mipmap_image_level_list(CompressorParams *params, size_t level, size_t size) {
        params->pParams->m_source_mipmap_images[level].resize(size);
    }

    void compressor_params_clear_source_mipmap_image_list(CompressorParams *params) {
        params->pParams->m_source_mipmap_images.clear();
    }

    //
    // These set parameters for compression
    //
    void compressor_params_set_status_output(CompressorParams *params, bool status_output) {
        params->pParams->m_status_output = status_output;
    }

    // According to CLI --help, this only affects ETC1S
    void compressor_params_set_quality_level(CompressorParams *params, int quality_level) {
        params->pParams->m_etc1s_quality_level = quality_level;
    }

    UastcPackFlags compressor_params_get_pack_uastc_flags(CompressorParams *params) {
        return static_cast<UastcPackFlags>(params->pParams->m_pack_uastc_ldr_4x4_flags);
    }

    void compressor_params_set_pack_uastc_flags(CompressorParams *params, UastcPackFlags pack_uastc_flags) {
        params->pParams->m_pack_uastc_ldr_4x4_flags = static_cast<uint32_t>(pack_uastc_flags);
    }

    void compressor_params_set_format_mode(CompressorParams *params, basist::basis_tex_format m) {
        params->pParams->set_format_mode(m);
    }

    void compressor_params_set_perceptual(CompressorParams *params, bool perceptual) {
        params->pParams->m_perceptual = perceptual;
    }

    void compressor_params_set_mip_srgb(CompressorParams *params, bool mip_srgb) {
        params->pParams->m_mip_srgb = mip_srgb;
    }

    void compressor_params_set_no_selector_rdo(CompressorParams *params, bool no_selector_rdo) {
        params->pParams->m_no_selector_rdo = no_selector_rdo;
    }

    void compressor_params_set_no_endpoint_rdo(CompressorParams *params, bool no_endpoint_rdo) {
        params->pParams->m_no_endpoint_rdo = no_endpoint_rdo;
    }

    void compressor_params_set_rdo_uastc(CompressorParams *params, bool rdo_uastc) {
        params->pParams->m_rdo_uastc_ldr_4x4 = rdo_uastc;
    }

    void compressor_params_set_rdo_uastc_quality_scalar(CompressorParams *params, float rdo_uastc_quality_scalar) {
        params->pParams->m_rdo_uastc_ldr_4x4_quality_scalar = rdo_uastc_quality_scalar;
    }

    void compressor_params_set_generate_mipmaps(CompressorParams *params, bool generate_mipmaps) {
        params->pParams->m_mip_gen = generate_mipmaps;
    }

    void compressor_params_set_mip_smallest_dimension(CompressorParams *params, int mip_smallest_dimension) {
        params->pParams->m_mip_smallest_dimension = mip_smallest_dimension;
    }

    void compressor_params_set_mip_fast(CompressorParams *params, bool mip_fast) {
        params->pParams->m_mip_fast = mip_fast;
    }

    void compressor_params_set_hdr(CompressorParams *params, bool hdr) {
        params->pParams->m_hdr = hdr;
    }

    void compressor_params_set_hdr_mode(CompressorParams *params, basisu::hdr_modes hdr_mode) {
        params->pParams->m_hdr_mode = hdr_mode;
    }

    void compressor_params_set_hdr_favor_astc(CompressorParams *params, bool favor_astc) {
        params->pParams->m_hdr_favor_astc = favor_astc;
    }

    void compressor_params_set_create_ktx2_file(CompressorParams *params, bool ktx2_file) {
        params->pParams->m_create_ktx2_file = ktx2_file;
    }

    void compressor_params_set_ktx2_srgb_transfer_func(CompressorParams *params, bool srgb) {
        params->pParams->m_ktx2_srgb_transfer_func = srgb;
    }

    void compressor_params_set_ktx2_uastc_supercompression(CompressorParams *params, basist::ktx2_supercompression supercompression) {
        params->pParams->m_ktx2_uastc_supercompression = supercompression;
    }

    void compressor_params_set_ktx2_zstd_supercompression_level(CompressorParams *params, int level) {
        params->pParams->m_ktx2_zstd_supercompression_level = level;
    }

    void compressor_params_set_userdata(CompressorParams *params, uint32_t userdata0, uint32_t userdata1) {
        params->pParams->m_userdata0 = userdata0;
        params->pParams->m_userdata1 = userdata1;
    }

    // compressor_params_set_multithreaded is not implemented because this parameter is controlled by thread count
    // passed to compressor_new()

    //
    // basisu_compressor
    //
    struct Compressor {
        basisu::basis_compressor *pCompressor;

        // I'm making the job pool owned by the compressor because it doesn't look like sharing a job pool between
        // compressors is intended (compressors call wait_for_all on the job pool).
        basisu::job_pool *pJobPool;
    };

    // num_threads is passed directly to basisu::job_pool
    // num_threads is the TOTAL number of job pool threads, including the calling thread! So 2=1 new thread, 3=2 new threads, etc.
    Compressor *compressor_new(int num_threads) {
        Compressor *compressor = new Compressor;
        compressor->pCompressor = new basisu::basis_compressor();
        compressor->pJobPool = new basisu::job_pool(num_threads);
        return compressor;
    };

    void compressor_delete(Compressor *compressor) {
        delete compressor->pCompressor;
        delete compressor->pJobPool;
        delete compressor;
    }

    bool compressor_init(Compressor *compressor, const CompressorParams *params) {
        // Since this wrapper ties the job pool to the compressor, temporarily set it on the params and then clear it
        // later. (init() makes a copy of the params stored in the compressor)
        params->pParams->m_pJob_pool = compressor->pJobPool;
        params->pParams->m_multithreading = compressor->pJobPool->get_total_threads() > 1;
        bool result = compressor->pCompressor->init(*params->pParams);
        params->pParams->m_pJob_pool = nullptr;
        return result;
    }

    basisu::basis_compressor::error_code compressor_process(Compressor *compressor) {
        return compressor->pCompressor->process();
    }

    struct CompressorBasisFile {
        const uint8_t *pData;
        size_t length;
    };

    CompressorBasisFile compressor_get_output_basis_file(Compressor *compressor) {
        CompressorBasisFile file;
        const basisu::uint8_vec &basis_file = compressor->pCompressor->get_output_basis_file();
        file.pData = basis_file.data();
        file.length = basis_file.size();
        return file;
    }

    struct CompressorKtx2File {
        const uint8_t *pData;
        size_t length;
    };

    CompressorKtx2File compressor_get_output_ktx2_file(Compressor *compressor) {
        CompressorKtx2File file;
        const basisu::uint8_vec &ktx2_file = compressor->pCompressor->get_output_ktx2_file();
        file.pData = ktx2_file.data();
        file.length = ktx2_file.size();
        return file;
    }

    // Not implemented:
    //    const std::vector<image_stats> &compressor_get_stats();

    uint32_t compressor_get_basis_file_size(const Compressor *compressor) {
        return compressor->pCompressor->get_basis_file_size();
    }

    double compressor_get_basis_bits_per_texel(const Compressor *compressor) {
        return compressor->pCompressor->get_basis_bits_per_texel();
    }

    bool compressor_get_any_source_image_has_alpha(const Compressor *compressor) {
        return compressor->pCompressor->get_any_source_image_has_alpha();
    }

    void basisu_encoder_init() {
        basisu::basisu_encoder_init();
    }
}