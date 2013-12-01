//
//  main.cpp
//  BinaryCoder
//
//  Created by hailong on 13-2-7.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#include "Stream.h"
#include "Encoder.h"
#include "Decoder.h"
#include "DESWrapper.h"
#include "CrypticStream.h"

void print_block(const unsigned char* block, int size) {
    for (int x=0; x<size; x++) {
        if (0==(x&7) && x>0) printf("\n");
        printf("0x%02x ", block[x]);
    }
    printf("\n");
}

int main(int argc, const char * argv[])
{
    // insert code here...
    unsigned char* key = (unsigned char*)"abcdefgh";
    const char* filename = "a.out";
    binary_coder::OutputFile file(filename, true);
    binary_coder::OutputStreamDES desOutput(&file, false);
    desOutput.setDESKey(key);
    binary_coder::Encoder encoder(&desOutput);
    
    encoder.WriteByte(123);
    encoder.WriteShort(5000);
    encoder.WriteBits(0x07, 4);
    encoder.AlignToByte();
    encoder.WriteInt(10010002);
    encoder.WriteString("hello, world");
    encoder.Flush();
    desOutput.Seal();
    
    printf("---------------------------\n");
    
    binary_coder::InputFile inFile(filename, true);
    binary_coder::InputStreamDES desInput(&inFile, false);
    desInput.setDESKey(key);
    binary_coder::Decoder decoder(&desInput);
    int8_t b1 = decoder.ReadSignedByte();
    int16_t s1 = decoder.ReadSignedShort();
    int bits = decoder.ReadSignedBits(4);
    decoder.AlignToByte();
    int32_t i = decoder.ReadSignedInt();
    std::string hello;
    decoder.ReadString(hello);
    printf("%d\n%d\n%x\n%d\n%s\n\n", b1, s1, bits, i, hello.c_str());
    
//    unsigned char* key = (unsigned char*)"abcdefgh";
//    unsigned char* s2 = (unsigned char*)"Abcdefgh";
    
//    DESWrapper des;
//    char hello[] = "hello, world";
//    char buffer[32];
//    size_t size = sizeof(hello);
//    print_block((unsigned char*)hello, (int)size); printf("\n");
//    
//    size_t l = des.encrypt(key, (unsigned char*)hello, size, (unsigned char*)buffer);
//    print_block((unsigned char*)buffer, (int)l); printf("\n");
//    
//    l = des.decrypt(key, (unsigned char*)buffer, l, (unsigned char*)buffer);
//    print_block((unsigned char*)buffer, (int)l); printf("\n");
    
    return 0;
}

