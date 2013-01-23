//#include <QtCore>
//
//#include "archives.h"
//#include "zip.h"
//#include "unzip.h"
//#include "../memleakdetect.h"
//
//namespace Picto {
//
//void testZip()
//{
//	zipFile zf;
//
//	zf = zipOpen("C:\\test.zip",0);
//
//    zip_fileinfo zi;
//    int opt_compress_level=Z_BEST_COMPRESSION;
//
//    zi.tmz_date.tm_sec = 0;
//	zi.tmz_date.tm_min = 0;
//	zi.tmz_date.tm_hour = 0;
//	zi.tmz_date.tm_mday = 0;
//	zi.tmz_date.tm_min = 0;
//	zi.tmz_date.tm_year = 0;
//    zi.dosDate = 0;
//    zi.internal_fa = 0;
//    zi.external_fa = 0;
//
//	zipOpenNewFileInZip(zf,"foo/testing.txt",&zi,NULL,0,NULL,0,NULL,(opt_compress_level != 0) ? Z_DEFLATED : 0,opt_compress_level);
//
//	QString contents = "testing testing 123";
//
//	zipWriteInFileInZip(zf,contents.toLatin1().data(),contents.toLatin1().length());
//
//	zipCloseFileInZip(zf);
//
//	zipOpenNewFileInZip(zf,"bar/testing2.txt",&zi,NULL,0,NULL,0,NULL,(opt_compress_level != 0) ? Z_DEFLATED : 0,opt_compress_level);
//
//	contents = "foobar foobar 456";
//
//	zipWriteInFileInZip(zf,contents.toLatin1().data(),contents.toLatin1().length());
//
//	zipCloseFileInZip(zf);
//
//	zipClose(zf,NULL);
//}
//
//void testUnZip()
//{
//}
//
//}; //namespace Picto
