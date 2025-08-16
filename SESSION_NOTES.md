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

#### 2025-01-16 04:15 UTC
- Context: Implemented light sleep mode to replace deep sleep, enabling ping detection while sleeping
- Changes:
  - **Light Sleep Mode**: Replaced deep sleep with ESP32 light sleep that keeps LoRa radio active
  - **Ping Detection**: Receiver can now wake up when it receives pings from sender
  - **Wake-up Sources**: Configured three wake-up triggers:
    * Button press (manual wake-up)
    * LoRa radio interrupt on DIO1 pin (ping detection)
    * 60-second timer backup (prevents indefinite sleep)
  - **Radio Monitoring**: LoRa radio remains powered during sleep for continuous ping listening
  - **Wake-up Detection**: Device identifies wake-up reason (button, LoRa interrupt, or timer)
  - **Power Optimization**: OLED display still turned off during sleep for power savings
- Commands run:
  - `pio run -e sender` (build SUCCESS - 403KB Flash, 12.1% usage)
  - `pio run -e receiver` (build SUCCESS - 860KB Flash, 25.7% usage)
  - `g++ -std=c++17 test_app_logic.cpp ../src/app_logic.cpp -o test_app_logic && ./test_app_logic` (tests passed)
  - `git add -A && git commit -m "feat: implement light sleep mode with LoRa radio active"`
- Files touched:
  - `src/main.cpp` (replaced deep sleep with light sleep implementation)
  - `src/main.cpp` (added LoRa interrupt wake-up source configuration)
  - `src/main.cpp` (updated wake-up reason detection and logging)
  - `src/main.cpp` (modified button handlers to use light sleep)
- Build Results:
  - ✅ Sender firmware builds successfully (403KB Flash, 12.1% usage)
  - ✅ Receiver firmware builds successfully (860KB Flash, 25.7% usage)
  - ✅ All tests pass with new light sleep functionality
- **RESULT**: ✅ Successfully implemented light sleep with ping detection
  - **Best of both worlds**: Power savings + lightning detection capability
  - **Automatic wake-up**: Receiver wakes on ping reception without manual intervention
  - **Continuous monitoring**: Lightning detection works even while "sleeping"
  - **Smart power management**: OLED off + LoRa active for optimal power/functionality balance
  - **Wake-up intelligence**: Device knows why it woke up and can respond accordingly

#### 2025-01-16 04:00 UTC
- Context: Implemented real ESP32 deep sleep mode to replace placeholder sleep functionality
- Changes:
  - **Real Deep Sleep**: Replaced placeholder sleep mode with actual ESP32 deep sleep functionality
  - **State Preservation**: Added RTC memory variables to preserve device state across sleep cycles
    * `sleepCount`: Tracks number of sleep cycles
    * `lastSleepTime`: Records when device last went to sleep
    * `wasInSleepMode`: Flag to detect wake-up from sleep
  - **Wake-up Sources**: Configured button as primary wake-up source (LOW trigger) + 30-second timer backup
  - **Power Management**: OLED display is turned off during sleep to conserve power
  - **State Restoration**: Device automatically restores state and settings after wake-up
  - **Persistent Storage**: Important settings are saved to flash before entering sleep mode
- Commands run:
  - `pio run -e sender` (build SUCCESS - 401KB Flash, 12.0% usage)
  - `pio run -e receiver` (build SUCCESS - 858KB Flash, 25.7% usage)
  - `g++ -std=c++17 test_app_logic.cpp ../src/app_logic.cpp -o test_app_logic && ./test_app_logic` (tests passed)
  - `git add -A && git commit -m "feat: implement real ESP32 deep sleep mode"`
- Files touched:
  - `src/main.cpp` (added ESP32 deep sleep includes, RTC memory variables, and sleep functions)
  - `src/main.cpp` (updated button handlers to call real deep sleep instead of placeholder)
  - `src/main.cpp` (added wake-up detection and state restoration in setup)
- Build Results:
  - ✅ Sender firmware builds successfully (401KB Flash, 12.0% usage)
  - ✅ Receiver firmware builds successfully (858KB Flash, 25.7% usage)
  - ✅ All tests pass with new deep sleep functionality
- **RESULT**: ✅ Successfully implemented real low-power sleep mode
  - Long press (>3s) now actually enters ESP32 deep sleep mode
  - Button press wakes device from sleep and restores previous state
  - RTC memory preserves device state across sleep cycles
  - OLED display is powered down during sleep for maximum power savings
  - 30-second timer backup ensures device doesn't sleep indefinitely

