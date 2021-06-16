/****************************************************************************
 *
 * Copyright (C) 2016 Neutrino International Inc.
 *
 * Author   : Brian Lin ( Vladimir Lin , Vladimir Forest )
 * E-mail   : lin.foxman@gmail.com
 *          : lin.vladimir@gmail.com
 *          : wolfram_lin@yahoo.com
 *          : wolfram_lin@sina.com
 *          : wolfram_lin@163.com
 * Skype    : wolfram_lin
 * WeChat   : 153-0271-7160
 * WhatsApp : 153-0271-7160
 * QQ       : lin.vladimir@gmail.com
 * URL      : http://qtlzo.sourceforge.net/
 *
 * QtLZO acts as an interface between Qt and LZO library.
 * Please keep QtLZO as simple as possible.
 *
 * Copyright 2001 ~ 2016
 *
 ****************************************************************************/

#include <qtlzo.h>

#include <lzo/lzoutil.h>
#include <lzo/lzo1.h>
#include <lzo/lzo1a.h>
#include <lzo/lzo1b.h>
#include <lzo/lzo1c.h>
#include <lzo/lzo1f.h>
#include <lzo/lzo1x.h>
#include <lzo/lzo1y.h>
#include <lzo/lzo1z.h>
#include <lzo/lzo2a.h>

QT_BEGIN_NAMESPACE

/* Lempel-Ziv-Oberhumer */

static lzo_uint32_t xread32(unsigned char b[])
{
  lzo_uint32_t v      ;
  v  = b[0] ; v <<= 8 ;
  v |= b[1] ; v <<= 8 ;
  v |= b[2] ; v <<= 8 ;
  v |= b[3]           ;
  return v            ;
}

static void xwrite32(unsigned char b[],lzo_uint v)
{
  b[3] = (unsigned char) ( v        & 0xff) ;
  b[2] = (unsigned char) ((v >>  8) & 0xff) ;
  b[1] = (unsigned char) ((v >> 16) & 0xff) ;
  b[0] = (unsigned char) ((v >> 24) & 0xff) ;
}

typedef struct               {
  lzo_bytep    in            ;
  lzo_bytep    out           ;
  lzo_voidp    work          ;
  lzo_uint     inLength      ;
  lzo_uint     outLength     ;
  lzo_uint     workSize      ;
  lzo_uint32_t checksum      ;
  int          method        ;
  int          level         ;
  int          LastError     ;
  bool         Writing       ;
  bool         InitialisedOk ;
  bool         headerWritten ;
} LzoFile                    ;

#define LZO_HEADER_SIZE 7

static const unsigned char magic [ LZO_HEADER_SIZE ] = {
               0x00                                    ,
               0xe9                                    ,
               0x4c                                    ,
               0x5a                                    ,
               0x4f                                    ,
               0xff                                    ,
               0x1a                                  } ;

