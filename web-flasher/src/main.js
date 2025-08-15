// SBT PIO Web Flasher using ESP Web Tools
// Based on working examples like Squeezelite-ESP32 installer
// This provides a modern, reliable way to flash ESP32/ESP8266 devices

// Global variables
let selectedDeviceType = 'receiver';
let isFlashing = false;
let currentPort = null;
let installButton = null;
let hasPortError = false; // Track if we've had port errors
let latestRelease = null; // Store latest release info
let currentManifestBlobUrl = null; // Store current blob URL for cleanup

// DOM elements
const elements = {};

// Initialize DOM elements
function initializeElements() {
  elements.deviceSelector = document.querySelector('input[name="deviceType"]:checked');
  elements.status = document.getElementById('status');
  elements.espWebToolsContainer = document.getElementById('espWebToolsContainer');
  elements.firmwareDetails = document.getElementById('firmwareDetails');
  elements.errorGuidance = document.getElementById('errorGuidance');
  elements.errorDetails = document.getElementById('errorDetails');
  elements.improvStatus = document.getElementById('improvStatus');
  elements.improvStatusText = document.getElementById('improvStatusText');
}

// Fetch latest release from GitHub
async function fetchLatestRelease() {
  try {
    updateStatus('Fetching latest release information from GitHub...', 'info');

    const response = await fetch('https://api.github.com/repos/Skeyelab/LightningDetector/releases/latest', {
      headers: {
        'Accept': 'application/vnd.github.v3+json',
        'User-Agent': 'SBT-PIO-Web-Flasher/1.0.0'
      }
    });

    if (!response.ok) {
      if (response.status === 403) {
        throw new Error('GitHub API rate limit exceeded. Using local manifests as fallback.');
      }
      throw new Error(`GitHub API error: ${response.status}`);
    }

    latestRelease = await response.json();
    console.log('Latest release:', latestRelease);

    updateStatus(`Latest release loaded: ${latestRelease.name}`, 'success');
    return latestRelease;

  } catch (error) {
    console.error('Failed to fetch latest release:', error);

    if (error.message.includes('rate limit')) {
      updateStatus('GitHub API rate limited - using local manifests', 'warning');
      return null; // Return null to trigger fallback
    } else {
      updateStatus(`Failed to fetch release: ${error.message}`, 'error');
      return null;
    }
  }
}

// Get manifest URL for selected device type
function getManifestUrl() {
  if (!latestRelease || !latestRelease.assets) {
    console.warn('No release assets found');
    return null;
  }

  const deviceType = selectedDeviceType;
  const manifestName = deviceType === 'transmitter' ? 'sender_manifest.json' : 'receiver_manifest.json';

  console.log('Looking for manifest:', manifestName);
  console.log('Available assets:', latestRelease.assets.map(a => a.name));

  const manifestAsset = latestRelease.assets.find(asset => asset.name === manifestName);
  if (!manifestAsset) {
    console.warn('Manifest not found in release assets');
    return null;
  }

  console.log('Found manifest asset:', manifestAsset);

  // Use the browser_download_url from the GitHub API response
  const downloadUrl = manifestAsset.browser_download_url;
  console.log('Using download URL:', downloadUrl);

  return downloadUrl;
}

