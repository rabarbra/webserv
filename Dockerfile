FROM alpine:latest
RUN apk add --no-cache clang make musl-dev
RUN ln -sf /usr/bin/clang++ /usr/bin/c++
COPY . /app/
RUN cd app && make re
EXPOSE 8000
WORKDIR /app
CMD [ "./webserv",  "conf/docker.conf" ]
