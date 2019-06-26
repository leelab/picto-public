/*
*/
#include <QPainter>
#include "MediaMuxer.h"
#include "ffmpeg.h"
#include <stdio.h>
#include <stdlib.h>

#define N 8000

/******************************************************************************
*******************************************************************************
* MediaMuxer   MediaMuxer   MediaMuxer   MediaMuxer   MediaMuxer
*******************************************************************************
******************************************************************************/


/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/

/**
gop: maximal interval in frames between keyframes
**/
MediaMuxer::MediaMuxer()
{
	initVars();
	initCodec();

	lastSpikePts_ = 0;
}
MediaMuxer::~MediaMuxer()
{
	close();
}
bool MediaMuxer::createFile(QString fileName, unsigned width, unsigned height, unsigned bitrate, unsigned gop, unsigned fps)
{
	// If we had an open video, close it.
	close();

	Width = width;
	Height = height;
	Gop = gop;
	Bitrate = bitrate;
	FPS = fps;

	if (!isSizeValid())
	{
		printf("Invalid size\n");
		return false;
	}

	/* Allocate the output media context */
	pOutputFormat_ = ffmpeg::av_guess_format(NULL, fileName.toStdString().c_str(), NULL);
	if (!pOutputFormat_) {
		printf("Could not deduce output format from file extension: using MPEG.\n");
		pOutputFormat_ = ffmpeg::av_guess_format("mpeg", NULL, NULL);
	}

	pOutputFormatCtx_ = ffmpeg::avformat_alloc_context();
	if (!pOutputFormatCtx_)
	{
		printf("Error allocating format context\n");
		return false;
	}
	pOutputFormat_->audio_codec = ffmpeg::CODEC_ID_PCM_S16LE;
	pOutputFormatCtx_->oformat = pOutputFormat_;
	_snprintf_s(pOutputFormatCtx_->filename, sizeof(pOutputFormatCtx_->filename), "%s", fileName.toStdString().c_str());

	// Add the video stream

	/* Add the audio and video streams using the default format codecs and initialize the codecs. */
	/* allocate the output media context */
	addVideoStream(fileName);

	ffmpeg::AVOutputFormat *fmt = pOutputFormatCtx_->oformat;
	if (fmt->audio_codec)
		addAudioStream(fileName, fmt->audio_codec);

	pAudioStream_->time_base.den = FPS;
	pAudioStream_->time_base.num = 1;
	pVideoStream_->time_base.den = FPS;
	pVideoStream_->time_base.num = 1;


	//If we were using a file, erase it
	if (rewardFile_)
		rewardFile_->remove();
	if (rewardFileStream_)
		rewardFileStream_.clear();
	//create spikes output file
	QString filename = "C:/Projects/Picto/output/bin/TemporaryFiles/ExpVideos/rewardData.txt";
	//QFile file(filename);
	rewardFile_ = QSharedPointer<QFile>(new QFile(filename));
	if (rewardFile_->open(QIODevice::ReadWrite))
	{
		rewardFileStream_ = QSharedPointer<QTextStream>(new QTextStream(rewardFile_.data()));
		QString header = "Start here,";
		(*rewardFileStream_) << QString(header);
		(*rewardFileStream_).flush();
	}

	ok = true;
	return true;

	

}

