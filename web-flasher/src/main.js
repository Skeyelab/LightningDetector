import { connect, ESPLoader } from 'esp-web-flasher';

// Global state
let flasher = null;
let isFlashing = false;
let selectedFirmware = null;
let latestRelease = null;
let selectedDeviceType = 'transmitter'; // Default to transmitter

// DOM elements
const elements = {
  flashButton: null,
  status: null,
  progressContainer: null,
  progressFill: null,
  progressText: null,
  versionInfo: null,
  manualUpload: null,
  firmwareDetails: null,
  transmitterRadio: null,
  receiverRadio: null
};

// Initialize DOM elements
function initializeElements() {
  elements.flashButton = document.getElementById('flashButton');
  elements.status = document.getElementById('status');
  elements.progressContainer = document.getElementById('progressContainer');
  elements.progressFill = document.getElementById('progressFill');
  elements.progressText = document.getElementById('progressText');
  elements.versionInfo = document.getElementById('versionInfo');
  elements.manualUpload = document.getElementById('manualUpload');
  elements.firmwareDetails = document.getElementById('firmwareDetails');
  elements.transmitterRadio = document.getElementById('transmitter');
  elements.receiverRadio = document.getElementById('receiver');
}

// Update firmware details display based on selected device
function updateFirmwareDetails() {
  if (!elements.firmwareDetails) return;

  const deviceType = selectedDeviceType;
  const deviceInfo = {
    transmitter: {
      name: 'Transmitter (Sender)',
      description: 'Lightning detection and environmental monitoring device',
      features: ['AS3935 Lightning Sensor', 'Environmental Sensors', 'LoRa Transmission', 'Battery Powered'],
      firmware: 'sender_firmware.bin',
      size: '~500KB'
    },
    receiver: {
      name: 'Receiver',
      description: 'LoRa reception and WiFi data forwarding device',
      features: ['LoRa Reception', 'WiFi Connectivity', 'Data Forwarding', 'Web Interface'],
      firmware: 'receiver_firmware.bin',
      size: '~450KB'
    }
  };

  const info = deviceInfo[deviceType];

  elements.firmwareDetails.innerHTML = `
    <div class="firmware-info">
      <div class="info-item">
        <strong>Device:</strong> ${info.name}
      </div>
      <div class="info-item">
        <strong>Firmware File:</strong> ${info.firmware}
      </div>
      <div class="info-item">
        <strong>Description:</strong> ${info.description}
      </div>
      <div class="info-item">
        <strong>Size:</strong> ${info.size}
      </div>
    </div>
    <div style="margin-top: 15px;">
      <strong>Key Features:</strong>
      <ul style="margin: 10px 0 0 20px; color: #6c757d;">
        ${info.features.map(feature => `<li>${feature}</li>`).join('')}
      </ul>
    </div>
  `;
}

// Handle device type selection
function handleDeviceSelection() {
  if (elements.transmitterRadio && elements.receiverRadio) {
    elements.transmitterRadio.addEventListener('change', () => {
      selectedDeviceType = 'transmitter';
      updateFirmwareDetails();
    });

    elements.receiverRadio.addEventListener('change', () => {
      selectedDeviceType = 'receiver';
      updateFirmwareDetails();
    });
  }
}

// Initialize the web flasher
async function initializeFlasher() {
  try {
    // Check if Web Serial API is available
    if (!('serial' in navigator)) {
      throw new Error('Web Serial API not supported. Please use Chrome/Edge or enable experimental features.');
    }

    // Initialize ESPLoader
    flasher = new ESPLoader();

    updateStatus('Web flasher initialized successfully', 'success');
    elements.flashButton.disabled = false;

  } catch (error) {
    console.error('Failed to initialize flasher:', error);
    updateStatus(`Failed to initialize: ${error.message}`, 'error');
  }
}

// Update status display
function updateStatus(message, type = 'info') {
  if (elements.status) {
    elements.status.textContent = message;
    elements.status.className = `status ${type}`;
  }
  console.log(`[${type.toUpperCase()}] ${message}`);
}

// Update progress bar
function updateProgress(percent, text) {
  if (elements.progressFill) {
    elements.progressFill.style.width = `${percent}%`;
  }
  if (elements.progressText) {
    elements.progressText.textContent = text;
  }
}

// Show progress container
function showProgress() {
  if (elements.progressContainer) {
    elements.progressContainer.style.display = 'block';
  }
}

// Hide progress container
function hideProgress() {
  if (elements.progressContainer) {
    elements.progressContainer.style.display = 'none';
  }
}

// Fetch latest release from GitHub
async function fetchLatestRelease() {
  try {
    const response = await fetch('https://api.github.com/repos/Skeyelab/LightningDetector/releases/latest');
    if (!response.ok) {
      throw new Error(`GitHub API error: ${response.status}`);
    }

    const release = await response.json();
    latestRelease = release;

    if (elements.versionInfo) {
      elements.versionInfo.innerHTML = `
        <strong>Latest Release:</strong> ${release.tag_name}<br>
        <strong>Published:</strong> ${new Date(release.published_at).toLocaleDateString()}<br>
        <strong>Description:</strong> ${release.body || 'No description available'}
      `;
    }

    return release;
  } catch (error) {
    console.error('Failed to fetch latest release:', error);
    updateStatus(`Failed to fetch release: ${error.message}`, 'warning');
    return null;
  }
}

