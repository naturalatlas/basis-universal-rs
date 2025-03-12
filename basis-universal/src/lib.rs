//! Bindings for Binomial LLC's basis-universal Supercompressed GPU Texture Codec
//!
//! basis-universal functionality can be grouped into two categories:
//!
//! * Encoding: Compresses and encode textures (optionally combining multiple images and mipmap
//!   layers in a single file/binary blob)
//! * Transcoding: Unpacks the texture into GPU-friendly compression formats. The final format can
//!   be chosen based on what the available GPU hardware can support.
//!
//! Encoding can be done ahead of time using a command line tool in the upstream repository.
//!
//! The encoded data can either be stored as a file or a binary blob. This data can include multiple
//! images, and each image can store multiple levels. This is commonly used for storing cube
//! textures and textures with precomputed mipmaps. This library also supports generating mipmaps
//! for you.
//!
//! Please refer to https://github.com/BinomialLLC/basis_universal for more details.

/// Support for transcoding basis-universal form to GPU-friendly formats.
#[cfg(feature = "transcoding")]
pub mod transcoding;
#[cfg(feature = "transcoding")]
pub use transcoding::*;

/// Support for compressing raw image data to basis-universal form
#[cfg(feature = "encoding")]
pub mod encoding;
#[cfg(feature = "encoding")]
pub use encoding::*;

pub use basis_universal_sys as sys;

/// Arbitrary data that can be attached to a basis-universal file/binary blob
#[derive(Default, Debug, Copy, Clone)]
pub struct UserData {
    pub userdata0: u32,
    pub userdata1: u32,
}
