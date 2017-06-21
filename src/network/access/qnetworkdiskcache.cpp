
#ifdef SSLAB__DEBUG
#ifndef SSLAB__QDATETIME
#define SSLAB__QDATETIME
#include <QDateTime>
#endif
#endif

#ifdef SSLAB__DEBUG
#ifndef SSLAB__QTHREAD
#define SSLAB__QTHREAD
#include <QThread>
#endif
#endif
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QNETWORKDISKCACHE_DEBUG

#include "qnetworkdiskcache.h"
#include "qnetworkdiskcache_p.h"
#include "QtCore/qscopedpointer.h"

#include <qfile.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdiriterator.h>
#include <qcryptographichash.h>
#include <qurl.h>

#include <qdebug.h>

#define CACHE_PREFIX  QLatin1String("cache_")
#define CACHE_POSTFIX QLatin1String(".cache")
#define CACHESD_POSTFIX QLatin1String(".sd") /*[sslab cache]*/
#define MAX_COMPRESSION_SIZE (1024 * 1024 * 3)
//#define MAX_COMPRESSION_SIZE (1024 * 60) /*[sslab cache]*/

#ifndef QT_NO_NETWORKDISKCACHE

QT_BEGIN_NAMESPACE

/*!
    \class QNetworkDiskCache
    \since 4.5
    \inmodule QtNetwork

    \brief The QNetworkDiskCache class provides a very basic disk cache.

    QNetworkDiskCache stores each url in its own file inside of the
    cacheDirectory using QDataStream.  Files with a text MimeType
    are compressed using qCompress.  Each cache file starts with "cache_"
    and ends in ".cache".  Data is written to disk only in insert()
    and updateMetaData().

    Currently you can not share the same cache files with more then
    one disk cache.

    QNetworkDiskCache by default limits the amount of space that the cache will
    use on the system to 50MB.

    Note you have to set the cache directory before it will work.

    A network disk cache can be enabled by:

    \snippet doc/src/snippets/code/src_network_access_qnetworkdiskcache.cpp 0

    When sending requests, to control the preference of when to use the cache
    and when to use the network, consider the following:

    \snippet doc/src/snippets/code/src_network_access_qnetworkdiskcache.cpp 1

    To check whether the response came from the cache or from the network, the
    following can be applied:

    \snippet doc/src/snippets/code/src_network_access_qnetworkdiskcache.cpp 2
*/

/*!
    Creates a new disk cache. The \a parent argument is passed to
    QAbstractNetworkCache's constructor.
 */
QNetworkDiskCache::QNetworkDiskCache(QObject *parent)
    : QAbstractNetworkCache(*new QNetworkDiskCachePrivate, parent)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

/*!
    Destroys the cache object.  This does not clear the disk cache.
 */
QNetworkDiskCache::~QNetworkDiskCache()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QNetworkDiskCache);
    QHashIterator<QIODevice*, QCacheItem*> it(d->inserting);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }
}

/*!
    Returns the location where cached files will be stored.
*/
QString QNetworkDiskCache::cacheDirectory() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QNetworkDiskCache);
	 return d->cacheDirectory;
}

/*!
    Sets the directory where cached files will be stored to \a cacheDir

    QNetworkDiskCache will create this directory if it does not exists.

    Prepared cache items will be stored in the new cache directory when
    they are inserted.

    \sa QDesktopServices::CacheLocation
*/
void QNetworkDiskCache::setCacheDirectory(const QString &cacheDir)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::setCacheDirectory()" << cacheDir;
#endif
    Q_D(QNetworkDiskCache);
    if (cacheDir.isEmpty())
        return;
    d->cacheDirectory = cacheDir;
    QDir dir(d->cacheDirectory);
    d->cacheDirectory = dir.absolutePath();
    if (!d->cacheDirectory.endsWith(QLatin1Char('/')))
        d->cacheDirectory += QLatin1Char('/');
}