static int lzo_function               (
             bool            compress ,
             int             method   ,
             int             level    ,
             const lzo_bytep src      ,
             lzo_uint        src_len  ,
             lzo_bytep       dst      ,
             lzo_uintp       dst_len  ,
             lzo_voidp       wrkmem   )
{
  if ( compress )                     {
    switch ( method )                 {
      case QtLzo::LZO1x               :
      if ( level == 9                 )
      return ::lzo1x_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      return ::lzo1x_1_compress       (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1                :
      return ::lzo1_compress          (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1_99             :
      return ::lzo1_99_compress       (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1a               :
      return ::lzo1a_compress         (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1a_99            :
      return ::lzo1a_99_compress      (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1b               :
      return ::lzo1b_compress         (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem                 ,
               level                ) ;
      case QtLzo::LZO1b_99            :
      return ::lzo1b_99_compress      (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1b_999           :
      return ::lzo1b_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1c               :
      return ::lzo1c_compress         (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem                 ,
               level                ) ;
      case QtLzo::LZO1c_99            :
      return ::lzo1c_99_compress      (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1c_999           :
      return ::lzo1c_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1f_1             :
      return ::lzo1f_1_compress       (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1f_999           :
      return ::lzo1f_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1x_1             :
      return ::lzo1x_1_compress       (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1x_1_11          :
      return ::lzo1x_1_11_compress    (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1x_1_12          :
      return ::lzo1x_1_12_compress    (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1x_1_15          :
      return ::lzo1x_1_15_compress    (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1x_999           :
      return ::lzo1x_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1y_1             :
      return ::lzo1y_1_compress       (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1y_999           :
      return ::lzo1y_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      break                           ;
      case QtLzo::LZO1z_999           :
      return ::lzo1z_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO2a_999           :
      return ::lzo2a_999_compress     (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
    }                                 ;
  } else                              {
    switch ( method )                 {
      case QtLzo::LZO1                :
      case QtLzo::LZO1_99             :
      return ::lzo1_decompress        (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1a               :
      case QtLzo::LZO1a_99            :
      return ::lzo1a_decompress       (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1b               :
      case QtLzo::LZO1b_99            :
      case QtLzo::LZO1b_999           :
      return ::lzo1b_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1c               :
      case QtLzo::LZO1c_99            :
      case QtLzo::LZO1c_999           :
      return ::lzo1c_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1f_1             :
      case QtLzo::LZO1f_999           :
      return ::lzo1f_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1x               :
      case QtLzo::LZO1x_1             :
      case QtLzo::LZO1x_1_11          :
      case QtLzo::LZO1x_1_12          :
      case QtLzo::LZO1x_1_15          :
      case QtLzo::LZO1x_999           :
      return ::lzo1x_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1y_1             :
      case QtLzo::LZO1y_999           :
      return ::lzo1x_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO1z_999           :
      return ::lzo1z_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
      case QtLzo::LZO2a_999           :
      return ::lzo2a_decompress_safe  (
               src                    ,
               src_len                ,
               dst                    ,
               dst_len                ,
               wrkmem               ) ;
    }                                 ;
  }                                   ;
  return LZO_E_INTERNAL_ERROR         ;
}

static int lzo_worksize(bool compress,int method,int level)
{
  if ( compress )                    {
    switch ( method )                {
      case QtLzo::LZO1x              :
      if ( 9 == level                )
      return LZO1X_999_MEM_COMPRESS  ;
      return LZO1X_1_MEM_COMPRESS    ;
      case QtLzo::LZO1               :
      return LZO1_MEM_COMPRESS       ;
      case QtLzo::LZO1_99            :
      return LZO1_99_MEM_COMPRESS    ;
      case QtLzo::LZO1a              :
      return LZO1A_MEM_COMPRESS      ;
      case QtLzo::LZO1a_99           :
      return LZO1A_99_MEM_COMPRESS   ;
      case QtLzo::LZO1b              :
      return LZO1B_MEM_COMPRESS      ;
      case QtLzo::LZO1b_99           :
      return LZO1B_99_MEM_COMPRESS   ;
      case QtLzo::LZO1b_999          :
      return LZO1B_999_MEM_COMPRESS  ;
      case QtLzo::LZO1c              :
      return LZO1C_MEM_COMPRESS      ;
      case QtLzo::LZO1c_99           :
      return LZO1C_99_MEM_COMPRESS   ;
      case QtLzo::LZO1c_999          :
      return LZO1C_999_MEM_COMPRESS  ;
      case QtLzo::LZO1f_1            :
      return LZO1F_MEM_COMPRESS      ;
      case QtLzo::LZO1f_999          :
      return LZO1F_999_MEM_COMPRESS  ;
      case QtLzo::LZO1x_1            :
      return LZO1X_1_MEM_COMPRESS    ;
      case QtLzo::LZO1x_1_11         :
      return LZO1X_1_11_MEM_COMPRESS ;
      case QtLzo::LZO1x_1_12         :
      return LZO1X_1_12_MEM_COMPRESS ;
      case QtLzo::LZO1x_1_15         :
      return LZO1X_1_15_MEM_COMPRESS ;
      case QtLzo::LZO1x_999          :
      return LZO1X_999_MEM_COMPRESS  ;
      case QtLzo::LZO1y_1            :
      return LZO1Y_MEM_COMPRESS      ;
      case QtLzo::LZO1y_999          :
      return LZO1Y_999_MEM_COMPRESS  ;
      case QtLzo::LZO1z_999          :
      return LZO1Z_999_MEM_COMPRESS  ;
      case QtLzo::LZO2a_999          :
      return LZO2A_999_MEM_COMPRESS  ;
    }                                ;
  } else                             {
    switch ( method )                {
      case QtLzo::LZO1               :
      case QtLzo::LZO1_99            :
      return LZO1_MEM_DECOMPRESS     ;
      case QtLzo::LZO1a              :
      case QtLzo::LZO1a_99           :
      return LZO1A_MEM_DECOMPRESS    ;
      case QtLzo::LZO1b              :
      case QtLzo::LZO1b_99           :
      case QtLzo::LZO1b_999          :
      return LZO1B_MEM_DECOMPRESS    ;
      case QtLzo::LZO1c              :
      case QtLzo::LZO1c_99           :
      case QtLzo::LZO1c_999          :
      return LZO1C_MEM_DECOMPRESS    ;
      case QtLzo::LZO1f_1            :
      case QtLzo::LZO1f_999          :
      return LZO1F_MEM_DECOMPRESS    ;
      case QtLzo::LZO1x              :
      case QtLzo::LZO1x_1            :
      case QtLzo::LZO1x_1_11         :
      case QtLzo::LZO1x_1_12         :
      case QtLzo::LZO1x_1_15         :
      case QtLzo::LZO1x_999          :
      return LZO1X_MEM_DECOMPRESS    ;
      case QtLzo::LZO1y_1            :
      case QtLzo::LZO1y_999          :
      return LZO1Y_MEM_DECOMPRESS    ;
      case QtLzo::LZO1z_999          :
      return LZO1Z_MEM_DECOMPRESS    ;
      case QtLzo::LZO2a_999          :
      return LZO2A_MEM_DECOMPRESS    ;
    }                                ;
  }                                  ;
  return 0                           ;
}

//////////////////////////////////////////////////////////////////////////////

QtLzo:: QtLzo     (void)
      : LzoPacket (NULL)
{
}

QtLzo::~QtLzo (void)
{
  CleanUp ( ) ;
}

QString QtLzo::Version(void)
{
  return QString::fromUtf8 ( LZO_VERSION_STRING ) ;
}

bool QtLzo::isLZO (QByteArray & header)
{
  if ( header . size ( ) < LZO_HEADER_SIZE ) return false  ;
  unsigned char * d = (unsigned char *) header . data ( )  ;
  return ( 0 == ::memcmp ( d , magic , LZO_HEADER_SIZE ) ) ;
}

bool QtLzo::IsCorrect(int returnCode)
{
  if ( returnCode >= LZO_E_OK ) return true ;
  return false                              ;
}

bool QtLzo::IsEnd(int returnCode)
{
  return ( returnCode > 0 ) ;
}

bool QtLzo::IsFault(int returnCode)
{
  return ( returnCode < LZO_E_OK ) ;
}

void QtLzo::CleanUp(void)
{
  if ( NULL == LzoPacket ) return ;
  /////////////////////////////////
  LzoFile * lzo                   ;
  lzo = (LzoFile *) LzoPacket     ;
  /////////////////////////////////
  if ( NULL != lzo -> in   )      {
    ::free ( lzo -> in  )         ;
  }                               ;
  /////////////////////////////////
  if ( NULL != lzo -> out  )      {
    ::free ( lzo ->out  )         ;
  }                               ;
  /////////////////////////////////
  if ( NULL != lzo -> work )      {
    ::free ( lzo ->work )         ;
  }                               ;
  /////////////////////////////////
  ::free ( LzoPacket )            ;
  LzoPacket = NULL                ;
}

void QtLzo::CompressHeader(QByteArray & Compressed)
{
  if ( NULL == LzoPacket ) return                     ;
  LzoFile * lzo = (LzoFile *) LzoPacket               ;
  lzo_uint32_t  flags = 1                             ;
  unsigned char b [ 4 ]                               ;
  Compressed . append ( (const char *) magic    , 7 ) ;
  xwrite32            ( b , flags                   ) ;
  Compressed . append ( (const char *) b        , 4 ) ;
  Compressed . append ( (char) lzo -> method        ) ;
  if ( LZO1x == lzo -> method )                       {
    Compressed . append ( (char) lzo -> level       ) ;
  } else                                              {
    xwrite32            ( b , lzo -> level          ) ;
    Compressed . append ( (const char *) b      , 4 ) ;
  }                                                   ;
  xwrite32            ( b    , lzo -> inLength      ) ;
  Compressed . append ( (const char *) b        , 4 ) ;
  lzo -> headerWritten = true                         ;
}

void QtLzo::CompressTail(unsigned int checksum,QByteArray & Compressed)
{
  unsigned char b [ 4 ]                        ;
  xwrite32            ( b , 0                ) ;
  Compressed . append ( (const char *) b , 4 ) ;
  xwrite32            ( b , checksum         ) ;
  Compressed . append ( (const char *) b , 4 ) ;
}

int QtLzo::DecompressHeader(const QByteArray & Source)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR               ;
  LzoFile * lzo   = (LzoFile *)LzoPacket                    ;
  quint64   size  = Source . size ( )                       ;
  quint64   index = 0                                       ;
  if ( size < 7  ) return LZO_E_INVALID_ARGUMENT            ;
  unsigned char * p = (unsigned char *) Source . data ( )   ;
  if ( ::memcmp ( magic , p , 7 ) != 0 ) return LZO_E_ERROR ;
  ///////////////////////////////////////////////////////////
  lzo_uint32_t flags    = 0                                 ;
  int          method   = 0                                 ;
  int          level    = 0                                 ;
  int          blocks   = 0                                 ;
  int          xlen     = 0                                 ;
  int          worksize = 0                                 ;
  index  = 7                                                ;
  if ( size <  11    ) return -1                            ;
  flags  = xread32 ( p + index )                            ;
  if ( 1    != flags ) return -1                            ;
  index += 4                                                ;
  method = (int)(*(unsigned char *)(p+index))               ;
  index += 1                                                ;
  if ( 0     == method ) return -1                          ;
  if ( LZO1x == method )                                    {
    level  = (int)(*(unsigned char *)(p+index))             ;
    index += 1                                              ;
  } else                                                    {
    level  = xread32 ( p + index )                          ;
    index += 4                                              ;
  }                                                         ;
  if ( 0 == level  ) return -1                              ;
  blocks = xread32 ( p + index )                            ;
  index += 4                                                ;
  if ( 0 >= blocks ) return -1                              ;
  xlen     = blocks + 67 + ( blocks / 16 )                  ;
  worksize = ::lzo_worksize ( false , method , level )      ;
  ///////////////////////////////////////////////////////////
  lzo -> inLength  = blocks                                 ;
  lzo -> outLength = xlen                                   ;
  lzo -> workSize  = worksize                               ;
  lzo -> method    = method                                 ;
  lzo -> level     = level                                  ;
  lzo -> LastError = LZO_E_OK                               ;
  ///////////////////////////////////////////////////////////
  lzo -> in   = (lzo_bytep)::malloc ( lzo -> inLength  )    ;
  lzo -> out  = (lzo_bytep)::malloc ( lzo -> outLength )    ;
  if ( worksize > 0 )                                       {
    lzo -> work = (lzo_voidp)::malloc ( worksize  )         ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  if ( ( NULL == lzo -> in ) || ( NULL == lzo -> out  ) )   {
    CleanUp ( )                                             ;
    return LZO_E_OUT_OF_MEMORY                              ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  lzo -> headerWritten = true                               ;
  return (int) index                                        ;
}

int QtLzo::BeginCompress(QVariantList arguments)
{
  int level    = 9                                         ;
  int method   = 1                                         ;
  int blocks   = 8192                                      ;
  int outlen   = 0                                         ;
  int xlen     = 0                                         ;
  int worksize = 0                                         ;
  if (arguments.count() > 0) level  = arguments[0].toInt() ;
  if (arguments.count() > 1) method = arguments[1].toInt() ;
  if (arguments.count() > 2) blocks = arguments[2].toInt() ;
  if (arguments.count() > 3) outlen = arguments[3].toInt() ;
  //////////////////////////////////////////////////////////
  LzoFile * lzo = NULL                                     ;
  int       init                                           ;
  if ( level < 1 ) level = 1                               ;
  if ( level > 9 ) level = 9                               ;
  init = lzo_init ( )                                      ;
  if ( LZO_E_OK != init ) return init                      ;
  //////////////////////////////////////////////////////////
  lzo = (LzoFile *) ::malloc ( sizeof(LzoFile) )           ;
  if ( NULL == lzo ) return LZO_E_OUT_OF_MEMORY            ;
  //////////////////////////////////////////////////////////
  xlen = blocks + 67 + ( blocks / 16 )                     ;
  if ( xlen > outlen ) outlen = xlen                       ;
  worksize = ::lzo_worksize ( true , method , level )      ;
  //////////////////////////////////////////////////////////
  ::memset ( lzo , 0 , sizeof(LzoFile) )                   ;
  //////////////////////////////////////////////////////////
  lzo -> inLength      = blocks                            ;
  lzo -> outLength     = outlen                            ;
  lzo -> workSize      = worksize                          ;
  lzo -> method        = method                            ;
  lzo -> level         = level                             ;
  lzo -> Writing       = true                              ;
  lzo -> LastError     = LZO_E_OK                          ;
  lzo -> InitialisedOk = false                             ;
  lzo -> headerWritten = false                             ;
  lzo -> checksum      = ::lzo_adler32 ( 0 , NULL , 0 )    ;
  //////////////////////////////////////////////////////////
  lzo -> in   = (lzo_bytep) ::malloc ( lzo -> inLength  )  ;
  lzo -> out  = (lzo_bytep) ::malloc ( lzo -> outLength )  ;
  lzo -> work = (lzo_voidp) ::malloc ( lzo -> workSize  )  ;
  //////////////////////////////////////////////////////////
  if ( ( NULL == lzo -> in   )                            ||
       ( NULL == lzo -> out  )                            ||
       ( NULL == lzo -> work )                           ) {
    CleanUp ( )                                            ;
    return LZO_E_OUT_OF_MEMORY                             ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  lzo -> InitialisedOk = true                              ;
  //////////////////////////////////////////////////////////
  LzoPacket = lzo                                          ;
  //////////////////////////////////////////////////////////
  return LZO_E_OK                                          ;
}

int QtLzo::BeginCompress(int level,int method)
{
  QVariantList args             ;
  args << level                 ;
  args << method                ;
  return BeginCompress ( args ) ;
}

int QtLzo::doCompress(const QByteArray & Source,QByteArray & Compressed)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR         ;
  int       ret                                       ;
  LzoFile * lzo = (LzoFile *)LzoPacket                ;
  /////////////////////////////////////////////////////
  if ( ! lzo -> Writing ) return LZO_E_INTERNAL_ERROR ;
  /////////////////////////////////////////////////////
  Compressed . clear ( )                              ;
  ret = LZO_E_OK                                      ;
  quint64 size = Source . size ( )                    ;
  if ( size <= 0 ) return ret                         ;
  /////////////////////////////////////////////////////
  quint64       index = 0                             ;
  lzo_bytep     p     = (lzo_bytep) Source . data ( ) ;
  lzo_uint      inLen                                 ;
  lzo_uint      outLen                                ;
  unsigned char b [ 4 ]                               ;
  /////////////////////////////////////////////////////
  CompressHeader ( Compressed )                       ;
  /////////////////////////////////////////////////////
  while ( true )                                      {
    inLen = size - index                              ;
    if ( inLen >= lzo -> inLength )                   {
      inLen = lzo -> inLength                         ;
    }                                                 ;
    if ( inLen < 0 ) inLen = 0                        ;
    outLen = 0                                        ;
    if ( inLen > 0 )                                  {
      ::memcpy ( lzo -> in , p + index , inLen )      ;
      lzo -> checksum = ::lzo_adler32                 (
                          lzo -> checksum             ,
                          lzo -> in                   ,
                          inLen                     ) ;
      ret = ::lzo_function                            (
              true                                    ,
              lzo -> method                           ,
              lzo -> level                            ,
              lzo -> in                               ,
              inLen                                   ,
              lzo -> out                              ,
              &outLen                                 ,
              lzo -> work                           ) ;
      if ( LZO_E_OK != ret ) return ret               ;
      if ( outLen    > lzo -> outLength )             {
        return LZO_E_INTERNAL_ERROR                   ;
      }                                               ;
    }                                                 ;
    if ( outLen > 0 )                                 {
      xwrite32              ( b , inLen             ) ;
      Compressed . append   ( (const char *) b  , 4 ) ;
      if ( outLen < inLen )                           {
        xwrite32            ( b , outLen            ) ;
        Compressed . append ( (const char *) b  , 4 ) ;
        Compressed . append                           (
          (const char *) lzo -> out                   ,
          outLen                                    ) ;
      } else                                          {
        xwrite32            ( b , inLen             ) ;
        Compressed . append ( (const char *) b  , 4 ) ;
        Compressed . append                           (
          (const char *) lzo -> in                    ,
          inLen                                     ) ;
      }                                               ;
    }                                                 ;
    index += inLen                                    ;
    if ( inLen != lzo -> inLength )                   {
      CompressTail ( lzo -> checksum , Compressed )   ;
      return 1                                        ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  return LZO_E_ERROR                                  ;
}

int QtLzo::doSection(QByteArray & Source,QByteArray & Compressed)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR         ;
  int       ret                                       ;
  LzoFile * lzo = (LzoFile *)LzoPacket                ;
  /////////////////////////////////////////////////////
  if ( ! lzo -> Writing ) return LZO_E_INTERNAL_ERROR ;
  /////////////////////////////////////////////////////
  Compressed . clear ( )                              ;
  ret = LZO_E_OK                                      ;
  if ( Source . size ( ) <= 0 ) return ret            ;
  if ( ! lzo -> headerWritten )                       {
    CompressHeader ( Compressed )                     ;
  }                                                   ;
  /////////////////////////////////////////////////////
  lzo_bytep     p     = (lzo_bytep) Source . data ( ) ;
  lzo_uint      inLen = lzo -> inLength               ;
  lzo_uint      outLen                                ;
  unsigned char b [ 4 ]                               ;
  /////////////////////////////////////////////////////
  if ( Source . size ( ) < lzo -> inLength )          {
    inLen = Source.size()                             ;
  }                                                   ;
  /////////////////////////////////////////////////////
  outLen = 0                                          ;
  if ( inLen > 0 )                                    {
    ::memcpy ( lzo -> in , p , inLen )                ;
    lzo -> checksum = ::lzo_adler32                   (
                        lzo -> checksum               ,
                        lzo -> in                     ,
                        inLen                       ) ;
    ret = ::lzo_function                              (
            true                                      ,
            lzo -> method                             ,
            lzo -> level                              ,
            lzo -> in                                 ,
            inLen                                     ,
            lzo -> out                                ,
            &outLen                                   ,
            lzo -> work                             ) ;
    if ( LZO_E_OK != ret ) return ret                 ;
    if ( outLen    > lzo -> outLength )               {
      return LZO_E_INTERNAL_ERROR                     ;
    }                                                 ;
  }                                                   ;
  if ( outLen > 0 )                                   {
    xwrite32              ( b , inLen               ) ;
    Compressed . append   ( (const char *) b  , 4   ) ;
    if ( outLen < inLen )                             {
      xwrite32            ( b , outLen              ) ;
      Compressed . append ( (const char *) b  , 4   ) ;
      Compressed . append                             (
        (const char *) lzo -> out                     ,
        outLen                                      ) ;
    } else                                            {
      xwrite32            ( b , inLen               ) ;
      Compressed . append ( (const char *) b  , 4   ) ;
      Compressed . append                             (
        (const char *) lzo -> in                      ,
        inLen                                       ) ;
    }                                                 ;
    Source . remove ( 0 , inLen )                     ;
  }                                                   ;
  /////////////////////////////////////////////////////
  if ( inLen < lzo -> inLength ) return 1             ;
  return LZO_E_OK                                     ;
}

int QtLzo::CompressDone(QByteArray & Compressed)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR   ;
  LzoFile * lzo = (LzoFile *) LzoPacket         ;
  CompressTail ( lzo -> checksum , Compressed ) ;
  return LZO_E_OK                               ;
}

int QtLzo::BeginDecompress(void)
{
  LzoFile * lzo = NULL                                  ;
  int       ret = LZO_E_OK                              ;
  ///////////////////////////////////////////////////////
  ret = lzo_init ( )                                    ;
  if ( LZO_E_OK != ret ) return ret                     ;
  ///////////////////////////////////////////////////////
  lzo = (LzoFile *)::malloc(sizeof(LzoFile))            ;
  if ( NULL == lzo ) return LZO_E_OUT_OF_MEMORY         ;
  ///////////////////////////////////////////////////////
  ::memset ( lzo , 0 , sizeof(LzoFile) )                ;
  ///////////////////////////////////////////////////////
  lzo -> in            = NULL                           ;
  lzo -> out           = NULL                           ;
  lzo -> work          = NULL                           ;
  lzo -> inLength      = 0                              ;
  lzo -> outLength     = 0                              ;
  lzo -> workSize      = 0                              ;
  lzo -> method        = 0                              ;
  lzo -> level         = 0                              ;
  lzo -> Writing       = false                          ;
  lzo -> LastError     = LZO_E_OK                       ;
  lzo -> InitialisedOk = true                           ;
  lzo -> headerWritten = false                          ;
  lzo -> checksum      = ::lzo_adler32 ( 0 , NULL , 0 ) ;
  ///////////////////////////////////////////////////////
  LzoPacket = lzo                                       ;
  return LZO_E_OK                                       ;
}

int QtLzo::doDecompress(const QByteArray & Source,QByteArray & Decompressed)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR              ;
  //////////////////////////////////////////////////////////
  LzoFile * lzo = (LzoFile *) LzoPacket                    ;
  if ( lzo -> Writing ) return LZO_E_INTERNAL_ERROR        ;
  //////////////////////////////////////////////////////////
  qint64 index = 0                                         ;
  if ( ! lzo -> headerWritten )                            {
    index = DecompressHeader ( Source )                    ;
    if ( index < 0 ) return (int) index                    ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  quint64  size   = Source . size ( )                      ;
  lzo_uint inLen  = 0                                      ;
  lzo_uint outLen = 0                                      ;
  lzo_uint newLen = 0                                      ;
  int      ret    = LZO_E_OK                               ;
  lzo -> LastError = ret                                   ;
  if ( size <= 0 )                                         {
    Decompressed . clear ( )                               ;
    return 1                                               ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  lzo_bytep p = (lzo_bytep) Source . data ( )              ;
  while ( true )                                           {
    if ( ( size - index ) < 4 ) return 1                   ;
    outLen = ::xread32 ( p + index )                       ;
    if ( 0 == outLen )                                     {
      lzo_uint32_t c                                       ;
      index += 4                                           ;
      c      = ::xread32 ( p + index )                     ;
      if ( c != lzo -> checksum )                          {
        return LZO_E_EOF_NOT_FOUND                         ;
      }                                                    ;
      return 1                                             ;
    }                                                      ;
    index += 4                                             ;
    if ( ( size - index ) < 4 ) return 1                   ;
    inLen  = ::xread32 ( p + index )                       ;
    index += 4                                             ;
    if ( ( inLen   > lzo -> inLength )                    ||
         ( inLen   > outLen          )                    ||
         ( outLen  > lzo -> inLength )                    ||
         ( 0      == inLen           )                   ) {
      return LZO_E_INVALID_ARGUMENT                        ;
    }                                                      ;
    if ( ( index + inLen ) > size )                        {
      return LZO_E_INPUT_OVERRUN                           ;
    }                                                      ;
    if ( inLen < outLen )                                  {
      ::memcpy ( lzo -> in , p + index , inLen )           ;
      index += inLen                                       ;
      newLen = outLen                                      ;
      ret    = ::lzo_function                              (
                 false                                     ,
                 lzo->method                               ,
                 lzo->level                                ,
                 lzo->in                                   ,
                 inLen                                     ,
                 lzo->out                                  ,
                 &newLen                                   ,
                 lzo->work                               ) ;
      if ( LZO_E_OK != ret    ) return ret                 ;
      if ( newLen   != outLen )                            {
        return LZO_E_LOOKBEHIND_OVERRUN                    ;
      }                                                    ;
      Decompressed . append                                (
        (const char *) lzo -> out                          ,
        outLen                                           ) ;
      lzo -> checksum = ::lzo_adler32                      (
                          lzo->checksum                    ,
                          lzo->out                         ,
                          outLen                         ) ;
    } else                                                 {
      Decompressed . append                                (
        (const char *) ( p + index )                       ,
        inLen                                            ) ;
      lzo->checksum = ::lzo_adler32                        (
                        lzo->checksum                      ,
                        p + index                          ,
                        inLen                            ) ;
      index += inLen                                       ;
    }                                                      ;
  }                                                        ;
  return LZO_E_INTERNAL_ERROR                              ;
}

int QtLzo::undoSection(QByteArray & Source,QByteArray & Decompressed)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR              ;
  //////////////////////////////////////////////////////////
  LzoFile * lzo = (LzoFile *) LzoPacket                    ;
  if ( lzo -> Writing ) return LZO_E_INTERNAL_ERROR        ;
  //////////////////////////////////////////////////////////
  qint64 index = 0                                         ;
  if ( ! lzo -> headerWritten )                            {
    index = DecompressHeader ( Source )                    ;
    if ( index < 0 ) return (int)index                     ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  quint64  size   = Source . size ( )                      ;
  lzo_uint inLen  = 0                                      ;
  lzo_uint outLen = 0                                      ;
  lzo_uint newLen = 0                                      ;
  int      ret    = LZO_E_OK                               ;
  lzo -> LastError = ret                                   ;
  if ( size <= 0 ) return 1                                ;
  //////////////////////////////////////////////////////////
  lzo_bytep p = (lzo_bytep) Source . data ( )              ;
  if ( ( size - index ) < 4 ) return 1                     ;
  outLen = xread32 ( p + index )                           ;
  if ( 0 == outLen )                                       {
    lzo_uint32_t c                                         ;
    index += 4                                             ;
    c      = xread32 ( p + index )                         ;
    if ( c != lzo -> checksum )                            {
      return LZO_E_EOF_NOT_FOUND                           ;
    }                                                      ;
    return 1                                               ;
  }                                                        ;
  index += 4                                               ;
  if ( ( size - index ) < 4 ) return 1                     ;
  inLen  = xread32 ( p + index )                           ;
  index += 4                                               ;
  if ( ( inLen   > lzo -> inLength )                      ||
       ( inLen   > outLen          )                      ||
       ( outLen  > lzo -> inLength )                      ||
       ( 0      == inLen           )                     ) {
    return LZO_E_INVALID_ARGUMENT                          ;
  }                                                        ;
  if ( ( index + inLen ) > size )                          {
    return LZO_E_INPUT_OVERRUN                             ;
  }                                                        ;
  if ( inLen < outLen )                                    {
    ::memcpy ( lzo -> in , p + index , inLen )             ;
    index += inLen                                         ;
    newLen = outLen                                        ;
    ret    = ::lzo_function                                (
               false                                       ,
               lzo->method                                 ,
               lzo->level                                  ,
               lzo->in                                     ,
               inLen                                       ,
               lzo->out                                    ,
               &newLen                                     ,
               lzo->work                                 ) ;
    if ( LZO_E_OK != ret    ) return ret                   ;
    if ( newLen   != outLen )                              {
      return LZO_E_LOOKBEHIND_OVERRUN                      ;
    }                                                      ;
    Decompressed . append                                  (
      (const char *) lzo -> out                            ,
      outLen                                             ) ;
    lzo -> checksum = ::lzo_adler32                        (
                        lzo->checksum                      ,
                        lzo->out                           ,
                        outLen                           ) ;
  } else                                                   {
    Decompressed . append                                  (
      (const char *) ( p + index )                         ,
      inLen                                              ) ;
    lzo->checksum = ::lzo_adler32                          (
                      lzo->checksum                        ,
                      p + index                            ,
                      inLen                              ) ;
    index += inLen                                         ;
  }                                                        ;
  if ( index > 0 )                                         {
    Source . remove ( 0 , index )                          ;
    return LZO_E_OK                                        ;
  }                                                        ;
  return LZO_E_INTERNAL_ERROR                              ;
}

int QtLzo::DecompressDone(void)
{
  if ( NULL == LzoPacket ) return LZO_E_ERROR ;
  LzoFile * lzo = (LzoFile *) LzoPacket       ;
  return lzo -> InitialisedOk                 ;
}

bool QtLzo::IsTail(QByteArray & header)
{
  if ( NULL == LzoPacket     ) return false                    ;
  if ( header . size ( ) < 8 ) return false                    ;
  unsigned char * footer = (unsigned char *) header . data ( ) ;
  bool         correct = true                                  ;
  lzo_uint32_t zero                                            ;
  zero = ::xread32 ( footer )                                  ;
  if ( 0 != zero ) correct = false                             ;
  if ( correct )                                               {
    LzoFile * lzo = (LzoFile *) LzoPacket                      ;
    zero = ::xread32 ( footer + 4 )                            ;
    correct = ( zero == lzo -> checksum )                      ;
  }                                                            ;
  return correct                                               ;
}

//////////////////////////////////////////////////////////////////////////////

bool ToLZO(const QByteArray & data,QByteArray & lzo,int level,int method)
{
  if ( data . size ( ) <= 0 ) return false ;
  //////////////////////////////////////////
  QtLzo        L                           ;
  int          r                           ;
  QVariantList v                           ;
  v << level                               ;
  v << method                              ;
  r = L . BeginCompress ( v )              ;
  if ( L . IsCorrect ( r ) )               {
    L . doCompress ( data , lzo )          ;
  }                                        ;
  //////////////////////////////////////////
  return ( lzo . size ( ) > 0 )            ;
}

//////////////////////////////////////////////////////////////////////////////

bool FromLZO(const QByteArray & lzo,QByteArray & data)
{
  if ( lzo . size ( ) <= 0 ) return false ;
  /////////////////////////////////////////
  QtLzo L                                 ;
  int   r                                 ;
  r = L . BeginDecompress ( )             ;
  if ( L . IsCorrect ( r ) )              {
    L . doDecompress ( lzo , data )       ;
  }                                       ;
  /////////////////////////////////////////
  return ( data . size ( ) > 0 )          ;
}

bool SaveLZO (QString filename,QByteArray & data,int level,int method)
{
  if ( data . size ( ) <= 0 ) return false                         ;
  QByteArray lzo                                                   ;
  if ( level < 0 ) level = 9                                       ;
  if ( ! ToLZO ( data , lzo , level , method ) ) return false      ;
  if ( lzo . size ( ) <= 0                     ) return false      ;
  QFile F ( filename )                                             ;
  if ( ! F . open ( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
    return false                                                   ;
  }                                                                ;
  F . write ( lzo )                                                ;
  F . close (     )                                                ;
  return true                                                      ;
}

bool LoadLZO (QString filename,QByteArray & data)
{
  QFile F ( filename )                                   ;
  if ( ! F . open ( QIODevice::ReadOnly ) ) return false ;
  QByteArray lzo                                         ;
  lzo = F . readAll ( )                                  ;
  F . close         ( )                                  ;
  if ( lzo . size ( ) <= 0 ) return false                ;
  if ( ! FromLZO ( lzo , data ) ) return false           ;
  return ( data . size ( ) > 0 )                         ;
}

bool FileToLzo (QString filename,QString lzo,int level,int method)
{
  QFile F ( filename )                                   ;
  if ( ! F . open ( QIODevice::ReadOnly ) ) return false ;
  QByteArray data                                        ;
  data = F . readAll ( )                                 ;
  F . close ( )                                          ;
  if ( data . size ( ) <= 0 ) return false               ;
  return SaveLZO ( lzo , data , level , method )         ;
}

bool LzoToFile (QString lzo,QString filename)
{
  QByteArray data                                        ;
  if ( ! LoadLZO ( lzo , data ) ) return false           ;
  if ( data . size ( ) <=0      ) return false           ;
  QFile F ( filename )                                   ;
  if ( ! F . open ( QIODevice::WriteOnly                 |
                    QIODevice::Truncate ) ) return false ;
  F . write ( data )                                     ;
  F . close (      )                                     ;
  return true                                            ;
}

QT_END_NAMESPACE