void MediaMuxer::addAudioStream(QString fileName, ffmpeg::CodecID codec_id)
{

	// Add the audio stream
	ffmpeg::AVCodec *encoder = avcodec_find_encoder(codec_id);
	pAudioStream_ = ffmpeg::av_new_stream(pOutputFormatCtx_, 0);
	if (!pAudioStream_) {
		printf("Could not allocate stream\n");
		return;
	}

	pAudioCodecCtx_ = pAudioStream_->codec;
	pAudioCodecCtx_->codec_id = codec_id;
	pAudioCodecCtx_->codec_type = ffmpeg::AVMEDIA_TYPE_AUDIO;
	//pAudioCodecCtx_->sample_fmt = ffmpeg::AV_SAMPLE_FMT_S16;
	pAudioCodecCtx_->sample_fmt = encoder->sample_fmts[0];
	pAudioCodecCtx_->bit_rate = 16000;
	pAudioCodecCtx_->sample_rate = N;
	pAudioCodecCtx_->channels = 1;

	pAudioCodecCtx_->time_base.den = FPS;
	pAudioCodecCtx_->time_base.num = 1;

	avcodec_thread_init(pAudioCodecCtx_, 10);

	// some formats want stream headers to be separate
	if (pOutputFormatCtx_->oformat->flags & AVFMT_GLOBALHEADER)
		pAudioCodecCtx_->flags |= CODEC_FLAG_GLOBAL_HEADER;


	if (av_set_parameters(pOutputFormatCtx_, NULL) < 0)
	{
		printf("Invalid output format parameters\n");
		return;
	}

	ffmpeg::dump_format(pOutputFormatCtx_, 0, fileName.toStdString().c_str(), 1);

	// open_video
	// find the audio encoder
	pAudioCodec_ = avcodec_find_encoder(pAudioCodecCtx_->codec_id);
	if (!pAudioCodec_)
	{
		printf("codec not found\n");
		return;
	}
	// open the codec
	if (avcodec_open(pAudioCodecCtx_, pAudioCodec_) < 0)
	{
		printf("could not open codec\n");
		return;
	}
	
	if (url_fopen(&pOutputFormatCtx_->pb, fileName.toStdString().c_str(), URL_WRONLY) < 0)
	{
		printf("Could not open '%s'\n", fileName.toStdString().c_str());
		return;
	}

	av_write_header(pOutputFormatCtx_);


}

void MediaMuxer::addVideoStream(QString fileName)
{
	// Add the video stream

	pVideoStream_ = ffmpeg::av_new_stream(pOutputFormatCtx_, 0);
	if (!pVideoStream_)
	{
		printf("Could not allocate stream\n");
		return;
	}

	pVideoCodecCtx_ = pVideoStream_->codec;
	pVideoCodecCtx_->codec_id = pOutputFormat_->video_codec;
	pVideoCodecCtx_->codec_type = ffmpeg::AVMEDIA_TYPE_VIDEO;

	pVideoCodecCtx_->bit_rate = Bitrate;
	pVideoCodecCtx_->width = getWidth();
	pVideoCodecCtx_->height = getHeight();
	pVideoCodecCtx_->time_base.den = FPS;
	pVideoCodecCtx_->time_base.num = 1;
	pVideoCodecCtx_->gop_size = Gop;
	pVideoCodecCtx_->pix_fmt = ffmpeg::PIX_FMT_YUV420P;

	avcodec_thread_init(pVideoCodecCtx_, 10);

	//if (c->codec_id == CODEC_ID_MPEG2VIDEO)
	//{
	//c->max_b_frames = 2;  // just for testing, we also add B frames
	//}

	// some formats want stream headers to be separate
	if (pOutputFormatCtx_->oformat->flags & AVFMT_GLOBALHEADER)
		pVideoCodecCtx_->flags |= CODEC_FLAG_GLOBAL_HEADER;


	if (av_set_parameters(pOutputFormatCtx_, NULL) < 0)
	{
		printf("Invalid output format parameters\n");
		return;
	}

	ffmpeg::dump_format(pOutputFormatCtx_, 0, fileName.toStdString().c_str(), 1);

	// open_video
	// find the video encoder
	pVideoCodec_ = avcodec_find_encoder(pVideoCodecCtx_->codec_id);
	if (!pVideoCodec_)
	{
		printf("codec not found\n");
		return;
	}
	// open the codec
	if (avcodec_open(pVideoCodecCtx_, pVideoCodec_) < 0)
	{
		printf("could not open codec\n");
		return;
	}

	// Allocate memory for output
	if (!initOutputBuf())
	{
		printf("Can't allocate memory for output bitstream\n");
		return;
	}

	// Allocate the YUV frame
	if (!initFrame())
	{
		printf("Can't init frame\n");
		return;
	}

	if (url_fopen(&pOutputFormatCtx_->pb, fileName.toStdString().c_str(), URL_WRONLY) < 0)
	{
		printf("Could not open '%s'\n", fileName.toStdString().c_str());
		return;
	}

	//av_write_header(pOutputFormatCtx_);
}
/**
\brief Completes writing the stream, closes it, release resources.
**/
bool MediaMuxer::close()
{
	if (!isOk())
		return false;

	av_write_trailer(pOutputFormatCtx_);

	if (pAudioStream_)
		// close_audio
		ffmpeg::avcodec_close(pAudioStream_->codec);

	if (pVideoStream_)
		// close_video
		ffmpeg::avcodec_close(pVideoStream_->codec);
	

	freeFrame();
	freeOutputBuf();


	/* free the streams */
	for (int i = 0; i < pOutputFormatCtx_->nb_streams; i++)
	{
		av_freep(&pOutputFormatCtx_->streams[i]->codec);
		av_freep(&pOutputFormatCtx_->streams[i]);
	}

	// Close file
	url_fclose(pOutputFormatCtx_->pb);

	// Free the stream
	av_free(pOutputFormatCtx_);

	initVars();

	if (!rewardFile_.isNull() && rewardFile_->isOpen())
	{
		rewardFile_->close();
	}

	return true;
}


