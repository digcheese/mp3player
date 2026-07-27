FROM espressif/idf:release-v6.1

RUN apt-get update && apt-get install -y --no-install-recommends \
    udev \
    && rm -rf /var/lib/apt/lists/*

RUN usermod -aG dialout root

RUN echo "source /opt/esp/idf/export.sh" >> /root/.bashrc