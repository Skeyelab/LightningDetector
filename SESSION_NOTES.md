# Lightning Detector Project - Session Notes

## Setup summary
- **Project**: SBT PIO Lightning Detector for Heltec WiFi LoRa 32 V3
- **Platform**: PlatformIO with ESP32-S3
- **Architecture**: Modular C++ with sensor abstraction, communication interfaces, and state management
- **Web Flasher**: ESP Web Tools v10 (modern, reliable ESP32/ESP8266 flashing)
- **Status**: Core firmware complete, web flasher completely rebuilt to avoid CORS issues

## Recent Changes
- **Web Flasher**: Completely rebuilt from scratch to avoid CORS issues when deploying to GitHub Pages
- **CORS Resolution**: Eliminated all external API calls and GitHub API dependencies
- **Local Manifests**: Created local manifest files that reference local firmware files
- **Deployment Ready**: Added GitHub Actions workflow for automatic deployment to GitHub Pages

### Session log

#### 2025-01-15 18:30 UTC
- Context: Implemented ERI-14 Linear issue to remove RX/TX switching from button functionality
- Changes:
  - Removed RX/TX mode switching from button short press functionality
  - Eliminated `savePersistedRole()` and `oledRole()` functions
  - Updated `loadPersistedSettingsAndRole()` to `loadPersistedSettings()` (no role loading)
  - Reworked button functionality to be device-specific:
    * Sender: Short press cycles LoRa parameters (SF, BW, TX Power), Medium press broadcasts config
    * Receiver: Short press cycles display modes, Medium press changes network mode
    * Both: Long press enters config mode, 5 rapid clicks enters sleep mode
  - Updated `ButtonAction` enum to remove `ToggleMode` and add device-specific actions
  - Implemented multi-click detection for sleep mode functionality
  - Updated integration tests to reflect new button behavior
  - Device role now fixed at build time via `ROLE_SENDER`/`ROLE_RECEIVER` build flags
- Commands run:
  - `g++ -std=c++17 verify_button_logic.cpp -o verify_button_logic && ./verify_button_logic`
- Files touched:
  - `src/app_logic.h`, `src/app_logic.cpp` (updated button action classification)
  - `src/main.cpp` (completely reworked button handling, removed role switching)
  - `test/test_integration.cpp` (updated tests for new button functionality)
  - `verify_button_logic.cpp` (created verification script)
- Next steps:
  - Test compiled firmware on actual hardware
  - Implement sleep mode functionality with sensor/actuator monitoring
  - Consider adding configuration mode implementations

#### 2025-01-15 17:00 UTC
- Context: GitHub Actions workflow failed with "Permission denied" error when trying to push to gh-pages branch
- Changes:
  - Fixed GitHub Actions permissions by adding proper permissions block (contents: read, pages: write, id-token: write)
  - Updated workflow to use official GitHub Pages actions instead of third-party peaceiris/actions-gh-pages
  - Added proper firmware file copying to dist folder before upload step
  - Created .nojekyll file to ensure proper static file serving on GitHub Pages
  - Updated deployment script to handle firmware files correctly
  - Resolved workflow permission issues that prevented deployment
- Commands run:
  - `cd web-flasher && ./deploy.sh` (tested updated deployment script)
  - `git add -A && git commit -m "fix github-actions permissions"'` (committed fixes)
  - `git push` (pushed to GitHub)
- Files touched:
  - `.github/workflows/deploy-web-flasher.yml` (fixed permissions and workflow)
  - `web-flasher/deploy.sh` (updated to handle firmware files correctly)
  - `web-flasher/.nojekyll` (created for proper GitHub Pages serving)
  - `SESSION_NOTES.md` (updated with new session log)
- Next steps:
  - Monitor GitHub Actions workflow execution
  - Verify successful deployment to GitHub Pages
  - Test web flasher functionality on deployed site
  - Ensure manifests and firmware files are accessible

#### 2025-01-15 16:45 UTC
- Context: User reported "Failed to download manifest" error after web flasher deployment
- Changes:
  - Fixed manifest download issues by updating paths from relative (./filename) to absolute (/filename)
  - Updated both sender_manifest.json and receiver_manifest.json to use absolute paths
  - Modified JavaScript to use absolute paths for manifest URLs
  - Added manifest accessibility testing and better error handling
  - Created test-manifests.html for debugging file accessibility
  - Rebuilt and redeployed with corrected paths