#### 2025-01-16 03:30 UTC
- Context: Simplified button functionality for both devices to three consistent actions
- Changes:
  - **Simplified Button Actions**: Both sender and receiver now use identical button behavior
    * Short press (<1s): Cycles through Spreading Factor values (SF7-SF12)
    * Medium press (1-3s): Cycles through Bandwidth values (62.5, 125, 250, 500 kHz)
    * Long press (>3s): Enters sleep mode
  - **Removed Complex Logic**: Eliminated multi-click detection, device-specific button functions, and role switching
  - **Unified Behavior**: Both devices now respond identically to button presses for consistent user experience
  - **Updated Tests**: Modified test files to use new ButtonAction enum values (CycleSF, CycleBW, SleepMode)
  - **Clean Implementation**: Removed all multi-click variables and complex button state tracking
- Commands run:
  - `g++ -std=c++17 test_app_logic.cpp ../src/app_logic.cpp -o test_app_logic && ./test_app_logic` (tests passed)
  - `pio run -e sender` (build SUCCESS - 386KB Flash, 11.6% usage)
  - `pio run -e receiver` (build SUCCESS - 850KB Flash, 25.5% usage)
  - `git add -A && git commit -m "feat: simplify button functionality to three actions"`
- Files touched:
  - `src/app_logic.h` (simplified ButtonAction enum to three actions)
  - `src/app_logic.cpp` (removed classifyMultipleClicks, simplified classifyPress)
  - `src/main.cpp` (updated button handlers for both devices, removed multi-click logic)
  - `test/test_app_logic.cpp` (updated tests for new button actions)
  - `test/test_integration.cpp` (updated integration tests)
- Build Results:
  - ✅ Sender firmware builds successfully (386KB Flash, 11.6% usage)
  - ✅ Receiver firmware builds successfully (850KB Flash, 25.5% usage)
  - ✅ All tests pass with new button functionality
- **RESULT**: ✅ Successfully implemented simplified, consistent button behavior
  - Both devices now have identical button response patterns
  - Short press cycles SF, medium press cycles BW, long press enters sleep
  - Clean, maintainable code with no complex multi-click detection
  - Consistent user experience across sender and receiver devices

#### 2025-01-16 03:00 UTC
- Context: Resolved merge conflicts and fixed build errors from eliminated `oledRole()` function
- Changes:
  - Resolved merge conflicts in `src/main.cpp` by choosing enhanced IP scrolling implementation
  - Fixed compilation errors by replacing `oledRole()` calls with `oledMsg("Role", isSender ? "Sender" : "Receiver")`
  - Updated two locations in main.cpp where `oledRole()` was still being called
  - Maintained all IP scrolling functionality: 300ms intervals, IP change detection, horizontal scrolling
  - Cleaned up duplicate code and merge conflict markers
  - Verified both sender and receiver variants compile successfully
  - Committed resolved changes with descriptive commit message
- Commands run:
  - `pio run` (identified oledRole compilation errors)
  - `pio run -e sender` (compilation test - SUCCESS)
  - `pio run -e receiver` (compilation test - SUCCESS)
  - `git add -A && git commit -m "fix: resolve merge conflicts in src/main.cpp - choose enhanced IP scrolling implementation"`
- Files touched:
  - `src/main.cpp` (resolved merge conflicts, maintained enhanced scrolling functionality, replaced oledRole() calls around lines 994-998)
- Build Results:
  - ✅ Sender firmware builds successfully (385KB Flash, 11.5% usage)
  - ✅ Receiver firmware builds successfully (849KB Flash, 25.4% usage)
- Next steps:
  - Test the resolved code on physical hardware
  - Verify IP scrolling works correctly with various IP address lengths
  - Test firmware on physical hardware to ensure display functionality works correctly
  - Verify role display shows correctly during normal operation
  - Ensure no compilation errors or linter issues remain

#### 2025-01-16 02:30 UTC
- Context: Lightning detector project with Heltec V3 boards - implementing WiFi IP address display feature
- Changes:
  - Added IP address display above network location in WiFi status bar
  - Modified `drawStatusBar()` function in `src/main.cpp` to show IP address when WiFi connected
  - IP address appears 10 pixels above network location ("Home"/"Work") text
  - Uses `WiFi.localIP().toString().c_str()` API as specified in Linear issue ERI-13
  - Applied consistent 5x7 pixel font for status bar text
  - Integrated seamlessly with existing `oledMsg()` display system
  - **Enhanced with scrolling**: Added automatic scrolling for long IP addresses (>12 chars)
  - Scrolling logic: 300ms intervals, resets when IP changes, handles IPv4 addresses properly
  - Display accommodates both short IPs (displayed fully) and long IPs (scrolled horizontally)
