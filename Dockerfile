FROM alpine:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
ENV PROFILER_BACKEND='/language-data'
ENV GITURL="https://github.com/cisocrgroup"
ENV DEPS='cppunit-dev xerces-c icu-dev'
ENV BUILD_DEPS='g++ make build-base cmake git perl'
ENV LANG='C'
ENV MUSL_LOCPATH="/usr/local/share/i18n/locales/musl"
ENV LC_ALL='C'

# locale
RUN apk --no-cache add libintl \
	&& apk --no-cache --virtual .locale_build add cmake make musl-dev gcc gettext-dev git \
	&& git clone https://gitlab.com/rilian-la-te/musl-locales \
	&& cd musl-locales \
	&& cmake -DLOCALE_PROFILE=OFF -DCMAKE_INSTALL_PREFIX:PATH=/usr . \
	&& make && make install \
	&& cd .. && rm -r musl-locales \
	&& apk del .locale_build

COPY . /build
RUN apk add -U --repository http://dl-3.alpinelinux.org/alpine/edge/testing/ ${DEPS} ${BUILD_DEPS} \
	&& cd /build \
	&& cmake -DCMAKE_BUILD_TYPE=release . \
	&& make compileFBDic trainFrequencyList profiler \
	&& mkdir -p /apps \
	&& cp bin/compileFBDic bin/trainFrequencyList bin/profiler /apps/ \
	&& git clone ${GITURL}/Resources --branch master --single-branch \
	&& cd Resources/lexica \
	&& make FBDIC=/apps/compileFBDic TRAIN=/apps/trainFrequencyList \
	&& mkdir -p /language-data \
	&& cp -r german latin greek german.ini latin.ini greek.ini ${PROFILER_BACKEND} \
	&& cd / \
	&& rm -rf /build \
	&& apk del 	${BUILD_DEPS}
