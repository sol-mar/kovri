/**
 * Copyright (c) 2015-2016, The Kovri I2P Router Project
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>
#include <cryptopp/integer.h>
#include <cryptopp/eccrypto.h>
#include "util/Log.h"
#include "Signature.h"

namespace i2p {
namespace crypto {

DSASigner::DSASigner(const uint8_t * signingPrivateKey)
{
    m_PrivateKey.Initialize(
        dsap, dsaq, dsag,
        CryptoPP::Integer(signingPrivateKey, DSA_PRIVATE_KEY_LENGTH)
    );
}

void DSASigner::Sign(CryptoPP::RandomNumberGenerator& rnd, const uint8_t * buf,
 int len, uint8_t * signature) const
{
    CryptoPP::DSA::Signer signer(m_PrivateKey);
    signer.SignMessage(rnd, buf, len, signature);
}

void CreateDSARandomKeys(CryptoPP::RandomNumberGenerator& rnd,
  uint8_t* signingPrivateKey, uint8_t* signingPublicKey)
{
    CryptoPP::DSA::PrivateKey privateKey;
    CryptoPP::DSA::PublicKey publicKey;
    privateKey.Initialize(rnd, dsap, dsaq, dsag);
    privateKey.MakePublicKey(publicKey);
    privateKey.GetPrivateExponent().Encode(signingPrivateKey, DSA_PRIVATE_KEY_LENGTH);    
    publicKey.GetPublicElement().Encode(signingPublicKey, DSA_PUBLIC_KEY_LENGTH);
}

   
} // crypto
} // i2p