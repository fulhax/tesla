#ifndef __SHA256_HPP__
    #define __SHA256_HPP__

    #include <unistd.h>
    void calculate_sha256(void *buffer, size_t len, unsigned char *result);

#endif //__SHA256_HPP__