- Commands run:
  - `cd web-flasher && npm run build` (successful rebuild)
  - `./deploy.sh` (deployment preparation)
  - `git add -A && git commit -m "fix manifest download issues"` (committed fixes)
  - `git push` (pushed to GitHub)
- Files touched:
  - `web-flasher/sender_manifest.json` (updated paths to absolute)
  - `web-flasher/receiver_manifest.json` (updated paths to absolute)
  - `web-flasher/src/main.js` (added error handling and absolute paths)
  - `web-flasher/test-manifests.html` (created for debugging)
  - `SESSION_NOTES.md` (updated with new session log)
- Next steps:
  - Test the deployed web flasher to ensure manifests load correctly
  - Verify ESP Web Tools can access the manifest files
  - Test firmware flashing with actual ESP32 devices
  - Monitor for any remaining manifest access issues

#### 2025-01-15 16:30 UTC
- Context: Web flasher was not working due to CORS issues when trying to fetch from GitHub API and releases
- Changes:
  - Completely rebuilt web flasher HTML and JavaScript to eliminate CORS issues
  - Removed all GitHub API calls and external dependencies
  - Created local manifest files (sender_manifest.json, receiver_manifest.json) that reference local firmware files
  - Simplified JavaScript to use ESP Web Tools directly with local manifests
  - Added device type selection (transmitter/receiver) with proper manifest switching
  - Created deployment script (deploy.sh) for GitHub Pages preparation
  - Added GitHub Actions workflow (.github/workflows/deploy-web-flasher.yml) for automatic deployment
  - Updated README with comprehensive deployment and troubleshooting instructions
  - Tested build process successfully (npm run build)
  - Verified deployment script works correctly
- Commands run:
  - `cd web-flasher && npm run dev` (started dev server)
  - `npm run build` (successful build)
  - `./deploy.sh` (deployment preparation)
  - `chmod +x deploy.sh` (made script executable)
- Files touched:
  - `web-flasher/index.html` (completely rebuilt with simplified structure)
  - `web-flasher/src/main.js` (simplified to avoid CORS issues)
  - `web-flasher/sender_manifest.json` (created local manifest)
  - `web-flasher/receiver_manifest.json` (created local manifest)
  - `web-flasher/README.md` (comprehensive deployment guide)
  - `web-flasher/deploy.sh` (deployment script)
  - `.github/workflows/deploy-web-flasher.yml` (GitHub Actions workflow)
  - `SESSION_NOTES.md` (updated with new session log)
- Next steps:
  - Test web flasher locally to ensure it works
  - Deploy to GitHub Pages using the deployment script
  - Enable GitHub Pages in repository settings
  - Test the deployed web flasher with actual ESP32 devices

#### 2025-01-15 22:30 UTC
- Context: GitHub API returning 403 Forbidden error, preventing release fetching
- Changes:
  - Added proper GitHub API headers (Accept, User-Agent) to reduce rate limiting
  - Implemented fallback system to local manifests when GitHub API fails
  - Recreated local firmware files for fallback operation
  - Added status indicators to show when fallback mode is active
  - Improved error handling for API failures (rate limiting vs other errors)
  - Ensured web-flasher works even without GitHub API access
  - Added comprehensive logging for debugging API and fallback behavior
- Commands run:
  - `cp .pio/build/sender/firmware.bin web-flasher/sender_firmware_v1.0.0.bin`
  - `cp .pio/build/receiver/firmware.bin web-flasher/receiver_firmware_v1.0.0.bin`
  - `git add -A && git commit -m "feat(web-flasher): add fallback to local manifests when GitHub API fails"`
- Files touched:
  - `web-flasher/src/main.js` (added API headers, fallback system, status indicators)
  - `web-flasher/sender_firmware_v1.0.0.bin` (recreated for fallback)
  - `web-flasher/receiver_firmware_v1.0.0.bin` (recreated for fallback)
  - `SESSION_NOTES.md` (updated with new session log)
- Next steps:
  - Test web-flasher with fallback system
  - Verify local manifests work when GitHub API fails
  - Test ESP Web Tools integration with fallback manifests
  - Monitor GitHub API rate limiting and fallback behavior

#### 2025-01-15 22:00 UTC
- Context: Encountered CORS policy blocking manifest download from GitHub releases
- Changes:
  - Replaced direct manifest fetching with local manifest generation approach
  - Use GitHub API to get release info (no CORS issues)
  - Generate ESP Web Tools compatible manifests locally using release asset information
  - Create blob URLs from generated manifests to avoid CORS restrictions
  - Add proper blob URL cleanup to prevent memory leaks
  - Remove local firmware files (now using GitHub release assets directly)
  - Clean up obsolete manifest files and examples