/**
\brief Encode one frame

The frame must be of the same size as specified in the createFile call.

This is the standard method to encode videos with fixed frame rates.
Each call to encodeImage adds a frame, which will be played back at the frame rate
specified in the createFile call.
**/
int MediaMuxer::encodeImage(const QImage &img)
{
	return encodeImage_p(img);
}
/**
\brief Encode one frame

The frame must be of the same size as specified in the createFile call.

This mehtod allows to specify the presentation time stamp (pts) of the frame.
pts is specified in multiples of 1/framerate, where framerate was specified in the createFile call
E.g. to encode frames with a 1ms resolution: set the frame rate to 1000, and pts is the presentation
time in milliseconds.
pts must be monotonously increasing.
The first frame ought to have a pts of 0 to be immediately displayed.
**/
int MediaMuxer::encodeImagePts(const QImage &img, unsigned pts)
{
	return encodeImage_p(img, true, pts);
}


/******************************************************************************
* INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL
******************************************************************************/

void MediaMuxer::initVars()
{
	ok = false;
	pOutputFormatCtx_ = 0;
	pOutputFormat_ = 0;
	pVideoCodecCtx_ = 0;
	pAudioCodecCtx_ = 0;
	pVideoStream_ = 0;
	pAudioStream_ = 0;
	ppicture = 0;
	outbuf = 0;
	audio_outbuf = 0;
	picture_buf = 0;
	img_convert_ctx = 0;
}


/**
\brief Register the codecs
**/
bool MediaMuxer::initCodec()
{
	ffmpeg::avcodec_init();
	ffmpeg::av_register_all();

	printf("License: %s\n", ffmpeg::avformat_license());
	printf("AVCodec version %d\n", ffmpeg::avformat_version());
	printf("AVFormat configuration: %s\n", ffmpeg::avformat_configuration());

	return true;
}
/**
\brief Encode one frame - internal function
custompts: true if a custom presentation time stamp  is used
pts: presentation time stamp in milliseconds
**/
int MediaMuxer::encodeImage_p(const QImage &img, bool custompts, unsigned pts)
{
	if (!isOk())
		return -1;

	printf("encodeImage_p: %d\n", pts);

	//convertImage(img);       // Custom conversion routine
	convertImage_sws(img);     // SWS conversion


	if (custompts)                             // Handle custom pts
		pVideoCodecCtx_->coded_frame->pts = pts;  // Set the time stamp

	int out_size = ffmpeg::avcodec_encode_video(pVideoCodecCtx_, outbuf, outbuf_size, ppicture);
	//printf("Frame size: %d\n",out_size);

	if (custompts)                             // Handle custom pts
		pVideoCodecCtx_->coded_frame->pts = pts;  // Set the time stamp

	if (out_size > 0)
	{
		ffmpeg::av_init_packet(&pkt);

		//if (pCodecCtx->coded_frame->pts != AV_NOPTS_VALUE)
		if (pVideoCodecCtx_->coded_frame->pts != (0x8000000000000000LL))
			pkt.pts = av_rescale_q(pVideoCodecCtx_->coded_frame->pts, pVideoCodecCtx_->time_base, pVideoStream_->time_base);
		if (pVideoCodecCtx_->coded_frame->key_frame)
			pkt.flags |= AV_PKT_FLAG_KEY;

		//printf("c %d. pts %d. codedframepts: %ld pkt.pts: %ld\n",custompts,pts,pCodecCtx->coded_frame->pts,pkt.pts);

		pkt.stream_index = pVideoStream_->index;
		pkt.data = outbuf;
		pkt.size = out_size;
		int ret = ffmpeg::av_interleaved_write_frame(pOutputFormatCtx_, &pkt);
		//printf("Wrote %d\n",ret);
		if (ret<0)
			return -1;

	}
	return out_size;
}

