/**                                                                                           //
 * Copyright (c) 2013-2016, The Kovri I2P Router Project                                      //
 *                                                                                            //
 * All rights reserved.                                                                       //
 *                                                                                            //
 * Redistribution and use in source and binary forms, with or without modification, are       //
 * permitted provided that the following conditions are met:                                  //
 *                                                                                            //
 * 1. Redistributions of source code must retain the above copyright notice, this list of     //
 *    conditions and the following disclaimer.                                                //
 *                                                                                            //
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list     //
 *    of conditions and the following disclaimer in the documentation and/or other            //
 *    materials provided with the distribution.                                               //
 *                                                                                            //
 * 3. Neither the name of the copyright holder nor the names of its contributors may be       //
 *    used to endorse or promote products derived from this software without specific         //
 *    prior written permission.                                                               //
 *                                                                                            //
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY        //
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF    //
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL     //
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       //
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,               //
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    //
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,          //
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF    //
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               //
 *                                                                                            //
 * Parts of the project are originally copyright (c) 2013-2015 The PurpleI2P Project          //
 */

#include "base64.h"

#include <stdlib.h>

namespace i2p {
namespace util {

static void iT64Build(void);

// BASE64 Substitution Table
// -------------------------

// Direct Substitution Table
static char T64[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '-', '~'
};

const char* GetBase64SubstitutionTable() {
  return T64;
}

// Reverse Substitution Table (built in run time)
static char iT64[256];
static int isFirstTime = 1;


// Padding
static char P64 = '=';


size_t ByteStreamToBase64(
    const uint8_t* InBuffer,
    size_t InCount,
    char* OutBuffer,
    size_t len) {
  unsigned char* ps;
  unsigned char* pd;
  unsigned char   acc_1;
  unsigned char   acc_2;
  int       i;
  int       n;
  int       m;
  size_t outCount;
  ps = (unsigned char *)InBuffer;
  n = InCount / 3;
  m = InCount % 3;
  if (!m)
     outCount = 4 * n;
  else
     outCount = 4 * (n + 1);
  if (outCount > len) return 0;
  pd = (unsigned char *)OutBuffer;
  for (i = 0; i < n; i++) {
     acc_1 = *ps++;
     acc_2 = (acc_1<<4)&0x30;
     acc_1 >>= 2;           // base64 digit #1
     *pd++ = T64[acc_1];
     acc_1 = *ps++;
     acc_2 |= acc_1 >> 4;   // base64 digit #2
     *pd++ = T64[acc_2];
     acc_1 &= 0x0f;
     acc_1 <<=2;
     acc_2 = *ps++;
     acc_1 |= acc_2>>6;     // base64 digit #3
     *pd++ = T64[acc_1];
     acc_2 &= 0x3f;         // base64 digit #4
     *pd++ = T64[acc_2];
  }
  if (m == 1) {
     acc_1 = *ps++;
     acc_2 = (acc_1<<4)&0x3f;  // base64 digit #2
     acc_1 >>= 2;            // base64 digit #1
     *pd++ = T64[acc_1];
     *pd++ = T64[acc_2];
     *pd++ = P64;
     *pd++ = P64;
  } else if (m == 2) {
     acc_1 = *ps++;
     acc_2 = (acc_1<<4)&0x3f;
     acc_1 >>= 2;           // base64 digit #1
     *pd++ = T64[acc_1];
     acc_1 = *ps++;
     acc_2 |= acc_1 >> 4;   // base64 digit #2
     *pd++ = T64[acc_2];
     acc_1 &= 0x0f;
     acc_1 <<=2;            // base64 digit #3
     *pd++ = T64[acc_1];
     *pd++ = P64;
  }
  return outCount;
}


size_t Base64ToByteStream(
    const char* InBuffer,
    size_t InCount,
    uint8_t* OutBuffer,
    size_t len) {
  unsigned char *ps, *pd,
                acc_1, acc_2;
  int       i, n, m;
  size_t outCount;
  if (isFirstTime)
    iT64Build();
  n = InCount / 4;
  m = InCount % 4;
  if (InCount && !m) {
     outCount = 3 * n;
  } else {
     outCount = 0;
     return 0;
  }
  ps = (unsigned char *)(InBuffer + InCount - 1);
  while (*ps-- == P64)
    outCount--;
  ps = (unsigned char *)InBuffer;
  if (outCount > len)
    return 0;
  pd = OutBuffer;
  auto endOfOutBuffer = OutBuffer + outCount;
  for (i = 0; i < n; i++) {
     acc_1 = iT64[*ps++];
     acc_2 = iT64[*ps++];
     acc_1 <<= 2;
     acc_1 |= acc_2>>4;
     *pd++  = acc_1;
     if (pd >= endOfOutBuffer)
       break;
     acc_2 <<= 4;
     acc_1 = iT64[*ps++];
     acc_2 |= acc_1 >> 2;
     *pd++ = acc_2;
     if (pd >= endOfOutBuffer)
       break;
     acc_2 = iT64[*ps++];
     acc_2 |= acc_1 << 6;
     *pd++ = acc_2;
  }
  return outCount;
}

// iT64
// ----
// Reverse table builder. P64 character is replaced with 0
static void iT64Build() {
  int  i;
  isFirstTime = 0;
  for (i = 0; i < 256; i++)
    iT64[i] = -1;
  for (i = 0; i < 64; i++)
    iT64[static_cast<int>(T64[i])] = i;
  iT64[static_cast<int>(P64)] = 0;
}

size_t Base32ToByteStream(
    const char* inBuf,
    size_t len,
    uint8_t* outBuf,
    size_t outLen) {
  int tmp = 0, bits = 0;
  size_t ret = 0;
  for (size_t i = 0; i < len; i++) {
    char ch = inBuf[i];
    if (ch >= '2' && ch <= '7')  // digit
      ch = (ch - '2') + 26;  // 26 means a-z
    else if (ch >= 'a' && ch <= 'z')
      ch = ch - 'a';  // a = 0
    else
      return 0;  // unexpected character
    tmp |= ch;
    bits += 5;
    if (bits >= 8) {
      if (ret >= outLen)
        return ret;
      outBuf[ret] = tmp >> (bits - 8);
      bits -= 8;
      ret++;
    }
    tmp <<= 5;
  }
  return ret;
}

size_t ByteStreamToBase32(
    const uint8_t* inBuf,
    size_t len,
    char* outBuf,
    size_t outLen) {
  if (!len)
    return 0;  // No data given
  size_t ret = 0, pos = 1;
  int bits = 8, tmp = inBuf[0];
  while (ret < outLen && (bits > 0 || pos < len)) {
    if (bits < 5) {
      if (pos < len) {
        tmp <<= 8;
        tmp |= inBuf[pos] & 0xFF;
        pos++;
        bits += 8;
      } else {  // last byte
        tmp <<= (5 - bits);
        bits = 5;
      }
    }
    bits -= 5;
    int ind = (tmp >> bits) & 0x1F;
    outBuf[ret] = (ind < 26) ? (ind + 'a') : ((ind - 26) + '2');
    ret++;
  }
  return ret;
}

}  // namespace util
}  // namespace i2p
