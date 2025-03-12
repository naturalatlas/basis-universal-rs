#[allow(non_upper_case_globals)]
#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[allow(deref_nullptr)]
#[rustfmt::skip]
#[cfg(feature = "transcoding")]
mod transcoding_bindings;
#[cfg(feature = "transcoding")]
pub use transcoding_bindings::*;

#[allow(non_upper_case_globals)]
#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[allow(deref_nullptr)]
#[rustfmt::skip]
#[cfg(feature = "encoding")]
mod encoding_bindings;
#[cfg(feature = "encoding")]
pub use encoding_bindings::*;
