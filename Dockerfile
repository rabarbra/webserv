FROM alpine:latest
RUN apk add --no-cache clang make musl-dev \
	python3 py3-flask perl
RUN ln -sf /usr/bin/clang++ /usr/bin/c++
COPY . /app/
RUN cd app && make re
EXPOSE 9000
WORKDIR /app
CMD [ "./webserv",  "webserv.conf" ]