- Commands run:
  - `git add -A && git commit -m "fix(web-flasher): resolve CORS issues by generating manifests locally"`
- Files touched:
  - `web-flasher/src/main.js` (added local manifest generation, blob URL handling)
  - `web-flasher/firmware_manifest_example.json` (deleted - obsolete)
  - `web-flasher/receiver_firmware_v1.0.0.bin` (deleted - using GitHub assets)
  - `web-flasher/sender_firmware_v1.0.0.bin` (deleted - using GitHub assets)
  - `SESSION_NOTES.md` (updated with new session log)
- Next steps:
  - Test web-flasher with local manifest generation
  - Verify ESP Web Tools can access generated manifests
  - Test firmware flashing with GitHub release assets
  - Monitor for any remaining CORS or manifest issues

#### 2025-01-15 21:30 UTC
- Context: User requested to fetch manifests from the repository's latest release instead of using local files
- Changes:
  - Integrated GitHub API to fetch latest release information
  - Updated manifest loading to use GitHub release assets instead of local files
  - Added error handling for manifest download failures
  - Updated device selection to refresh manifests from releases
  - Added release information display in firmware details section
  - Fixed manifest names to match actual GitHub release assets (sender_manifest.json, receiver_manifest.json)
  - Added comprehensive logging for debugging manifest loading process
- Commands run:
  - `curl -s https://api.github.com/repos/Skeyelab/LightningDetector/releases/latest` (verified release exists)
  - `git add -A && git commit -m "feat(web-flasher): integrate GitHub releases for manifest fetching"`
- Files touched:
  - `web-flasher/src/main.js` (added GitHub API integration and release fetching)
  - `web-flasher/firmware_manifest.json` (updated with absolute paths)
  - `web-flasher/receiver_manifest.json` (updated with absolute paths)
  - `SESSION_NOTES.md` (updated with new session log)
- Next steps:
  - Test web-flasher with GitHub release manifests
  - Verify manifests can be downloaded from GitHub releases
  - Test ESP Web Tools integration with remote manifests
  - Consider adding fallback to local manifests if GitHub fails

#### 2025-01-15 21:00 UTC
- Context: Web flasher was not working due to missing firmware files and incorrect manifest format
- Changes:
  - Built both sender and receiver firmware using PlatformIO (`pio run -e sender` and `pio run -e receiver`)
  - Copied actual firmware files to web-flasher directory: `sender_firmware_v1.0.0.bin` (375KB) and `receiver_firmware_v1.0.0.bin` (828KB)
  - Fixed firmware manifest format to match ESP Web Tools requirements (based on npmjs.com/package/esp-web-tools documentation)
  - Created separate manifests for transmitter and receiver firmware
  - Updated main.js to use actual manifest files instead of dynamic creation
  - Removed obsolete `createFirmwareManifest()` function
  - Started Vite development server on port 3000
- Commands run:
  - `pio run -e sender` (built transmitter firmware)
  - `pio run -e receiver` (built receiver firmware)
  - `cp .pio/build/sender/firmware.bin web-flasher/sender_firmware_v1.0.0.bin`
  - `cp .pio/build/receiver/firmware.bin web-flasher/receiver_firmware_v1.0.0.bin`
  - `cd web-flasher && npm run dev` (started development server)
- Files touched:
  - `web-flasher/sender_firmware_v1.0.0.bin` (created from built firmware)
  - `web-flasher/receiver_firmware_v1.0.0.bin` (created from built firmware)
  - `web-flasher/firmware_manifest.json` (updated to ESP Web Tools format)
  - `web-flasher/receiver_manifest.json` (created for receiver firmware)
  - `web-flasher/src/main.js` (updated to use manifest files)
- Next steps:
  - Test web-flasher at http://localhost:3000
  - Verify ESP Web Tools can detect and flash firmware
  - Test with actual ESP32 device
  - Consider building and testing both firmware variants

