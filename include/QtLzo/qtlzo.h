/****************************************************************************
 *
 * Copyright (C) 2001~2016 Neutrino International Inc.
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

#ifndef QT_LZO_H
#define QT_LZO_H

#include <QtCore>
#ifndef QT_STATIC
#include <QtScript>
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_QTLZO_LIB)
#    define Q_LZO_EXPORT  Q_DECL_EXPORT
#  else
#    define Q_LZO_EXPORT  Q_DECL_IMPORT
#  endif
#    define __LZO_EXPORT1 Q_DECL_IMPORT
#else
#    define Q_LZO_EXPORT
#endif

#define QT_LZO_LIB 1

class Q_LZO_EXPORT QtLzo         ;
#ifndef QT_STATIC
class Q_LZO_EXPORT ScriptableLzo ;
#endif

// Lempel-Ziv-Oberhumer

class Q_LZO_EXPORT QtLzo
{
  public:

    typedef enum      {
      LZO1x      =  1 ,
      LZO1       =  2 ,
      LZO1_99    =  3 ,
      LZO1a      =  4 ,
      LZO1a_99   =  5 ,
      LZO1b      =  6 ,
      LZO1b_99   =  7 ,
      LZO1b_999  =  8 ,
      LZO1c      =  9 ,
      LZO1c_99   = 10 ,
      LZO1c_999  = 11 ,
      LZO1f_1    = 12 ,
      LZO1f_999  = 13 ,
      LZO1x_1    = 14 ,
      LZO1x_1_11 = 15 ,
      LZO1x_1_12 = 16 ,
      LZO1x_1_15 = 17 ,
      LZO1x_999  = 18 ,
      LZO1y_1    = 19 ,
      LZO1y_999  = 20 ,
      LZO1z_999  = 21 ,
      LZO2a_999  = 22 }
      LzoMethods      ;

    explicit     QtLzo   (void) ;
    virtual     ~QtLzo   (void) ;

    static  QString Version (void) ;

    virtual bool isLZO   (QByteArray & header) ;

    virtual void CleanUp (void) ;

    virtual bool IsCorrect          (int returnCode) ;
    virtual bool IsEnd              (int returnCode) ;
    virtual bool IsFault            (int returnCode) ;

    // Compression functions

    virtual int  BeginCompress      (int level = 9,int method = LZO1x) ;
    virtual int  BeginCompress      (QVariantList arguments = QVariantList() ) ;
    virtual int  doCompress         (const QByteArray & Source      ,
                                           QByteArray & Compressed) ;
    virtual int  doSection          (      QByteArray & Source      ,
                                           QByteArray & Compressed) ;
    virtual int  CompressDone       (QByteArray & Compressed) ;

    virtual int  BeginDecompress    (void) ;
    virtual int  doDecompress       (const QByteArray & Source        ,
                                           QByteArray & Decompressed) ;
    virtual int  undoSection        (      QByteArray & Source        ,
                                           QByteArray & Decompressed) ;
    virtual int  DecompressDone     (void) ;

    virtual bool IsTail             (QByteArray & header) ;

  protected:

    void * LzoPacket ;

    virtual void CompressHeader     (QByteArray & Compressed) ;
    virtual void CompressTail       (unsigned int checksum,QByteArray & Compressed) ;
    virtual int  DecompressHeader   (const QByteArray & Source) ;

  private:

} ;

#ifndef QT_STATIC

class Q_LZO_EXPORT ScriptableLzo : public QObject
                                 , public QScriptable
                                 , public QtLzo
{
  Q_OBJECT
  public:

    static QScriptValue Attachment    (QScriptContext * context,QScriptEngine * engine) ;

    explicit            ScriptableLzo (QObject * parent) ;
    virtual            ~ScriptableLzo (void) ;

  protected:

  private:

  public slots:

    virtual bool        ToLzo         (QString file,QString lzo,int level = 9,int method = QtLzo::LZO1x) ;
    virtual bool        ToFile        (QString lzo,QString file) ;

  protected slots:

  private slots:

  signals:

} ;

#endif

Q_LZO_EXPORT bool ToLZO     (const QByteArray & data,QByteArray & lzo,int level = 9,int method = QtLzo::LZO1x) ;
Q_LZO_EXPORT bool FromLZO   (const QByteArray & lzo ,QByteArray & data) ;
Q_LZO_EXPORT bool SaveLZO   (QString filename,QByteArray & data,int level = 9,int method = QtLzo::LZO1x) ;
Q_LZO_EXPORT bool LoadLZO   (QString filename,QByteArray & data) ;
Q_LZO_EXPORT bool FileToLzo (QString file,QString lzo,int level = 9,int method = QtLzo::LZO1x) ;
Q_LZO_EXPORT bool LzoToFile (QString lzo,QString file) ;

QT_END_NAMESPACE

#endif
