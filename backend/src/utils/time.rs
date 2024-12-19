use serde::{Deserialize, Deserializer, Serializer};
use time::{OffsetDateTime, PrimitiveDateTime};

pub fn deserialize_from_unix_timestamp<'de, D>(
    deserializer: D,
) -> Result<PrimitiveDateTime, D::Error>
where
    D: Deserializer<'de>,
{
    let timestamp = i64::deserialize(deserializer)?;
    let offset_datetime =
        OffsetDateTime::from_unix_timestamp(timestamp).map_err(serde::de::Error::custom)?;
    Ok(PrimitiveDateTime::new(
        offset_datetime.date(),
        offset_datetime.time(),
    ))
}

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
