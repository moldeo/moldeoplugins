ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive -std=c++11 -msse -msse2 -msse3
else
DFLAGS = -O2 -fpermissive -std=c++11 -msse -msse2 -msse3
endif

moduletype = resources
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

classifiersdir = ${datadir}/openface/classifiers
modeldir = ${datadir}/openface/model
AU_predictorsdir = ${datadir}/openface/AU_predictors

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\" -I/usr/include/opencv2 -I./OpenFace/lib/local/LandmarkDetector/include -I./OpenFace/lib/3rdParty/dlib/include -I./OpenFace/lib/local/FaceAnalyser/include -I./OpenFace/lib/local/GazeAnalyser/include
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_openface.la
libmoldeo_openface_la_SOURCES = moOpenFace.cpp moOpenFace.h
libmoldeo_openface_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${LIBOPENCV_CFLAGS}
libmoldeo_openface_la_LDFLAGS = -L./OpenFace/build/lib/3rdParty/dlib -L./OpenFace/build/lib/local/LandmarkDetector -L./OpenFace/build/lib/local/FaceAnalyser -L./OpenFace/build/lib/local/GazeAnalyser
libmoldeo_openface_la_LIBADD = -lLandmarkDetector -lFaceAnalyser -lGazeAnalyser -ldlib ${LIBOPENCV_LIBS} -ltbb -latomic -lippicv -lopenblas
libmoldeo_openface_la_LIBS = ${LIBMOLDEO_LIBS}  -lLandmarkDetector -lFaceAnalyser -lGazeAnalyser -ldlib ${LIBOPENCV_LIBS} -ltbb -latomic -lippicv -lopenblas

libmoldeo_openface_includedir = $(includedir)/moldeo/${moduletype}/openface
libmoldeo_openface_include_HEADERS = moOpenFace.h

dist_classifiers_DATA = ./OpenFace/build/bin/classifiers/*.*
dist_model_DATA = ./OpenFace/build/bin/model/*.*
dist_AU_predictors_DATA = ./OpenFace/build/bin/AU_predictors/*.*

METASOURCES = AUTO
