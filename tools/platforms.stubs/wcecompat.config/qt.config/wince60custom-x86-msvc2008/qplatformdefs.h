/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qmake spec of the Qt Toolkit.
**
** No Commercial Usage
** This file contains pre-release code and may only be used for
** evaluation and testing purposes.  It may not be used for commercial
** development.  You may use this file in accordance with the terms and
** conditions contained in the either Technology Preview License
** Agreement or the Beta Release License Agreement.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef QPLATFORMDEFS_H
#define QPLATFORMDEFS_H

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

// Get Qt defines/settings

#include "qglobal.h"
#include "qfunctions_wince.h"

#define _POSIX_
#include <limits.h>
#undef _POSIX_

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define Q_FS_FAT
#ifdef QT_LARGEFILE_SUPPORT
#define QT_STATBUF		struct _stati64		// non-ANSI defs
#define QT_STATBUF4TSTAT	struct _stati64		// non-ANSI defs
#define QT_STAT			::_stati64
#define QT_FSTAT		::_fstati64
#else
#define QT_STATBUF		struct stat		// non-ANSI defs
#define QT_STATBUF4TSTAT	struct stat		// non-ANSI defs
#define QT_STAT			::qt_wince_stat
#define QT_FSTAT		::qt_wince__fstat
#endif
#define QT_STAT_REG		_S_IFREG
#define QT_STAT_DIR		_S_IFDIR
#define QT_STAT_MASK		_S_IFMT
#if defined(_S_IFLNK)
#  define QT_STAT_LNK		_S_IFLNK
#endif
#define QT_FILENO		::qt_wince___fileno
#define QT_OPEN			::qt_wince_open
#define QT_CLOSE		::qt_wince__close
#ifdef QT_LARGEFILE_SUPPORT
#define QT_LSEEK		::_lseeki64
#define QT_TSTAT		::_tstati64
#else
#define QT_LSEEK		::qt_wince__lseek
#define QT_TSTAT		::_tstat
#endif
#define QT_READ			::qt_wince__read
#define QT_WRITE		::qt_wince__write
#define QT_ACCESS		::qt_wince__access
#define QT_GETCWD		::_getcwd
#define QT_CHDIR		::_chdir
#define QT_MKDIR		::qt_wince__mkdir
#define QT_RMDIR		::qt_wince__rmdir
#define QT_OPEN_RDONLY		_O_RDONLY
#define QT_OPEN_WRONLY		_O_WRONLY
#define QT_OPEN_RDWR		_O_RDWR
#define QT_OPEN_CREAT		_O_CREAT
#define QT_OPEN_TRUNC		_O_TRUNC
#define QT_OPEN_APPEND		_O_APPEND
# define QT_OPEN_TEXT		_O_TEXT
# define QT_OPEN_BINARY		_O_BINARY

#define QT_FOPEN                ::fopen
#define QT_FSEEK                ::fseek
#define QT_FTELL                ::ftell
#define QT_FGETPOS              ::fgetpos
#define QT_FSETPOS              ::fsetpos
#define QT_FPOS_T               fpos_t
#define QT_OFF_T                long

#define QT_SIGNAL_ARGS		int

#define QT_VSNPRINTF(buffer, count, format, arg) \
    _vsnprintf(buffer, count, format, arg)

#define QT_SNPRINTF		::_snprintf

# define F_OK	0
# define X_OK	1
# define W_OK	2
# define R_OK	4

typedef int mode_t;

#endif // QPLATFORMDEFS_H
