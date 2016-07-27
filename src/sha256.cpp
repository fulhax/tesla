#include "sha256.hpp"
#include <stdint.h>

struct sha256 {
    uint64_t bits;
    unsigned int datalen;
    unsigned char data[64];
    unsigned int hash[8];
};

unsigned int constants[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void init_sha256(sha256 *sha)
{
    sha->bits    = 0;
    sha->datalen = 0;
    sha->hash[0] = 0x6a09e667;
    sha->hash[1] = 0xbb67ae85;
    sha->hash[2] = 0x3c6ef372;
    sha->hash[3] = 0xa54ff53a;
    sha->hash[4] = 0x510e527f;
    sha->hash[5] = 0x9b05688c;
    sha->hash[6] = 0x1f83d9ab;
    sha->hash[7] = 0x5be0cd19;
}
inline unsigned int rotate_bits(unsigned int x, unsigned char bits)
{
    return (x << bits | x >> (32 - bits));
}

void transform_sha256(sha256 *sha)
{
    unsigned int a, b, c, d, e, f, g, h, m[64] = {0};

    for (unsigned int i = 0; i < 16; i++) {
        unsigned int j = i * 4;
        m[i] = (sha->data[j] << 24) | (sha->data[j + 1] << 16) |
               (sha->data[j + 2] << 8) | (sha->data[j + 3]);
    }

    for (unsigned int i = 16; i < 64; i++) {
        unsigned int s0 = (rotate_bits(m[i - 15], 7)) ^ (rotate_bits(m[i - 15],
                          18)) ^ (m[i - 15] >> 3);
        unsigned int s1 = (rotate_bits(m[i - 2], 17)) ^ (rotate_bits(m[i - 2],
                          19)) ^ (m[i - 2] >> 10);
        m[i] = m[i - 16] + s0 + m[i - 7] + s1;
    }

    a = sha->hash[0];
    b = sha->hash[1];
    c = sha->hash[2];
    d = sha->hash[3];
    e = sha->hash[4];
    f = sha->hash[5];
    g = sha->hash[6];
    h = sha->hash[7];

    for (unsigned int i = 0; i < 64; i++) {
        unsigned int S1   = (rotate_bits(e, 6)) ^ (rotate_bits(e,
                            11)) ^ (rotate_bits(e, 25));
        unsigned int ch   = (e & f) ^ ((~e) & g);
        unsigned int tmp1 = h + S1 + ch + constants[i] + m[i];
        unsigned int S0   = (rotate_bits(a, 2)) ^ (rotate_bits(a,
                            13)) ^ (rotate_bits(a, 22));
        unsigned int maj  = (a & b) ^ (a & c) ^ (b & c);
        unsigned int tmp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + tmp1;
        d = c;
        c = b;
        b = a;
        a = tmp1 + tmp2;
    }

    sha->hash[0] += a;
    sha->hash[1] += b;
    sha->hash[2] += c;
    sha->hash[3] += d;
    sha->hash[4] += e;
    sha->hash[5] += f;
    sha->hash[6] += g;
    sha->hash[7] += h;
}

void update_sha256(unsigned char *buffer, size_t len, sha256 *sha)
{
    for (size_t i = 0; i < len; i++) {
        sha->data[sha->datalen] = buffer[i];
        sha->datalen++;

        if (sha->datalen == 64) {
            transform_sha256(sha);
            sha->bits += 512;
            sha->datalen = 0;
        }
    }
}

void final_sha256(sha256 *sha) // pads the buffer to the right length
{
    if (sha->datalen < 56) {
        unsigned int i = sha->datalen;
        sha->data[i++] = 0x80;

        while (i < 56) {
            sha->data[i++] = 0;
        }
    } else {
        unsigned int i = sha->datalen;
        sha->data[i] = 0x80;
        i++;

        while (i < 64) {
            sha->data[i++] = 0;
        }

        transform_sha256(sha);

        for (i = 0; i < 56; i++) {
            sha->data[i] = 0;
        }
    }

    sha->bits += sha->datalen * 8;
    sha->data[56] = sha->bits >> 56;
    sha->data[57] = sha->bits >> 48;
    sha->data[58] = sha->bits >> 40;
    sha->data[59] = sha->bits >> 32;
    sha->data[60] = sha->bits >> 24;
    sha->data[61] = sha->bits >> 16;
    sha->data[62] = sha->bits >> 8;
    sha->data[63] = sha->bits;

    transform_sha256(sha);
}

void calculate_sha256(void *buffer, size_t len, unsigned char *result)
{
    sha256 sha;
    init_sha256(&sha);
    update_sha256((unsigned char *)buffer, len, &sha);
    final_sha256(&sha);

    for (unsigned int i = 0; i < 4; ++i) { // change endianness
        result[i]      = (sha.hash[0] >> (24 - i * 8)) & 0x000000ff;
        result[i + 4]  = (sha.hash[1] >> (24 - i * 8)) & 0x000000ff;
        result[i + 8]  = (sha.hash[2] >> (24 - i * 8)) & 0x000000ff;
        result[i + 12] = (sha.hash[3] >> (24 - i * 8)) & 0x000000ff;
        result[i + 16] = (sha.hash[4] >> (24 - i * 8)) & 0x000000ff;
        result[i + 20] = (sha.hash[5] >> (24 - i * 8)) & 0x000000ff;
        result[i + 24] = (sha.hash[6] >> (24 - i * 8)) & 0x000000ff;
        result[i + 28] = (sha.hash[7] >> (24 - i * 8)) & 0x000000ff;
    }
}
