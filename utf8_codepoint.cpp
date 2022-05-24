
#include "utf8_codepoint.h"

UTF8CodePoint::UTF8CodePoint(char32_t code_point) : code_point(code_point) {}

UTF8CodePoint::operator char32_t() const {
    return code_point;
}

// | First code point |  Byte 1  |  Byte 2  |  Byte 3  |  Byte 4  |
// |:----------------:|:--------:|:--------:|:--------:|:--------:|
// |           U+0000 | 0xxxxxxx |          |          |          |
// |           U+0080 | 110xxxxx | 10xxxxxx |          |          |
// |           U+0800 | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
// |          U+10000 | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
ostream &operator<<(ostream &os, const UTF8CodePoint cp) {
    if (cp.code_point < 0x80) {
        os.put(cp.code_point);
    } else if (cp.code_point < 0x800) {
        os.put(0b1100'0000 | (cp.code_point >> 6 & 0b0001'1111));
        os.put(0b1000'0000 | (cp.code_point & 0b0011'1111));
    } else if (cp.code_point < 0x10000) {
        os.put(0b1110'0000 | (cp.code_point >> 12 & 0b0000'1111));
        os.put(0b1000'0000 | (cp.code_point >> 6 & 0b0011'1111));
        os.put(0b1000'0000 | (cp.code_point & 0b0011'1111));
    } else {  // if (cp.code_point < 0x200000)
        os.put(0b1111'0000 | (cp.code_point >> 18 & 0b0000'0111));
        os.put(0b1000'0000 | (cp.code_point >> 12 & 0b0011'1111));
        os.put(0b1000'0000 | (cp.code_point >> 6 & 0b0011'1111));
        os.put(0b1000'0000 | (cp.code_point & 0b0011'1111));
    }
    return os;
}

// | First code point |  Byte 1  |  Byte 2  |  Byte 3  |  Byte 4  |
// |:----------------:|:--------:|:--------:|:--------:|:--------:|
// |           U+0000 | 0xxxxxxx |          |          |          |
// |           U+0080 | 110xxxxx | 10xxxxxx |          |          |
// |           U+0800 | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
// |          U+10000 | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
istream &operator>>(istream &is, UTF8CodePoint &cp) {
    unsigned char bytes[4];
    is.read(reinterpret_cast<char *>(&bytes[0]), 1);

    // Note: We don't need to check if is.read(..., 1) failed here because we
    // check if (!is) after is.read().

    // Figure out how many bytes we need to read for this UTF-8 code point.
    int num_bytes;
    if ((bytes[0] & 0b1000'0000) == 0b0000'0000) {
        num_bytes = 1;
    } else if ((bytes[0] & 0b1110'0000) == 0b1100'0000) {
        num_bytes = 2;
    } else if ((bytes[0] & 0b1111'0000) == 0b1110'0000) {
        num_bytes = 3;
    } else if ((bytes[0] & 0b1111'1000) == 0b1111'0000) {
        num_bytes = 4;
    } else {
        is.putback(bytes[0]);
        is.setstate(std::ios_base::failbit);
        return is;
    }

    // We've already read the 0th byte and we need to read the rest.
    is.read(reinterpret_cast<char *>(bytes + 1), num_bytes - 1);
    if (!is) {
        return is;
    }

    // Check bytes after bytes[0] are valid (i.e. highest 2 bits are "10")
    for (int i = 1; i < num_bytes; ++i) {
        if ((bytes[i] & 0b1100'0000) != 0b1000'0000) {
            for (int j = 1; j < num_bytes; ++j) {
                is.putback(bytes[j]);
            }
            is.setstate(std::ios_base::failbit);
            return is;
        }
    }

    switch (num_bytes) {
        case 1:
            cp.code_point = bytes[0];
            return is;
        case 2:
            cp.code_point = (bytes[1] & 0b0011'1111) | (bytes[0] & 0b0001'1111) << 6;
            return is;
        case 3:
            cp.code_point = ((bytes[2] & 0b0011'1111) |
                             (bytes[1] & 0b0011'1111) << 6 |
                             (bytes[0] & 0b0000'1111) << 12);
            return is;
        case 4:
            cp.code_point = ((bytes[3] & 0b0011'1111) |
                             (bytes[2] & 0b0011'1111) << 6 |
                             (bytes[1] & 0b0011'1111) << 12 |
                             (bytes[0] & 0b0000'0111) << 18);
            return is;
    }

    // Should never reach this line
    return is;
}
