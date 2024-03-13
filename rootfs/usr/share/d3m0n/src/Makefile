SRC=*.cpp lib/*.c
OUT=d3m0n
CFLAGS=-std=c++17 -lstdc++ -lstdc++fs -lm -DBCM
LDFLAGS=-Llib -Ilib -I. -I/usr/include/opencv4/opencv2
# CFLAGS= -lm -DBCM -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
WARNINGS_IGNORE=-Wno-unused-result -Wno-unused-parameter -Wno-unused-variable -Wno-format-security -Wno-write-strings
GCC=g++
LIBS:=-I/usr/include/opencv4 -lopencv_stitching -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_ml -lopencv_videostab -lopencv_videoio -lopencv_viz -lopencv_ximgproc -lopencv_video -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc -lopencv_core
# $(shell pkg-config --cflags --libs opencv4)


all:	$(OUT)

$(OUT):	$(SRC)
	$(GCC) $(LDFLAGS)  $(WARNINGS_IGNORE) -o $@ $? $(LIBS) $(CFLAGS)

debug:
	gdb $(OUT)

clean:
	$(RM) $(OUT)