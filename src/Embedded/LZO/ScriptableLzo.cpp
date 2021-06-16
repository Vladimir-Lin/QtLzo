/****************************************************************************
 *
 * Copyright (C) 2015 Neutrino International Inc.
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
 * Copyright 2001 ~ 2015
 *
 ****************************************************************************/

#include <qtlzo.h>

QT_BEGIN_NAMESPACE

ScriptableLzo:: ScriptableLzo ( QObject * parent )
              : QObject       (           parent )
              , QScriptable   (                  )
              , QtLzo         (                  )
{
}

ScriptableLzo::~ScriptableLzo (void)
{
}

bool ScriptableLzo::ToLzo(QString file,QString lzo,int level,int method)
{
  return FileToLzo ( file , lzo , level , method ) ;
}

bool ScriptableLzo::ToFile(QString lzo,QString file)
{
  return LzoToFile ( lzo , file ) ;
}

QT_END_NAMESPACE
