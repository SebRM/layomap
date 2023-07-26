# Use an appropriate Ubuntu base image.
FROM ubuntu:20.04

# Disable interactive prompts to not block the build process.
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies.
RUN apt-get update && \
    apt-get install -y libqt5widgets5 libqt5gui5 libqt5printsupport5 libpolyclipping22 libqt5positioning5 libqt5sensors5 libqt5serialport5 libqt5network5 libqt5core5a libproj15 libgdal26 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Copy the compiled program into the Docker image.
# This means one must run ./build.sh before docker build.
COPY build/layomap/layomap /usr/local/bin/

# This ENTRYPOINT directive makes the Docker image behave as an executable.
# This means when the image is run, it will run layomap by default, 
# but it will still accept arguments like -s and -l.
ENTRYPOINT ["layomap"]
