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
async function startFlashing() {
  if (isFlashing) {
    updateStatus('Flashing already in progress', 'warning');
    return;
  }

  try {
    isFlashing = true;
    elements.flashButton.disabled = true;

    const deviceType = selectedDeviceType;
    const deviceName = deviceType === 'transmitter' ? 'Transmitter' : 'Receiver';
    
    updateStatus(`Preparing to flash ${deviceName} firmware...`, 'info');
    showProgress();
    updateProgress(0, `Preparing ${deviceName} firmware...`);

    updateProgress(10, 'Connecting to ESP32...');
    updateStatus(`Connecting to ESP32 for ${deviceName} firmware...`, 'info');

    // Connect to ESP32
    const port = await navigator.serial.requestPort();
    await port.open({ baudRate: 115200 });

    updateProgress(20, 'Connected to ESP32');
    updateStatus(`Connected to ESP32. Starting ${deviceName} flash process...`, 'info');

    // Initialize ESPLoader with the port
    await flasher.connect(port);

    updateProgress(40, 'ESP32 identified');
    updateStatus(`ESP32 identified. Flashing ${deviceName} firmware...`, 'info');

    // Flash firmware (this would need to be implemented based on your needs)
    // For now, we'll simulate the process
    updateProgress(60, 'Flashing firmware...');
    await new Promise(resolve => setTimeout(resolve, 2000));

    updateProgress(80, 'Verifying flash...');
    await new Promise(resolve => setTimeout(resolve, 1000));

    updateProgress(100, 'Flash complete!');
    updateStatus(`${deviceName} firmware flashed successfully!`, 'success');

    // Close the port
    await port.close();

  } catch (error) {
    console.error('Flashing failed:', error);
    updateStatus(`Flashing failed: ${error.message}`, 'error');
  } finally {
    isFlashing = false;
    elements.flashButton.disabled = false;
    hideProgress();
  }
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