// Generate manifest content locally based on GitHub release info
function generateLocalManifest() {
  if (!latestRelease || !latestRelease.assets) {
    console.log('No GitHub release info available, using fallback manifest');
    return generateFallbackManifest();
  }
  
  const deviceType = selectedDeviceType;
  const manifestName = deviceType === 'transmitter' ? 'sender_manifest.json' : 'receiver_manifest.json';
  
  // Find the firmware file for this device type
  const firmwareName = deviceType === 'transmitter' ? 'sender_firmware.bin' : 'receiver_firmware.bin';
  const firmwareAsset = latestRelease.assets.find(asset => asset.name === firmwareName);
  
  if (!firmwareAsset) {
    console.warn('Firmware asset not found:', firmwareName);
    console.log('Using fallback manifest with local firmware paths');
    return generateFallbackManifest();
  }
  
  // Create ESP Web Tools compatible manifest
  const manifest = {
    name: `SBT PIO Lightning Detector ${deviceType === 'transmitter' ? 'Transmitter' : 'Receiver'}`,
    version: latestRelease.tag_name || "1.0.0",
    description: `Lightning detection ${deviceType} firmware for Heltec WiFi LoRa 32 V3`,
    new_install_prompt_erase: true,
    builds: [
      {
        chipFamily: "ESP32-S3",
        parts: [
          { path: firmwareAsset.browser_download_url, offset: 0x10000 }
        ]
      },
      {
        chipFamily: "ESP32",
        parts: [
          { path: firmwareAsset.browser_download_url, offset: 0x10000 }
        ]
      },
      {
        chipFamily: "ESP8266",
        parts: [
          { path: firmwareAsset.browser_download_url, offset: 0x00000 }
        ]
      }
    ]
  };
  
  console.log('Generated local manifest:', manifest);
  return manifest;
}

// Generate local manifest as fallback when GitHub API fails
function generateFallbackManifest() {
  const deviceType = selectedDeviceType;
  
  // Create ESP Web Tools compatible manifest with local firmware paths
  const manifest = {
    name: `SBT PIO Lightning Detector ${deviceType === 'transmitter' ? 'Transmitter' : 'Receiver'}`,
    version: "1.0.0",
    description: `Lightning detection ${deviceType} firmware for Heltec WiFi LoRa 32 V3`,
    new_install_prompt_erase: true,
    builds: [
      {
        chipFamily: "ESP32-S3",
        parts: [
          { path: `./${deviceType === 'transmitter' ? 'sender_firmware_v1.0.0.bin' : 'receiver_firmware_v1.0.0.bin'}`, offset: 0x10000 }
        ]
      },
      {
        chipFamily: "ESP32",
        parts: [
          { path: `./${deviceType === 'transmitter' ? 'sender_firmware_v1.0.0.bin' : 'receiver_firmware_v1.0.0.bin'}`, offset: 0x10000 }
        ]
      },
      {
        chipFamily: "ESP8266",
        parts: [
          { path: `./${deviceType === 'transmitter' ? 'sender_firmware_v1.0.0.bin' : 'receiver_firmware_v1.0.0.bin'}`, offset: 0x00000 }
        ]
      }
    ]
  };
  
  console.log('Generated fallback manifest:', manifest);
  return manifest;
}

// Initialize the web flasher
async function initializeFlasher() {
  try {
    // Check if Web Serial API is available
    if (!('serial' in navigator)) {
      throw new Error('Web Serial API not supported. Please use Chrome/Edge or enable experimental features.');
    }

    // Set up minimal error monitoring for critical issues
    setupMinimalErrorMonitoring();

    // Fetch latest release from GitHub
    await fetchLatestRelease();

    // Create ESP Web Tools install button
    createESPWebToolsButton();

    updateStatus('Web flasher initialized successfully with ESP Web Tools', 'success');

  } catch (error) {
    console.error('Failed to initialize flasher:', error);
    updateStatus(`Failed to initialize: ${error.message}`, 'error');
  }
}

// Set up minimal error monitoring for critical issues only
function setupMinimalErrorMonitoring() {
  // Only intercept the most critical errors that break the user experience
  const originalConsoleError = console.error;
  console.error = function(...args) {
    const message = args.join(' ');

    // Only intercept critical serial port errors that break functionality
    if (message.includes('Failed to execute \'setSignals\' on \'SerialPort\'') ||
        message.includes('Failed to set control signals')) {

      console.log('[CRITICAL] Control signal error detected');
      hasPortError = true;

      // Show user guidance immediately
      setTimeout(() => {
        showCriticalErrorGuidance('Control signal error detected. This usually means the ESP32 is not in download mode or there are USB connection issues.');
      }, 1000);

      return; // Don't spam console with these errors
    }

    // Only intercept port already open errors
    if (message.includes('Failed to execute \'open\' on \'SerialPort\'') ||
        message.includes('The port is already open')) {

      console.log('[CRITICAL] Port conflict detected');
      hasPortError = true;

      // Show user guidance immediately
      setTimeout(() => {
        showCriticalErrorGuidance('Port conflict detected. Another application is using the serial port.');
      }, 1000);

      return; // Don't spam console with these errors
    }

    // Log all other errors normally (including Improv errors)
    originalConsoleError.apply(console, args);
  };
}