/*[sslab cache]*/
QString QNetworkDiskCache::migrateDirectory() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QNetworkDiskCache);
	 return d->migrateDirectory;
}
/*[sslab cache]*/
void QNetworkDiskCache::setMigrateDirectory(const QString &cacheDir)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::setCacheDirectory()" << cacheDir;
#endif
    Q_D(QNetworkDiskCache);
    if (cacheDir.isEmpty())
        return;
    d->migrateDirectory = cacheDir;
    QDir dir(d->migrateDirectory);
    d->migrateDirectory = dir.absolutePath();
    if (!d->migrateDirectory.endsWith(QLatin1Char('/')))
        d->migrateDirectory += QLatin1Char('/');
}
/*[sslab cache]*/
bool QNetworkDiskCache::lastCache() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QNetworkDiskCache);
	 return d->lastCache;
}
/*[sslab cache]*/
void QNetworkDiskCache::setLastCache(bool last)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QNetworkDiskCache);
	 d->lastCache = last;
/*	 if (last) //true => 2nd cache
		d->maximumCacheSize = 1024 * 1024 * 20;
	 else //flase => 1st cache
		d->maximumCacheSize = 1024 * 1024 * 10;	*/
}

/*!
    \reimp
*/
qint64 QNetworkDiskCache::cacheSize() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::cacheSize()";
#endif
    Q_D(const QNetworkDiskCache);
    if (d->cacheDirectory.isEmpty())
        return 0;
    if (d->currentCacheSize < 0) {
        QNetworkDiskCache *that = const_cast<QNetworkDiskCache*>(this);
        that->d_func()->currentCacheSize = that->expire();
    }
    return d->currentCacheSize;
}

/*!
    \reimp
*/
QIODevice *QNetworkDiskCache::prepare(const QNetworkCacheMetaData &metaData)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::prepare()" << metaData.url();
#endif
    Q_D(QNetworkDiskCache);
    if (!metaData.isValid() || !metaData.url().isValid() || !metaData.saveToDisk())
        return 0;

    if (d->cacheDirectory.isEmpty()) {
        qWarning() << "QNetworkDiskCache::prepare() The cache directory is not set";
        return 0;
    }

    foreach (QNetworkCacheMetaData::RawHeader header, metaData.rawHeaders()) {
        if (header.first.toLower() == "content-length") {
            qint64 size = header.second.toInt();
            if (size > (maximumCacheSize() * 3)/4)
                return 0;
            break;
        }
    }
    QScopedPointer<QCacheItem> cacheItem(new QCacheItem);
    cacheItem->metaData = metaData;

    QIODevice *device = 0;
    if (cacheItem->canCompress()) {
        cacheItem->data.open(QBuffer::ReadWrite);
        device = &(cacheItem->data);
    } else {
        QString templateName = d->tmpCacheFileName();
        QT_TRY {
            cacheItem->file = new QTemporaryFile(templateName, &cacheItem->data);
        } QT_CATCH(...) {
            cacheItem->file = 0;
        }
        if (!cacheItem->file || !cacheItem->file->open()) {
            qWarning() << "QNetworkDiskCache::prepare() unable to open temporary file";
            cacheItem.reset();
            return 0;
        }
        cacheItem->writeHeader(cacheItem->file);
        device = cacheItem->file;
    }
    d->inserting[device] = cacheItem.take();
    return device;
}

/*!
    \reimp
*/
void QNetworkDiskCache::insert(QIODevice *device)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::insert()" << device;
#endif
    Q_D(QNetworkDiskCache);
    QHash<QIODevice*, QCacheItem*>::iterator it = d->inserting.find(device);
    if (it == d->inserting.end()) {
        qWarning() << "QNetworkDiskCache::insert() called on a device we don't know about" << device;
        return;
    }

    d->storeItem(it.value());
    delete it.value();
    d->inserting.erase(it);
}