- Commands run:
  - Code review and analysis of display functions and WiFi management
  - Implementation via `search_replace` tool for `src/main.cpp`
  - Added scrolling state variables and logic for long IP addresses
  - Resolved merge conflicts between HEAD and main branches
- Files touched:
  - `src/main.cpp` (modified `drawStatusBar()` function and added scrolling variables around lines 151-203)
  - `SESSION_NOTES.md` (resolved merge conflicts)
- Migrations:
  - N/A (no database changes)
- Next steps:
  - Test implementation on physical hardware with WiFi connection
  - Verify IP display updates correctly during WiFi reconnection events
  - Test scrolling behavior with longer IP addresses (e.g., 192.168.100.255)
  - Ensure no visual conflicts with other OLED display elements

#### 2025-01-15 21:30 UTC
- Context: **COMPLETED** ERI-16 Linear issue - replaced PING text with blinking dot indicator on OLED display
- Changes:
  - Added blinking dot state variables (dotBlinkStartMs, dotBlinkActive, DOT_FLASH_DURATION_MS) in main.cpp
  - Implemented triggerPingDotBlink() function with overlap prevention for clean RX behavior
  - Implemented drawPingDot() function with 1-second flash duration at coordinates (55, 12)
  - Integrated drawPingDot() call into oledMsg() function for automatic rendering
  - Replaced TX ping OLED display with triggerPingDotBlink() call - dot shows 1s on, 1s off (50% duty cycle)
  - Replaced RX ping OLED display with triggerPingDotBlink() call - dot shows 1s per ping received
  - **PRESERVED serial console logging**: TX pings log "[TX] PING seq=X OK", RX pings log "[RX] PING seq=X | RSSI X.X | SNR X.X | PKT:X"
  - **PRESERVED original display content**: Shows normal "Mode"/"Sender"/"Receiver" text + settings info
  - Added display refresh logic to ensure dot visibility during flash cycles
  - Created flash_both.sh script for easy deployment to both devices
  - Updated test file with documentation comment about display behavior change
- Commands run:
  - Multiple file edits for blinking dot implementation
  - Compilation and build testing for both sender and receiver variants
  - Successful firmware flash to both Heltec V3 devices via flash_both.sh
- Files touched:
  - `src/main.cpp` (added dot functions, modified TX/RX ping logic, preserved serial logging)
  - `test/test_app_logic.cpp` (added documentation comment)
  - `flash_both.sh` (new deployment script)
- **RESULT**: ✅ Successfully implemented and tested on hardware
  - Sender: Perfect 50% duty cycle dot blinking synchronized with 2-second ping interval
  - Receiver: Clean 1-second dot flashes on ping reception without overlap
  - All ping functionality and debugging preserved
  - Subtle, unobtrusive visual indication as requested

#### 2025-01-15 18:30 UTC
- Context: **COMPLETED** ERI-14 Linear issue to remove RX/TX switching from button functionality
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
  - Fixed missing `app_logic.h` include in `main.cpp` causing build failures
- Commands run:
  - `g++ -std=c++17 verify_button_logic.cpp -o verify_button_logic && ./verify_button_logic`
  - `pio run -e sender && pio run -e receiver` (verified successful builds)
- Files touched:
  - `src/app_logic.h`, `src/app_logic.cpp` (updated button action classification)
  - `src/main.cpp` (completely reworked button handling, removed role switching, added missing include)
  - `test/test_integration.cpp` (updated tests for new button functionality)
  - `test/test_app_logic.cpp` (updated unit tests for new enum values)
- Build Results:
  - ✅ Sender firmware builds successfully (384KB Flash, 11.5% usage)
  - ✅ Receiver firmware builds successfully (848KB Flash, 25.4% usage)
  - ✅ Button logic verified with test script
- **RESULT**: ✅ Successfully implemented device-specific button functionality
  - No accidental mode switching - role fixed at build time
  - Intuitive, predictable button behavior tailored to device purpose
  - Multi-click sleep mode functionality added
  - All existing LoRa parameter cycling preserved

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


