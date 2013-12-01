//
//  DESWrapper.h
//  BinaryCoder
//
//  Created by hailong on 13-4-5.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#ifndef __BinaryCoder__DESWrapper__
#define __BinaryCoder__DESWrapper__

class DESWrapper
{
public:
    /* Encrypt data
       key          64 bits key (only 56 bits used)
       data         input data
       num_bytes    byte length of input data
       output       output buffer, its minimal length is the multiple of 8 which is not less than `num_bytes'
       return   the length of output data
     */
    unsigned long encrypt(const unsigned char key[8], const unsigned char* data, unsigned long num_bytes, unsigned char* output);
    /* Decrypt data
       key          64 bits key (only 56 bits used)
       data         the encrypted data
       num_bytes    byte length of input data, should be the multiple of 8
       output       output buffer, its length must not less than `num_bytes'
       return   the length of output data
     */
    unsigned long decrypt(const unsigned char key[8], const unsigned char* data, unsigned long num_bytes, unsigned char* output);
};

#endif /* defined(__BinaryCoder__DESWrapper__) */