/**
* encode one audio frame and send it to the muxer
* return 1 when encoding is finished, 0 otherwise
*/
int MediaMuxer::encodeAudioFrame(unsigned pts, double spikeTime, bool reward)
{	
	printf("encodeAudioFrame: %d\n", pts);

	pAudioCodecCtx_->coded_frame->pts = pts;  // Set the time stamp

	unsigned currentSpikePts = (unsigned)pts; //ms: in theory *1000/FPS but here FPS=1000 (fps)

	double PtsD = (double)(currentSpikePts);

	if (rewardFileStream_ && reward)
	{
		QString timeStr = "R_" + QString::number(PtsD) + ",";
		(*rewardFileStream_) << QString(timeStr);
		(*rewardFileStream_).flush();
	}


	// simple sound encoding	
	// N * 0.005 * 16 = 3528
	double Fs = 8000.; //8 samples per ms - sound is 1ms
	uint8_t samples[5 * N * 8] = { 0 }; // large buffer for 5 sec in case time between spikes fired is more than 1sec... //has to do with 16bits/sample encoding or utf_8 (8bits per sample)
	int n;                // buffer index
	
	double duration = 1.0; //ms
	double silenceDuration = 50; //ms - not detectable until at least 10ms
	double amplitude = 100.; //amplitude	
	
	if (lastSpikePts_ == 0)
	{
		lastSpikePts_ = currentSpikePts;
	}


	int num_silent_samples = 0;

	// add silence
	double tmp = (double)(currentSpikePts - lastSpikePts_);
	if (tmp > 0)
	{
		double FPSd = (double)(FPS);
		silenceDuration = (double)((tmp * 1000 / FPS) - 1); //time difference in number of frames converted to number of ms
		if (silenceDuration > 0)
		{
			num_silent_samples = round(silenceDuration * (Fs / 1000.0));
			num_silent_samples = 2 * num_silent_samples;

			for (int x = 0; x < num_silent_samples; ++x)
			{
				samples[x] = 0.0;
			}

		}
	}
	
	if (reward) //1 ms
	{
		duration = 1.;
		amplitude = 90;
		double freq = 250;

		// add sound
		int num_samples = round(duration * (Fs / 1000.0));
		num_samples = 2 * num_samples;

		for (int x = num_silent_samples; x < num_silent_samples + num_samples; ++x)
		//for (int x = 0; x < num_samples; ++x)
		{
			samples[x] = amplitude * sin(2 * M_PI * freq * (x / Fs));
		}

		audio_outbuf_size = 1 * (num_silent_samples + num_samples);
		//audio_outbuf_size = 1 * (num_samples);
		
	}
	else //1ms
	{		
		// add sound
		int num_samples = round(duration * (Fs / 1000.0));
		num_samples = 2 * num_samples;

		for (int x = num_silent_samples; x < num_silent_samples + num_samples; ++x)
		{
			samples[x] = amplitude;
		}
		//set buffer
		audio_outbuf_size = 1 * (num_silent_samples + num_samples);  //(nb of samples * 16bits per sample/8bits per samples(utf8)?
	
	}
	lastSpikePts_ = currentSpikePts;

	audio_outbuf = new uint8_t[audio_outbuf_size];

	if (audio_outbuf == 0)
		return false;
	
	int  out_size = ffmpeg::avcodec_encode_audio(pAudioCodecCtx_, audio_outbuf, audio_outbuf_size, (const short*)samples);

	pAudioCodecCtx_->coded_frame->pts = pts;  // Set the time stamp

	if (out_size>0)
	{
		// Packet
		ffmpeg::AVPacket pkt = { 0 };
		av_init_packet(&pkt);
		pkt.data = NULL; // packet data will be allocated by the encoder
		pkt.size = 0;
		if (pAudioCodecCtx_->coded_frame->pts != (0x8000000000000000LL))
			pkt.pts = av_rescale_q(pAudioCodecCtx_->coded_frame->pts, pAudioCodecCtx_->time_base, pAudioStream_->time_base);

		if (pAudioCodecCtx_->coded_frame->key_frame)
			pkt.flags |= AV_PKT_FLAG_KEY;

		pkt.stream_index = pAudioStream_->index;
		pkt.data = audio_outbuf;

		pkt.size = out_size;
		//pkt.duration = 100;
		pkt.dts = pkt.pts;

		int ret = av_interleaved_write_frame(pOutputFormatCtx_, &pkt);
		if (ret<0)
			return -1;
		av_free_packet(&pkt);
	}

	//end simple sound encoding


	return pkt.size;
}

