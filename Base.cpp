/*
 *   Copyright (c) 2015 Damian Obernikowicz <damin.obernikowicz@gmail.com>, BitMarket Limited Global Gateway 8, Rue de la Perle, Providence, Mahe, Seszele
 *
 *   This file is part of Bitkom.
 *
 *   Bitkom is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Bitkom is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Bitkom.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include <string>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <curl/curl.h>
#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
#include "Base.h"

string last,path;

void log(string s)
{
    FILE *f;
    time_t tt;
    char times[50];

    tt=time(NULL);
    strcpy(times,ctime(&tt));
    times[strlen(times)-1]=0;
    f=fopen(string(path+"/log.log").c_str(),"a");
    if (f>(void *)0)
    {
        fprintf(f,"%s\n%s\n",times,s.c_str());
        fclose(f);
    }
    printf("%s",s.c_str());
    fflush(stdout);
    last=s;
}

Ticker::Ticker()
{
    ask=0;
    bid=0;
    last=0;
    volume=0;
    high=0;
    low=0;
}

void logError(QString s)
{
    log(s);
}

string randomString(int len)
{
    static const char alphanum[] = "0123456789";
    string s;

    s="";
    for (int i = 0; i < len; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return(s);
}

void log(QString s)
{
    log(s.toStdString());
}

void log(const char *s)
{
    QString ss(s);
    log(ss);
}

int currency(string market,int k)
{
    if (market=="BTCPLN")
    {
        if (k==FIRST) return(BTC);
        else return(PLN);
    }
    else if (market=="LTCPLN")
    {
        if (k==FIRST) return(LTC);
        else return(PLN);
    }
    else if (market=="BTCEUR")
    {
        if (k==FIRST) return(BTC);
        else return(EUR);
    }
    else if (market=="LTCBTC")
    {
        if (k==FIRST) return(LTC);
        else return(BTC);
    }else if (market=="KBMBTC")
    {
        if (k==FIRST) return(KBM);
        else return(BTC);
    }
    return(0);
}

QString to_stringd(double d)
{
    char s[50];
    snprintf(s,50,"%.8f",d);
    return(s);
}

QString to_stringl(long l)
{
    char s[50];
    snprintf(s,50,"%ld",l);
    return(s);
}

QString to_stringp(double d)
{
    char s[50];
    snprintf(s,50,"%.4f",d);
    return(s);
}

QString to_string2(double d)
{
    char s[50];
    snprintf(s,50,"%.2f",d);
    return(s);
}

QString to_string1(double d)
{
    char s[50];
    snprintf(s,50,"%.1f",d);
    return(s);
}

QString Offer::getPrice()
{
    return(to_string4(price));
}

QString Offer::getAmount()
{
    return(to_stringd(amount));
}

QString Offer::format(double d)
{
    return(to_string2(d));
}

QString to_string4(double d)
{
    char s[50];
    snprintf(s,50,"%.4f",d);
    return(s);
}

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        log("not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int64_t getctime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000ULL + time.tv_usec / 1000.0);
}

inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    ret="";
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3)) ret += '=';
    }

    return ret;

}

std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    ret="";
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

long tosato(double in)
{
    return((long)round(in*SATO));
}

double fromsato(long in)
{
    return((double)in/SATO);
}

std::vector<unsigned char> sha256(const std::string& data)
{
    std::vector<unsigned char> digest(SHA256_DIGEST_LENGTH);

    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.c_str(), data.length());
    SHA256_Final(digest.data(), &ctx);

    return digest;
}

void sha256buf(const std::string& data,unsigned char *buf)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.c_str(), data.length());
    SHA256_Final(buf, &ctx);
    buf[SHA256_DIGEST_LENGTH]=0;
}

std::vector<unsigned char>
hmac_sha512(const std::vector<unsigned char>& data,
            const std::vector<unsigned char>& key)
{
    unsigned int len = EVP_MAX_MD_SIZE;
    std::vector<unsigned char> digest(len);

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);

    HMAC_Init_ex(&ctx, key.data(), key.size(), EVP_sha512(), NULL);
    HMAC_Update(&ctx, data.data(), data.size());
    HMAC_Final(&ctx, digest.data(), &len);

    HMAC_CTX_cleanup(&ctx);

    return digest;
}

std::vector<unsigned char>
hmac_sha256(const std::vector<unsigned char>& data,
            const std::vector<unsigned char>& key)
{
    unsigned int len = 32;
    std::vector<unsigned char> digest(len);

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);

    HMAC_Init_ex(&ctx, key.data(), key.size(), EVP_sha256(), NULL);
    HMAC_Update(&ctx, data.data(), data.size());
    HMAC_Final(&ctx, digest.data(), &len);

    HMAC_CTX_cleanup(&ctx);

    return digest;
}

std::vector<unsigned char> b64_decode(const std::string& data)
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO* bmem = BIO_new_mem_buf((void*)data.c_str(),data.length());
    bmem = BIO_push(b64, bmem);

    std::vector<unsigned char> output(data.length());
    //   int decoded_size = BIO_read(bmem, output.data(), output.size());
    BIO_read(bmem, output.data(), output.size());
    BIO_free_all(bmem);

    //if (decoded_size < 0)
    //  throw std::runtime_error("failed while decoding base64.");

    return output;
}

//------------------------------------------------------------------------------
// helper function to encode a vector of bytes to a base64 string:
std::string b64_encode(const std::vector<unsigned char>& data)
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    BIO_write(b64, data.data(), data.size());
    (void)BIO_flush(b64);

    BUF_MEM* bptr = NULL;
    BIO_get_mem_ptr(b64, &bptr);

    std::string output(bptr->data, bptr->length);
    BIO_free_all(b64);

    return output;
}

void Exchange::init()
{
    online=true;
}

void Exchange::offline()
{
    online=false;
}

void Exchange::logerror()
{
    log("Error: "+lasterror+"\n");
}

void Exchange::clearmem()
{
    chunk.memory = (char *)realloc(chunk.memory,1);
    chunk.size = 0;
}

Exchange::Exchange()
{
    online=true;
    id=0;
}

Exchange::~Exchange()
{

}

