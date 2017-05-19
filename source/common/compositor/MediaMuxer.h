#ifndef __MediaMuxer_H
#define __MediaMuxer_H

#include <QSharedPointer>
#include <QIODevice>
#include <QFile>
#include <QImage>
#include <QTextStream>
#include <stdint.h>
#include <QTime>

#include "../common.h"
#include "../../../3rdparty/qtffmpegwrapper/QTFFmpegWrapper/ffmpeg.h"


namespace Picto {
	/*! \brief The MediaMuxer allows to programmatically encode interleaved Video 
	*          and Audio Stream when recording a Picto movie, using the ffmpeg platform.
    *
	*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2017
	*/
#if defined WIN32 || defined WINCE
	class PICTOLIB_CLASS MediaMuxer : public QObject
#else
	class MediaMuxer : public QObject
#endif
	{
		Q_OBJECT
		public:
			MediaMuxer();
			virtual ~MediaMuxer();

			bool createFile(QString filename, unsigned width, unsigned height, unsigned bitrate, unsigned gop, unsigned fps = 25);
			bool close();

			int encodeImage(const QImage &);
			int encodeImagePts(const QImage &, unsigned pts);
			int encodeAudioFrame(unsigned pts, double spikeTime, bool reward = false);

			bool isOk();					

			int encodeImage_p(const QImage &, bool custompts = false, unsigned pts = 0);
			void addVideoStream(QString fileName);
			void addAudioStream(QString fileName, ffmpeg::CodecID codec_id);

		protected:
			unsigned Width, Height;
			unsigned Bitrate;
			unsigned Gop;
			bool ok;
			unsigned FPS;

			// FFmpeg stuff
			ffmpeg::AVFormatContext *pOutputFormatCtx_;
			ffmpeg::AVOutputFormat *pOutputFormat_;
			ffmpeg::AVStream * pVideoStream_;
			ffmpeg::AVStream * pAudioStream_;
			ffmpeg::AVCodecContext *pVideoCodecCtx_;
			ffmpeg::AVCodecContext *pAudioCodecCtx_;
			ffmpeg::AVCodec* pVideoCodec_;
			ffmpeg::AVCodec* pAudioCodec_;
	
			// Frame data
			ffmpeg::AVFrame *ppicture;
			uint8_t *picture_buf;
			// Compressed data
			int outbuf_size;
			uint8_t* outbuf;
			// Conversion
			ffmpeg::SwsContext *img_convert_ctx;
	
			//Audio
			int audio_outbuf_size;
			uint8_t* audio_outbuf;
			uint8_t *samples;

			// File
			QString fileName;

			// Packet
			ffmpeg::AVPacket pkt;

			unsigned getWidth();
			unsigned getHeight();
			bool isSizeValid();

			void initVars();
			bool initCodec();

			// Alloc/free the output buffer
			bool initOutputBuf();
			void freeOutputBuf();

			// Alloc/free the audio output buffer
			void freeAudioOutputBuf();

			// Alloc/free a frame
			bool initFrame();
			void freeFrame();

			// Frame conversion
			bool convertImage(const QImage &img);
			bool convertImage_sws(const QImage &img);
					
			unsigned lastSpikePts_;
			double firstSpike_;
			unsigned firstPts_;
	};

}; //namespace Picto

#endif