void QNetworkDiskCachePrivate::storeItem(QCacheItem *cacheItem)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_Q(QNetworkDiskCache);
    Q_ASSERT(cacheItem->metaData.saveToDisk());

    QString fileName = cacheFileName(cacheItem->metaData.url());
    Q_ASSERT(!fileName.isEmpty());

	 qWarning("[store]url:%s", cacheItem->metaData.url().toString().toAscii().data());
	 qWarning("[store]file:%s", fileName.toAscii().data());

    if (QFile::exists(fileName)) {
        if (!QFile::remove(fileName)) {
            qWarning() << "QNetworkDiskCache: couldn't remove the cache file " << fileName;
            return;
        }
    }

    if (currentCacheSize > 0)
        currentCacheSize += 1024 + cacheItem->size();
    currentCacheSize = q->expire();
    if (!cacheItem->file) {
        QString templateName = tmpCacheFileName();
        cacheItem->file = new QTemporaryFile(templateName, &cacheItem->data);
        if (cacheItem->file->open()) {
            cacheItem->writeHeader(cacheItem->file);
            cacheItem->writeCompressedData(cacheItem->file);
        }
    }

    if (cacheItem->file
        && cacheItem->file->isOpen()
        && cacheItem->file->error() == QFile::NoError) {
        cacheItem->file->setAutoRemove(false);
        // ### use atomic rename rather then remove & rename
        if (cacheItem->file->rename(fileName))
            currentCacheSize += cacheItem->file->size();
        else
            cacheItem->file->setAutoRemove(true);
    }
    if (cacheItem->metaData.url() == lastItem.metaData.url())
        lastItem.reset();
}

/*!
    \reimp
*/
bool QNetworkDiskCache::remove(const QUrl &url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::remove()" << url;
#endif
    Q_D(QNetworkDiskCache);

    // remove is also used to cancel insertions, not a common operation
    QHashIterator<QIODevice*, QCacheItem*> it(d->inserting);
    while (it.hasNext()) {
        it.next();
        QCacheItem *item = it.value();
        if (item && item->metaData.url() == url) {
            delete item;
            d->inserting.remove(it.key());
            return true;
        }
    }

    if (d->lastItem.metaData.url() == url)
        d->lastItem.reset();
    return d->removeFile(d->cacheFileName(url));
}

/*!
    Put all of the misc file removing into one function to be extra safe
 */
bool QNetworkDiskCachePrivate::removeFile(const QString &file)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::removFile()" << file;
#endif
    if (file.isEmpty())
        return false;
    QFileInfo info(file);
    QString fileName = info.fileName();
    if (!fileName.endsWith(CACHE_POSTFIX) || !fileName.startsWith(CACHE_PREFIX))
        return false;
    qint64 size = info.size();
    if (QFile::remove(file)) {
        currentCacheSize -= size;
        return true;
    }
    return false;
}

/*!
    \reimp
*/
QNetworkCacheMetaData QNetworkDiskCache::metaData(const QUrl &url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::metaData()" << url;
#endif
    Q_D(QNetworkDiskCache);
    if (d->lastItem.metaData.url() == url)
        return d->lastItem.metaData;
    return fileMetaData(d->cacheFileName(url));
}

/*[sslab cache]
  for debugging */
QString QNetworkDiskCache::_cacheFileName(const QUrl &url) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	Q_D(const QNetworkDiskCache);
	return d->cacheFileName(url);
}

/*!
    Returns the QNetworkCacheMetaData for the cache file \a fileName.

    If \a fileName is not a cache file QNetworkCacheMetaData will be invalid.
 */
QNetworkCacheMetaData QNetworkDiskCache::fileMetaData(const QString &fileName) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::fileMetaData()" << fileName;
#endif
    Q_D(const QNetworkDiskCache);
    QFile file(fileName);
	 
	 /*[sslab cache]*/
	 QString fixFileName = fileName + QLatin1String(CACHESD_POSTFIX);
	 QFile fixFile(fixFileName);
	 bool fixed = false;

    if (!file.open(QFile::ReadOnly)) { /*[sslab cache]*/
		  if (!fixFile.open(QFile::ReadOnly))
	        return QNetworkCacheMetaData();
		  fixed = true;
	 }
	 if (fixed && fixFileName.endsWith(CACHESD_POSTFIX)) {	// remove POSTFIX ".sd"
		  fixFile.rename(fileName);
		  if (!file.open(QFile::ReadOnly))
			  return QNetworkCacheMetaData();		  
		  fixFile.close();
		  qWarning("[onHit][from size]:%s",fixFileName.toAscii().data());
		  qWarning("[onHit][to lru]:%s",fileName.toAscii().data());  
	 }
    if (!d->lastItem.read(&file, false)) {
        file.close();
        QNetworkDiskCachePrivate *that = const_cast<QNetworkDiskCachePrivate*>(d);
        that->removeFile(fileName);
    }
    return d->lastItem.metaData;
}