// Show critical error guidance for issues that break functionality
function showCriticalErrorGuidance(message) {
  if (!elements.errorGuidance || !elements.errorDetails) return;

  const guidance = `
    <h5>Critical Issue Detected</h5>
    <p>${message}</p>
    <ul>
      <li><strong>Immediate Action Required:</strong> This issue prevents flashing from working</li>
      <li><strong>Check Device:</strong> Ensure ESP32 is in download mode (hold BOOT while connecting USB)</li>
      <li><strong>USB Connection:</strong> Use a data cable, not just a charging cable</li>
      <li><strong>Close Other Apps:</strong> Close Arduino IDE, PlatformIO, or serial monitors</li>
      <li><strong>Try Different Port:</strong> Use a different USB port on your computer</li>
      <li><strong>Refresh Page:</strong> If issues persist, refresh this page</li>
    </ul>
  `;

  elements.errorDetails.innerHTML = guidance;
  elements.errorGuidance.style.display = 'block';

  // Update status to alert user
  updateStatus('Critical issue detected - see guidance below', 'error');
}

// Create ESP Web Tools install button with minimal, focused error handling
function createESPWebToolsButton() {
  // Clear the container
  if (elements.espWebToolsContainer) {
    elements.espWebToolsContainer.innerHTML = '';
  }

  // Create container for ESP Web Tools
  const container = document.createElement('div');
  container.className = 'esp-web-tools-container';
  container.style.cssText = 'margin: 20px 0; padding: 20px; border: 2px dashed #ccc; border-radius: 8px; text-align: center;';

  // Create the ESP Web Tools install button
  installButton = document.createElement('esp-web-install-button');

  // Generate manifest content locally based on GitHub release info
  const manifestContent = generateLocalManifest();
  if (!manifestContent) {
    // Show error if no manifest can be generated
    container.innerHTML = `
      <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
        <strong>Manifest Generation Failed</strong><br>
        Unable to generate manifest for ${selectedDeviceType} firmware.<br>
        Please check the console for errors.
      </div>
    `;
    
    if (elements.espWebToolsContainer) {
      elements.espWebToolsContainer.appendChild(container);
    }
    return;
  }

  console.log('Using generated manifest:', manifestContent);
  
  // Update status to show if using fallback
  if (!latestRelease || !latestRelease.assets) {
    updateStatus('Using local fallback manifests (GitHub API unavailable)', 'warning');
  }
  
  // Create a blob URL from the manifest content
  const manifestBlob = new Blob([JSON.stringify(manifestContent)], { type: 'application/json' });
  currentManifestBlobUrl = URL.createObjectURL(manifestBlob);
  installButton.manifest = currentManifestBlobUrl;

  // Customize the button appearance
  installButton.style.cssText = `
    --esp-tools-button-color: #667eea;
    --esp-tools-button-text-color: white;
    --esp-tools-button-border-radius: 8px;
  `;

  // Add custom slots for better UX
  const customButton = document.createElement('button');
  customButton.slot = 'activate';
  customButton.textContent = `Flash ${selectedDeviceType === 'transmitter' ? 'Transmitter' : 'Receiver'} Firmware`;
  customButton.className = 'btn btn-primary btn-lg';
  customButton.style.cssText = 'padding: 15px 30px; font-size: 18px; margin: 10px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; border-radius: 8px; cursor: pointer;';

  const unsupportedMessage = document.createElement('div');
  unsupportedMessage.slot = 'unsupported';
  unsupportedMessage.innerHTML = `
    <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
      <strong>Browser Not Supported</strong><br>
      Please use Google Chrome or Microsoft Edge to flash firmware.
    </div>
  `;

  const notAllowedMessage = document.createElement('div');
  notAllowedMessage.slot = 'not-allowed';
  notAllowedMessage.innerHTML = `
    <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
      <strong>HTTPS Required</strong><br>
      Web Serial requires a secure connection (HTTPS) to work.
    </div>
  `;

  // Add elements to the install button
  installButton.appendChild(customButton);
  installButton.appendChild(unsupportedMessage);
  installButton.appendChild(notAllowedMessage);

  // Add simple, focused event listeners
  installButton.addEventListener('install-start', handleInstallStart);
  installButton.addEventListener('install-success', handleInstallSuccess);
  installButton.addEventListener('install-error', handleInstallError);
  installButton.addEventListener('install-progress', handleInstallProgress);

  // Add simple instructions
  const instructions = document.createElement('div');
  instructions.innerHTML = `
    <div style="margin: 20px 0; text-align: left;">
      <h4>Instructions:</h4>
      <ol>
        <li>Connect your ESP32 device to your computer via USB</li>
        <li>Click the "Flash Firmware" button above</li>
        <li>Select the COM port when prompted</li>
        <li>Wait for the flashing process to complete</li>
        <li>Your device will restart with the new firmware</li>
      </ol>
      <p><strong>Note:</strong> Make sure your device is in download mode (hold BOOT button while connecting USB)</p>

      <h4>If Flashing Fails:</h4>
      <ul>
        <li><strong>Check Device:</strong> Ensure ESP32 is properly connected and in download mode</li>
        <li><strong>Try Again:</strong> Click the flash button again</li>
        <li><strong>Refresh Page:</strong> If issues persist, refresh the page</li>
        <li><strong>Different USB Port:</strong> Try a different USB port or cable</li>
      </ul>
    </div>
  `;

  // Add everything to the container
  container.appendChild(installButton);
  container.appendChild(instructions);

  // Insert the container into the espWebToolsContainer
  if (elements.espWebToolsContainer) {
    elements.espWebToolsContainer.appendChild(container);
  }
}