/**
Ensures sizes are some reasonable multiples
**/
bool MediaMuxer::isSizeValid()
{
	if (getWidth() % 8)
		return false;
	if (getHeight() % 8)
		return false;
	return true;
}

unsigned MediaMuxer::getWidth()
{
	return Width;
}
unsigned MediaMuxer::getHeight()
{
	return Height;
}
bool MediaMuxer::isOk()
{
	return ok;
}

/**
Free memory for the compressed bitstream
**/
void MediaMuxer::freeAudioOutputBuf()
{
	if (audio_outbuf)
	{
		delete[] audio_outbuf;
		audio_outbuf = 0;
	}
}

/**
Allocate memory for the compressed bitstream
**/
bool MediaMuxer::initOutputBuf()
{
	outbuf_size = getWidth()*getHeight() * 3;        // Some extremely generous memory allocation for the encoded frame.
	outbuf = new uint8_t[outbuf_size];
	if (outbuf == 0)
		return false;
	return true;
}
/**
Free memory for the compressed bitstream
**/
void MediaMuxer::freeOutputBuf()
{
	if (outbuf)
	{
		delete[] outbuf;
		outbuf = 0;
	}
}

bool MediaMuxer::initFrame()
{
	ppicture = ffmpeg::avcodec_alloc_frame();
	if (ppicture == 0)
		return false;

	int size = avpicture_get_size(pVideoCodecCtx_->pix_fmt, pVideoCodecCtx_->width, pVideoCodecCtx_->height);
	picture_buf = new uint8_t[size];
	if (picture_buf == 0)
	{
		av_free(ppicture);
		ppicture = 0;
		return false;
	}

	// Setup the planes
	avpicture_fill((ffmpeg::AVPicture *)ppicture, picture_buf, pVideoCodecCtx_->pix_fmt, pVideoCodecCtx_->width, pVideoCodecCtx_->height);

	return true;
}
void MediaMuxer::freeFrame()
{
	if (picture_buf)
	{
		delete[] picture_buf;
		picture_buf = 0;
	}
	if (ppicture)
	{
		av_free(ppicture);
		ppicture = 0;
	}
}

/**
\brief Convert the QImage to the internal YUV format

Custom conversion - not very optimized.

**/

