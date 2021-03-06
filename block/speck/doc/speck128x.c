/**
  Copyright © 2017 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include "speck.h"

#ifndef SINGLE

void speck128x_setkey(
    const void *in, 
    void *out)
{
  uint64_t i, t, k0, k1;

  // copy 128-bit key to local space
  k0 = ((uint64_t*)in)[0];
  k1 = ((uint64_t*)in)[1];

  // expand 128-bit key into round keys
  for (i=0; i<32; i++) {
    ((uint64_t*)out)[i] = k0;
    
    k1 = (ROTR64(k1, 8) + k0) ^ i;
    k0 = ROTL64(k0,  3) ^ k1;
  }
}

void speck128x_encrypt(
    void *in,
    const void *keys)
{
  uint64_t i;
  uint64_t *ks=(uint64_t*)keys;
  
  // copy input to local space
  uint64_t x = ((uint64_t*)in)[0];
  uint64_t y = ((uint64_t*)in)[1];
  
  for (i=0; i<32; i++) {
    y = (ROTR64(y, 8) + x) ^ ks[i];
    x = ROTL64(x, 3)  ^ y;
  }
  // save result
  ((uint64_t*)in)[0] = x;
  ((uint64_t*)in)[1] = y;
}

#include <stdio.h>

int main(void)
{
  uint8_t key[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t txt[16]={0x20,0x6d,0x61,0x64,0x65,0x20,0x69,0x74,0x20,0x65,0x71,0x75,0x69,0x76,0x61,0x6c};
  uint8_t enc[16]={0x18,0x0d,0x57,0x5c,0xdf,0xfe,0x60,0x78,0x65,0x32,0x78,0x79,0x51,0x98,0x5d,0xa6};  
  int i;
  uint64_t keys[64];
  uint8_t *p=(uint8_t*)txt;
  
  speck128x_setkey(key, keys);
  speck128x_encrypt(txt, keys);
  
  putchar('\n');
  for (i=0; i<16; i++) { printf ("0x%02x,", txt[i]); }
  
  return 0;
}

#else

void speck128_encryptx(
    const void *key, 
    void *in)
{
  uint64_t i, t, k0, k1, k2, k3, x0, x1;
  
  // copy 256-bit key to local space
  k0 = ((uint64_t*)key)[0];
  k1 = ((uint64_t*)key)[1];
  k2 = ((uint64_t*)key)[2];
  k3 = ((uint64_t*)key)[3];
  
  // copy input to local space
  x0 = ((uint64_t*)in)[0];
  x1 = ((uint64_t*)in)[1];
  
  for (i=0; i<34; i++)
  {
    // encrypt block
    x1 = (ROTR64(x1, 8) + x0) ^ k0;
    x0 =  ROTL64(x0, 3) ^ x1;
    
    // create next subkey
    k1 = (ROTR64(k1, 8) + k0) ^ i;
    k0 =  ROTL64(k0, 3) ^ k1;

    XCHG(k3, k2);
    XCHG(k3, k1);    
  }
  // save result
  ((uint64_t*)in)[0] = x0;
  ((uint64_t*)in)[1] = x1;
}

#endif