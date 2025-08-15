// SBT PIO Web Flasher using ESP Web Tools
// Simple, reliable implementation that avoids CORS issues

// Global variables
let selectedDeviceType = 'transmitter';
let installButton = null;

// DOM elements
const elements = {};

// Initialize DOM elements
function initializeElements() {
  elements.deviceSelector = document.querySelector('input[name="deviceType"]:checked');
  elements.status = document.getElementById('status');
  elements.espWebToolsContainer = document.getElementById('espWebToolsContainer');
}

// Get manifest URL for selected device type (local files to avoid CORS)
function getManifestUrl() {
  const deviceType = selectedDeviceType;
  const manifestName = deviceType === 'transmitter' ? 'sender_manifest.json' : 'receiver_manifest.json';
  
  // Use absolute paths that work on GitHub Pages
  return `/${manifestName}`;
}

// Create ESP Web Tools install button
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

  // Set the manifest URL (absolute path for GitHub Pages)
  const manifestUrl = getManifestUrl();
  installButton.manifest = manifestUrl;

  console.log('Using manifest:', manifestUrl);
  updateStatus(`Loading manifest: ${manifestUrl}`, 'info');

  // Test manifest accessibility
  fetch(manifestUrl)
    .then(response => {
      if (response.ok) {
        console.log('Manifest accessible:', response.status);
        updateStatus('Manifest loaded successfully', 'success');
      } else {
        console.error('Manifest not accessible:', response.status);
        updateStatus(`Manifest error: ${response.status}`, 'error');
      }
    })
    .catch(error => {
      console.error('Failed to fetch manifest:', error);
      updateStatus(`Manifest fetch failed: ${error.message}`, 'error');
    });

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

  // Add event listeners
  installButton.addEventListener('install-start', handleInstallStart);
  installButton.addEventListener('install-success', handleInstallSuccess);
  installButton.addEventListener('install-error', handleInstallError);
  installButton.addEventListener('install-progress', handleInstallProgress);

  // Add instructions
  const instructions = document.createElement('div');
  instructions.className = 'instructions';
  instructions.innerHTML = `
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
}

// Handle installation success
function handleInstallSuccess(event) {
  console.log('Installation successful:', event.detail);
  updateStatus('Firmware flashed successfully! Device will restart shortly.', 'success');
}

// Handle installation progress
function handleInstallProgress(event) {
  console.log('Installation progress:', event.detail);
  if (event.detail && event.detail.progress) {
    updateStatus(`Flashing in progress: ${Math.round(event.detail.progress * 100)}%`, 'info');
  }
}

// Handle installation errors
function handleInstallError(event) {
  console.error('Installation failed:', event.detail);
  const error = event.detail.error || 'Unknown error occurred';
  updateStatus(`Installation failed: ${error}`, 'error');
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
    });
  });
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

  // Create ESP Web Tools button
  createESPWebToolsButton();

  console.log('Initializing device selection...');
  handleDeviceSelection();

  updateStatus('Web flasher ready! Select device type and click Flash Firmware.', 'success');
  console.log('Page initialization complete');
});

