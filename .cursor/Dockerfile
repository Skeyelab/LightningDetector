# PlatformIO Development Container for Heltec ESP32 Projects
# Optimized for Cursor IDE background agents with caching support

FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install system dependencies
RUN apt-get update && apt-get install -y \
    # Core build tools
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    # Compilers and toolchains
    gcc \
    g++ \
    clang \
    clang-tidy \
    # Version control
    git \
    # Python and pipx
    python3 \
    python3-pip \
    python3-venv \
    python3-dev \
    # USB/Serial libraries for ESP32 flashing
    libusb-1.0-0-dev \
    libusb-1.0-0 \
    # Additional system libraries
    curl \
    wget \
    unzip \
    software-properties-common \
    # Cleanup
    && rm -rf /var/lib/apt/lists/*

# Install pipx for PlatformIO installation
RUN python3 -m pip install --no-cache-dir --upgrade pip && \
    python3 -m pip install --no-cache-dir pipx && \
    pipx ensurepath

# Install PlatformIO Core
RUN pipx install platformio

# Add PlatformIO to PATH for root user
ENV PATH="/root/.local/bin:$PATH"

# Create non-root user for development
RUN useradd -m -s /bin/bash -G dialout,tty,video platformio && \
    mkdir -p /home/platformio/.platformio && \
    chown -R platformio:platformio /home/platformio

# Install PlatformIO platforms and frameworks as root first
RUN pio platform install "espressif32" && \
    pio platform install "native" && \
    pio lib --global install "olikraus/U8g2@^2.36.0" && \
    pio lib --global install "jgromes/RadioLib@^6.5.0" && \
    pio lib --global install "ArduinoJson@^6.21.4"

# Install additional tools as root (WiFi and ArduinoOTA are available)
RUN pio lib --global install "arduino-libraries/WiFi@^1.2.7" && \
    pio lib --global install "ArduinoOTA@^1.1.0"

# Switch to platformio user for final setup
USER platformio
WORKDIR /home/platformio

# Install cppcheck (need to switch back to root for this)
USER root
RUN apt-get update && \
    apt-get install -y cppcheck && \
    rm -rf /var/lib/apt/lists/*

# Create directories for caching
RUN mkdir -p /home/platformio/.platformio/packages && \
    mkdir -p /home/platformio/.platformio/platforms && \
    mkdir -p /home/platformio/.platformio/lib && \
    mkdir -p /home/platformio/.platformio/cache

# Switch back to root for final setup
USER root

# Create symlinks for better caching
RUN ln -sf /home/platformio/.platformio /root/.platformio

# Set up volume mount points for caching
VOLUME ["/home/platformio/.platformio", "/root/.platformio"]

# Create entrypoint script for Cursor background agents
RUN echo '#!/bin/bash\n\
if [ "$1" = "platformio" ]; then\n\
    exec gosu platformio platformio "$@"\n\
else\n\
    exec "$@"\n\
fi' > /usr/local/bin/entrypoint && \
    chmod +x /usr/local/bin/entrypoint

# Install gosu for proper user switching
RUN apt-get update && \
    apt-get install -y gosu && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Expose common ports for development
EXPOSE 8008 8009

# Set default command
ENTRYPOINT ["/usr/local/bin/entrypoint"]
CMD ["platformio", "--version"]