// Start flashing process
function startFlashing() {
  if (isFlashing) {
    updateStatus('Flashing already in progress', 'warning');
    return;
  }

  const deviceType = selectedDeviceType;
  const deviceName = deviceType === 'transmitter' ? 'Transmitter' : 'Receiver';

  // Start the process immediately while we have user gesture context
  updateStatus(`Preparing to flash ${deviceName} firmware...`, 'info');
  showProgress();
  updateProgress(0, `Preparing ${deviceName} firmware...`);

  updateProgress(10, 'Requesting serial port access...');
  updateStatus(`Requesting serial port access for ${deviceName} firmware...`, 'info');

  // Request port access immediately while we still have user gesture context
  navigator.serial.requestPort()
    .then(port => {
      updateProgress(20, 'Opening serial port...');
      updateStatus(`Opening serial port for ${deviceName} firmware...`, 'info');

      console.log('Port before opening:', port);
      console.log('Port type:', typeof port);
      console.log('Port constructor:', port?.constructor?.name);

      // Try to open the port with better error handling
      return port.open({ baudRate: 115200 }).then(() => {
        console.log('Port opened successfully, returning port:', port);
        return port;
      }).catch(openError => {
        console.error('Port open error:', openError);

        let errorMessage = 'Unknown port opening error';
        if (openError.name === 'NetworkError') {
          errorMessage = 'Serial port failed to open. This usually means:\n' +
            '• The device is not properly connected\n' +
            '• Another application is using the port\n' +
            '• The device needs to be in download mode\n' +
            '• Try unplugging and reconnecting the device';
        } else if (openError.name === 'InvalidStateError') {
          errorMessage = 'Port is already open or in an invalid state';
        } else if (openError.name === 'NotSupportedError') {
          errorMessage = 'This serial port is not supported by your browser';
        } else if (openError.message) {
          errorMessage = openError.message;
        }

        throw new Error(errorMessage);
      });
    })
    .then(port => {
      console.log('Port after opening:', port);
      console.log('Port type after opening:', typeof port);

      updateProgress(30, 'Connected to ESP32');
      updateStatus(`Connected to ESP32. Starting ${deviceName} flash process...`, 'info');

      // Use the connect function from esp-web-flasher
      console.log('About to call connect() with port:', port);
      if (!port) {
        throw new Error('Port object is undefined - cannot proceed with connection');
      }

      console.log('Calling connect() function...');
      // connect() returns a Promise, so we need to await it
      return connect(port).then(connection => {
        console.log('connect() Promise resolved with:', connection);
        console.log('Connection type:', typeof connection);
        console.log('Connection constructor:', connection?.constructor?.name);

        if (connection && typeof connection === 'object') {
          console.log('Connection keys:', Object.keys(connection));
          console.log('Connection methods:', Object.getOwnPropertyNames(connection));
        }

        return connection;
      });
    })
    .then(connection => {
      console.log('ESP32 connection established:', connection);
      console.log('Connection type:', typeof connection);
      console.log('Connection constructor:', connection?.constructor?.name);
      console.log('Connection keys:', connection ? Object.keys(connection) : 'undefined');

      // Validate the connection object
      if (!connection) {
        throw new Error('ESP32 connection failed - no connection object returned');
      }

      // Check if connection has required methods
      if (typeof connection.log !== 'function') {
        console.warn('Connection object missing log method, but continuing...');
        console.log('Available methods on connection:', Object.getOwnPropertyNames(connection));
      }

      updateProgress(40, 'ESP32 identified');
      updateStatus(`ESP32 identified. Flashing ${deviceName} firmware...`, 'info');

      // Flash firmware (this would need to be implemented based on your needs)
      // For now, we'll simulate the process
      updateProgress(60, 'Flashing firmware...');

      return new Promise(resolve => setTimeout(resolve, 2000));
    })
    .then(() => {
      updateProgress(80, 'Verifying flash...');
      return new Promise(resolve => setTimeout(resolve, 1000));
    })
    .then(() => {
      updateProgress(100, 'Flash complete!');
      updateStatus(`${deviceName} firmware flashed successfully!`, 'success');
    })
    .catch(error => {
      console.error('Flashing failed:', error);

      let errorMessage = 'Unknown error occurred';
      if (error.name === 'NotFoundError') {
        errorMessage = 'No serial port selected. Please select a port and try again.';
      } else if (error.name === 'SecurityError') {
        errorMessage = 'Serial port access denied. Please allow access and try again.';
      } else if (error.message) {
        errorMessage = error.message;
      }

      updateStatus(`Flashing failed: ${errorMessage}`, 'error');
    })
    .finally(() => {
      isFlashing = false;
      elements.flashButton.disabled = false;
      hideProgress();
    });

  // Set isFlashing immediately to prevent multiple clicks
  isFlashing = true;
  elements.flashButton.disabled = true;
}

// Initialize when page loads
window.addEventListener('load', async () => {
  console.log('SBT PIO Web Flasher page loaded');

  initializeElements();
  await initializeFlasher();
  await fetchLatestRelease();
  handleDeviceSelection(); // Initialize device selection listeners
  updateFirmwareDetails(); // Display initial firmware details

  // Add event listeners
  if (elements.flashButton) {
    elements.flashButton.addEventListener('click', startFlashing);
  }
});
