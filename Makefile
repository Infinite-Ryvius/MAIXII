ECHO="/bin/echo"
PRINTF="/usr/bin/printf"
MAKE="/usr/bin/make"

CROSS_COMPILE=arm-openwrt-linux-
CC=$(CROSS_COMPILE)gcc
CFLAGS += -I ./include/ \
		  -I ./include/media \
		  -I ./include/utils \
		  -I ./include/libisp/isp_tuning/ \
		  -I ./include/libisp/include/ \
		  -I ./include/libisp/include/V4l2Camera/ \
		  -I ./include/media/LIBRARY/libcedarc/include \
		  -I ./include/media/component 

SLIB = -L./lib/ -lz -lISP -lcdc_base -lion -lglog -llog -lVE -lisp_ini \
				-lmedia_utils -lMemAdapter -lmpp_isp -lmpp_vi \
			    -lcdx_base -lvideoengine -lcdx_stream \
				-lcdx_parser -lvdecoder -lvenc_codec -lvenc_base -lmpp_vo -lmedia_mpp \
				-lcedarxrender -lcedarxstream -lcdx_common -ladecoder -lhwdisplay \
				-lcutils -lcedarx_aencoder -lasound -lmpp_component -lvencoder 

#CFLAGS += -Wall -O -g -o

src=$(wildcard *.c)
src_obj = $(patsubst %.c,%,$(src))



#.c.o:
#	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $< 
all:$(src_obj)
%:%.c
	$(CC) $(SLIB) $(CFLAGS) -O2 -o $@ $<

clean:
	rm -rf $(src_obj)