// Handle installation start
function handleInstallStart(event) {
  console.log('Installation started:', event.detail);
  updateStatus('Installation started - connecting to device...', 'info');
  isFlashing = true;
  hideErrorGuidance();
  hideImprovStatus();

  // Reset port error flag for new attempt
  hasPortError = false;
}

// Handle installation success
function handleInstallSuccess(event) {
  console.log('Installation successful:', event.detail);
  updateStatus('Firmware flashed successfully! Device will restart shortly.', 'success');
  isFlashing = false;
  currentPort = null;
  hideErrorGuidance();
  hideImprovStatus();

  // Reset port error flag on success
  hasPortError = false;

  // Clean up blob URL
  if (currentManifestBlobUrl) {
    URL.revokeObjectURL(currentManifestBlobUrl);
    currentManifestBlobUrl = null;
  }
}

// Handle installation progress
function handleInstallProgress(event) {
  console.log('Installation progress:', event.detail);
  if (event.detail && event.detail.progress) {
    updateStatus(`Flashing in progress: ${Math.round(event.detail.progress * 100)}%`, 'info');
  }
}

// Handle installation errors with simple, focused guidance
function handleInstallError(event) {
  console.error('Installation failed:', event.detail);
  const error = event.detail.error || 'Unknown error occurred';
  updateStatus(`Installation failed: ${error}`, 'error');
  isFlashing = false;
  currentPort = null;

  // Show simple error guidance
  showSimpleErrorGuidance(error);

  // Clean up blob URL
  if (currentManifestBlobUrl) {
    URL.revokeObjectURL(currentManifestBlobUrl);
    currentManifestBlobUrl = null;
  }
}