#### 2025-01-15 20:00 UTC
- Context: Web flasher was experiencing serial port errors, Improv detection failures, and port state conflicts
- Changes:
  - Completely rewrote main.js with robust error handling based on Squeezelite-ESP32 installer example
  - Added comprehensive event listeners for install-start, install-success, install-error, and install-progress
  - Implemented detailed error guidance system with specific troubleshooting steps for common issues
  - Disabled Improv protocol to avoid detection failures
  - Added port state management and cleanup to prevent conflicts
  - Enhanced HTML with error guidance section and recovery button
  - Added CSS styling for error guidance and recovery interface
  - **NEW**: Added automatic port recovery system for control signal and port conflict errors
  - **NEW**: Added fallback flasher option when ESP Web Tools fails repeatedly
  - **NEW**: Enhanced error guidance with automatic recovery instructions
  - **NEW**: Added manual port recovery button for advanced troubleshooting
  - **NEW**: Implemented error interception system to catch and handle Improv failures gracefully
  - **NEW**: Added Improv status indicator to show users that Improv errors are expected and handled
  - **NEW**: Added aggressive Improv suppression in firmware manifest
  - **NEW**: Intercepted console errors, promise rejections, and global errors related to Improv
  - **NEW**: Enhanced error interception to catch serial port control signal errors and port conflicts
  - **NEW**: Added aggressive port recovery with forced port cleanup and state reset
  - **NEW**: Added deep recovery function that completely recreates the ESP Web Tools button
  - **NEW**: Added comprehensive error guidance for port recovery failures
  - **NEW**: Added three-tier recovery system: Try Again, Fix Port Issues, Deep Recovery
  - **NEW**: Added recovery state tracking to prevent duplicate recovery attempts
  - **NEW**: Added recovery progress indicators with real-time user feedback
  - **NEW**: Enhanced recovery process with step-by-step status updates
  - **NEW**: Added recovery cooldown system to prevent rapid-fire recovery attempts
  - **REFACTOR**: Simplified approach based on working Squeezelite-ESP32 installer strategy
  - **REFACTOR**: Removed complex error interception in favor of letting ESP Web Tools handle its own errors
  - **REFACTOR**: Simplified recovery system to focus on user guidance rather than automatic fixes
  - **REFACTOR**: Streamlined user interface to match successful working examples
  - **HYBRID**: Implemented minimal, targeted error handling for critical serial port issues
  - **HYBRID**: Added critical error monitoring for issues that break functionality
  - **HYBRID**: Combined simplicity of working examples with targeted fixes for specific problems
  - **HYBRID**: Added manual port state reset for user-initiated conflict resolution
- Commands run:
  - None (code changes only)
- Files touched:
  - `web-flasher/src/main.js` (complete rewrite with hybrid error handling - simple approach + targeted fixes)
  - `web-flasher/index.html` (simplified error guidance section with two recovery buttons)
- Next steps:
  - Test the hybrid error handling approach
  - Verify that critical serial port issues are caught and handled
  - Test with actual ESP32 devices to ensure reliability
  - Monitor user experience with targeted error handling
  - Consider this approach successful if it resolves specific issues while maintaining simplicity

#### 2025-01-15 19:30 UTC
- Context: Web flasher was failing due to deprecated esp-web-flasher package compatibility issues
- Changes:
  - Completely refactored web flasher to use ESP Web Tools v10
  - Replaced deprecated esp-web-flasher with modern esp-web-tools
  - Updated HTML interface to work with ESP Web Tools
  - Simplified JavaScript code by removing complex ESPLoader initialization
  - Created test firmware files for development
- Commands run:
  - `npm install esp-web-tools@10`
  - `npm run dev`
- Files touched:
  - `web-flasher/package.json` (updated dependencies)
  - `web-flasher/src/main.js` (complete refactor)
  - `web-flasher/index.html` (updated for ESP Web Tools)
  - `web-flasher/receiver_firmware_v1.0.0.bin` (created test file)
  - `web-flasher/sender_firmware_v1.0.0.bin` (created test file)
- Next steps:
  - Test the new ESP Web Tools integration
  - Verify firmware flashing works correctly
  - Update documentation to reflect new approach

#### 2025-01-15 19:00 UTC
- Context: Web flasher was experiencing ESPLoader initialization failures
- Changes:
  - Attempted to fix ESPLoader initialization issues
  - Added extensive debugging for ESPLoader methods
  - Tried manual method binding from prototype
  - Identified that esp-web-flasher package is deprecated
- Commands run:
  - `npm run dev` (multiple attempts)
- Files touched:
  - `web-flasher/src/main.js` (extensive debugging additions)
- Next steps:
  - Consider alternative flashing approaches
  - Research modern ESP32 web flashing solutions


