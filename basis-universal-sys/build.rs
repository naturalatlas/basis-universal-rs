// args from the basis cmake file
fn build_with_common_settings() -> cc::Build {
    let mut build = cc::Build::new();
    build
        .flag_if_supported("-fvisibility=hidden")
        .flag_if_supported("-fno-strict-aliasing")
        .flag_if_supported("-Wall")
        .flag_if_supported("-Wextra")
        .flag_if_supported("-Wno-bitwise-instead-of-logical")
        .flag_if_supported("-Wno-unknown-warning-option")
        .flag_if_supported("-Wno-unused-function")
        .flag_if_supported("-Wno-unused-but-set-variable")
        .flag_if_supported("-Wno-unused-local-typedefs")
        .flag_if_supported("-Wno-unused-value")
        .flag_if_supported("-Wno-unused-parameter")
        .flag_if_supported("-Wno-unused-variable");

    build
}

fn main() {
    let mut build = build_with_common_settings();

    build
        .cpp(true)
        .flag_if_supported("--std=c++17")
        .define("BASISU_SUPPORT_SSE", "0");

    // build.define("BASISU_FORCE_DEVEL_MESSAGES", "1");

    // build
    //     .define("BASISD_SUPPORT_DXT1", "0")
    //     .define("BASISD_SUPPORT_PVRTC1", "0")
    //     .define("BASISD_SUPPORT_ETC2_EAC_A8", "0")
    //     .define("BASISD_SUPPORT_ASTC", "0")
    //     .define("BASISD_SUPPORT_ATC", "0")
    //     .define("BASISD_SUPPORT_ASTC_HIGHER_OPAQUE_QUALITY", "0")
    //     .define("BASISD_SUPPORT_ETC2_EAC_RG11", "0")
    //     .define("BASISD_SUPPORT_FXT1", "0")
    //     .define("BASISD_SUPPORT_PVRTC2", "0");

    #[cfg(feature = "ktx2")]
    build.define("BASISD_SUPPORT_KTX2", "1");
    #[cfg(not(feature = "ktx2"))]
    build.define("BASISD_SUPPORT_KTX2", "0");

    #[cfg(feature = "transcoding")]
    {
        build.file("vendor/basis_universal/transcoder/basisu_transcoder.cpp");
        build.file("vendor/transcode_wrapper.cpp");
    }

    #[cfg(feature = "encoding")]
    {
        build.file("vendor/basis_universal/encoder/basisu_backend.cpp");
        build.file("vendor/basis_universal/encoder/basisu_basis_file.cpp");
        build.file("vendor/basis_universal/encoder/basisu_comp.cpp");
        build.file("vendor/basis_universal/encoder/basisu_enc.cpp");
        build.file("vendor/basis_universal/encoder/basisu_etc.cpp");
        build.file("vendor/basis_universal/encoder/basisu_frontend.cpp");
        build.file("vendor/basis_universal/encoder/basisu_gpu_texture.cpp");
        build.file("vendor/basis_universal/encoder/basisu_pvrtc1_4.cpp");
        build.file("vendor/basis_universal/encoder/basisu_resampler.cpp");
        build.file("vendor/basis_universal/encoder/basisu_resample_filters.cpp");
        build.file("vendor/basis_universal/encoder/basisu_ssim.cpp");
        build.file("vendor/basis_universal/encoder/basisu_uastc_enc.cpp");
        build.file("vendor/basis_universal/encoder/basisu_bc7enc.cpp");
        build.file("vendor/basis_universal/encoder/jpgd.cpp");
        build.file("vendor/basis_universal/encoder/basisu_kernels_sse.cpp");
        build.file("vendor/basis_universal/encoder/basisu_opencl.cpp");
        build.file("vendor/basis_universal/encoder/pvpngreader.cpp");
        build.file("vendor/basis_universal/encoder/basisu_uastc_hdr_4x4_enc.cpp");
        build.file("vendor/basis_universal/encoder/basisu_astc_hdr_6x6_enc.cpp");
        build.file("vendor/basis_universal/encoder/basisu_astc_hdr_common.cpp");
        build.file("vendor/basis_universal/encoder/3rdparty/android_astc_decomp.cpp");
        build.file("vendor/basis_universal/encoder/3rdparty/tinyexr.cpp");
        build.file("vendor/basis_universal/transcoder/basisu_transcoder.cpp");

        build.file("vendor/encode_wrapper.cpp");
    }

    #[cfg(feature = "zstd")]
    {
        build.define("BASISD_SUPPORT_KTX2_ZSTD", "1");
        build.file("vendor/basis_universal/zstd/zstd.c");
    }

    #[cfg(not(feature = "zstd"))]
    build.define("BASISD_SUPPORT_KTX2_ZSTD", "0");

    build.compile("basisuniversal");

    // We regenerate binding code and check it in. (See generate_bindings.sh)
}