// Show simple, focused error guidance
function showSimpleErrorGuidance(error) {
  if (!elements.errorGuidance || !elements.errorDetails) return;

  let guidance = '';

  if (error.includes('Failed to set control signals') || error.includes('setSignals')) {
    guidance = `
      <h5>Connection Issue</h5>
      <p>There was a problem connecting to your ESP32 device.</p>
      <ul>
        <li><strong>Enter Download Mode:</strong> Hold BOOT button while connecting USB, then release</li>
        <li><strong>Check USB Cable:</strong> Use a data cable, not just a charging cable</li>
        <li><strong>Try Different Port:</strong> Use a different USB port on your computer</li>
        <li><strong>Reconnect Device:</strong> Unplug and reconnect the ESP32</li>
      </ul>
    `;
  } else if (error.includes('port is already open') || error.includes('already open')) {
    guidance = `
      <h5>Port in Use</h5>
      <p>Another application is using the serial port.</p>
      <ul>
        <li><strong>Close Other Apps:</strong> Close Arduino IDE, PlatformIO, or serial monitors</li>
        <li><strong>Refresh Page:</strong> Refresh this page to reset connections</li>
        <li><strong>Reconnect Device:</strong> Unplug and reconnect the ESP32</li>
      </ul>
    `;
  } else if (error.includes('Improv')) {
    guidance = `
      <h5>Improv Not Available</h5>
      <p>This is normal - the flasher will continue with standard serial flashing.</p>
      <ul>
        <li><strong>Continue:</strong> The error is expected and flashing should proceed</li>
        <li><strong>Check Device:</strong> Ensure your ESP32 is properly connected</li>
        <li><strong>Try Again:</strong> Click the flash button again</li>
      </ul>
    `;
  } else {
    guidance = `
      <h5>General Error</h5>
      <p>An unexpected error occurred during flashing.</p>
      <ul>
        <li><strong>Try Again:</strong> Click the flash button to retry</li>
        <li><strong>Check Connection:</strong> Ensure ESP32 is properly connected</li>
        <li><strong>Refresh Page:</strong> If issues persist, refresh the page</li>
      </ul>
    `;
  }

  elements.errorDetails.innerHTML = guidance;
  elements.errorGuidance.style.display = 'block';
}

// Hide error guidance
function hideErrorGuidance() {
  if (elements.errorGuidance) {
    elements.errorGuidance.style.display = 'none';
  }
}

// Show Improv status indicator
function showImprovStatus(message, type = 'info') {
  if (!elements.improvStatus || !elements.improvStatusText) return;

  elements.improvStatusText.textContent = message;
  elements.improvStatus.style.display = 'block';

  // Update colors based on type
  if (type === 'error') {
    elements.improvStatus.style.background = '#f8d7da';
    elements.improvStatus.style.borderColor = '#f5c6cb';
    elements.improvStatusText.style.color = '#721c24';
  } else if (type === 'warning') {
    elements.improvStatus.style.background = '#fff3cd';
    elements.improvStatus.style.borderColor = '#ffeaa7';
    elements.improvStatusText.style.color = '#856404';
  } else {
    elements.improvStatus.style.background = '#e8f4fd';
    elements.improvStatus.style.borderColor = '#bee5eb';
    elements.improvStatusText.style.color = '#0c5460';
  }
}

// Hide Improv status indicator
function hideImprovStatus() {
  if (elements.improvStatus) {
    elements.improvStatus.style.display = 'none';
  }
}



