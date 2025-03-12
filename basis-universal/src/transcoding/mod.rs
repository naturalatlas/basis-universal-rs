use basis_universal_sys as sys;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Mutex;

mod enums;
pub use enums::*;

mod transcode_basis;
pub use transcode_basis::*;

mod transcode_lowlevel_uastc_ldr_4x4;
pub use transcode_lowlevel_uastc_ldr_4x4::*;
mod transcode_lowlevel_uastc_hdr_4x4;
pub use transcode_lowlevel_uastc_hdr_4x4::*;

#[cfg(feature = "ktx2")]
mod transcode_ktx2;
#[cfg(feature = "ktx2")]
pub use transcode_ktx2::*;

#[cfg(test)]
mod transcoding_tests;

/// Error result from trying to transcode an image
#[derive(Debug, Clone)]
pub enum TranscodeError {
    TranscodeFormatNotSupported,
    ImageLevelNotFound,
    TranscodeFailed,
    Invalid(String),
}

static TRANSCODER_INIT_CALLED: AtomicBool = AtomicBool::new(false);
lazy_static::lazy_static! {
    static ref TRANSCODER_INIT_LOCK: Mutex<()> = Mutex::default();
}

/// The underlying C++ library requires that transcoder_init() has been called before a .basis file
/// can be encoded. This function allows a user to do this early in the application explicitly. It
/// is protected by a lock and AtomicBool flag so it is safe and cheap to call multiple times, and
/// correctly handles multiple threads trying to initialize at the same time.
pub fn transcoder_init() {
    unsafe {
        // Early out if it has been initialized
        if !TRANSCODER_INIT_CALLED.load(Ordering::Acquire) {
            // Lock and check again to ensure that exactly one thread runs the init code and that
            // all other threads wait for it to complete and don't re-run it.
            let lock = TRANSCODER_INIT_LOCK.lock().unwrap();
            if !TRANSCODER_INIT_CALLED.load(Ordering::Acquire) {
                // Run the init code
                sys::basisu_transcoder_init();
                TRANSCODER_INIT_CALLED.store(true, Ordering::Release);
            }
            std::mem::drop(lock);
        }
    }
}
