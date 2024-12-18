use serde::Serializer;
use time::PrimitiveDateTime;

pub fn serialize_primitive_date_time_as_unix_timestamp<S>(
    date: &PrimitiveDateTime,
    serializer: S,
) -> Result<S::Ok, S::Error>
where
    S: Serializer,
{
    let unix_timestamp = date.assume_utc().unix_timestamp();
    serializer.serialize_i64(unix_timestamp)
}