/*!
    \reimp
*/
QIODevice *QNetworkDiskCache::data(const QUrl &url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::data()" << url;
#endif
    Q_D(QNetworkDiskCache);
    QScopedPointer<QBuffer> buffer;
    if (!url.isValid())
        return 0;
    if (d->lastItem.metaData.url() == url && d->lastItem.data.isOpen()) {
        buffer.reset(new QBuffer);
        buffer->setData(d->lastItem.data.data());
    } else {
        QScopedPointer<QFile> file(new QFile(d->cacheFileName(url)));
        if (!file->open(QFile::ReadOnly | QIODevice::Unbuffered))
            return 0;

        if (!d->lastItem.read(file.data(), true)) {
            file->close();
            remove(url);
            return 0;
        }
        if (d->lastItem.data.isOpen()) {
            // compressed
            buffer.reset(new QBuffer);
            buffer->setData(d->lastItem.data.data());
        } else {
            buffer.reset(new QBuffer);
            // ### verify that QFile uses the fd size and not the file name
            qint64 size = file->size() - file->pos();
            const uchar *p = 0;
#ifndef Q_OS_WINCE
            p = file->map(file->pos(), size);
#endif
            if (p) {
                buffer->setData((const char *)p, size);
                file.take()->setParent(buffer.data());
            } else {
                buffer->setData(file->readAll());
            }
        }
    }
    buffer->open(QBuffer::ReadOnly);
    return buffer.take();
}

/*!
    \reimp
*/
void QNetworkDiskCache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::updateMetaData()" << metaData.url();
#endif
    QUrl url = metaData.url();
    QIODevice *oldDevice = data(url);
    if (!oldDevice) {
#if defined(QNETWORKDISKCACHE_DEBUG)
        qDebug() << "QNetworkDiskCache::updateMetaData(), no device!";
#endif
        return;
    }

    QIODevice *newDevice = prepare(metaData);
    if (!newDevice) {
#if defined(QNETWORKDISKCACHE_DEBUG)
        qDebug() << "QNetworkDiskCache::updateMetaData(), no new device!" << url;
#endif
        return;
    }
    char data[1024];
    while (!oldDevice->atEnd()) {
        qint64 s = oldDevice->read(data, 1024);
        newDevice->write(data, s);
    }
    delete oldDevice;
    insert(newDevice);
}

/*!
    Returns the current maximum size for the disk cache.

    \sa setMaximumCacheSize()
 */
qint64 QNetworkDiskCache::maximumCacheSize() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QNetworkDiskCache);
    return d->maximumCacheSize;
}

/*!
    Sets the maximum size of the disk cache to be \a size.

    If the new size is smaller then the current cache size then the cache will call expire().

    \sa maximumCacheSize()
 */
void QNetworkDiskCache::setMaximumCacheSize(qint64 size)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QNetworkDiskCache);
    bool expireCache = (size < d->maximumCacheSize);
    d->maximumCacheSize = size;
    if (expireCache)
        d->currentCacheSize = expire();
}

/*!
    Cleans the cache so that its size is under the maximum cache size.
    Returns the current size of the cache.

    When the current size of the cache is greater than the maximumCacheSize()
    older cache files are removed until the total size is less then 90% of
    maximumCacheSize() starting with the oldest ones first using the file
    creation date to determine how old a cache file is.

    Subclasses can reimplement this function to change the order that cache
    files are removed taking into account information in the application
    knows about that QNetworkDiskCache does not, for example the number of times
    a cache is accessed.

    Note: cacheSize() calls expire if the current cache size is unknown.

    \sa maximumCacheSize(), fileMetaData()
 */
