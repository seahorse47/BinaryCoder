//
//  DESWrapper.cpp
//  BinaryCoder
//
//  Created by hailong on 13-4-5.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#include "DESWrapper.h"
#include "memory.h"
extern "C" {
#include "des.h"
}

unsigned long DESWrapper::encrypt(const unsigned char key[8], const unsigned char* data, unsigned long num_bytes, unsigned char* output)
{
    DES_KS ks;
    deskey(ks, (unsigned char*)key, 0);
    
    memcpy(output, data, num_bytes);
    unsigned long l = ((num_bytes+7)>>3)<<3;
    for (unsigned long i=num_bytes; i<l; i++) {
        output[i] = 0;
    }
    
    unsigned char* ptr = output;
    unsigned char* end = output + l;
    while (ptr < end) {
        des(ks, ptr);
        ptr += 8;
    }
    
    return l;
}

unsigned long DESWrapper::decrypt(const unsigned char key[8], const unsigned char* data, unsigned long num_bytes, unsigned char* output)
{
    DES_KS ks;
    deskey(ks, (unsigned char*)key, 1);
    
    unsigned long l = num_bytes;
//    assert((num_bytes & 0x07) == 0);
    memcpy(output, data, num_bytes);
    unsigned char* ptr = output;
    unsigned char* end = output + l;
    while (ptr < end) {
        des(ks, ptr);
        ptr += 8;
    }
    
    return l;
}
