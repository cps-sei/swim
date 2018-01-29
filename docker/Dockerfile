FROM gabrielmoreno/omnetpp-vnc:5.2.1
LABEL Description="Docker image for SWIM"

USER root
RUN apt update && apt install -y --no-install-recommends libboost-all-dev git r-base r-cran-reshape r-cran-ggplot2 r-cran-rsqlite evince ristretto

ENV BASE_DIR=${HOME}/seams-swim

RUN mkdir -p ${BASE_DIR}/swim

WORKDIR ${BASE_DIR}

RUN wget https://github.com/cps-sei/swim/archive/seams-artifact.tar.gz
RUN tar -xzf seams-artifact.tar.gz -C swim --strip-components=1 && rm seams-artifact.tar.gz

RUN tar xzf swim/queueinglib.tgz

WORKDIR ${BASE_DIR}/queueinglib

RUN make

WORKDIR ${BASE_DIR}/swim

RUN make cleanall && make makefiles && make

WORKDIR ${BASE_DIR}/swim/examples/simple_am

RUN make

RUN R -e 'install.packages("https://github.com/wilkelab/cowplot/archive/0.6.3.tar.gz", repos=NULL)'

WORKDIR ${HOME}
