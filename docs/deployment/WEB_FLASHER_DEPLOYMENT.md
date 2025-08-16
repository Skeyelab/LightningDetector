# Web Flasher Deployment Guide

## 🎯 Problem Solved

Your web flasher was experiencing **CORS (Cross-Origin Resource Sharing) issues** when trying to:
- Fetch from GitHub API
- Download firmware files from GitHub releases
- Access external resources from GitHub Pages

## ✅ Solution Implemented

We completely rebuilt the web flasher to eliminate all CORS issues by:

1. **Removing External Dependencies**: No more GitHub API calls or external resource fetching
2. **Local Manifests**: Created local manifest files that reference local firmware files
3. **ESP Web Tools Integration**: Direct integration with ESP Web Tools library
4. **CORS-Free Architecture**: Everything served from the same origin

## 🚀 What's New

### Simplified Architecture
- **HTML**: Clean, modern interface with device selection
- **JavaScript**: Streamlined logic using ESP Web Tools directly
- **Manifests**: Local JSON files for transmitter and receiver firmware
- **No External APIs**: Completely self-contained

### Device Support
- **Transmitter (Sender)**: Lightning detection and environmental monitoring
- **Receiver**: LoRa reception and WiFi data forwarding
- **Easy Switching**: Radio button selection between device types

### Deployment Ready
- **Build System**: Vite-based build process
- **Deployment Script**: `deploy.sh` for easy GitHub Pages preparation
- **GitHub Actions**: Automated workflow for continuous deployment
- **Documentation**: Comprehensive README and troubleshooting guide

## 📁 File Structure

```
web-flasher/
├── src/
│   └── main.js              # Main application logic
├── index.html                # Main HTML file
├── sender_manifest.json      # Transmitter firmware manifest
├── receiver_manifest.json    # Receiver firmware manifest
├── sender_firmware_v1.0.0.bin    # Transmitter firmware
├── receiver_firmware_v1.0.0.bin  # Receiver firmware
├── package.json              # Dependencies and scripts
├── vite.config.js            # Build configuration
├── deploy.sh                 # Deployment script
└── README.md                 # Comprehensive documentation
```

## 🔧 How to Deploy

### Option 1: Manual Deployment (Recommended for first time)

1. **Build the project**:
   ```bash
   cd web-flasher
   npm install
   npm run build
   ```

2. **Run deployment script**:
   ```bash
   ./deploy.sh
   ```

3. **Deploy to GitHub Pages**:
   - Go to your repository settings
   - Enable GitHub Pages
   - Set source to `gh-pages` branch
   - Or manually copy files from `gh-pages/` folder

### Option 2: Automated Deployment

1. **Enable GitHub Actions**:
   - The workflow file is already created
   - It will automatically deploy on pushes to main branch
   - Only deploys when `web-flasher/` files change

2. **First deployment**:
   - Push your changes to main branch
   - GitHub Actions will build and deploy automatically
   - Check Actions tab for deployment status

## 🌐 Access Your Web Flasher

Once deployed, your web flasher will be available at:
```
https://yourusername.github.io/LightningDetector/
```

## 🧪 Testing

### Local Testing
```bash
cd web-flasher
npm run dev
# Open http://localhost:3000
```

### Production Testing
```bash
cd web-flasher
npm run build
npm run preview
# Open http://localhost:4173
```

## 🔍 Troubleshooting

### Common Issues

1. **"Browser Not Supported"**
   - Use Chrome or Edge (recommended)
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

## 📋 Browser Requirements

- **Chrome/Edge**: Full support (recommended)
- **Firefox**: Limited support
- **Safari**: Not supported
- **Mobile**: Limited support due to USB restrictions

## 🔒 Security Features

- Web Serial API requires user consent for USB access
- No firmware data is sent to external servers
- All flashing happens locally through your browser
- Firmware files are served from the same origin

## 🎉 Success Indicators

Your web flasher is working correctly when:

1. ✅ Page loads without CORS errors
2. ✅ Device type selection works (transmitter/receiver)
3. ✅ ESP Web Tools button appears
4. ✅ Firmware files can be accessed locally
5. ✅ No external API calls in browser console
6. ✅ Build process completes successfully
7. ✅ Deployment script runs without errors

## 🚀 Next Steps

1. **Test locally** to ensure everything works
2. **Deploy to GitHub Pages** using the deployment script
3. **Test with actual ESP32 devices** to verify flashing works
4. **Monitor GitHub Actions** for automated deployments
5. **Update firmware files** as needed for new releases

## 📞 Support

If you encounter issues:

1. Check the browser console for error messages
2. Verify all firmware files are present
3. Ensure GitHub Pages is properly configured
4. Check the Actions tab for deployment errors
5. Review the troubleshooting section in the README

---

**Your web flasher is now CORS-free and ready for GitHub Pages deployment! 🎉**
