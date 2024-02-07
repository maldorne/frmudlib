
FROM ghcr.io/maldorne/mudos:v21.7 as base

USER root

RUN apt-get update
# temporary, will be removed from final image
RUN apt-get install -y --force-yes procps telnet

# clone the full contents of the mudlib
WORKDIR /opt/mud
COPY --chown=mud:mud start.sh config.frmudlib frmudlib/
COPY --chown=mud:mud mudlib frmudlib/mudlib/

# reuse the same user created by the maldorne-mudos image
USER mud

# finishing touches
WORKDIR /opt/mud/frmudlib
RUN mv config.frmudlib ../bin/config
RUN chmod +x start.sh

ENTRYPOINT ./start.sh && /bin/bash

# expose telnet mudos port
EXPOSE 5000/tcp
