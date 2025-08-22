#!/bin/bash

# Upload SPIFFS data files to receiver device
# This uploads the web interface files to the device's SPIFFS filesystem

echo "==========================================="
echo "Uploading SPIFFS Data Files"
echo "==========================================="

echo ""
echo "📁 Uploading web interface files to SPIFFS..."
echo "---------------------------------------------"
pio run -e receiver --target uploadfs --upload-port /dev/cu.usbserial-0001

if [ $? -eq 0 ]; then
    echo "✅ SPIFFS upload completed successfully!"
    echo ""
    echo "Web interface files uploaded:"
    echo "- /index.html - Main web page"
    echo "- /css/styles.css - Styling"
    echo "- /js/main.js - JavaScript functionality"
    echo ""
    echo "You can now access the web interface at:"
    echo "http://<device-ip>/index.html"
else
    echo "❌ SPIFFS upload failed!"
    exit 1
fi

echo "==========================================="
