uint8_t
decode_hex_char(
    _In_ char c
);

extern uint32_t
decode_hex_buffer(
    _In_z_ const char* HexBuffer,
    _In_ uint32_t OutBufferLen,
    _Out_writes_to_(OutBufferLen, return)
        uint8_t* OutBuffer
);