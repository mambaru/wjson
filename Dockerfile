FROM debian
ENV LANG ru_RU.UTF-8
ENV TZ="Europe/Moscow"
RUN apt update
RUN apt -y install g++ cmake git wget make
COPY . /prj
WORKDIR "/prj"
RUN make release
RUN make install
RUN rm -rf /prj