qint64 QNetworkDiskCache::expire()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QNetworkDiskCache);
    if (d->currentCacheSize >= 0 && d->currentCacheSize < maximumCacheSize())
        return d->currentCacheSize;

    if (cacheDirectory().isEmpty()) {
        qWarning() << "QNetworkDiskCache::expire() The cache directory is not set";
        return 0;
    }

    QDir::Filters filters = QDir::AllDirs | QDir:: Files | QDir::NoDotAndDotDot;
    QDirIterator it(cacheDirectory(), filters, QDirIterator::Subdirectories);

    QMultiMap<QDateTime, QString> cacheItems;
    qint64 totalSize = 0;
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo info = it.fileInfo();
        QString fileName = info.fileName();
        if (fileName.endsWith(CACHE_POSTFIX) && fileName.startsWith(CACHE_PREFIX)) {
    //        cacheItems.insert(info.created(), path); //create time
			   cacheItems.insert(info.lastRead(), path); //accese time /*[sslab cache]*/
            totalSize += info.size();
        }
    }
    
	 int removedFiles = 0;
    qint64 goal = (maximumCacheSize() * 9) / 10;
	 if (lastCache() == false) goal /= 2; /*[sslab cache]*/
    QMultiMap<QDateTime, QString>::const_iterator i = cacheItems.constBegin();
  
 	 /*[sslab cache]*/ 
	 if (lastCache() == false) {
		while (i != cacheItems.constEnd()) {
			// In 1st cache, LRU(Size-Independent region)'s item moves to SIZE(Size-Dependent region)
	      if (totalSize < goal){
				qWarning("[expire]break,totalSize => %lld / goal => %lld",totalSize, goal);
	          break;
			}
	      QString name = i.value();
	      QFile file(name);
	      qint64 size = file.size();
	
		   QString migrateFileName = name;
		   migrateFileName += QLatin1String(CACHESD_POSTFIX);
		   qWarning("[expire][lru->size]mName => %s",migrateFileName.toAscii().data());			  
		   file.rename(migrateFileName);
	
			qWarning("[expire]mFileSize => %lld / totalSize => %lld",size, totalSize);
	      totalSize -= size;
	      ++i;
		}

		QDirIterator it2(cacheDirectory(), filters, QDirIterator::Subdirectories);
		// SIZE iterator
		QMultiMap<qint64, QString> cacheItems2;
		qint64 totalSize2 = 0;
		while(it2.hasNext()) {
			 QString path = it2.next();
			 QFileInfo info = it2.fileInfo();
			 QString fileName = info.fileName();
			 if (fileName.endsWith(CACHESD_POSTFIX) && fileName.startsWith(CACHE_PREFIX)) {
				 cacheItems2.insert((0-info.size()), path);
				 totalSize2 += info.size();
			 }
		}
	//		qWarning("[size]=>%lld",totalSize2);
		QMultiMap<qint64, QString>::const_iterator j = cacheItems2.constBegin();

		while (j != cacheItems2.constEnd()) {
			// Item of the SIZE in 1st cache that moves to 2nd cache
	      if (totalSize2 < goal) {
				qWarning("[expire]break,totalSize => %lld / goal => %lld",totalSize2, goal);
	          break;
			}
	      QString name = j.value();
	      QFile file(name);
	      qint64 size = file.size();

			QString migrateFileName = migrateDirectory();
			char tmp[128] ={0,};
			char *ptr;
			sprintf(tmp, "%s", name.toAscii().data());
			qint32 full_len = strlen(tmp);
			QString s = CACHESD_POSTFIX;
			qint32 postfix_len = strlen(s.toAscii().data());
			tmp[full_len-postfix_len] = '\0';
			qint32 len = strlen(cacheDirectory().toAscii().data());
			ptr = &(tmp[len]);
			migrateFileName += QLatin1String(ptr);
			qWarning("[expire][1st(size)->2nd]mName => %s",migrateFileName.toAscii().data());			  
			file.copy(migrateFileName);

			file.remove();
	      totalSize2 -= size;
			qWarning("[expire]mFileSize => %lld / totalSize => %lld",size, totalSize);
	      ++j;
	   }
	 } //end of if(lastCache()==false)

    else { //if "lastCache() == true" => is 2nd cache
		 while (i != cacheItems.constEnd()) {
	      if (totalSize < goal)
	          break;
	      QString name = i.value();
	      QFile file(name);
	      qint64 size = file.size();
	      file.remove();
	      totalSize -= size;
	      ++removedFiles;
	      ++i;
	    }
	 }
	