// Handle device type selection
function handleDeviceSelection() {
  const radioButtons = document.querySelectorAll('input[name="deviceType"]');

  radioButtons.forEach(radio => {
    radio.addEventListener('change', async (event) => {
      selectedDeviceType = event.target.value;
      console.log(`${selectedDeviceType} selected`);

      // Update the ESP Web Tools button with new manifest
      createESPWebToolsButton();

      // Update status
      const deviceName = selectedDeviceType === 'transmitter' ? 'Transmitter' : 'Receiver';
      updateStatus(`${deviceName} firmware selected`, 'info');

      // Update firmware details
      updateFirmwareDetails();

      // Hide any previous error guidance
      hideErrorGuidance();
    });
  });
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
      firmware: 'sender_firmware_v1.0.0.bin',
      size: '~100KB'
    },
    receiver: {
      name: 'Receiver',
      description: 'LoRa reception and WiFi data forwarding device',
      features: ['LoRa Reception', 'WiFi Connectivity', 'Data Forwarding', 'Web Interface'],
      firmware: 'receiver_firmware_v1.0.0.bin',
      size: '~100KB'
    }
  };

  const info = deviceInfo[deviceType];

  // Add release information if available
  let releaseInfo = '';
  if (latestRelease) {
    releaseInfo = `
      <div class="info-item" style="grid-column: 1 / -1; background: #e8f4fd; border-left-color: #17a2b8;">
        <strong>Latest Release:</strong> ${latestRelease.name} (${latestRelease.published_at ? new Date(latestRelease.published_at).toLocaleDateString() : 'Unknown date'})
      </div>
      <div class="info-item" style="grid-column: 1 / -1; background: #e8f4fd; border-left-color: #17a2b8;">
        <strong>Release Notes:</strong> ${latestRelease.body ? latestRelease.body.substring(0, 100) + '...' : 'No release notes available'}
      </div>
    `;
  }

  elements.firmwareDetails.innerHTML = `
    <div class="firmware-info">
      ${releaseInfo}
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

// Update status display
function updateStatus(message, type = 'info') {
  if (elements.status) {
    elements.status.textContent = message;
    elements.status.className = `status ${type}`;
  }
  console.log(`[${type.toUpperCase()}] ${message}`);
}

// Initialize when page loads
window.addEventListener('load', async () => {
  console.log('SBT PIO Web Flasher page loaded');
  console.log('Using ESP Web Tools for reliable firmware flashing');

  initializeElements();
  await initializeFlasher();

  console.log('Initializing device selection...');
  handleDeviceSelection();

  console.log('Initializing firmware details...');
  updateFirmwareDetails();

  console.log('Page initialization complete');
});

// Add cleanup on page unload
window.addEventListener('beforeunload', () => {
  if (currentPort && currentPort.readable) {
    try {
      currentPort.close();
    } catch (error) {
      console.log('Error closing port on unload:', error);
    }
  }
  if (currentManifestBlobUrl) {
    URL.revokeObjectURL(currentManifestBlobUrl);
  }
});

// Simple recovery function
window.recoverFromError = function() {
  hideErrorGuidance();
  updateStatus('Ready to flash firmware', 'info');
  if (installButton) {
    installButton.style.opacity = '1';
    installButton.style.pointerEvents = 'auto';
  }

  // Reset port error flag
  hasPortError = false;
};

// Reset port state function for manual port conflict resolution
window.resetPortState = function() {
  console.log('[MANUAL] User requested port state reset');
  updateStatus('Resetting port state...', 'warning');

  try {
    // Try to close any existing ports
    if (currentPort && currentPort.readable) {
      try {
        currentPort.close();
        console.log('[MANUAL] Closed existing port');
      } catch (closeError) {
        console.log('[MANUAL] Error closing port:', closeError);
      }
    }

    // Reset state variables
    currentPort = null;
    hasPortError = false;

    // Hide error guidance
    hideErrorGuidance();

    // Update status
    updateStatus('Port state reset completed. Try flashing again.', 'success');

    // Re-enable the install button
    if (installButton) {
      installButton.style.opacity = '1';
      installButton.style.pointerEvents = 'auto';
    }

    console.log('[MANUAL] Port state reset completed');

  } catch (error) {
    console.error('[MANUAL] Port state reset failed:', error);
    updateStatus('Port state reset failed. Please refresh the page.', 'error');
  }
};

