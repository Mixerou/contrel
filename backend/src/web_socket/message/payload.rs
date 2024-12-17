use paste::paste;
use rmpv::{Utf8String, Value};
use serde::de::Error;
use serde::ser::SerializeSeq;
use serde::{Deserialize, Deserializer, Serialize, Serializer};

use crate::payload_enum_helper;

payload_enum_helper! {
    #[derive(Clone, Debug, Default)]
    enum WebSocketMessagePayload {
        // Opcode: Request
        RequestPostTestMessage { message: String } = "100" | 100,

        // Opcode: Response
        ResponseTestMessage { message: String } = "200" | 200,

        // Opcode: Authorize
        Authorize { token: String, } = "300" | 300,

        // Other
        #[default]
        None = "0" | 0,
    }
}

impl WebSocketMessagePayload {
    pub fn is_none(&self) -> bool {
        matches!(self, WebSocketMessagePayload::None)
    }
}

impl<'de> Deserialize<'de> for WebSocketMessagePayload {
    fn deserialize<D: Deserializer<'de>>(deserializer: D) -> Result<Self, D::Error> {
        let mut values = Vec::new();
        let value = Value::deserialize(deserializer)?;
        let Some(payload) = value.as_array() else {
            return Err(Error::custom("Not an array"));
        };
        let Some(payload_type) = &payload.first() else {
            return Err(Error::custom("There are no elements in payload"));
        };

        values.append(&mut payload.to_vec());
        values[0] = Value::String(Utf8String::from(payload_type.to_string()));

        Ok(
            WebSocketMessagePayloadHelper::deserialize(Value::Array(values))
                .unwrap_or_default()
                .into(),
        )
    }
}

impl Serialize for WebSocketMessagePayload {
    fn serialize<S: Serializer>(&self, serializer: S) -> Result<S::Ok, S::Error> {
        let mut message: S::SerializeSeq = serializer.serialize_seq(None)?;

        self.serialize_fields::<S>(&mut message)?;
        message.end()
    }
}