#if defined(QNETWORKDISKCACHE_DEBUG)
    if (removedFiles > 0) {
        qDebug() << "QNetworkDiskCache::expire()"
                << "Removed:" << removedFiles
                << "Kept:" << cacheItems.count() - removedFiles;
    }
#endif
    if (removedFiles > 0)
        d->lastItem.reset();
    return totalSize;
}

/*!
    \reimp
*/
void QNetworkDiskCache::clear()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKDISKCACHE_DEBUG)
    qDebug() << "QNetworkDiskCache::clear()";
#endif
    Q_D(QNetworkDiskCache);
    qint64 size = d->maximumCacheSize;
    d->maximumCacheSize = 0;
    d->currentCacheSize = expire();
    d->maximumCacheSize = size;
}

QByteArray QNetworkDiskCachePrivate::generateId(const QUrl &url) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QUrl cleanUrl = url;
    cleanUrl.setPassword(QString());
    cleanUrl.setFragment(QString());

    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(cleanUrl.toEncoded());
    return hash.result().toHex();
}

QString QNetworkDiskCachePrivate::tmpCacheFileName() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QDir dir;
    dir.mkpath(cacheDirectory + QLatin1String("prepared/"));
    return cacheDirectory + QLatin1String("prepared/") + CACHE_PREFIX + QLatin1String("XXXXXX") + CACHE_POSTFIX;
}

QString QNetworkDiskCachePrivate::cacheFileName(const QUrl &url) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!url.isValid())
        return QString();
    QString directory = cacheDirectory + url.scheme() + QLatin1Char('/');
    if (!QFile::exists(directory)) {
        // ### make a static QDir function for this...
        QDir dir;
        dir.mkpath(directory);
    }

    QString fileName = CACHE_PREFIX + QLatin1String(generateId(url)) + CACHE_POSTFIX;
    return  directory + fileName;
}

/*!
    We compress small text and JavaScript files.
 */
bool QCacheItem::canCompress() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    bool sizeOk = false;
    bool typeOk = false;
    foreach (QNetworkCacheMetaData::RawHeader header, metaData.rawHeaders()) {
        if (header.first.toLower() == "content-length") {
            qint64 size = header.second.toLongLong();
            if (size > MAX_COMPRESSION_SIZE)
                return false;
            else
                sizeOk = true;
        }

        if (header.first.toLower() == "content-type") {
            QByteArray type = header.second;
            if (type.startsWith("text/")
                    || (type.startsWith("application/")
                        && (type.endsWith("javascript") || type.endsWith("ecmascript"))))
                typeOk = true;
            else
                return false;
        }
        if (sizeOk && typeOk)
            return true;
    }
    return false;
}

enum
{
    CacheMagic = 0xe8,
    CurrentCacheVersion = 7
};

void QCacheItem::writeHeader(QFile *device) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QDataStream out(device);

    out << qint32(CacheMagic);
    out << qint32(CurrentCacheVersion);
    out << metaData;
    bool compressed = canCompress();
    out << compressed;
}

void QCacheItem::writeCompressedData(QFile *device) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QDataStream out(device);

    out << qCompress(data.data());
}

/*!
    Returns false if the file is a cache file,
    but is an older version and should be removed otherwise true.
 */
bool QCacheItem::read(QFile *device, bool readData)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reset();

    QDataStream in(device);

    qint32 marker;
    qint32 v;
    in >> marker;
    in >> v;
    if (marker != CacheMagic)
        return true;

    // If the cache magic is correct, but the version is not we should remove it
    if (v != CurrentCacheVersion)
        return false;

    bool compressed;
    QByteArray dataBA;
    in >> metaData;
    in >> compressed;
    if (readData && compressed) {
        in >> dataBA;
        data.setData(qUncompress(dataBA));
        data.open(QBuffer::ReadOnly);
    }
    return metaData.isValid();
}

QT_END_NAMESPACE

#endif // QT_NO_NETWORKDISKCACHE
