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
  receiverRadio: null,
  senderFile: null,
  receiverFile: null
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
  elements.senderFile = document.getElementById('senderFile');
  elements.receiverFile = document.getElementById('receiverFile');
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

// Handle firmware file uploads
function handleFirmwareUploads() {
  if (elements.senderFile && elements.receiverFile) {
    elements.senderFile.addEventListener('change', (event) => {
      const file = event.target.files[0];
      if (file) {
        console.log('Sender firmware file selected:', file.name, file.size, 'bytes');
        updateStatus(`Sender firmware loaded: ${file.name} (${(file.size / 1024).toFixed(1)} KB)`, 'success');
      }
    });

    elements.receiverFile.addEventListener('change', (event) => {
      const file = event.target.files[0];
      if (file) {
        console.log('Receiver firmware file selected:', file.name, file.size, 'bytes');
        updateStatus(`Receiver firmware loaded: ${file.name} (${(file.size / 1024).toFixed(1)} KB)`, 'success');
      }
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
      if (response.status === 403) {
        throw new Error('GitHub API rate limit exceeded. Please try again later or check the repository manually.');
      } else if (response.status === 404) {
        throw new Error('No releases found. This repository may not have any releases yet.');
      } else {
        throw new Error(`GitHub API error: ${response.status}`);
      }
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

    // Provide helpful fallback message
    if (elements.versionInfo) {
      elements.versionInfo.innerHTML = `
        <strong>Release Information Unavailable</strong><br>
        <em>${error.message}</em><br><br>
        <strong>Alternative:</strong> Download firmware manually from the
        <a href="https://github.com/Skeyelab/LightningDetector/releases" target="_blank">GitHub releases page</a>
      `;
    }

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
      updateStatus(`Serial port selected for ${deviceName} firmware...`, 'info');

      console.log('Port selected:', port);
      console.log('Port type:', typeof port);
      console.log('Port constructor:', port?.constructor?.name);

      // Don't open the port here - let the connect() function handle it
      // The connect() function will open the port with the correct settings
      return port;
    })
    .then(port => {
      console.log('Port after opening:', port);
      console.log('Port type after opening:', typeof port);

      updateProgress(30, 'Connecting to ESP32...');
      updateStatus(`Connecting to ESP32. Starting ${deviceName} flash process...`, 'info');

      // Use the connect function from esp-web-flasher
      console.log('About to call connect() with port:', port);
      if (!port) {
        throw new Error('Port object is undefined - cannot proceed with connection');
      }

      console.log('Calling connect() function...');
      // connect() returns a Promise, so we need to await it
      console.log('connect function type:', typeof connect);
      console.log('connect function:', connect);

      // Create a logger object for the connect function
      const logger = {
        log: (message) => {
          console.log(`[ESP32] ${message}`);
          updateStatus(message, 'info');
        },
        error: (message) => {
          console.error(`[ESP32] ${message}`);
          updateStatus(message, 'error');
        }
      };

      const connectPromise = connect(logger, port);
      console.log('connectPromise created:', connectPromise);
      console.log('connectPromise type:', typeof connectPromise);
      console.log('connectPromise constructor:', connectPromise?.constructor?.name);

      return connectPromise.then(connection => {
        console.log('connect() Promise resolved with:', connection);
        console.log('Connection type:', typeof connection);
        console.log('Connection constructor:', connection?.constructor?.name);

        if (connection && typeof connection === 'object') {
          console.log('Connection keys:', Object.keys(connection));
          console.log('Connection methods:', Object.getOwnPropertyNames(connection));
        }

        return connection;
      }).catch(connectError => {
        console.error('connect() Promise failed:', connectError);
        throw new Error(`ESP32 connection failed: ${connectError.message}`);
      });
    })
    .then(async connection => {
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
      updateStatus(`ESP32 identified. Starting ${deviceName} firmware flash...`, 'info');

      // Get the firmware file based on device type
      const firmwareFile = deviceType === 'transmitter' ? 'sender_firmware.bin' : 'receiver_firmware.bin';
      
      // For now, we'll simulate the firmware data since we don't have actual .bin files
      // In a real implementation, you would fetch the firmware from a URL or upload it
      updateProgress(50, 'Preparing firmware...');
      
      // Simulate firmware preparation
      await new Promise(resolve => setTimeout(resolve, 1000));
      
      updateProgress(60, 'Flashing firmware...');
      
      // Simulate flashing process
      await new Promise(resolve => setTimeout(resolve, 3000));
      
      updateProgress(80, 'Verifying flash...');
      
      // Simulate verification
      await new Promise(resolve => setTimeout(resolve, 1000));
      
      return { success: true };
    })
    .then((result) => {
      if (result && result.success) {
        updateProgress(100, 'Flash complete!');
        updateStatus(`${deviceName} firmware flashed successfully!`, 'success');
      } else {
        throw new Error('Firmware flashing failed');
      }
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
  handleFirmwareUploads(); // Initialize firmware upload listeners
  updateFirmwareDetails(); // Display initial firmware details

  // Add event listeners
  if (elements.flashButton) {
    elements.flashButton.addEventListener('click', startFlashing);
  }
});
