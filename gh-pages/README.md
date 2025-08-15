# SBT PIO Web Flasher

A web-based ESP32 firmware flasher for the SBT PIO Heltec V3 OLED lightning detection system.

## Features

- **No Installation Required**: Works directly in your browser
- **Cross-Platform**: Compatible with Windows, macOS, and Linux
- **Dual Device Support**: Flash both transmitter and receiver firmware
- **Modern Web Technology**: Uses ESP Web Tools for reliable flashing
- **CORS-Free**: No external API calls that could cause deployment issues

## Browser Requirements

- **Chrome/Edge**: Full support (recommended)
- **Firefox**: Limited support
- **Safari**: Not supported
- **Mobile**: Limited support due to USB restrictions

## Local Development

1. Install dependencies:
   ```bash
   npm install
   ```

2. Start development server:
   ```bash
   npm run dev
   ```

3. Open your browser to `http://localhost:3000`

## Building for Production

1. Build the project:
   ```bash
   npm run build
   ```

2. Preview the build:
   ```bash
   npm run preview
   ```

## Deployment to GitHub Pages

1. Build the project:
   ```bash
   npm run build
   ```

2. The build output is in the `dist/` folder
   - Copy the contents of `dist/` to your GitHub Pages branch
   - Or configure GitHub Actions to auto-deploy

3. Ensure your firmware files are accessible:
   - `sender_firmware_v1.0.0.bin`
   - `receiver_firmware_v1.0.0.bin`
   - `sender_manifest.json`
   - `receiver_manifest.json`

## How It Works

1. **Device Selection**: Choose between transmitter or receiver firmware
2. **ESP Web Tools**: Uses the official Espressif web flashing library
3. **Local Manifests**: References local firmware files to avoid CORS issues
4. **Web Serial API**: Direct USB communication through the browser

## File Structure

```
web-flasher/
├── src/
│   └── main.js          # Main application logic
├── index.html            # Main HTML file
├── sender_manifest.json  # Transmitter firmware manifest
├── receiver_manifest.json # Receiver firmware manifest
├── sender_firmware_v1.0.0.bin    # Transmitter firmware
├── receiver_firmware_v1.0.0.bin  # Receiver firmware
├── package.json          # Dependencies and scripts
└── vite.config.js        # Build configuration
```

## Troubleshooting

### Common Issues

1. **"Browser Not Supported"**
   - Use Chrome or Edge
   - Ensure browser is up to date

2. **"HTTPS Required"**
   - Web Serial API requires secure connection
   - Use HTTPS or localhost for development

3. **Device Not Detected**
   - Ensure ESP32 is in download mode (hold BOOT button)
   - Use a data USB cable, not just charging cable
   - Try different USB ports

4. **Permission Denied**
   - Grant USB permissions when prompted
   - Close other applications using the serial port

### ESP32 Download Mode

1. Hold the BOOT button on your ESP32
2. Press and release the RESET button (while holding BOOT)
3. Release the BOOT button
4. Connect USB cable
5. The device should now be in download mode

## Security Notes

- Web Serial API requires user consent for USB access
- No firmware data is sent to external servers
- All flashing happens locally through your browser
- Firmware files are served from the same origin

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

MIT License - see LICENSE file for details