bool MediaMuxer::convertImage(const QImage &img)
{
	// Check if the image matches the size
	if (img.width() != getWidth() || img.height() != getHeight())
	{
		printf("Wrong image size!\n");
		return false;
	}
	if (img.format() != QImage::Format_RGB32	&& img.format() != QImage::Format_ARGB32)
	{
		printf("Wrong image format\n");
		return false;
	}

	// RGB32 to YUV420

	int size = getWidth()*getHeight();
	// Y
	for (unsigned y = 0; y<getHeight(); y++)
	{

		unsigned char *s = (unsigned char*)img.scanLine(y);
		unsigned char *d = (unsigned char*)&picture_buf[y*getWidth()];
		//printf("Line %d. d: %p. picture_buf: %p\n",y,d,picture_buf);

		for (unsigned x = 0; x<getWidth(); x++)
		{
			unsigned int r = s[2];
			unsigned int g = s[1];
			unsigned int b = s[0];

			unsigned Y = (r * 2104 + g * 4130 + b * 802 + 4096 + 131072) >> 13;
			if (Y>235) Y = 235;

			*d = Y;

			d += 1;
			s += 4;
		}
	}

	// U,V
	for (unsigned y = 0; y<getHeight(); y += 2)
	{
		unsigned char *s = (unsigned char*)img.scanLine(y);
		unsigned int ss = img.bytesPerLine();
		unsigned char *d = (unsigned char*)&picture_buf[size + y / 2 * getWidth() / 2];

		//printf("Line %d. d: %p. picture_buf: %p\n",y,d,picture_buf);

		for (unsigned x = 0; x<getWidth(); x += 2)
		{
			// Cr = 128 + 1/256 * ( 112.439 * R'd -  94.154 * G'd -  18.285 * B'd)
			// Cb = 128 + 1/256 * (- 37.945 * R'd -  74.494 * G'd + 112.439 * B'd)

			// Get the average RGB in a 2x2 block
			int r = (s[2] + s[6] + s[ss + 2] + s[ss + 6] + 2) >> 2;
			int g = (s[1] + s[5] + s[ss + 1] + s[ss + 5] + 2) >> 2;
			int b = (s[0] + s[4] + s[ss + 0] + s[ss + 4] + 2) >> 2;

			int Cb = (-1214 * r - 2384 * g + 3598 * b + 4096 + 1048576) >> 13;
			if (Cb<16)
				Cb = 16;
			if (Cb>240)
				Cb = 240;

			int Cr = (3598 * r - 3013 * g - 585 * b + 4096 + 1048576) >> 13;
			if (Cr<16)
				Cr = 16;
			if (Cr>240)
				Cr = 240;

			*d = Cb;
			*(d + size / 4) = Cr;

			d += 1;
			s += 8;
		}
	}
	return true;
}

/**
**/

bool MediaMuxer::convertImage_sws(const QImage &img)
{
	// Check if the image matches the size
	if (img.width() != getWidth() || img.height() != getHeight())
	{
		printf("Wrong image size!\n");
		return false;
	}
	if (img.format() != QImage::Format_RGB32	&& img.format() != QImage::Format_ARGB32)
	{
		printf("Wrong image format\n");
		return false;
	}

	img_convert_ctx = ffmpeg::sws_getCachedContext(img_convert_ctx, getWidth(), getHeight(), ffmpeg::PIX_FMT_BGRA, getWidth(), getHeight(), ffmpeg::PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	//img_convert_ctx = ffmpeg::sws_getCachedContext(img_convert_ctx,getWidth(),getHeight(),ffmpeg::PIX_FMT_BGRA,getWidth(),getHeight(),ffmpeg::PIX_FMT_YUV420P,SWS_FAST_BILINEAR, NULL, NULL, NULL);
	if (img_convert_ctx == NULL)
	{
		printf("Cannot initialize the conversion context\n");
		return false;
	}

	uint8_t *srcplanes[3];
	srcplanes[0] = (uint8_t*)img.bits();
	srcplanes[1] = 0;
	srcplanes[2] = 0;

	int srcstride[3];
	srcstride[0] = img.bytesPerLine();
	srcstride[1] = 0;
	srcstride[2] = 0;


	ffmpeg::sws_scale(img_convert_ctx, srcplanes, srcstride, 0, getHeight(), ppicture->data, ppicture->linesize);

	return true;
}

